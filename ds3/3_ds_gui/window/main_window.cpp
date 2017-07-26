//
// The MIT License(MIT)
//
// Copyright(c) 2014 Demonsaw LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifdef _MSC_VER
#pragma warning(disable: 4005 4075)
#endif

#include <cassert>
#include <thread>

#include <QAbstractButton>
#include <QAction>
#include <QButtonGroup>
#include <QByteArray>
#include <QCloseEvent>
#include <QContextMenuEvent>
#include <QDesktopWidget>
#include <QDir>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QPainter>
#include <QResizeEvent>
#include <QStackedWidget>
#include <QString>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

#include "component/endpoint_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/client/client_service_component.h"
#include "component/io/upload_component.h"
#include "component/message/message_acceptor_component.h"
#include "component/status/status_component.h"

#include "dialog/wizard_dialog.h"
#include "entity/entity.h"
#include "entity/entity_action.h"
#include "entity/entity_factory.h"
#include "entity/entity_window.h"
#include "font/font_action.h"
#include "font/font_button.h"
#include "object/function.h"
#include "resource/resource.h"
#include "security/filter/base.h"
#include "system/type.h"
#include "thread/thread_info.h"
#include "resource/icon_provider.h"
#include "utility/value.h"
#include "window/main_window.h"
#include "window/client/client_window.h"
#include "window/router/router_window.h"

namespace eja
{
	// Static
	main_window::ptr main_window::s_ptr = nullptr;

	// Constructor
	main_window::main_window(qt_application& app, QWidget* parent /*= 0*/) : QMainWindow(parent), m_app(app)
	{
		// Singleton
		assert(!s_ptr);
		s_ptr = this;

		// Window
		setWindowIcon(QIcon(resource::icon));
		setWindowTitle(demonsaw::titlebar);
		setDockOptions(QMainWindow::AnimatedDocks);
		setToolTipDuration(0);

		// UI
		create();
		layout();
		signal();
	}

	void main_window::create()
	{
		// Action
		m_add_button = make_button(fa::plus, "Add Client/Router");
		m_save_button = make_button(fa::save, "Save Options");
		m_update_button = make_button(fa::refresh, "Restart Clients/Routers");

		m_add_action = make_action("Add", fa::plus);
		m_remove_action = make_action("Remove", fa::minus);
		m_update_action = make_action("Restart", fa::refresh);

		// Menubar		
		m_menubar = addToolBar("");
		m_menubar->setObjectName("titlebar");
		m_menubar->setToolTipDuration(0);
		m_menubar->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextOnly);
		m_menubar->setFont(QFont(demonsaw::font_awesome));
		m_menubar->setFloatable(false);
		m_menubar->setMovable(true);

		m_menubar->addWidget(m_add_button);
		m_menubar->addWidget(m_save_button);
		m_menubar->addWidget(m_update_button);

		m_menu_stack = new QStackedWidget(this);

#if _MACX
		// HACK: OSX needs a menu hook to close & save properly (from the system menu)
		m_quit_action = new QAction(tr("&Quit"), this);
		connect(m_quit_action, &QAction::triggered, this, &main_window::close);

		const auto menu = menuBar();
		m_file_menu = menu->addMenu(tr("&File"));
		m_file_menu->addAction(m_quit_action);
#endif
		// Break
		addToolBarBreak(Qt::TopToolBarArea);

