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

#include <boost/format.hpp>

#include <QAction>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPoint>
#include <QSortFilterProxyModel>

#include "component/callback/callback.h"
#include "component/client/client_service_component.h"
#include "component/router/router_component.h"

#include "delegate/radio_button_delegate.h"
#include "entity/entity.h"
#include "entity/entity_table_view_ex.h"
#include "factory/client_factory.h"
#include "font/font_awesome.h"
#include "model/router_model.h"
#include "resource/resource.h"
#include "thread/thread_info.h"
#include "widget/router_widget.h"

namespace eja
{
	// Constructor
	router_widget::router_widget(const entity::ptr& entity, QWidget* parent /*= 0*/) : entity_dock_widget(entity, "Routers", fa::server, parent)
	{
		assert(thread_info::main());

		// Action
		m_add_action = make_action(" Add", fa::plus);
		m_remove_action = make_action(" Remove", fa::minus);
		m_close_action = make_action(" Close", fa::close);

		m_move_up_action = make_action(" Up", fa::angle_up);
		m_move_down_action = make_action(" Down", fa::angle_down);
		m_move_top_action = make_action(" Top", fa::angle_double_up);
		m_move_bottom_action = make_action(" Bottom", fa::angle_double_down);

		// Menu
		add_button("Add", fa::plus, [&]() { add(); });
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
		m_table = new entity_table_view_ex(m_entity, this);
		m_model = new router_model(m_entity, this);
		init_ex(m_table, m_model);

		const auto header = m_table->horizontalHeader();
		header->setSortIndicator(router_model::column::row, Qt::SortOrder::AscendingOrder);
		header->setSectionResizeMode(router_model::column::row, QHeaderView::Fixed);

		m_table->setDragEnabled(true);
		m_table->setAcceptDrops(true);
		m_table->setDropIndicatorShown(true);
		m_table->setDragDropMode(QAbstractItemView::InternalMove);
		m_table->setDefaultDropAction(Qt::MoveAction);
		m_table->setDragDropOverwriteMode(false);

		m_table->setSelectionMode(QAbstractItemView::SingleSelection);
		m_table->setItemDelegateForColumn(router_model::column::row, new radio_button_delegate(m_table));
		m_table->sortByColumn(router_model::column::row);
		m_table->set_column_sizes();

		setWidget(m_table);

		// Event
		m_table->installEventFilter(this);

		// Callback
		add_callback(callback::router | callback::update, [&]() { m_model->update(); });
		add_callback(callback::router | callback::clear, [&]() { m_model->clear(); });

		// Signal
		connect(this, &router_widget::customContextMenuRequested, this, &router_widget::show_menu);
		connect(m_table, &router_widget::customContextMenuRequested, this, &router_widget::show_menu);
		connect(header, &QHeaderView::customContextMenuRequested, [&](const QPoint& pos) { show_header_menu({ "Address", "Port", "Password" }, { router_model::column::address, router_model::column::port, router_model::column::password }, m_table, m_model, pos); });

		connect(m_add_action, &QAction::triggered, [&]() { add(); });
		connect(m_remove_action, &QAction::triggered, [&]() { remove(); });
		connect(m_close_action, &QAction::triggered, [&]() { close(); });

		connect(m_move_up_action, &QAction::triggered, [&]() { m_table->move_up(); });
		connect(m_move_down_action, &QAction::triggered, [&]() { m_table->move_down(); });
		connect(m_move_top_action, &QAction::triggered, [&]() { m_table->move_top(); });
		connect(m_move_bottom_action, &QAction::triggered, [&]() { m_table->move_bottom(); });
	}

	// Interface
	void router_widget::on_add()
	{
		assert(thread_info::main());

		// Entity
		const auto entity = client_factory::create_router(m_entity);
		entity->disable();

		// Component
		const auto router = entity->get<router_component>();
		router->set_address(router::address);
		router->set_port(router::port);

		const auto router_list = m_entity->get<router_list_component>();
		const auto router_name = boost::str(boost::format("%s %d") % router::name % (router_list->size() + 1));
		router->set_name(router_name);

		m_model->add(entity);
	}

	void router_widget::on_remove()
	{
		assert(thread_info::main());

		m_table->execute([&](const QModelIndex& index)
		{
			const auto entity = m_model->get_entity(index);
			if (entity && entity->disabled())
				m_model->remove(entity);
		});
	}

	// Menu
	void router_widget::show_menu(const QPoint& pos)
	{
		QMenu menu;
		menu.setStyleSheet(QString("QMenu::item { height:%1px; width:%2px; }").arg(resource::get_row_height()).arg(resource::get_menu_width()));

		if (!m_model->empty())
		{
			const auto index = m_table->indexAt(pos);
			if (index.isValid())
			{
				menu.addAction(m_add_action);

				const auto proxy = m_table->get_proxy();
				const auto sindex = proxy->mapToSource(index);
				if (sindex.isValid())
				{
					const auto entity = m_model->get_entity(sindex);
					if (entity && entity->disabled())
						menu.addAction(m_remove_action);
				}

				auto move_menu = make_menu("  Move...", fa::arrows);
				move_menu->setStyleSheet(QString("QMenu::item { height:%1px; width:%2px; }").arg(resource::get_row_height()).arg(resource::get_menu_width()));

				move_menu->addAction(m_move_up_action);
				move_menu->addAction(m_move_down_action);
				move_menu->addAction(m_move_top_action);
				move_menu->addAction(m_move_bottom_action);
				menu.addMenu(move_menu);

				menu.addSeparator();
			}
			else
			{
				menu.addAction(m_add_action);
				menu.addSeparator();
			}
		}
		else
		{
			menu.addAction(m_add_action);
			menu.addSeparator();
		}

		menu.addAction(m_close_action);

		menu.exec(m_table->viewport()->mapToGlobal(pos));
	}
}
