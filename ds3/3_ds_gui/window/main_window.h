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

#include <memory>

#include <QMainWindow>

#include "component/ui_component.h"
#include "config/qt_application.h"
#include "font/font_awesome.h"
#include "system/type.h"

class QAbstractButton;
class QAction;
class QButtonGroup;
class QCloseEvent;
class QPoint;
class QResizeEvent;
class QStackedWidget;
class QToolBar;

namespace eja
{
	class entity;
	class entity_action;
	class entity_window;
	class font_action;
	class font_button;

	class main_window final : public QMainWindow
	{
		Q_OBJECT

	public:
		using ptr = main_window*;

	private:
		static main_window* s_ptr;

		qt_application& m_app;
		QStackedWidget* m_entity_stack = nullptr;

		// Menu
		QToolBar* m_menubar = nullptr;
		QStackedWidget* m_menu_stack = nullptr; 
		
		font_button* m_add_button = nullptr;
		font_button* m_save_button = nullptr;
		font_button* m_update_button = nullptr;

#if _MACX
		// HACK: OSX needs a menu hook to close & save properly (from the system menu)
		QMenu* m_file_menu = nullptr;
		QAction* m_quit_action = nullptr;
#endif

		// Action		
		QAction* m_add_action = nullptr; 
		QAction* m_remove_action = nullptr;
		QAction* m_update_action = nullptr;

		// Toolbar		
		QToolBar* m_toolbar = nullptr;
		QButtonGroup* m_button_group = nullptr;

	private:
		// Interface
		void create();
		void layout();
		void signal();		

		void init_client(const std::shared_ptr<entity> entity);
		void init_router(const std::shared_ptr<entity> entity);

		// Utility
		void add();
		void add_client(const std::string& name, const std::string& address, const u16 port, const u32 rgba = 0, const bool enabled = false);
		void add_router(const std::string& name, const std::string& address, const u16 port, const u32 rgba = 0, const bool enabled = false);

		void remove();
		void save();

		void toggle();
		void toggle(const std::shared_ptr<entity> entity);

		void restart();
		void restart(const std::shared_ptr<entity> entity);

		font_action* make_action(const int font, const char* tooltip = "");
		QAction* make_action(const char* text, const int font, const char* tooltip = nullptr, const size_t size = 14);
		QAction* make_action(const char* text, const char* icon, const char* tooltip = nullptr);
		font_button* make_button(const int font, const char* tooltip);

		// Is
		bool is_client(const std::shared_ptr<entity> entity) const { return !is_router(entity); }
		bool is_router(const std::shared_ptr<entity> entity) const;

		// Event
		virtual void closeEvent(QCloseEvent* event) override;
		virtual bool eventFilter(QObject* object, QEvent* event) override;
		virtual void resizeEvent(QResizeEvent* event) override;

	signals:
		void set_enabled(font_button* button);
		void set_status(const std::shared_ptr<entity> entity);

	private slots:
		void on_show_menu(const QPoint& point);
		void on_select_entity(QAbstractButton* button);
		void on_set_status(const std::shared_ptr<entity> entity);

	public:
		main_window(qt_application& app, QWidget* parent = 0);

		// Interface
		void init();
		void update();		
		void shutdown();
		void clear();

		// Utility
		void flash(const size_t ms = sec_to_ms(1)) const;	
		void notify(const std::shared_ptr<entity> entity);
		void show();

		// Has
		bool has_routers() const { return m_app.has_routers(); }
		bool has_clients() const { return m_app.has_clients(); }

		// Set
		void set_modified(const bool value = true) { setWindowModified(value); }

		// Get
		const entity_action* get_action() const;
		entity_action* get_action() { return const_cast<entity_action*>(reinterpret_cast<const main_window*>(this)->get_action()); }

		const qt_application& get_application() const { return m_app; }
		qt_application& get_application() { return m_app; }

		const entity_list& get_clients() const { return m_app.get_clients(); }
		entity_list& get_clients() { return m_app.get_clients(); }

		const std::shared_ptr<entity> get_entity() const;
		std::shared_ptr<entity> get_entity() { return reinterpret_cast<const main_window*>(this)->get_entity(); }

		const entity_list& get_routers() const { return m_app.get_routers(); }
		entity_list& get_routers() { return m_app.get_routers(); }

		const entity_window* get_window(const entity::ptr entity) const;
		entity_window* get_window(const entity::ptr entity) { return const_cast<entity_window*>(reinterpret_cast<const main_window*>(this)->get_window(entity)); }

		const entity_window* get_window() const;
		entity_window* get_window() { return const_cast<entity_window*>(reinterpret_cast<const main_window*>(this)->get_window()); }

		// Static
		static ptr get() { return s_ptr; }
	};
}

#endif
