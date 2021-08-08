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

#include <cassert>

#include <QAction>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMenu>
#include <QTimer>

#include "component/callback/callback.h"

#include "delegate/error/error_delegate.h"
#include "delegate/row_delegate.h"
#include "delegate/time_delegate.h"

#include "entity/entity.h"
#include "entity/entity_table_view.h"
#include "font/font_awesome.h"
#include "model/error_model.h"
#include "resource/resource.h"
#include "thread/thread_info.h"
#include "widget/error_widget.h"

namespace eja
{
	// Constructor
	error_widget::error_widget(const entity::ptr& entity, QWidget* parent /*= nullptr*/) : entity_dock_widget(entity, "Errors", fa::exclamation_triangle, parent)
	{
		assert(thread_info::main());

		// Action
		m_remove_action = make_action(" Remove", fa::minus);
		m_clear_action = make_action(" Clear", fa::trash);
		m_close_action = make_action(" Close", fa::close);

		// Menu
		add_button("Close", fa::close, [&]() { close(); });

		// Layout
		const auto hlayout = resource::get_hbox_layout();
		hlayout->addWidget(m_icon);
		hlayout->addWidget(m_title);
		hlayout->addStretch(1);
		hlayout->addWidget(m_toolbar);

		m_titlebar = new QWidget(this);
		m_titlebar->setObjectName("menubar");
		m_titlebar->setLayout(hlayout);
		setTitleBarWidget(m_titlebar);

		// Table
		m_table = new entity_table_view(m_entity, this);
		m_model = new error_model(m_entity, this);
		init(m_table, m_model);

		const auto header = m_table->horizontalHeader();
		header->setObjectName("stretch");
		header->setSortIndicator(error_model::column::row, Qt::SortOrder::AscendingOrder);
		header->setSectionResizeMode(error_model::column::message, QHeaderView::Stretch);

		m_table->setItemDelegateForColumn(error_model::column::row, new row_delegate(m_table));
		m_table->setItemDelegateForColumn(error_model::column::message, new error_delegate(m_table));
		m_table->setItemDelegateForColumn(error_model::column::time, new time_delegate(m_table));
		m_table->sortByColumn(error_model::column::row);
		m_table->set_column_sizes();

		setWidget(m_table);

		// Timer
		m_resize_timer = new QTimer(this);
		m_resize_timer->setSingleShot(true);

		// Event
		m_table->installEventFilter(this);

		// Callback
		add_callback(callback::error | callback::add, [&](const entity::ptr entity) { add(entity); });
		add_callback(callback::error | callback::remove, [&](const entity::ptr entity) { m_model->remove(entity); });
		add_callback(callback::error | callback::clear, [&]() { m_model->clear(); });

		// Signal
		connect(this, &error_widget::customContextMenuRequested, this, &error_widget::show_menu);
		connect(m_table, &entity_table_view::customContextMenuRequested, this, &error_widget::show_menu);
		connect(header, &QHeaderView::sectionResized, this, &error_widget::on_resize);
		connect(header, &QHeaderView::customContextMenuRequested, [&](const QPoint& pos) { show_header_menu({ "Row", "Time" }, { error_model::column::row, error_model::column::time }, m_table, m_model, pos); });

		// NOTE: Needed to format 1st row properly
		connect(m_table, &entity_table_view::resize, [&]()
		{
			if (m_model->rowCount() <= 1)
			{
				m_model->update();
				m_table->resizeRowsToContents();
			}
		});

		connect(m_resize_timer, &QTimer::timeout, this, [&]
		{
			m_table->resizeRowsToContents();
			m_table->scroll();
		});

		connect(m_remove_action, &QAction::triggered, [&]() { m_table->remove(); });
		connect(m_clear_action, &QAction::triggered, [&]() { m_table->clear(); });
		connect(m_close_action, &QAction::triggered, [&]() { close(); });
	}

	// Interface
	void error_widget::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		m_model->add(entity);

		resize();
	}

	// Utility
	void error_widget::resize()
	{
		assert(thread_info::main());

		const auto row = m_model->size() - 1;
		m_table->resizeRowToContents(row);
		m_table->scroll();
	}

	// Event
	void error_widget::on_resize()
	{
		assert(thread_info::main());

		// Get Viewport
		QRect view_rect = m_table->viewport()->rect();

		// Get Top And Bottom Rows
		const auto top_index = m_table->indexAt(view_rect.topLeft());
		if (!top_index.isValid())
			return;

		const auto bottom_index = m_table->indexAt(QPoint(view_rect.x(), view_rect.y() + view_rect.height() - resource::get_row_height() / 2));
		int end = bottom_index.row();
		if (!bottom_index.isValid())
			end = m_table->model()->rowCount() - 1;

		// Resize Rows in Viewport
		int start = top_index.row();
		for (int i = start; i <= end; ++i)
			m_table->resizeRowToContents(i);

		m_table->scroll();

		m_resize_timer->start(sec_to_ms(1));
	}

	// Menu
	void error_widget::show_menu(const QPoint& pos)
	{
		assert(thread_info::main());

		QMenu menu;
		menu.setStyleSheet(QString("QMenu::item { height:%1px; width:%2px; }").arg(resource::get_row_height()).arg(resource::get_menu_width()));

		if (!m_model->empty())
		{
			const auto index = m_table->indexAt(pos);
			if (index.isValid())
				menu.addAction(m_remove_action);

			menu.addAction(m_clear_action);
			menu.addSeparator();
		}

		menu.addAction(m_close_action);

		menu.exec(m_table->viewport()->mapToGlobal(pos));
	}
}
