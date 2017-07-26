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

#ifndef _EJA_WIDGET_WINDOW_
#define _EJA_WIDGET_WINDOW_

#include <cassert>

#include <QSortFilterProxyModel>
#include <QToolBar>
#include <QWidget>

#include "entity/entity.h"
#include "entity/entity_callback.h"
#include "system/thread/thread.h"

class QToolButton;

namespace eja
{
	enum class widget_button { add, remove, clear, refresh };

	class widget_window : public QWidget, public entity_callback
	{
		Q_OBJECT

	protected:
		// Toolbar
		QToolBar* m_toolbar = nullptr;
		QToolButton* m_add_button = nullptr;
		QToolButton* m_remove_button = nullptr;
		QToolButton* m_clear_button = nullptr;
		QToolButton* m_refresh_button = nullptr;

	protected:
		// Interface
		template <typename T, typename M> void init(T* t, M* m);

		virtual void create(QWidget* parent = 0);
		virtual void layout(QWidget* parent = 0) { }
		virtual void signal(QWidget* parent = 0) { }

		// Utility
		void add_button(widget_button item, const bool enabled = true);
		void add_separator() { m_toolbar->addSeparator(); }

	protected slots:
		virtual void on_add() { assert(thread::main()); }
		virtual void on_add(const entity::ptr entity) { assert(thread::main()); }

		virtual void on_remove() { assert(thread::main()); }
		virtual void on_remove(const entity::ptr entity) { assert(thread::main()); }
		
		virtual void on_refresh();
		virtual void on_refresh(const entity::ptr entity);
		
		virtual void on_clear() { assert(thread::main()); }
		virtual void on_clear(const entity::ptr entity) { assert(thread::main()); }

	public:
		widget_window(const entity::ptr entity, QWidget* parent = 0) : entity_callback(entity), QWidget(parent) { }
		virtual ~widget_window() { }
		
		// Interface
		virtual void init();
		virtual void shutdown() { }
		virtual void update() { }

		// Callback
		virtual void add() { emit added(); }
		virtual void add(const entity::ptr entity) override { emit added(entity); }

		virtual void remove() { emit removed(); }
		virtual void remove(const entity::ptr entity) override { emit removed(entity); }
		
		virtual void refresh() { emit refreshed(); }
		virtual void refresh(const entity::ptr entity) override { emit refreshed(entity); }
		
		virtual void clear() { emit cleared(); }
		virtual void clear(const entity::ptr entity) override { emit cleared(entity); }

		// Accessor
		QToolBar* get_toolbar() const { return m_toolbar; }

	signals:
		void added();
		void added(const entity::ptr entity);

		void removed();
		void removed(const entity::ptr entity);
		
		void refreshed();
		void refreshed(const entity::ptr entity);
		
		void cleared();
		void cleared(const entity::ptr entity);
	};

	// Interface
	template <typename T, typename M>
	void widget_window::init(T* window, M* model)
	{
		// Proxy
		QSortFilterProxyModel* proxy = new QSortFilterProxyModel(this);
		proxy->setSortRole(Qt::UserRole);
		proxy->setSourceModel(model);
		window->setModel(proxy);

		// Model
		model->init();
		window->init();
	}
}

#endif