		// Toolbar
		m_toolbar = addToolBar("");
		m_toolbar->setObjectName("main");
		m_toolbar->setToolTipDuration(0);
		m_toolbar->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextUnderIcon);
		m_toolbar->setContextMenuPolicy(Qt::CustomContextMenu);
		m_toolbar->setIconSize(QSize(64, 32));
		m_toolbar->setFloatable(false);
		m_toolbar->setMovable(true);

		// Spacer
		QWidget* spacer = new QWidget(m_menubar);
		spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		m_menubar->addWidget(spacer);

		// Entity
		m_entity_stack = new QStackedWidget(this);

		// Group
		m_button_group = new QButtonGroup(this);
		m_button_group->setExclusive(true);
	}

	void main_window::layout()
	{		
		// Menu
		m_menubar->addWidget(m_menu_stack);

		// Entity
		setCentralWidget(m_entity_stack);
	}

	void main_window::signal()
	{
		// Slot		
		connect(this, &main_window::set_status, this, &main_window::on_set_status);
		connect(this, &main_window::set_enabled, [&](font_button* button) { button->setEnabled(true); }); 
		connect(m_button_group, static_cast<void (QButtonGroup::*)(QAbstractButton*)>(&QButtonGroup::buttonClicked), this, &main_window::on_select_entity); 
		connect(m_toolbar, &main_window::customContextMenuRequested, this, &main_window::on_show_menu);

		connect(m_add_button, &font_button::clicked, this, [&]() { add(); });		
		connect(m_save_button, &font_button::clicked, this, [&]() { save(); });
		connect(m_update_button, &font_button::clicked, this, [&]() { update(); });

		connect(m_add_action, &QAction::triggered, this, [&]() { add(); });
		connect(m_remove_action, &QAction::triggered, this, [&]() { remove(); });
		connect(m_update_action, &QAction::triggered, this, [&]() { restart(); });
	}

	// Interface
	void main_window::init()
	{		
		// Router
		const auto& routers = get_routers();
		for (const auto& entity : routers)
		{
			// Status
			const auto status = entity->get<status_component>();
			const auto function = function::create([this](const entity::ptr entity) { set_status(entity); });
			status->add(function);

			init_router(entity);
		}

		// Separator
		if (!routers->empty())
			m_toolbar->addSeparator();

		// Client
		const auto& clients = get_clients();
		for (const auto& entity : clients)
		{
			// Status
			const auto status = entity->get<status_component>();
			const auto function = function::create([this](const entity::ptr entity) { set_status(entity); });
			status->add(function);

			init_client(entity);
		}

		m_app.init();

		if (m_app.empty())
			add();
	}

	void main_window::init_client(const entity::ptr entity)
	{
		// Selected
		const auto selected = !m_entity_stack->count();

		// Action
		const auto endpoint = entity->get<endpoint_component>();		
		entity_action* action = new entity_action(entity, this);
		action->setText(QString::fromStdString(endpoint->get_name()));		

		action->setIcon(QIcon(resource::logo_gray));
		action->setCheckable(true);
		action->setChecked(selected);

		// Button
		auto button = new QToolButton(this);
		button->installEventFilter(this);
		button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		button->setDefaultAction(action);

		// Window
		const auto window = new client_window(entity, this);
		action->setData(QVariant::fromValue(window));
		window->set_action(action);
		
		// Container
		auto action_ex = m_toolbar->addWidget(button);
		action->set_action(action_ex);

		m_button_group->addButton(button);
		m_entity_stack->addWidget(window);
		m_menu_stack->addWidget(window->get_toolbar());		

		if (selected)
			emit on_select_entity(button);
	}

	void main_window::init_router(const entity::ptr entity)
	{
		// Selected
		const auto selected = !m_entity_stack->count();

		// Action		
		const auto endpoint = entity->get<endpoint_component>();
		entity_action* action = new entity_action(entity, this);
		action->setText(QString::fromStdString(endpoint->get_name()));

		action->setIcon(QIcon(resource::logo_gray));
		action->setCheckable(true);
		action->setChecked(selected);

		// Button
		auto button = new QToolButton(this);
		button->installEventFilter(this);
		button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		button->setDefaultAction(action);

		// Window
		const auto window = new router_window(entity, this);
		action->setData(QVariant::fromValue(window));
		window->set_action(action);

		// Container
		auto action_ex = m_toolbar->addWidget(button);
		action->set_action(action_ex);

		m_button_group->addButton(button);
		m_entity_stack->addWidget(window);
		m_menu_stack->addWidget(window->get_toolbar());

		if (selected)
			emit on_select_entity(button);
	}

	void main_window::update()
	{
		if (!m_update_button->isEnabled())
			return;

		// Spam
		m_update_button->setEnabled(false);
		QTimer::singleShot(ui::refresh, [&]() { set_enabled(m_update_button); });

		if (has_routers())
		{
			// Router
			const auto& routers = get_routers();
			for (const auto& entity : routers)
			{
				const auto acceptor = entity->get<message_acceptor_component>();
				if (acceptor->valid())
					acceptor->restart();
			}

			QTimer::singleShot(ui::refresh / 2, [&]()
			{
				// Client
				const auto& clients = get_clients();
				for (const auto& entity : clients)
				{
					const auto service = entity->get<client_service_component>();
					if (service->valid())
						service->restart();
				}

				m_app.update();
			});
		}
		else
		{
			// Client
			const auto& clients = get_clients();
			for (const auto& entity : clients)
			{
				const auto service = entity->get<client_service_component>();
				if (service->valid())
					service->restart();
			}

			m_app.update();
		}
	}

	void main_window::shutdown()
	{
		// Geometry
		auto data = saveGeometry();
		const auto geometry = base64::encode(data.data(), data.size());
		m_app.set_geometry(geometry);

		// State
		data = saveState();
		const auto state = base64::encode(data.data(), data.size());
		m_app.set_state(state);

		// TODO
		//
		//

		m_app.shutdown();
	}

	void main_window::clear()
	{
		m_app.clear();
	}

	// Update
	void main_window::show()
	{
		// Geometry
		if (m_app.has_geometry())
		{
			const auto& geometry = m_app.get_geometry();
			auto data = base64::decode(geometry);
			QByteArray byteArray(data.c_str(), data.length());
			restoreGeometry(byteArray);

			// State
			const auto& state = m_app.get_state();
			data = base64::decode(state);
			byteArray.setRawData(data.c_str(), data.length());
			restoreState(byteArray);
		}
		else
		{
			// Window
			QSize size(1024, 576);
			adjustSize();
			resize(size);
		}

		QMainWindow::show();
	}

	void main_window::add()
	{
		wizard_dialog dlg(this);
		if (!isVisible())
		{
			QDesktopWidget* desktop = QApplication::desktop();
			dlg.move(QApplication::desktop()->screen()->rect().center() - dlg.rect().center());
		}

		dlg.exec();

		switch (dlg.get_status())
		{
			case wizard_status::done:
			{
				const auto color = dlg.get_color();
				const auto name = dlg.get_name();
				const auto address = dlg.get_address();
				const auto port = dlg.get_port();

				if (dlg.is_router())
					add_router(name, address, port, color, true);
				else
					add_client(name, address, port, color, true);

				break;
			}
			case wizard_status::quick:
			{
				if (dlg.is_router())
					add_router(router::name, router::localhost, router::port, true);
				else
					add_client(client::name, router::home, router::port_beta, true);

				break;
			}
			default:
			{
				break;
			}
		}
	}

	void main_window::add_client(const std::string& name, const std::string& address, const u16 port, const u32 rgba /*= 0*/, const bool enabled /*= false*/)
	{
		// Client
		const auto entity = entity_factory::create_client();
		entity->disable();

		auto endpoint = entity->get<endpoint_component>();
		endpoint->set_name(name);

		if (rgba)
			endpoint->set_color(rgba);

		auto& clients = get_clients();
		clients->push_back(entity);

		// Router
		const auto message_list = entity->get<message_list_component>();
		const auto e = entity_factory::create_client_message(entity);
		message_list->push_back(e);

		endpoint = e->get<endpoint_component>();
		endpoint->set_name(router::name);
		endpoint->set_address(address);
		endpoint->set_port(port);

		// Status
		const auto status = entity->get<status_component>();
		const auto function = function::create([this](const entity::ptr entity) { set_status(entity); });
		status->add(function);

		init_client(entity);

		if (enabled)
			toggle(entity);
	}

	void main_window::add_router(const std::string& name, const std::string& address, const u16 port, const u32 rgba /*= 0*/, const bool enabled /*= false*/)
	{
		// Client
		const auto entity = entity_factory::create_message();
		entity->disable();

		auto endpoint = entity->get<endpoint_component>();
		endpoint->set_name(name);
		endpoint->set_address(address);
		endpoint->set_port(port);

		if (rgba)
			endpoint->set_color(rgba);

		auto& routers = get_routers();
		routers->push_back(entity);

		// Status
		const auto status = entity->get<status_component>();
		const auto function = function::create([this](const entity::ptr entity) { set_status(entity); });
		status->add(function);

		init_router(entity);

		if (enabled)
			toggle(entity);
	}

	void main_window::remove()
	{		
		// Button
		const auto button = m_button_group->checkedButton();
		if (unlikely(!button || button->actions().empty()))
			return;

		// Entity
		const auto action = static_cast<entity_action*>(button->actions().front());
		const auto entity = action->get_entity();

		const auto& name = entity->get<endpoint_component>()->get_name();
		const QString message = QString("Do you really want to remove \"%1\"?").arg(name.c_str());
		const auto result = QMessageBox::question(this, "Remove Confirmation", message, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (result == QMessageBox::No)
			return;

		// Action
		auto action_ex = action->get_action();
		m_toolbar->removeAction(action_ex);
		delete action;

		// Button
		m_button_group->removeButton(button);

		// Window
		const auto window = get_window(entity);
		if (window)
		{
			auto index = m_entity_stack->currentIndex();
			m_entity_stack->removeWidget(window);

			// Adjust the index (more user-friendly)
			if (m_entity_stack->count() > 0)
			{
				if (index == m_entity_stack->count())
					--index;

				m_entity_stack->setCurrentIndex(index);

				assert(!m_button_group->buttons().empty());
				const auto button = m_button_group->buttons()[index];
				emit m_button_group->buttonClicked(button);

				const auto action = static_cast<entity_action*>(button->actions().front());
				action->setChecked(true);
			}

			delete window;
		}

		if (is_client(entity))
		{
			auto& clients = get_clients();
			clients->remove(entity);

			// Quit - Careful with the ptr lifetime!
			if (entity->enabled())
			{
				std::thread thread([entity]()
				{
					const auto service = entity->get<client_service_component>();
					if (service->valid())
					{
						service->async_quit();

						std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds::quit));
					}						
				});

				thread.detach();
			}
		}
		else
		{
			auto& routers = get_routers();
			routers->remove(entity);

			if (entity->enabled())
			{
				const auto acceptor = entity->get<message_acceptor_component>();
				if (acceptor->valid())
					acceptor->stop();
			}
		}
	}

	void main_window::save()
	{
		if (!m_save_button->isEnabled())
			return;

		// Spam
		m_save_button->setEnabled(false);
		QTimer::singleShot(ui::refresh, [&]() { set_enabled(m_save_button); });

		m_app.save();
	}

	void main_window::restart()
	{
		if (!m_update_button->isEnabled())
			return;

		// Spam
		m_update_button->setEnabled(false);
		QTimer::singleShot(ui::refresh, [&]() { set_enabled(m_update_button); });

		const auto action = get_action();
		const auto entity = action->get_entity();
		restart(entity);
	}

	void main_window::restart(const entity::ptr entity)
	{
		assert(entity);

		if (is_client(entity))
		{
			const auto service = entity->get<client_service_component>();
			if (service->valid())
				service->restart();
		}
		else
		{
			const auto acceptor = entity->get<message_acceptor_component>();
			if (acceptor->valid())
				acceptor->restart();
		}

		set_modified(false);
	}

	void main_window::toggle()
	{
		const auto action = get_action();
		const auto entity = action->get_entity();
		toggle(entity);
	}

	void main_window::toggle(const entity::ptr entity)
	{
		if (is_client(entity))
		{
			if (entity->enabled()) 
			{				
				const auto service = entity->get<client_service_component>();
				if (service->valid())
				{
					service->async_quit();

					entity->disable();
				}
			}
			else
			{
				const auto service = entity->get<client_service_component>();
				if (service->invalid())
				{
					entity->enable();

					service->start();
				}
			}
		}
		else
		{			
			if (entity->enabled())
			{
				const auto acceptor = entity->get<message_acceptor_component>();
				if (acceptor->valid())
				{
					acceptor->stop();

					entity->disable();
				}					
			}
			else
			{
				const auto acceptor = entity->get<message_acceptor_component>();
				if (acceptor->invalid())
				{
					entity->enable();

					acceptor->start();
				}
			}
		}

		set_modified(false);
	}

	// Utility	
	void main_window::flash(const size_t ms /*= 0)*/) const
	{
		auto widget = const_cast<QWidget*>(qobject_cast<const QWidget*>(this));
		QApplication::alert(widget, ms);
	}

	font_action* main_window::make_action(const int font, const char* tooltip /*= ""*/)
	{
		auto action = new font_action(font, this);
		action->setObjectName("titlebar");
		action->setToolTip(tooltip);

		return action;
	}

	QAction* main_window::make_action(const char* text, const int font, const char* tooltip /*= nullptr*/, const size_t size /*= 14*/)
	{
		auto action = new QAction(text, this);
		const auto icon = icon_provider::get_icon(size, font);
		action->setIcon(icon);
		
		if (tooltip)
			action->setToolTip(tooltip);

		return action;
	}

	QAction* main_window::make_action(const char* text, const char* icon, const char* tooltip /*= nullptr*/)
	{
		auto action = new QAction(text, this);
		action->setIcon(QIcon(icon));
		
		if (tooltip)
			action->setToolTip(tooltip);

		return action;
	}

	font_button* main_window::make_button(const int font, const char* tooltip)
	{
		auto button = new font_button(font, this);
		button->setObjectName("titlebar");
		button->setFont(QFont(demonsaw::font_awesome));;
		button->setToolTip(tooltip);

		return button;
	}

	// Is
	bool main_window::is_router(const std::shared_ptr<entity> entity) const
	{
		assert(entity);

		const auto& routers = get_routers();
		return std::find(routers->cbegin(), routers->cend(), entity) != routers->cend();
	}

	// Event
	void main_window::closeEvent(QCloseEvent* event)
	{		
		const auto& clients = get_clients();

		for (const auto& entity : clients)
		{
			const auto upload_map = entity->get<upload_map_component>();
			if (!upload_map->empty())
			{
				const QString message = "You are uploading files. Do you really want to quit?";
				const auto button = QMessageBox::question(this, "Verification", message, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
				if (button == QMessageBox::No)
				{
					event->ignore();
					return;
				}

				break;
			}
		}

		hide();

		shutdown();

		// Wait (for quit)
		std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds::quit));

		QApplication::quit();
	}

	bool main_window::eventFilter(QObject* object, QEvent* event)
	{
		const auto button = qobject_cast<QAbstractButton* const>(object);

		if (button)
		{
			if (event->type() == QEvent::MouseButtonDblClick)
			{
				if (likely(button->actions().size()))
				{
					const auto action = static_cast<entity_action*>(button->actions().front());
					const auto entity = action->get_entity();
					toggle(entity);
				}				
			}
		}
		else
		{
			if (event->type() == QEvent::ContextMenu)
			{
				QContextMenuEvent* menu_event = static_cast<QContextMenuEvent *>(event);
				return menu_event->reason() == QContextMenuEvent::Mouse;
			}
		}

		return QObject::eventFilter(object, event);
	}

	void main_window::resizeEvent(QResizeEvent* event)
	{
		QMainWindow::resizeEvent(event);

		// Window
		/*const auto window = get_window();
		if (window)
			window->update();*/
	}

	// Slot
	void main_window::notify(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto window = get_window(entity);
		if (window)
		{
			const auto action = window->get_action();
			if (action)
				action->set_color();
		}
	}

	void main_window::on_show_menu(const QPoint& point)
	{
		assert(thread_info::main());

		// Button
		const auto button = qobject_cast<QAbstractButton*>(m_toolbar->childAt(point));
		if (!button)
			return;

		if (unlikely(button->actions().empty()))
			return;

		// Action
		emit m_button_group->buttonClicked(button);
		auto action = static_cast<entity_action*>(button->actions().front());
		action->setChecked(true);

		// Entity
		QMenu menu(this);
		QFont font(demonsaw::font_awesome);
		menu.setFont(font);

		menu.addAction(m_add_action);
		menu.addAction(m_remove_action);

		const auto entity = action->get_entity();

		if (entity->enabled())
		{
			if (is_client(entity))
			{
				const auto service = entity->get<client_service_component>();
				if (service->valid())
					menu.addAction(m_update_action);
			}
			else
			{
				const auto acceptor = entity->get<message_acceptor_component>();
				if (acceptor->valid())
					menu.addAction(m_update_action);
			}
		}

		menu.exec(m_toolbar->mapToGlobal(point));

	}

	void main_window::on_select_entity(QAbstractButton* button)
	{
		assert(thread_info::main());

		if (unlikely(!button || button->actions().empty()))
			return;

		// Action
		auto action = static_cast<entity_action*>(button->actions().front());
		const auto variant = action->data();
		entity_window* window = variant.value<entity_window*>();

		if (window)
		{
			m_entity_stack->setCurrentWidget(window);
			window->show();

			// Menu
			const auto toolbar = window->get_toolbar();
			m_menu_stack->setCurrentWidget(toolbar);
			const auto index = m_menu_stack->currentIndex();
			toolbar->show();
		}
	}

	void main_window::on_set_status(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto window = get_window(entity);
		if (window)
		{
			const auto action = window->get_action();
			if (action)
			{
				const auto status = entity->get<status_component>();
				const auto color = status->get_color();
				action->set_color(color);
			}
		}
	}

	// Get
	const entity_action* main_window::get_action() const
	{
		// Button
		const auto button = m_button_group->checkedButton();
		if (unlikely(!button || button->actions().empty()))
			return nullptr;

		return static_cast<entity_action*>(button->actions().front());
	}

	const entity_window* main_window::get_window() const
	{
		return qobject_cast<entity_window*>(m_entity_stack->currentWidget());
	}

	const entity_window* main_window::get_window(const entity::ptr entity) const
	{
		for (int i = 0; i < m_entity_stack->count(); ++i)
		{
			const auto window = qobject_cast<entity_window*>(m_entity_stack->widget(i));
			if (window && (entity == window->get_entity()))
				return window;
		}

		return nullptr;
	}

	const entity::ptr main_window::get_entity() const
	{
		const auto window = qobject_cast<entity_window*>(m_entity_stack->currentWidget());
		return window ? window->get_entity() : nullptr;
	}
}