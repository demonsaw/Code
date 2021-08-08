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

#include <QAction>
#include <QByteArray>
#include <QCloseEvent>
#include <QContextMenuEvent>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QMouseEvent>
#include <QKeySequence>
#include <QPushButton>
#include <QShortcut>
#include <QStackedWidget>
#include <QStyle>
#include <QTabBar>
#include <QTimer>
#include <QToolBar>

#include "component/chat_component.h"
#include "component/pixmap_component.h"
#include "component/callback/callback.h"
#include "component/callback/callback_service_component.h"
#include "component/client/client_component.h"
#include "component/client/client_service_component.h"
#include "component/group/group_component.h"
#include "component/router/router_component.h"
#include "component/status/status_component.h"
#include "component/transfer/download_component.h"
#include "component/transfer/upload_component.h"

#include "entity/entity.h"
#include "factory/client_factory.h"
#include "font/font_action.h"
#include "font/font_button.h"
#include "http/http_throttle.h"
#include "object/function.h"
#include "resource/resource.h"
#include "security/filter/base.h"
#include "system/type.h"
#include "thread/thread_info.h"
#include "resource/icon_provider.h"
#include "resource/resource.h"
#include "utility/value.h"
#include "utility/io/file_util.h"
#include "widget/emoji_widget.h"
#include "widget/global_widget.h"
#include "widget/wizard_widget.h"
#include "widget/tab_widget.h"
#include "window/client_window.h"
#include "window/main_window.h"

Q_DECLARE_METATYPE(eja::entity_window*);

namespace eja
{
	// Static
	main_window::ptr main_window::s_ptr = nullptr;

	// Constructor
	main_window::main_window(qt_application& app, QWidget* parent /*= nullptr*/) : QMainWindow(parent), m_app(app)
	{
		// Singleton
		assert(!s_ptr);
		s_ptr = this;

		// Window
		setFocusPolicy(Qt::StrongFocus);
		setMinimumWidth(296);
		setToolTipDuration(0);
		setWindowIcon(QIcon(resource::icon));
		setWindowTitle(software::titlebar);

		// Font
		auto font = QApplication::font();
		font.setFamily(font_family::main);
		setFont(font);

		// UI
		create();
		layout();
		signal();

		// Timer
		m_timer = new QTimer(this);
		m_timer->setInterval(sec_to_ms(1));

		connect(m_timer, &QTimer::timeout, [this]()
		{
			// Upload
			const auto upload_throttle = http_upload_throttle::get();
			upload_throttle->update();

			// Download
			const auto download_throttle = http_download_throttle::get();
			download_throttle->update();
		});

		m_timer->start();
	}

	void main_window::create()
	{
		// Actions
		m_connect_action = make_action(" Connect", fa::sign_in);
		m_disconnect_action = make_action(" Disconnect", fa::sign_out);
		m_reconnect_action = make_action(" Refresh", fa::refresh);

		// Font
		QFont font(font_family::font_awesome);
		font.setPixelSize(resource::get_large_icon_size());

		// Buttons
		m_add_button = make_button(fa::plus, "Add User");
		m_add_button->setObjectName("plus");
		m_add_button->setFont(font);
		m_add_button->setCheckable(true);

		m_settings_button = make_button(fa::cog, "Global Options");
		m_settings_button->setObjectName("settings");
		m_settings_button->setFont(font);
		m_settings_button->setCheckable(true);

		// HACK
		auto style = boost::str(boost::format("QPushButton#plus, QPushButton#settings { height: %spx; width: %spx; }") % static_cast<size_t>(18 * resource::get_ratio()) % static_cast<size_t>(24 * resource::get_ratio()));
		m_add_button->setStyleSheet(style.c_str());
		m_settings_button->setStyleSheet(style.c_str());

		// Shortcuts
		m_refresh_shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_R), this);
		m_remove_shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_W), this);
		m_switch_shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Tab), this);

		// Tab Bar
		m_tabbar = new QTabBar(this);
		m_tabbar->setMovable(true);
		m_tabbar->setObjectName("bar");
		m_tabbar->setContextMenuPolicy(Qt::CustomContextMenu);
		m_tabbar->setUsesScrollButtons(true);
		m_tabbar->setSelectionBehaviorOnRemove(QTabBar::SelectPreviousTab);
		m_tabbar->setDrawBase(false);
		m_tabbar->installEventFilter(this);

		// HACK
		style = boost::str(boost::format("QTabBar#bar::tab, QTabBar#bar_unselected::tab:selected, QTabBar#bar_unselected::tab { height: %spx; width: %spx; }") % static_cast<size_t>(18 * resource::get_ratio()) % static_cast<size_t>(188 * resource::get_ratio()));
		m_tabbar->setStyleSheet(style.c_str());

		// Toolbar
		m_toolbar = addToolBar("");
		m_toolbar->installEventFilter(this);
		m_toolbar->setObjectName("titlebar");
		m_toolbar->setToolTipDuration(0);
		m_toolbar->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextOnly);
		m_toolbar->setFont(QFont(software::font_awesome));
		m_toolbar->setFloatable(false);
		m_toolbar->setMovable(false);

		m_toolbar->addWidget(m_tabbar);
		m_toolbar->addWidget(m_add_button);

		QWidget* spacer = new QWidget();
		spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		spacer->setObjectName("spacer");
		m_toolbar->addWidget(spacer);

		m_toolbar->addWidget(m_settings_button);

