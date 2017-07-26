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

#include <memory>
#include <string>
#include <boost/format.hpp>

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QGridLayout>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QPixmap>
#include <QStackedWidget>
#include <QStandardPaths>
#include <QStatusBar>
#include <QTimer>
#include <QToolButton>

#include "main_window.h"
#include "entity/entity_action.h"
#include "command/client/request/client_quit_request_command.h"
#include "component/client/client_component.h"
#include "component/client/client_machine_component.h"
#include "component/client/client_option_component.h"
#include "component/group/group_component.h"
#include "component/session_component.h"
#include "component/error/error_component.h"
#include "component/router/router_component.h"
#include "component/router/router_machine_component.h"
#include "component/router/router_option_component.h"
#include "component/server/server_component.h"
#include "component/transfer/chunk_component.h"
#include "component/transfer/upload_component.h"
#include "component/transfer/upload_thread_component.h"
#include "dialog/about_dialog.h"
#include "entity/entity.h"
#include "resource/resource.h"
#include "utility/default_value.h"
#include "utility/client/client_util.h"
#include "utility/router/router_util.h"
#include "window/client_window.h"
#include "window/router_window.h"

namespace eja
{
	// Static
	main_window::ptr main_window::s_ptr = nullptr;

	// Constructor
	status_pane::status_pane(QWidget* parent /*= 0*/)
	{
		// Image
		m_image = new QLabel(parent);
		m_image->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		m_image->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
		m_image->setObjectName("info_image");
		//m_image->setFixedSize(QSize(20, 20));

		// Text
		m_text = new QLabel(parent);
		m_text->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		m_text->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
		m_text->setObjectName("info_text");
		m_text->setMinimumWidth(36);
	}

	main_window::main_window(QWidget *parent) : QMainWindow(parent)
	{
		create_actions();
		create_menus();		
		create_toolbars();
		create_statusbar();
		create_layouts();
		create_slots();

		// Window
		QSize size(1280, 720);
		setWindowIcon(QIcon(resource::icon));
		setWindowTitle(default_value::titlebar);
		adjustSize();
		resize(size);
	}

	// Interface
	void main_window::init()
	{
		// Window
		m_entity_stack = new QStackedWidget(this);
		setCentralWidget(m_entity_stack);

		// Router
		const auto& routers = get_routers();
		auto_lock_ref(routers);

		for (const auto& entity : routers)
			init(entity);

		// Client
		const auto& clients = get_clients();
		auto_lock_ref(clients);

		for (const auto& entity : clients)
			init(entity);
	}

	void main_window::init(const entity::ptr entity)
	{
		const bool checked = !m_entity_stack->count();

		if (entity->has<client_component>())
		{
			// Client
			const auto client = entity->get<client_component>();

			// Action
			entity_action* action = new entity_action(m_entity_stack);
			const auto& name = client->get_name();
			const auto qname = QString::fromStdString(name);

			action->setText(qname);
			action->setIconText(qname);
			action->setIcon(QIcon(resource::client::none));
			action->setCheckable(true);
			action->setChecked(checked);

			// Window
			client_window* cw = new client_window(entity, m_entity_stack, m_statusbar_stack, action);
			cw->show_options(m_dashboard_action->isChecked());
			cw->show_details(m_activity_action->isChecked());

			m_entity_stack->addWidget(cw);
			m_statusbar_stack->addWidget(cw->get_statusbar());

			// HACK: to get statusbar/tab signals working
			//
			cw->post_create();

			// Data
			action->set_entity(entity);
			QVariant variant = QVariant::fromValue(cw);
			action->setData(variant);

			// Toolbar
			m_entity_group->addAction(action);
			m_toolbar->addAction(action);

			if (checked)
				show_entity(action);

			// Callback
			const auto lambda = [this](const entity::ptr entity) { return update(entity); };
			const auto function = function::create(lambda);

			const auto machine = entity->get<client_machine_component>();
			machine->add(function);
		}
		else
		{
			// Router
			const auto router = entity->get<router_component>();

			// Action
			entity_action* action = new entity_action(m_entity_stack);
			const auto& name = router->get_name();
			const auto qname = QString::fromStdString(name);

			action->setText(qname);
			action->setIconText(qname);
			action->setIcon(QIcon(resource::router::none));
			action->setCheckable(true);
			action->setChecked(checked);

			// Window			
			router_window* rw = new router_window(entity, m_entity_stack, m_statusbar_stack, action);
			rw->show_options(m_dashboard_action->isChecked());
			rw->show_details(m_activity_action->isChecked());

			m_entity_stack->addWidget(rw);
			m_statusbar_stack->addWidget(rw->get_statusbar());

			// HACK: to get statusbar/tab signals working
			//
			rw->post_create();

			// Data
			action->set_entity(entity);
			QVariant variant = QVariant::fromValue(rw);
			action->setData(variant);

			// Toolbar
			m_entity_group->addAction(action);
			m_toolbar->addAction(action);

			if (checked)
				show_entity(action);

			// Callback
			const auto lambda = [this](const entity::ptr entity) { return update(entity); };
			const auto function = function::create(lambda);

			const auto machine = entity->get<router_machine_component>();
			machine->add(function);
		}
	}

