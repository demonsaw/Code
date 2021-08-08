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

#ifndef _EJA_MAIN_WINDOW_H_
#define _EJA_MAIN_WINDOW_H_

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QTabBar>

#include "config/qt_application.h"
#include "system/type.h"

class QAction;
class QCloseEvent;
class QPoint;
class QPushButton;
class QShortcut;
class QStackedWidget;
class QTimer;
class QToolBar;

namespace eja
{
	class emoji_widget;
	class entity;
	class client_window;
	class font_action;
	class font_button;
	class global_widget;
	class wizard_widget;

	class main_window final : public QMainWindow
	{
		Q_OBJECT

	public:
		using ptr = main_window*;

	private:
		static main_window* s_ptr;

		qt_application& m_app;
		QSystemTrayIcon* m_tray = nullptr;
		QStackedWidget* m_client_stack = nullptr;
		QTimer* m_timer = nullptr;

		// Widget
		emoji_widget* m_emoji_menu = nullptr;
		global_widget* m_settings = nullptr;
		wizard_widget* m_wizard = nullptr;

		// Action
		QAction* m_connect_action = nullptr;
		QAction* m_disconnect_action = nullptr;
		QAction* m_reconnect_action = nullptr;

		// Tab Menu
		QTabBar* m_tabbar = nullptr;
		QToolBar* m_toolbar = nullptr;

		// Buttons
		font_button* m_add_button = nullptr;
		font_button* m_settings_button = nullptr;

		// Shortcuts
		QShortcut* m_refresh_shortcut = nullptr;
		QShortcut* m_remove_shortcut = nullptr;
		QShortcut* m_switch_shortcut = nullptr;

		// Tab Button Active
		bool m_tab_button_active = false;
		bool m_close_alert = false;

#if _MACX
		// HACK: OSX needs a menu hook to close & save properly (from the system menu)
		QMenu* m_file_menu = nullptr;
		QAction* m_quit_action = nullptr;
#endif

	private:
		// Interface
		void create();
		void layout();
		void signal();
		void tray();

		void init(const std::shared_ptr<entity>& entity);

		// Utility
		void about();
		void help();
		void kill();
		void open();

		// Add
		void add(const std::string& name, const std::string& address, const u16 port, const std::string& password = "",  u32 rgba = 0, const bool enabled = false);
		void add();

		// Remove
		void remove(const size_t index);
		void remove();

		// Start
		void start(const size_t index);
		void start();

		// Stop
		void stop(const size_t index);
		void stop();

		// Restart
		void restart(const size_t index);
		void restart();

		// Utility
		void restore();
		void toggle(const size_t index);

		bool verify(const std::shared_ptr<entity>& entity);
		bool verify(const size_t index);
		bool verify();

		void show_tab_bar(const bool visible) { m_tabbar->setMaximumWidth(visible ? QWIDGETSIZE_MAX : 0); }
		void show_settings();

		void update_style(QWidget* widget, const char* selector);
		void update_tab(const client_window* window);

		font_action* make_action(const int fa, const char* tooltip = "");
		QAction* make_action(const char* text, const int fa, const char* tooltip = nullptr);
		QAction* make_action(const char* text, const char* icon, const char* tooltip = nullptr);
		font_button* make_button(const int icon, const char* tooltip);

		// Event
		virtual void closeEvent(QCloseEvent* event) override;
		virtual bool eventFilter(QObject* object, QEvent* event) override;

		// Set
		void set_entity(const int index);

		// Get
		const entity_list& get_clients() const { return m_app.get_clients(); }
		entity_list& get_clients() { return m_app.get_clients(); }

		std::shared_ptr<entity> get_entity() const;
		std::shared_ptr<entity> get_entity(const size_t index) const;

		size_t get_index(const std::shared_ptr<entity>& entity) const;

		const client_window* get_window(const size_t index) const;
		client_window* get_window(const size_t index) { return const_cast<client_window*>(reinterpret_cast<const main_window*>(this)->get_window(index)); }

	private slots:
		void on_icon_activated(QSystemTrayIcon::ActivationReason reason);
		void on_message_clicked();
		void on_show_menu(const QPoint& point);
		void on_wizard_done();

	signals:
		void set_enabled(font_button* button);

	public:
		main_window(qt_application& app, QWidget* parent = nullptr);

		// Interface
		void init();
		void shutdown();

		void clear_chat();
		void clear_pm(const std::shared_ptr<entity>& entity);
		void clear_room(const std::shared_ptr<entity>& entity);

		// Utility
		void save();
		void update_color();
		void update_theme();
		void toast(const std::shared_ptr<entity>& entity, const int fa) const;

		// Set
		void set_statusbar(const bool value);

		// Get
		emoji_widget* get_emoji() const { return m_emoji_menu; }

		const client_window* get_window(const std::shared_ptr<entity>& entity) const;
		client_window* get_window(const std::shared_ptr<entity>& entity) { return const_cast<client_window*>(reinterpret_cast<const main_window*>(this)->get_window(entity)); }

		const client_window* get_window() const;
		client_window* get_window() { return const_cast<client_window*>(reinterpret_cast<const main_window*>(this)->get_window()); }

		// Static
		static ptr get() { return s_ptr; }
		static qt_application& get_app() { return get()->m_app; }
	};
}

#endif
