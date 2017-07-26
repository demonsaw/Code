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

#include <QAction>
#include <QEvent>
#include <QHeaderView>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QTableView>
#include <QTimer>
#include <QToolBar>
#include <QVBoxLayout>

#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "entity/entity.h"
#include "entity/entity_table_view.h"
#include "font/font_awesome.h"
#include "model/error_model.h"
#include "widget/error_widget.h"

namespace eja
{
	// Constructor
	error_widget::error_widget(const entity::ptr entity, QWidget* parent /*= 0*/) : entity_widget(entity, "Errors", fa::exclamation, callback_type::error, parent)
	{
		assert(thread_info::main());

		// Action		
		m_remove_action = make_action("Remove", fa::minus);

		// Menu
		add_button("Remove", fa::minus, [&]() { m_table->clear(); });
		add_button("Popout", fa::clone, [&]() { popout(); });
		add_button("Close", fa::close, [&]() { close(); });

		// Layout
		auto hlayout = new QHBoxLayout;
		hlayout->setSpacing(0);
		hlayout->setMargin(0);

		hlayout->addWidget(m_icon);
		hlayout->addWidget(m_title);
		hlayout->addStretch(1);
		hlayout->addWidget(m_toolbar);

		m_titlebar = new QWidget(this);
		m_titlebar->installEventFilter(this);
		m_titlebar->setObjectName("menubar");
		m_titlebar->setLayout(hlayout);
		setTitleBarWidget(m_titlebar);

		// Table
		m_table = new entity_table_view(m_entity, this);
		m_model = new error_model(m_entity, { "", "Time", "Message" }, { 24, 96, 440 }, this);
		init(m_table, m_model);

		// Header
		const auto hheader = m_table->horizontalHeader();
		hheader->setStretchLastSection(true);
		hheader->setObjectName("stretch");
		hheader->setSortIndicator(error_model::column::index, Qt::SortOrder::AscendingOrder);

		// Callback
		connect(m_table, &entity_table_view::customContextMenuRequested, this, &error_widget::show_menu);
		add_callback(callback_action::add, [&](const entity::ptr entity) { add(entity); });
		add_callback(callback_action::remove, [&](const entity::ptr entity) { m_model->remove(entity); });
		add_callback(callback_action::clear, [&](const entity::ptr entity){ m_model->clear(); });

		// Widget
		setWidget(m_table);

		// Signal
		connect(m_remove_action, &QAction::triggered, [&]() { m_table->remove(); });
	}

	// Utility
	void error_widget::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		m_model->add(entity);

		m_table->scroll();
	}

	// Menu
	void error_widget::show_menu(const QPoint& pos)
	{
		assert(thread_info::main());

		const auto index = m_table->indexAt(pos);
		if (!index.isValid())
			return;

		QMenu menu(this);
		menu.addAction(m_remove_action);

		menu.exec(m_table->viewport()->mapToGlobal(pos));
	}
}