	void main_window::shutdown()
	{
		// View
		auto& config = get_config();
		config.set_detail(m_activity_action->isChecked());
		config.set_option(m_dashboard_action->isChecked());
		config.set_menubar(m_menubar_action->isChecked());
		config.set_statusbar(m_statusbar_action->isChecked());
		config.set_toolbar(m_toolbar_action->isChecked());

		// App
		const auto app = get_app();
		app->shutdown();
	}

	void main_window::update(const entity::ptr entity)
	{
		emit on_update_status(entity);
	}

	void main_window::add()
	{
		// TODO: Add wizard here
		//
		//
		add_client();
	}

	void main_window::add_client()
	{
		// Entity
		const auto entity = client_util::create();

		// Client
		const auto client = entity->get<client_component>();
		client->set_name(default_client::name);

		const auto router = entity->get<router_component>();
		router->set_address(default_network::address);

		// Option
		const auto option = entity->get<client_option_component>();
		const auto path = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
		option->set_download_path(path.toStdString());

		// Routers
		auto& clients = get_clients();
		clients.add(entity);

		init(entity);

		// Window
		m_entity_stack->setCurrentIndex(m_entity_stack->count() - 1);		
		const auto action = m_entity_group->actions()[m_entity_group->actions().size() - 1];
		action->setChecked(true);

		// Statusbar
		m_statusbar_stack->setCurrentIndex(m_statusbar_stack->count() - 1);

		try
		{
			save();
		}
		catch (const std::exception& ex)
		{
			qDebug() << ex.what();
		}
	}

	void main_window::add_router()
	{
		// Entity
		const auto entity = router_util::create();

		// Router
		const auto router = entity->get<router_component>();
		router->set_name(default_router::name);

		// List
		entity->add<session_entity_list_component>();
		entity->add<client_entity_vector_component>();
		entity->add<group_entity_vector_component>();
		entity->add<chunk_entity_list_component>();

		// Routers
		auto& routers = get_routers();
		routers.add(entity);

		init(entity);

		// Window
		m_entity_stack->setCurrentIndex(m_entity_stack->count() - 1);		
		const auto action = m_entity_group->actions()[m_entity_group->actions().size() - 1];
		action->setChecked(true);

		// Statusbar
		m_statusbar_stack->setCurrentIndex(m_statusbar_stack->count() - 1);

		try
		{
			save();
		}
		catch (const std::exception& ex)
		{
			qDebug() << ex.what();
		}
	}

