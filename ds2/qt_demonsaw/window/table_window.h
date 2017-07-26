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

#ifndef _EJA_TABLE_WINDOW_
#define _EJA_TABLE_WINDOW_

#include <QSortFilterProxyModel>
#include <QTableView>
#include <QToolBar>
#include <QWidget>

#include "entity/entity_table_model.h"
#include "system/function/function.h"

class QShowEvent;
class QToolButton;

namespace eja
{
	// Enum
	enum class client_statusbar;
	enum class table_button{ add, remove, clear, refresh };
	enum class table_callback : size_t { add, remove, clear, refresh, click, double_click, execute };

	// Container
	derived_type(table_function, mutex_map<table_callback, function>);

	class table_window : public QTableView
	{
		Q_OBJECT

	protected:
		table_function m_functions;

		QToolBar* m_toolbar = nullptr;
		QToolButton* m_add_button = nullptr;
		QToolButton* m_remove_button = nullptr;
		QToolButton* m_clear_button = nullptr;
		QToolButton* m_refresh_button = nullptr;		

	protected:
		// Interface
		virtual void create(QWidget* parent = 0);
		virtual void layout(QWidget* parent = 0) { }
		virtual void signal(QWidget* parent = 0) { }

		// Utility
		void add_button(table_button item, const bool enabled = true);
		void add_separator() { m_toolbar->addSeparator(); }

	public slots:
		virtual void add();
		virtual void remove();
		virtual void clear();
		virtual void refresh();
		virtual void click() { }
		virtual void double_click();
		virtual void execute();

	public:
		table_window(QWidget* parent = 0);
		virtual ~table_window() { }

		// Interface
		virtual void init();
		virtual void shutdown() { }
		virtual void update() { }

		// Utility
		void scroll();

		// Mutator
		virtual void setModel(QAbstractItemModel* model) override;

		// Accessor
		QSortFilterProxyModel* get_proxy_model() const { return qobject_cast<QSortFilterProxyModel*>(model()); }
		entity_table_model* get_source_model() const;

		table_function& get_functions() { return m_functions; }
		QToolBar* get_toolbar() const { return m_toolbar; }
		virtual int get_sort_column() const { return 0; }
	};
}

#endif
