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

#ifndef _EJA_MAIN_WINDOW_
#define _EJA_MAIN_WINDOW_

#include <QtEvents>
#include <QLabel>
#include <QMainWindow>
#include <QToolBar>
#include <QStatusBar>

#include "application.h"
#include "controller/client_controller.h"
#include "controller/router_controller.h"
#include "entity/entity.h"
#include "entity/entity_window.h"

// Declarations
class QAction;
class QActionGroup;
class QMenu;
class QStackedWidget;
class QStatusBar;
class QToolButton;

/*
The QMainWindow class provides a main application window, with a menu bar, dock windows (e.g. for toolbars), and a status bar.
Main windows are most often used to provide menus, toolbars and a status bar around a large central widget, such as a text edit, 
drawing canvas or QWorkspace (for MDI applications). QMainWindow is usually subclassed since this makes it easier to encapsulate 
the central widget, menus and toolbars as well as the window's state. Subclassing makes it possible to create the slots that are 
called when the user clicks menu items or toolbar buttons. The main window object should only hold data related to the main window.
*/

namespace eja
{
	class main_window final : public QMainWindow
	{
		Q_OBJECT

	public:
		using ptr = main_window*;

	private:
		static main_window* s_ptr;

		// Action				
		//QAction* m_add_action = nullptr;
		QAction* m_add_client_action = nullptr; 
		QAction* m_add_router_action = nullptr;
		
		QAction* m_remove_action = nullptr;
		QAction* m_save_action = nullptr; 
		QAction* m_restart_action = nullptr;
		QAction* m_quit_action = nullptr;
				
		QAction* m_activity_action = nullptr;
		QAction* m_dashboard_action = nullptr;
		QAction* m_menubar_action = nullptr; 
		QAction* m_statusbar_action = nullptr;
		QAction* m_toolbar_action = nullptr;		
		QAction* m_online_action = nullptr; 
		QAction* m_about_action = nullptr;

		// Menu
		QMenu* m_add_menu = nullptr; 
		QMenu* m_file_menu = nullptr;
		QMenu* m_view_menu = nullptr;		
		QMenu* m_help_menu = nullptr;
		QWidget* m_status_menu = nullptr;

		// Statusbar
		QStatusBar* m_statusbar = nullptr;

		// Toolbar
		QToolBar* m_toolbar = nullptr;
		QToolBar* m_menubar = nullptr;

		QToolButton* m_add_button = nullptr;
		QToolButton* m_remove_button = nullptr;
		QToolButton* m_save_button = nullptr;
		QToolButton* m_restart_button = nullptr;

		// Container
		QStackedWidget* m_entity_stack = nullptr;
		QStackedWidget* m_statusbar_stack = nullptr;
		QActionGroup* m_entity_group = nullptr;

	private:
		// Interface
		void init(const entity::ptr entity);
		void update(const entity::ptr entity);

		void add();
		void add_client();
		void add_router();

		void remove();
		void save();
		void restart();

		// Create
		void create_actions();
		void create_menus();
		void create_statusbar();
		void create_toolbars();		
		void create_layouts();
		void create_slots();

		// Utility
		void center();

	private slots:
		// Slot
		void show_entity(QAction* action);
		void show_toolbar(bool value) { m_toolbar->setVisible(value); }
		void show_menubar(bool value) { m_menubar->setVisible(value); }
		void show_statusbar(bool value) { m_statusbar->setVisible(value); }
		void show_options(bool value);
		void show_details(bool value);
		void show_help();
		void show_about();

		void update_status(entity::ptr entity);

	signals:
		void on_update_status(entity::ptr entity);

	private:
		// Event
		virtual void closeEvent(QCloseEvent* event) override;
		virtual bool eventFilter(QObject* object, QEvent* event) override;
		virtual void resizeEvent(QResizeEvent* event) override;

	public:
		main_window(QWidget* parent = 0);
		virtual ~main_window() override { }

		// Interface
		void init();
		void shutdown();

		// Utility		
		void show_ex() { show(); center(); }
		void flash(const size_t ms = 0) const;

		// Accessor
		application* get_app() const { return static_cast<application*>(QCoreApplication::instance()); }		
		entity_window* get_entity_window() const { return qobject_cast<entity_window*>(m_entity_stack->currentWidget()); }
		entity_window* get_entity_window(const entity::ptr entity) const;

		config_file& get_config() { return get_app()->get_config(); }
		client_controller& get_clients() { return get_app()->get_clients(); }
		router_controller& get_routers() { return get_app()->get_routers(); }

		// Static
		static ptr create(QWidget* parent = 0) { return s_ptr = new main_window(parent); }
		static ptr get_instance() { return s_ptr; }
	};
}

#endif