	void main_window::remove()
	{
		// Window
		const auto window = qobject_cast<entity_window*>(m_entity_stack->currentWidget());
		if (!window)
			return;

		// Entity
		const auto entity = window->get_value();
		const auto& name = entity->has<client_component>() ? entity->get<client_component>()->get_name() : entity->get<router_component>()->get_name();
		const QString message = QString("Are you sure you want to remove '%1'?").arg(name.c_str());
		const auto button = QMessageBox::question(this, "Remove Confirmation", message, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (button == QMessageBox::No)
			return;

		// Action
		const auto action = window->get_action();
		assert(action);

		m_entity_group->removeAction(action);
		m_toolbar->removeAction(action);
		delete action;

		// Window
		m_entity_stack->removeWidget(window);		

		if (m_entity_stack->count() > 0)
		{
			m_entity_stack->setCurrentIndex(0);
			
			assert(m_entity_group->actions().size());
			const auto action = m_entity_group->actions()[0];
			action->setChecked(true);
		}

		// Statusbar
		const auto statusbar = window->get_statusbar();
		m_statusbar_stack->removeWidget(statusbar);

		if (m_statusbar_stack->count() > 0)
			m_statusbar_stack->setCurrentIndex(0);

		if (entity->has<client_component>())
		{
			auto& clients = get_clients();
			clients.remove(entity);

			// Thread
			std::thread thread([this, entity]()
			{
				const auto machine = entity->get<client_machine_component>();
				if (machine)
				{
					if (machine->success())
					{
						const auto socket = http_socket::create();

						try
						{
							const auto router = entity->get<router_component>();
							if (router)
							{						
								// Socket
								const auto option = entity->get<client_option_component>();
								if (option)
									socket->set_timeout(option->get_socket_timeout());
								
								socket->open(router->get_address(), router->get_port());

								// Quit
								const auto request_command = client_quit_request_command::create(entity, socket);
								request_command->execute();
							}
						}
						catch (...) {}

						socket->close();
					}
					
					// Machine
					machine->stop();
				}

				// Entity
				entity->shutdown();
			});

			thread.detach();
		}
		else
		{
			auto& routers = get_routers();
			routers.remove(entity);

			// Machine
			const auto machine = entity->get<router_machine_component>();
			if (machine)
				machine->stop();
			
			// Entity
			entity->shutdown();
		}

		delete window;
	}

	void main_window::save()
	{
		// Cursor
		m_save_button->setEnabled(false);
		QApplication::setOverrideCursor(Qt::WaitCursor);

		QTimer::singleShot(default_timeout::client::refresh, [this]()
		{
			m_save_button->setEnabled(true);
			QApplication::restoreOverrideCursor();
		});

		// View
		auto& config = get_config();
		config.set_detail(m_activity_action->isChecked());
		config.set_option(m_dashboard_action->isChecked());
		config.set_menubar(m_menubar_action->isChecked());
		config.set_statusbar(m_statusbar_action->isChecked());
		config.set_toolbar(m_toolbar_action->isChecked());

		// App
		const auto app = get_app();
		app->save();
	}

	void main_window::restart()
	{
		// Window
		const auto window = qobject_cast<entity_window*>(m_entity_stack->currentWidget());
		if (!window)
			return;

		// Entity		
		const auto entity = window->get_value();
		if (!entity)
			return;

		// Component
		if (entity->has<client_component>())
		{
			const auto option = entity->get<client_option_component>();
			if (!option || option->disabled())
				return;

			// Thread
			std::thread thread([this, entity, option]()
			{
				const auto machine = entity->get<client_machine_component>();
				if (machine)
				{
					if (machine->success())
					{
						const auto socket = http_socket::create();

						try
						{
							const auto router = entity->get<router_component>();
							if (router)
							{
								// Socket
								socket->set_timeout(option->get_socket_timeout());
								socket->open(router->get_address(), router->get_port());

								// Quit
								const auto request_command = client_quit_request_command::create(entity, socket);
								request_command->execute();
							}
						}
						catch (...) {}

						socket->close();
					}

					// TODO: 3.0: Do not base a group refresh on an empty vector
					//
					//
					// Clients
					/*const auto client_vector = entity->get<client_entity_vector_component>();
					if (client_vector)
						client_vector->clear();*/

					// Machine
					machine->restart();
				}
			});

			thread.detach();

			// TODO: 3.0 should only clear out removed things upon restart
			//
			//
			// Function
			entity->call(function_type::client, function_action::clear, entity);
			entity->call(function_type::search, function_action::clear, entity);
			entity->call(function_type::browse, function_action::clear, entity);
			entity->call(function_type::group, function_action::clear, entity);
			entity->call(function_type::share, function_action::clear, entity);
			//entity->call(function_type::chat, function_action::clear, entity);
			entity->call(function_type::message, function_action::clear, entity);
		}
		else
		{
			const auto option = entity->get<router_option_component>();
			if (!option || option->disabled())
				return;

			// Function
			entity->call(function_type::client, function_action::clear, entity);
			entity->call(function_type::transfer, function_action::clear, entity);
			entity->call(function_type::group, function_action::clear, entity);

			// Machine
			const auto machine = entity->get<router_machine_component>();
			if (machine)
				machine->restart();
		}

		// Cursor
		m_restart_button->setEnabled(false);
		QApplication::setOverrideCursor(Qt::WaitCursor);

		QTimer::singleShot(default_timeout::client::refresh, [this]()
		{
			m_restart_button->setEnabled(true);
			QApplication::restoreOverrideCursor();
		});

		setWindowModified(false);
	}

	// Utility
	void main_window::center()
	{
		QDesktopWidget* desktop = QApplication::desktop();
		move(QApplication::desktop()->screen()->rect().center() - rect().center());
	}

	void main_window::flash(const size_t ms /*= 0)*/) const
	{
		auto widget = const_cast<QWidget*>(qobject_cast<const QWidget*>(this));
		QApplication::alert(widget, ms);
	}

	// Create
	void main_window::create_actions()
	{
		// File
		m_add_client_action = new QAction(tr("&Client"), this);
		m_add_client_action->setIcon(QIcon(resource::tab::client));

		m_add_router_action = new QAction(tr("&Router"), this);
		m_add_router_action->setIcon(QIcon(resource::tab::router));

		m_remove_action = new QAction(tr("&Remove"), this);
		m_remove_action->setIcon(QIcon(resource::menu::remove));

		m_save_action = new QAction(tr("&Save"), this);
		m_save_action->setIcon(QIcon(resource::menu::save));

		m_quit_action = new QAction(tr("&Exit"), this);
		m_quit_action->setIcon(QIcon(resource::menu::exit));

		// Edit
		m_restart_action = new QAction(tr("&Restart"), this);
		m_restart_action->setIcon(QIcon(resource::menu::restart));

		// View
		const auto& config = get_config();
		m_activity_action = new QAction(tr("&Activity"), this);
		m_activity_action->setCheckable(true);
		m_activity_action->setChecked(config.get_detail());

		m_dashboard_action = new QAction(tr("&Dashboard"), this);
		m_dashboard_action->setCheckable(true);
		m_dashboard_action->setChecked(config.get_option());

		m_menubar_action = new QAction(tr("&Menubar"), this);
		m_menubar_action->setCheckable(true);
		m_menubar_action->setChecked(config.get_menubar());

		m_statusbar_action = new QAction(tr("&Statusbar"), this);
		m_statusbar_action->setCheckable(true);
		m_statusbar_action->setChecked(config.get_statusbar());

		m_toolbar_action = new QAction(tr("&Toolbar"), this);
		m_toolbar_action->setCheckable(true);
		m_toolbar_action->setChecked(config.get_toolbar());

		// Help
		m_online_action = new QAction(tr("&Online"), this);
		m_online_action->setIcon(QIcon(resource::icon));

		m_about_action = new QAction(tr("&About"), this);
		m_about_action->setIcon(QIcon(resource::menu::about));
	}

	void main_window::create_menus()
	{
		// Menu
		const auto menu = menuBar();
		menu->installEventFilter(this);

		// File
		m_file_menu = menu->addMenu(tr("&File"));

		m_add_menu = m_file_menu->addMenu(tr("&Add"));
		m_add_menu->setIcon(QIcon(resource::menu::add));
		m_add_menu->addAction(m_add_client_action);
		m_add_menu->addAction(m_add_router_action);
		m_file_menu->addAction(m_remove_action);
		m_file_menu->addAction(m_restart_action);
		m_file_menu->addSeparator();
		m_file_menu->addAction(m_save_action);
		m_file_menu->addSeparator();
		m_file_menu->addAction(m_quit_action);

		// View
		m_view_menu = menu->addMenu(tr("&View"));
		m_view_menu->addAction(m_activity_action);
		m_view_menu->addAction(m_dashboard_action);
		m_view_menu->addAction(m_menubar_action);
		m_view_menu->addAction(m_statusbar_action);
		m_view_menu->addAction(m_toolbar_action);

		// Help
		m_help_menu = menu->addMenu(tr("&Help"));
		m_help_menu->addAction(m_online_action);
		m_help_menu->addAction(m_about_action);
	}

	void main_window::create_toolbars()
	{
		// Toolbar
		m_menubar = addToolBar(tr(""));
		m_menubar->setVisible(m_menubar_action->isChecked());
		m_menubar->installEventFilter(this);
		m_menubar->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);
		m_menubar->setIconSize(QSize(16, 16));
		m_menubar->setFloatable(false);

		m_toolbar = addToolBar(tr(""));
		m_toolbar->setVisible(m_toolbar_action->isChecked());
		m_toolbar->installEventFilter(this);
		m_toolbar->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextUnderIcon);
		m_toolbar->setIconSize(QSize(28, 28));
		m_toolbar->setFloatable(false);

