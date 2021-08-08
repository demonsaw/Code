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

#ifndef _EJA_ENTITY_WINDOW_H_
#define _EJA_ENTITY_WINDOW_H_

#include <cassert>

#include <QMainWindow>

#include "entity/entity_callback.h"

class QDockWidget;
class QWidget;

namespace eja
{
	class entity_dock_widget;
	class entity;
	class entity_action;	
	class font_button;

	class entity_window : public QMainWindow, public entity_callback
	{
		Q_OBJECT

	protected:
		entity_action* m_action = nullptr;
		QDockWidget* m_active = nullptr;
		QToolBar* m_toolbar = nullptr;		

	public:
		static const size_t s_min_height = 32;
		static const size_t s_max_height = (1 << 16);

		static const size_t s_min_width = 202;
		static const size_t s_max_width = (1 << 16);

	private:
		void about();		

	protected:		
		font_button* make_button(const char* tooltip, const int icon);
		void add_about();

	protected slots:
		virtual void on_create() { on_create(nullptr); }
		virtual void on_create(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_destroy() { on_destroy(nullptr); }
		virtual void on_destroy(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_init() { on_init(nullptr); }
		virtual void on_init(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_shutdown() { on_shutdown(nullptr); }
		virtual void on_shutdown(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_update() { on_update(nullptr); }
		virtual void on_update(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_add() { on_add(nullptr); }
		virtual void on_add(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_remove() { on_remove(nullptr); }
		virtual void on_remove(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_clear() { on_clear(nullptr); }
		virtual void on_clear(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_start() { assert(false); }
		virtual void on_start(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_stop() { assert(false); }
		virtual void on_stop(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_restart() { assert(false); }
		virtual void on_restart(const std::shared_ptr<entity> entity) { assert(false); }

		void on_activated(QDockWidget* dockWidget) { m_active = dockWidget; }
		virtual void on_dock_location_changed(Qt::DockWidgetArea area);

	signals:
		void create();
		void create(const std::shared_ptr<entity> entity);

		void destroy();
		void destroy(const std::shared_ptr<entity> entity);

		void init();
		void init(const std::shared_ptr<entity> entity);

		void shutdown();
		void shutdown(const std::shared_ptr<entity> entity);

		void update();
		void update(const std::shared_ptr<entity> entity);

		void add();
		void add(const std::shared_ptr<entity> entity);

		void remove();
		void remove(const std::shared_ptr<entity> entity);

		void clear();
		void clear(const std::shared_ptr<entity> entity); 
		
		void start();
		void start(const std::shared_ptr<entity> entity);

		void stop();
		void stop(const std::shared_ptr<entity> entity);

		void restart();
		void restart(const std::shared_ptr<entity> entity);

	public:
		entity_window(QWidget* parent = nullptr, Qt::WindowFlags flags = 0) : entity_window(nullptr, parent, flags) { }
		entity_window(const std::shared_ptr<entity>& entity, QWidget* parent = nullptr, Qt::WindowFlags flags = 0);

		// Set
		void set_action(const entity_action* action) { m_action = const_cast<entity_action*>(action); }

		// Get
		entity_action* get_action() const { return m_action; }
		QToolBar* get_toolbar() const { return m_toolbar; }
	};
}

#endif
