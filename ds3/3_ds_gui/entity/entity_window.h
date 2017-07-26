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

#include "component/callback/callback_type.h"
#include "entity/entity_callback.h"

class QWidget;

namespace eja
{
	class entity_widget;
	class entity;
	class entity_action;	
	class font_button;
	class font_trigger;

	class entity_window : public QMainWindow, public entity_callback
	{
		Q_OBJECT

	protected:
		entity_action* m_action = nullptr;
		QToolBar* m_toolbar = nullptr;

	public:
		static const size_t s_min_height = 32;
		static const size_t s_max_height = (1 << 16);

		static const size_t s_min_width = 160;
		static const size_t s_max_width = (1 << 16);

	private:
		void about();		

	protected:
		font_button* make_button(const char* tooltip, const int font);
		font_trigger* make_trigger(const char* tooltip, const int font);
		void add_about();

	protected slots:
		void on_dock_location_changed(Qt::DockWidgetArea area);

		virtual void on_create(const std::shared_ptr<entity> entity) { assert(false); }		
		virtual void on_destroy(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_init(const std::shared_ptr<entity> entity) { assert(false); }
		virtual void on_update(const std::shared_ptr<entity> entity) { assert(false); }
		virtual void on_shutdown(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_add(const std::shared_ptr<entity> entity) { assert(false); }
		virtual void on_remove(const std::shared_ptr<entity> entity) { assert(false); }
		virtual void on_clear(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_start(const std::shared_ptr<entity> entity) { assert(false); }
		virtual void on_stop(const std::shared_ptr<entity> entity) { assert(false); }
		virtual void on_restart(const std::shared_ptr<entity> entity) { assert(false); }
		virtual void on_reset(const std::shared_ptr<entity> entity) { assert(false); }

	signals:
		void create(const std::shared_ptr<entity> entity);		
		void destroy(const std::shared_ptr<entity> entity);

		void init(const std::shared_ptr<entity> entity);
		void update(const std::shared_ptr<entity> entity);
		void shutdown(const std::shared_ptr<entity> entity);

		void add(const std::shared_ptr<entity> entity);
		void remove(const std::shared_ptr<entity> entity);		
		void clear(const std::shared_ptr<entity> entity);

		void start(const std::shared_ptr<entity> entity);
		void stop(const std::shared_ptr<entity> entity);
		void restart(const std::shared_ptr<entity> entity);
		void reset(const std::shared_ptr<entity> entity);

	public:
		entity_window(const callback_type type = callback_type::none, QWidget* parent = 0, Qt::WindowFlags flags = 0) : entity_window(nullptr, type, parent, flags) { }
		entity_window(const std::shared_ptr<entity> entity, const callback_type type = callback_type::none, QWidget* parent = 0, Qt::WindowFlags flags = 0);


		// Set
		void set_action(const entity_action* action) { m_action = const_cast<entity_action*>(action); }

		// Get
		entity_action* get_action() const { return m_action; }
		QToolBar* get_toolbar() const { return m_toolbar; }
	};
}

#endif
