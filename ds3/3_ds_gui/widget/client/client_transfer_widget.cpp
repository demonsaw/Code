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
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPoint>
#include <QSplitter>
#include <QTimer>
#include <QToolBar>

#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "delegate/router_delegate.h"
#include "delegate/row_checkbox_delegate.h"
#include "delegate/endpoint_status_delegate.h"
#include "entity/entity.h"
#include "entity/entity_table_view.h"
#include "font/font_awesome.h"
#include "model/client/client_transfer_model.h"
#include "model/client/client_transfer_group_model.h"
#include "pane/client/client_transfer_pane.h"
#include "widget/client/client_transfer_widget.h"

namespace eja
{
	// Constructor
	client_transfer_widget::client_transfer_widget(const entity::ptr entity, QWidget* parent /*= 0*/) : entity_widget(entity, "Transfer Routers", fa::exchange, callback_type::transfer, parent)
	{
		assert(thread_info::main());

		// Action
		m_add_action = make_action("Add", fa::plus);
		m_remove_action = make_action("Remove", fa::minus);
		m_update_action = make_action("Update", fa::bolt);

		// Menu
		add_button("Add", fa::plus, [&]() { m_table->add(); });
		add_button("Remove", fa::minus, [&]() { m_table->remove(); });
		add_button("Update", fa::bolt, [&]() { update(); });
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
		m_table->setSelectionMode(QAbstractItemView::SingleSelection);
		m_model = new client_transfer_model(m_entity, { "", "", "Name", "Address", "Port", "Group", "Version" }, { 44, 24, 128, 256, 64, 96, 64 }, this);
		init(m_table, m_model);

		// Header
		const auto hheader = m_table->horizontalHeader();
		hheader->setSectionResizeMode(client_transfer_model::column::row, QHeaderView::Fixed);
		hheader->setSectionResizeMode(client_transfer_model::column::type, QHeaderView::Fixed);

		// Callback
		add_callback(callback_action::add, [&](const entity::ptr entity){ m_model->add(entity); });
		add_callback(callback_action::update, [&](const entity::ptr entity) { m_model->update(); });
		add_callback(callback_action::clear, [&](const entity::ptr entity) { emit clear(entity); });

		// Delegate
		m_table->setColumnWidth(client_transfer_model::column::row, 44);
		m_table->setColumnWidth(client_transfer_model::column::type, 24);
		m_table->setItemDelegateForColumn(client_transfer_model::column::row, new row_checkbox_delegate(m_table));
		m_table->setItemDelegateForColumn(client_transfer_model::column::type, new router_delegate(m_table));
		m_table->setItemDelegateForColumn(client_transfer_model::column::name, new endpoint_status_delegate(m_table));

		// Signal		
		connect(m_table, &entity_table_view::customContextMenuRequested, this, &client_transfer_widget::show_menu);
		connect(m_table, &entity_table_view::clicked, this, &client_transfer_widget::on_click);
		connect(m_table, &entity_table_view::unclick, this, &client_transfer_widget::on_unclick);

		connect(m_add_action, &QAction::triggered, [&]() { m_table->add(); });
		connect(m_remove_action, &QAction::triggered, [&]() { m_table->remove(); });
		connect(m_update_action, &QAction::triggered, [&]() { update(); });

		// Pane
		m_pane = new client_transfer_pane(this);

		// Splitter		
		QSplitter* splitter = new QSplitter(Qt::Vertical);
		splitter->setContentsMargins(0, 0, 0, 0);
		splitter->setChildrenCollapsible(true);
		splitter->addWidget(m_table);
		splitter->setStretchFactor(0, 4);
		splitter->addWidget(m_pane);
		splitter->setStretchFactor(1, 1);
		splitter->setHandleWidth(4);
		setWidget(splitter);
	}

	// Interface	
	void client_transfer_widget::on_update()
	{
		assert(thread_info::main());

		// Spam
		auto button = get_button(fa::bolt);
		if (button)
		{
			button->setEnabled(false);
			QTimer::singleShot(ui::refresh, [&]() { set_enabled(fa::bolt); });
		}

		m_table->update();
	}

	void client_transfer_widget::on_remove()
	{
		assert(thread_info::main());

		m_table->remove();
		m_pane->remove();
	}

	void client_transfer_widget::on_clear(const entity::ptr entity)
	{
		assert(thread_info::main());

		m_model->clear(entity);

		auto model = m_pane->get_model();
		model->clear();
	}

	// Slot
	void client_transfer_widget::on_click(const QModelIndex& index)
	{
		assert(thread_info::main());

		if (!index.isValid())
			return;

		const auto proxy = m_table->get_proxy_model();
		const auto sindex = proxy->mapToSource(index);
		if (!sindex.isValid())
			return;

		const auto entity = m_model->get_entity(sindex);
		assert(entity);

		if (entity)
		{
			auto model = m_pane->get_model();
			model->set_entity(entity);

			if (entity->enabled())
				m_pane->set_group_id(entity);
			else
				m_pane->set_group_id();
		}
	}

	void client_transfer_widget::on_unclick()
	{
		assert(thread_info::main());

		auto model = m_pane->get_model();
		model->set_entity();

		m_pane->set_group_id();
	}

	// Menu
	void client_transfer_widget::show_menu(const QPoint& pos)
	{
		assert(thread_info::main());

		const auto index = m_table->indexAt(pos);
		if (!index.isValid())
			return;

		on_click(index);

		QMenu menu(this);
		menu.addAction(m_add_action);
		menu.addAction(m_remove_action);
		menu.addAction(m_update_action);

		menu.exec(m_table->viewport()->mapToGlobal(pos));
	}
}