#if _MACX
		// HACK: OSX needs a menu hook to close & save properly (from the system menu)
		m_quit_action = new QAction(tr("&Quit"), this);
		connect(m_quit_action, &QAction::triggered, this, &main_window::close);

		const auto menu = menuBar();
		m_file_menu = menu->addMenu(tr("&File"));
		m_file_menu->addAction(m_quit_action);
#endif
		// Entity
		m_client_stack = new QStackedWidget(this);

		// Widget
		m_emoji_menu = new emoji_widget(this);
		m_wizard = new wizard_widget(this);
		m_settings = new global_widget(this);

		m_client_stack->addWidget(m_wizard);
		m_client_stack->addWidget(m_settings);
	}

	void main_window::layout()
	{
		setCentralWidget(m_client_stack);

		// Size
		const auto percent = 0.70;
		const auto size = QDesktopWidget().availableGeometry(this).size() * percent;

		// Center
		QRect rect(0, 0, size.width(), size.height());
		rect.moveCenter(QApplication::desktop()->availableGeometry().center());
		setGeometry(rect);
	}

	void main_window::signal()
	{
		// Signal
		connect(this, &main_window::set_enabled, [&](font_button* button) { button->setEnabled(true); });

		// Action
		connect(m_connect_action, &font_action::triggered, this, [&]() { start(); });
		connect(m_disconnect_action, &font_action::triggered, this, [&]() { stop(); });
		connect(m_reconnect_action, &font_action::triggered, this, [&]() { restart(); });

		// Button
		connect(m_add_button, &font_button::clicked, this, [&]() { add(); });
		connect(m_settings_button, &font_button::clicked, this, &main_window::show_settings);

		// Shortcuts
		connect(m_refresh_shortcut, &QShortcut::activated, this, [&]() { if(!m_tab_button_active) restart(); });
		connect(m_remove_shortcut, &QShortcut::activated, this, [&]()
		{
			if (m_tabbar->count() && !m_tab_button_active)
				remove(m_tabbar->currentIndex());
		});

		connect(m_switch_shortcut, &QShortcut::activated, this, [&]()
		{
			if (m_tabbar->count() > 1)
				set_entity((m_tabbar->currentIndex() + 1) % m_tabbar->count());
		});

		// Tab
		connect(m_tabbar, &QTabBar::customContextMenuRequested, this, &main_window::on_show_menu);
		connect(m_tabbar, &QTabBar::tabBarClicked, [&](int index) { set_entity(index); });
		connect(m_tabbar, &QTabBar::tabBarDoubleClicked, [&](int index) { toggle(index); });
		connect(m_tabbar, &QTabBar::tabCloseRequested, [&](int index) { remove(index); });

		// Wizard
		connect(m_wizard, &wizard_widget::done, this, &main_window::on_wizard_done);
	}

	void main_window::tray()
	{
		if (QSystemTrayIcon::isSystemTrayAvailable())
		{
			// Action
			QAction* about_action = new QAction(tr("&About Demonsaw"));
			QAction* help_action = new QAction(tr("&Community Wiki"));
			QAction* open_action = new QAction(tr("&Open Demonsaw"));
			QAction* quit_action = new QAction(tr("&Quit Demonsaw"));

			// Menu
			QMenu* menu = new QMenu;
			menu->addAction(open_action);
			menu->addAction(help_action);
			menu->addAction(about_action);
			menu->addSeparator();
			menu->addAction(quit_action);

			// Object
			m_tray = new QSystemTrayIcon(this);
			m_tray->setIcon(QIcon(resource::logo_rgb));
			m_tray->setContextMenu(menu);
			m_tray->setToolTip(software::title);
			m_tray->show();

			// Signal
			connect(m_tray, &QSystemTrayIcon::activated, this, &main_window::on_icon_activated);
			connect(m_tray, &QSystemTrayIcon::messageClicked, this, &main_window::on_message_clicked);

			connect(open_action, &QAction::triggered, this, &main_window::open);
			connect(help_action, &QAction::triggered, this, &main_window::help);
			connect(about_action, &QAction::triggered, this, &main_window::about);
			connect(quit_action, &QAction::triggered, this, &main_window::kill);
		}
	}

	// Interface
	void main_window::init()
	{
		restore();

		// Client
		if (m_app.has_clients())
		{
			const auto& clients = get_clients();
			for (const auto& e : clients)
				init(e);

			// Selected
			const auto selected = m_app.get_active();
			set_entity(selected);

			m_app.init();

			m_add_button->setEnabled(clients->size() < m_app.get_max_size());
		}
		else
		{
			show_tab_bar(false);
			add();
		}

		// Statusbar
		set_statusbar(m_app.is_statusbar());

		// System Tray
		if (m_app.is_system_tray())
			tray();
	}

	void main_window::init(const entity::ptr& entity)
	{
		// Button
		QAction* remove_action = new QAction(this);
		font_button* remove_button = make_button(fa::times, "Remove");
		remove_button->addAction(remove_action);
		remove_button->setObjectName("close_button");

		// Tab
		const auto client = entity->get<client_component>();
		QString client_name = QString::fromStdString(client->get_name());

		tab_widget* tab_content = new tab_widget(entity, m_tabbar);

		const auto index = m_tabbar->addTab("");
		m_tabbar->setTabButton(index, QTabBar::RightSide, remove_button);
		m_tabbar->setTabButton(index, QTabBar::LeftSide, tab_content);
		m_tabbar->setTabToolTip(index, client_name);

		// Signal
		connect(remove_button, &font_button::clicked, this, [&]
		{
			const auto pos = m_tabbar->mapFromGlobal(QCursor::pos());
			const auto index = m_tabbar->tabAt(pos);
			if (index >= 0)
				remove(index);

			if (m_tabbar->count() == 0)
			{
				show_tab_bar(false);
				add();
			}
		});

		// Window
		const auto window = new client_window(entity);
		window->restore();

		const auto variant = QVariant::fromValue(window);
		m_tabbar->setTabData(index, variant);
		m_client_stack->addWidget(window);
	}

	void main_window::shutdown()
	{
		// Clear
		m_app.clear();

		// Re-order
		auto& clients = m_app.get_clients();
		for (size_t i = 0; i < m_tabbar->count(); ++i)
		{
			const auto variant = m_tabbar->tabData(i);
			const auto window = variant.value<client_window*>();
			const auto e = window->get_entity();
			clients.push_back(e);
		}

		save();

		m_app.shutdown();
	}

	void main_window::clear_chat()
	{
		const auto index = m_tabbar->currentIndex();

		if (index != npos)
		{
			const auto tab = qobject_cast<tab_widget*>(m_tabbar->tabButton(index, QTabBar::LeftSide));
			tab->clear_chat();
		}
	}

	void main_window::clear_pm(const entity::ptr& entity)
	{
		const auto owner = entity->get_owner();
		const auto index = get_index(owner);

		if (index != npos)
		{
			const auto tab = qobject_cast<tab_widget*>(m_tabbar->tabButton(index, QTabBar::LeftSide));
			const auto e = tab->get_pm_entity();
			if (e)
				tab->clear_pm(e);
		}
	}

	void main_window::clear_room(const entity::ptr& entity)
	{
		const auto owner = entity->get_owner();
		const auto index = get_index(owner);

		if (index != npos)
		{
			const auto tab = qobject_cast<tab_widget*>(m_tabbar->tabButton(index, QTabBar::LeftSide));
			const auto e = tab->get_room_entity();
			if (e)
				tab->clear_room(e);
		}
	}

	// Utility
	void main_window::about()
	{
		QDesktopServices::openUrl(QUrl(software::website));
	}

	void main_window::help()
	{
		QDesktopServices::openUrl(QUrl(software::help));
	}

	void main_window::kill()
	{
		assert(thread_info::main());

		if (verify())
		{
			hide();

			shutdown();

			// Wait (for quit)
			std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds::quit));

			// NOTE: Give as much time as possible for the async_quit to finish
			//
			for (const auto& e : get_clients())
			{
				// Callback
				const auto callback = e->get<callback_service_component>();
				callback->stop();
			}

			QApplication::quit();
		}
	}

	void main_window::open()
	{
		activateWindow();
		raise();

		if (!isMaximized())
			showNormal();
		else
			showMaximized();
	}

	void main_window::toast(const entity::ptr& entity, const int fa) const
	{
		if (m_tray && m_app.is_system_tray())
		{
			const auto parent = entity->get_owner();
			const auto client = parent->get<client_component>();
			const auto chat = entity->get<chat_component>();

			const auto icon = icon_provider::get_icon(256, fa, client->get_color());
			m_tray->showMessage(QString::fromStdString(client->get_name()), QString::fromStdString(chat->get_text()), icon, sec_to_ms(3));
		}
	}

	// Add
	void main_window::add()
	{
		assert(thread_info::main());

		if (m_tab_button_active)
			update_style(m_settings_button, "settings");

		m_settings_button->setChecked(false);
		m_add_button->setChecked(true);

		m_tab_button_active = true;
		m_client_stack->setCurrentWidget(m_wizard);

		update_style(m_add_button, "plus_active");
		update_style(m_tabbar, "bar_unselected");
		m_wizard->activate();
	}

	void main_window::add(const std::string& name, const std::string& address, const u16 port, const std::string& password /*= ""*/, u32 rgba /*= 0*/, const bool enabled /*= false*/)
	{
		assert(thread_info::main());

		// Client
		const auto client_entity = client_factory::create_client();
		client_entity->set_enabled(enabled);

		const auto client = client_entity->get<client_component>();
		client->set_name(name);

		if (rgba)
			client->set_color(rgba);

		auto& clients = get_clients();
		clients.push_back(client_entity);
		m_add_button->setEnabled(clients->size() < m_app.get_max_size());

		// Group
		const auto group_entity = client_factory::create_group_option(client_entity);
		group_entity->add<pixmap_component>();
		group_entity->disable();

		const auto group_list = client_entity->get<group_list_component>();
		group_list->push_back(group_entity);

		// Router
		const auto router_entity = client_factory::create_router(client_entity);
		const auto router = router_entity->get<router_component>();
		router->set_address(address);
		router->set_port(port);
		router->set_password(password);

		const auto router_name = boost::str(boost::format("%s 1") % router::name);
		router->set_name(router_name);

		const auto router_list = client_entity->get<router_list_component>();
		router_list->push_back(router_entity);

		init(client_entity);

		const auto callback = client_entity->get<callback_service_component>();
		callback->start();

		const auto index = m_tabbar->count() - 1;
		set_entity(index);

		if (enabled)
			start(index);

		if (index == 0)
			show_tab_bar(true);

		if (m_tray && m_app.is_system_tray())
			m_close_alert = true;
	}

	// Remove
	void main_window::remove(const size_t index)
	{
		assert(thread_info::main());

		const auto entity = get_entity(index);
		if (likely(entity))
		{
			// Verify
			const auto client = entity->get<client_component>();
			const auto message = QString("Do you really want to remove \"%1\"?").arg(client->get_name().c_str());
			const auto result = QMessageBox::question(this, software::name, message, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

			if (result == QMessageBox::Yes)
			{
				stop(index);

				// NOTE: We must stop the callback before we delete client_window
				const auto callback = entity->get<callback_service_component>();
				callback->stop();

				// Window
				const auto window = get_window(index);
				if (likely(window))
				{
					// Tab
					m_tabbar->removeTab(index);
					if (m_tabbar->count() > 0)
						set_entity(m_tabbar->currentIndex());

					// Stack
					m_client_stack->removeWidget(window);

					delete window;
				}

				// Entity
				auto& clients = get_clients();
				clients.remove(entity);

				m_add_button->setEnabled(clients->size() < m_app.get_max_size());
			}
		}
	}

	void main_window::remove()
	{
		assert(thread_info::main());

		const auto index = m_tabbar->currentIndex();
		if (index >= 0)
			remove(index);
	}

	// Start
	void main_window::start(const size_t index)
	{
		assert(thread_info::main());

		const auto window = get_window(index);
		if (likely(window))
			window->start();
	}

	void main_window::start()
	{
		assert(thread_info::main());

		const auto index = m_tabbar->currentIndex();
		if (index >= 0)
			start(index);
	}

	// Stop
	void main_window::stop(const size_t index)
	{
		assert(thread_info::main());

		const auto window = get_window(index);
		if (likely(window))
		{
			const auto entity = window->get_entity();
			if (verify(entity))
				window->stop();
		}
	}

	void main_window::stop()
	{
		assert(thread_info::main());

		const auto index = m_tabbar->currentIndex();
		if (index >= 0)
			stop(index);
	}

	// Restart
	void main_window::restart(const size_t index)
	{
		assert(thread_info::main());

		const auto window = get_window(index);
		if (likely(window))
			window->restart();
	}

	void main_window::restart()
	{
		assert(thread_info::main());

		const auto index = m_tabbar->currentIndex();
		if (index >= 0)
			restart(index);
	}

	// Toggle
	void main_window::toggle(const size_t index)
	{
		assert(thread_info::main());

		const auto window = get_window(index);
		if (likely(window))
		{
			const auto entity = window->get_entity();

			if (entity->enabled() && verify(entity))
				window->stop();
			else
				window->start();
		}
	}

	// Utility
	void main_window::restore()
	{
		assert(thread_info::main());

		// Geometry
		if (m_app.has_geometry())
		{
			const auto& geometry = m_app.get_geometry();
			const auto data = base64::decode(geometry);
			QByteArray byteArray(data.c_str(), data.length());
			restoreGeometry(byteArray);
		}

		// State
		if (m_app.has_state())
		{
			// State
			const auto& state = m_app.get_state();
			const auto data = base64::decode(state);
			QByteArray byteArray(data.c_str(), data.length());
			restoreState(byteArray);
		}
	}

	void main_window::save()
	{
		assert(thread_info::main());

		// Active
		m_app.set_active(m_tabbar->currentIndex());

		// Geometry
		auto data = saveGeometry();
		const auto geometry = base64::encode(data.data(), data.size());
		m_app.set_geometry(geometry);

		// State
		data = saveState();
		const auto state = base64::encode(data.data(), data.size());
		m_app.set_state(state);

		for (size_t i = 0; i < m_tabbar->count(); ++i)
		{
			const auto window = get_window(i);
			const auto entity = window->get_entity();
			const auto client = entity->get<client_component>();

			// Geometry
			data = window->saveGeometry();
			const auto geometry = base64::encode(data.data(), data.size());
			client->set_geometry(geometry);

			// State
			data = window->saveState();
			const auto state = base64::encode(data.data(), data.size());
			client->set_state(state);
		}

		// Write
		m_app.write();
		m_app.backup();
	}

	bool main_window::verify()
	{
		for (const auto& e : get_clients())
		{
			const auto download_list = e->get<download_list_component>();
			if (!download_list->empty())
			{
				open();
				const QString message = "You're downloading files.  Do you really want to quit?";
				const auto result = QMessageBox::question(this, software::name, message, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
				return (result == QMessageBox::Yes);
			}

			const auto upload_list = e->get<upload_list_component>();
			if (!upload_list->empty())
			{
				open();
				const QString message = "You're uploading files.  Do you really want to quit?";
				const auto result = QMessageBox::question(this, software::name, message, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
				return (result == QMessageBox::Yes);
			}
		}

		return true;
	}

	bool main_window::verify(const entity::ptr& entity)
	{
		const auto download_list = entity->get<download_list_component>();
		if (!download_list->empty())
		{
			const QString message = "You're downloading files.  Do you really want to disconnect?";
			const auto result = QMessageBox::question(this, software::name, message, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
			return (result == QMessageBox::Yes);
		}

		const auto upload_list = entity->get<upload_list_component>();
		if (!upload_list->empty())
		{
			const QString message = "You're uploading files.  Do you really want to quit?";
			const auto result = QMessageBox::question(this, software::name, message, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
			return (result == QMessageBox::Yes);
		}

		return true;
	}

	bool main_window::verify(const size_t index)
	{
		assert(thread_info::main());

		const auto entity = get_entity(index);
		if (likely(entity))
			return verify(entity);

		return true;
	}

	void main_window::update_color()
	{
		for (const auto e : get_clients())
		{
			const auto window = get_window(e);
			window->update_color();
		}
	}

	void main_window::update_theme()
	{
		for (const auto e : get_clients())
		{
			const auto window = get_window(e);
			window->update_theme();
		}
	}

	void main_window::show_settings()
	{
		if (m_tab_button_active)
			update_style(m_add_button, "plus");

		m_add_button->setChecked(false);
		m_settings_button->setChecked(true);

		m_tab_button_active = true;
		m_client_stack->setCurrentWidget(m_settings);

		update_style(m_tabbar, "bar_unselected");
		update_style(m_settings_button, "settings_active");
	}

	void main_window::update_style(QWidget* widget, const char* selector)
	{
		widget->setObjectName(QString(selector));
		const auto style = widget->style();
		style->unpolish(widget);
		style->polish(widget);
	}

	void main_window::update_tab(const client_window* window)
	{
		const auto entity = window->get_entity();
		if (entity->disabled())
			return;

		if (window->is_chat_visible())
		{
			clear_chat();
		}
		else if (window->is_pm_visible())
		{
			const auto entity = window->get_pm_entity();
			if (entity)
				clear_pm(entity);
		}
		else if (window->is_room_visible())
		{
			const auto entity = window->get_room_entity();
			if (entity)
				clear_room(entity);
		}
	}

	font_action* main_window::make_action(const int fa, const char* tooltip /*= ""*/)
	{
		auto action = new font_action(fa, this);
		action->setObjectName("titlebar");
		action->setToolTip(tooltip);

		return action;
	}

	QAction* main_window::make_action(const char* text, const int fa, const char* tooltip /*= nullptr*/)
	{
		auto action = new QAction(text, this);
		const auto icon = icon_provider::get_icon(resource::get_icon_size(), fa);
		action->setIcon(icon);
		action->setText(text);

		if (tooltip)
			action->setToolTip(tooltip);

		return action;
	}

	QAction* main_window::make_action(const char* text, const char* icon, const char* tooltip /*= nullptr*/)
	{
		auto action = new QAction(text, this);
		action->setIcon(QIcon(icon));
		action->setText(text);

		if (tooltip)
			action->setToolTip(tooltip);

		return action;
	}

	font_button* main_window::make_button(const int icon, const char* tooltip)
	{
		auto button = new font_button(icon, this);
		button->setObjectName("titlebar");
		button->setToolTip(tooltip);

		return button;
	}

	// Event
	void main_window::closeEvent(QCloseEvent* event)
	{
		if (m_tray && m_app.is_system_tray())
		{
			event->ignore();
			hide();

			if (m_close_alert)
			{
				const auto text = "Demonsaw is still running in the background. Choose \"Quit\" from the system tray menu to exit the app.";
				m_tray->showMessage(software::title, text, QIcon(resource::logo_rgb), sec_to_ms(3));
				m_close_alert = false;
			}
		}
		else
		{
			kill();
		}
	}

	bool main_window::eventFilter(QObject* object, QEvent* event)
	{
		if (object == m_tabbar)
		{
			if (event->type() == QEvent::Wheel)
				return true;
		}
		else
		{
			if (event->type() == QEvent::WindowActivate)
			{
				const auto window = get_window();
				if (window)
					update_tab(window);
			}
			else if (event->type() == QEvent::ContextMenu)
			{
				QContextMenuEvent* menu_event = static_cast<QContextMenuEvent *>(event);
				return menu_event->reason() == QContextMenuEvent::Mouse;
			}
			else if (event->type() == QEvent::MouseButtonRelease)
			{
				QMouseEvent* mouse_event = static_cast<QMouseEvent*> (event);
				if (mouse_event->button() == Qt::MiddleButton)
					remove();
			}
		}

		return QObject::eventFilter(object, event);
	}

	// Slot
	void main_window::on_icon_activated(QSystemTrayIcon::ActivationReason reason)
	{
		switch (reason)
		{
			case QSystemTrayIcon::Trigger:
			case QSystemTrayIcon::DoubleClick:
			{
				open();
				break;
			}
		}
	}

	void main_window::on_message_clicked()
	{
		if (isHidden())
			open();
	}

	void main_window::on_show_menu(const QPoint& point)
	{
		assert(thread_info::main());

		// Button
		const auto index = m_tabbar->tabAt(point);
		if ((index == -1) || (m_tabbar->currentIndex() != index))
			return;

		// Entity
		const auto entity = get_entity(index);
		if (likely(entity))
		{
			// Menu
			QMenu menu;
			menu.setStyleSheet(QString("QMenu::item { height:%1px; width:%2px; }").arg(resource::get_row_height()).arg(resource::get_menu_width()));

			const auto client_service = entity->get<client_service_component>();

			if (entity->enabled() && client_service->valid())
			{
				menu.addAction(m_disconnect_action);
				menu.addAction(m_reconnect_action);
			}
			else
			{
				menu.addAction(m_connect_action);
			}

			menu.exec(m_toolbar->mapToGlobal(point));
		}
	}

	void main_window::on_wizard_done()
	{
		const auto color = m_wizard->get_color();
		const auto name = m_wizard->get_name();
		const auto address = m_wizard->get_address();
		const auto port = m_wizard->get_port();
		const auto password = m_wizard->get_password();
		const auto enabled = m_wizard->enabled();

		add(name, address, port, password, color, enabled);
	}

	// Set
	void main_window::set_entity(const int index)
	{
		assert(thread_info::main());

		m_add_button->setChecked(false);
		m_settings_button->setChecked(false);

		const auto window = get_window(index);
		if (likely(window))
		{
			m_tabbar->setCurrentIndex(index);

			if (m_tab_button_active)
			{
				update_style(m_add_button, "plus");
				update_style(m_settings_button, "settings");
				update_style(m_tabbar, "bar");
				m_tab_button_active = false;
			}

			m_client_stack->setCurrentWidget(window);
			m_app.set_active(index);
			window->show();
		}
	}

	void main_window::set_statusbar(const bool value)
	{
		for (size_t i = 0; i < m_tabbar->count(); ++i)
		{
			const auto window = get_window(i);
			window->set_statusbar(value);
		}
	}

	// Get
	const client_window* main_window::get_window() const
	{
		const auto index = m_tabbar->currentIndex();
		return (index != npos) ? get_window(index) : nullptr;
	}

	const client_window* main_window::get_window(const size_t index) const
	{
		const auto variant = m_tabbar->tabData(index);
		return variant.value<client_window*>();
	}

	const client_window* main_window::get_window(const entity::ptr& entity) const
	{
		for (size_t i = 0; i < m_tabbar->count(); ++i)
		{
			const auto window = get_window(i);
			if (entity == window->get_entity())
				return window;
		}

		return nullptr;
	}

	entity::ptr main_window::get_entity() const
	{
		const auto index = m_tabbar->currentIndex();
		return (index != npos) ? get_entity(index) : nullptr;
	}

	entity::ptr main_window::get_entity(const size_t index) const
	{
		const auto window = get_window(index);
		return window->get_entity();
	}

	size_t main_window::get_index(const entity::ptr& entity) const
	{
		// NOTE: Use tab bar since tab order can change
		for (size_t i = 0; i < m_tabbar->count(); ++i)
		{
			const auto window = get_window(i);
			if (entity == window->get_entity())
				return i;
		}

		return npos;
	}
}