		/*QWidget* empty = new QWidget();
		empty->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		m_toolbar->addWidget(empty);*/

		// Group
		m_entity_group = new QActionGroup(this);
		m_entity_group->setExclusive(true);

		// Client
		QSize size(16, 16);
		m_add_button = new QToolButton(this);
		m_add_button->setObjectName("grid");
		m_add_button->setIcon(QIcon(resource::menu::add));
		m_add_button->setIconSize(size);
		m_add_button->setToolTip("Add");
		m_add_button->setCursor(Qt::ArrowCursor);

		// Router
		m_remove_button = new QToolButton(this);
		m_remove_button->setObjectName("grid_bottom");
		m_remove_button->setIcon(QIcon(resource::menu::remove));
		m_remove_button->setIconSize(size);
		m_remove_button->setToolTip("Remove");
		m_remove_button->setCursor(Qt::ArrowCursor);

		// Save
		m_save_button = new QToolButton(this);
		m_save_button->setObjectName("grid");
		m_save_button->setIcon(QIcon(resource::menu::save));
		m_save_button->setIconSize(size);
		m_save_button->setToolTip("Save");
		m_save_button->setCursor(Qt::ArrowCursor);

		// Restart
		m_restart_button = new QToolButton(this);
		m_restart_button->setObjectName("grid_bottom");
		m_restart_button->setIcon(QIcon(resource::menu::restart));
		m_restart_button->setIconSize(size);
		m_restart_button->setToolTip("Restart");
		m_restart_button->setCursor(Qt::ArrowCursor);
	}

	void main_window::create_statusbar()
	{
		m_statusbar = statusBar();
		m_statusbar_stack = new QStackedWidget(m_statusbar);

		// Legend
		QWidget* widget = new QWidget;
		widget->setObjectName("info_status");

		QImage images[] =
		{
			QImage(resource::status::pending),
			QImage(resource::status::info),
			QImage(resource::status::success),
			QImage(resource::status::warning),
			QImage(resource::status::error),
			QImage(resource::status::cancelled),
			QImage(resource::status::none)
		};

		QString tooltips[] = { "Waiting", "Connecting", "Transferring", "Warning", "Error", "Timeout", "Finished" };

		QHBoxLayout* hlayout = new QHBoxLayout;
		hlayout->setContentsMargins(0, 0, 0, 0);
		hlayout->setSpacing(0);
		hlayout->setMargin(0);

		for (size_t i = 0; i < (sizeof(images) / sizeof(QImage)); ++i)
		{
			QLabel* label = new QLabel(this);
			label->setObjectName("info_status");
			label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
			label->setAlignment(Qt::AlignCenter);
			label->setPixmap(QPixmap::fromImage(images[i]));
			label->setToolTip(tooltips[i]);
			hlayout->addWidget(label);
		}

		hlayout->addSpacing(1);
		widget->setLayout(hlayout);
		m_statusbar->addWidget(widget);

		// Stack
		m_statusbar->addPermanentWidget(m_statusbar_stack);
		m_statusbar->setVisible(m_statusbar_action->isChecked());
	}

	void main_window::create_layouts()
	{
		// Toolbar
		QGridLayout* glayout = new QGridLayout;
		glayout->setSpacing(0);
		glayout->setMargin(0);

		glayout->addWidget(m_add_button, 0, 0);
		glayout->addWidget(m_remove_button, 1, 0);
		glayout->addWidget(m_save_button, 0, 1);
		glayout->addWidget(m_restart_button, 1, 1);

		QWidget* widget = new QWidget(this);
		widget->setObjectName("grid");
		widget->setMaximumSize(QSize(64, 64));
		widget->setLayout(glayout);
		m_menubar->addWidget(widget);
	}

	void main_window::create_slots()
	{
		// Actions
		connect(m_save_action, &QAction::triggered, this, &main_window::save);
		connect(m_quit_action, &QAction::triggered, this, &main_window::close);

		//connect(m_add_action, &QAction::triggered, this, &main_widget::add);
		connect(m_add_client_action, &QAction::triggered, this, &main_window::add_client);
		connect(m_add_router_action, &QAction::triggered, this, &main_window::add_router);
		connect(m_remove_action, &QAction::triggered, this, &main_window::remove);
		connect(m_restart_action, &QAction::triggered, this, &main_window::restart);

		connect(m_activity_action, &QAction::toggled, this, &main_window::show_details);
		connect(m_dashboard_action, &QAction::toggled, this, &main_window::show_options);
		connect(m_menubar_action, &QAction::toggled, this, &main_window::show_menubar);
		connect(m_statusbar_action, &QAction::toggled, this, &main_window::show_statusbar);
		connect(m_toolbar_action, &QAction::toggled, this, &main_window::show_toolbar);
		connect(m_online_action, &QAction::triggered, this, &main_window::show_help);
		connect(m_about_action, &QAction::triggered, this, &main_window::show_about);

		// Toolbar
		connect(m_add_button, &QToolButton::clicked, this, &main_window::add);
		connect(m_remove_button, &QToolButton::clicked, this, &main_window::remove);
		connect(m_restart_button, &QToolButton::clicked, this, &main_window::restart);
		connect(m_save_button, &QToolButton::clicked, this, &main_window::save);

		connect(m_entity_group, &QActionGroup::triggered, this, &main_window::show_entity);

		// Entity
		connect(this, &main_window::on_update_status, this, &main_window::update_status);
	}

	// Event
	void main_window::closeEvent(QCloseEvent* event)
	{
		// Verify
		const auto& clients = get_clients();
		auto_lock_ref(clients);

		for (const auto& entity : clients)
		{
			const auto upload_vector = entity->get<upload_entity_vector_component>();
			if (!upload_vector->empty())
			{
				const QString message = "You are uploading files. Do you really want to quit?";
				const auto button = QMessageBox::question(this, "Quit Verification", message, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
				if (button == QMessageBox::No)
				{
					event->ignore();
					return;
				}

				break;
			}
		}

		shutdown();

		QApplication::quit();
	}

	bool main_window::eventFilter(QObject* object, QEvent* event)
	{
		if (event->type() == QEvent::ContextMenu)
		{
			QContextMenuEvent* mevent = static_cast<QContextMenuEvent *>(event);
			if (mevent->reason() == QContextMenuEvent::Mouse)
			{
				qDebug("I have blocked the context menu.");
				return true;
			}
			else
				return false;
		}
		else
			return QObject::eventFilter(object, event);
	}

	void main_window::resizeEvent(QResizeEvent* event)
	{
		QMainWindow::resizeEvent(event);
		
		// Window
		const auto window = qobject_cast<entity_window*>(m_entity_stack->currentWidget());
		if (!window)
			return;

		window->update();
	}

	// Slot
	void main_window::show_entity(QAction* action)
	{
		assert(action);

		// Window
		const auto variant = action->data();
		entity_window* window = variant.value<entity_window*>();
		m_entity_stack->setCurrentWidget(window);
		window->show();

		// Statusbar
		const auto statusbar = window->get_statusbar();
		m_statusbar_stack->setCurrentWidget(statusbar);
		const auto index = m_statusbar_stack->currentIndex();
		statusbar->show();
	}

	void main_window::show_options(bool value)
	{
		for (const auto& action : m_entity_group->actions())
		{
			// TODO: Derive from parent class that implements show_option
			//
			const auto variant = action->data();
			entity_window* window = variant.value<entity_window*>();
			window->show_options(value);
		}
	}

	void main_window::show_details(bool value)
	{
		for (const auto& action : m_entity_group->actions())
		{
			// TODO: Derive from parent class that implements show_info
			//
			const auto variant = action->data();
			entity_window* window = variant.value<entity_window*>();
			window->show_details(value);
		}
	}

	void main_window::show_help()
	{
		QDesktopServices::openUrl(QUrl(default_value::website));
	}

	void main_window::show_about()
	{
		about_dialog about(this);
		about.exec();
	}

	void main_window::update_status(entity::ptr entity)
	{
		for (QAction* action : m_entity_group->actions())
		{
			const auto variant = action->data();
			entity_window* window = variant.value<entity_window*>();
			const auto value = window->get_value();

			if (value == entity)
			{
				if (entity->has<client_component>())
				{
					const auto machine = entity->get<client_machine_component>();
					const auto status = machine->get_status();

					switch (status)
					{
						case eja::status::success:
						{
							action->setIcon(QIcon(resource::client::success));
							break;
						}
						case eja::status::warning:
						{
							action->setIcon(QIcon(resource::client::warning));
							break;
						}
						case eja::status::error:
						{
							action->setIcon(QIcon(resource::client::error));
							break;
						}
						case eja::status::pending:
						{
							action->setIcon(QIcon(resource::client::pending));
							break;
						}
						case eja::status::info:
						{
							action->setIcon(QIcon(resource::client::info));
							break;
						}
						case eja::status::none:
						{
							action->setIcon(QIcon(resource::client::none));
							break;
						}
						default:
						{
							assert(false);
						}
					}
				}
				else
				{
					const auto machine = entity->get<router_machine_component>();
					const auto status = machine->get_status();

					switch (status)
					{
						case eja::status::success:
						{
							action->setIcon(QIcon(resource::router::success));
							break;
						}
						case eja::status::pending:
						{
							action->setIcon(QIcon(resource::router::pending));
							break;
						}
						case eja::status::warning:
						{
							action->setIcon(QIcon(resource::router::warning));
							break;
						}
						case eja::status::error:
						{
							action->setIcon(QIcon(resource::router::error));
							break;
						}
						case eja::status::info:
						{
							action->setIcon(QIcon(resource::router::info));
							break;
						}
						case eja::status::none:
						{
							action->setIcon(QIcon(resource::router::none));
							break;
						}
						default:
						{
							assert(false);
						}
					}
				}

				break;
			}
		}
	}

	// Accessor
	entity_window* main_window::get_entity_window(const entity::ptr entity) const
	{
		for (int i = 0; i < m_entity_stack->count(); ++i)
		{
			const auto window = qobject_cast<entity_window*>(m_entity_stack->widget(i));
			if (window && (entity == window->get_value()))
				return window;
		}		

		return nullptr;
	}
}

