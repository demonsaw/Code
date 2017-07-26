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
#include <QTimer>
#include <QToolBar>

#include "component/endpoint_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/client/client_service_component.h"
#include "delegate/endpoint_delegate.h"
#include "delegate/endpoint_mute_delegate.h"
#include "entity/entity.h"
#include "entity/entity_table_view.h"
#include "font/font_awesome.h"
#include "widget/client/client_client_widget.h"

namespace eja
{
	// Constructor
	client_client_widget::client_client_widget(const entity::ptr entity, QWidget* parent /*= 0*/) : entity_widget(entity, "Clients", fa::user, callback_type::client, parent)
	{
		assert(thread_info::main());

		// Action
		m_chat_action = make_action("Message", fa::envelope);
		m_mute_action = make_action("Mute", fa::ban);
		m_unmute_action = make_action("Unmute", fa::circleo);
		m_update_action = make_action("Update", fa::bolt);
		
		// Menu
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
		m_model = new client_client_model(m_entity, { "Name", "" }, { 128, 24 }, this);
		init(m_table, m_model);

		// Header
		const auto hheader = m_table->horizontalHeader();
		hheader->setSectionResizeMode(client_client_model::column::status, QHeaderView::Fixed);
		hheader->setSortIndicator(client_client_model::column::name, Qt::SortOrder::AscendingOrder);
		hheader->setSectionResizeMode(client_client_model::column::name, QHeaderView::Stretch);
		hheader->setObjectName("stretch");

		// Callback
		add_callback(callback_action::add, [&](const entity::ptr entity) { m_model->add(entity); }); 
		add_callback(callback_action::update, [&](const entity::ptr entity) { m_model->update(); });
		add_callback(callback_action::clear, [&](const entity::ptr entity){ m_model->clear(); });

		// Delegate
		m_table->setColumnWidth(client_client_model::column::status, 24);
		m_table->setItemDelegateForColumn(client_client_model::column::name, new endpoint_delegate(m_table));
		m_table->setItemDelegateForColumn(client_client_model::column::status, new endpoint_mute_delegate(m_table));

		// Signal
		connect(m_table, &entity_table_view::customContextMenuRequested, this, &client_client_widget::show_menu);
		connect(m_table, &entity_table_view::doubleClicked, m_table, &entity_table_view::double_click);

		connect(m_chat_action, &QAction::triggered, m_table, &entity_table_view::double_click);
		connect(m_mute_action, &QAction::triggered, [&]() { on_mute(true); });
		connect(m_unmute_action, &QAction::triggered, [&]() { on_mute(false); });
		connect(m_update_action, &QAction::triggered, [&]() { update(); });

		// Sort
		m_table->sortByColumn(client_client_model::column::name);

		setWidget(m_table);
	}

	// Interface
	void client_client_widget::on_update()
	{
		assert(thread_info::main());

		// Spam
		auto button = get_button(fa::bolt);
		if (button)
		{
			button->setEnabled(false);
			QTimer::singleShot(ui::refresh, [&]() { set_enabled(fa::bolt); });
		}

		// Service
		const auto service = m_entity->get<client_service_component>();
		if (service->valid())
			service->async_client();
	}

	// Utility
	void client_client_widget::on_mute(const bool mute)
	{
		assert(thread_info::main());

		auto selected = m_table->selectionModel()->selectedRows();

		if (!selected.empty())
		{
			const auto proxy = m_table->get_proxy_model();

			for (const auto& index : selected)
			{
				if (index.isValid())
				{
					const auto sindex = proxy->mapToSource(index);
					if (sindex.isValid())
					{
						const auto entity = m_model->get_entity(sindex);
						if (entity)
						{
							const auto endpoint = entity->get<endpoint_component>();
							endpoint->set_mute(mute);

							// HACK: Associate m_entity and the client list version
							//
							if (m_entity->equals<endpoint_component>(entity))
							{
								const auto endpoint = m_entity->get<endpoint_component>();
								endpoint->set_mute(mute);
							}
						}
					}
				}
			}
		}
	}

	// Menu
	void client_client_widget::show_menu(const QPoint& pos)
	{
		assert(thread_info::main());

		const auto index = m_table->indexAt(pos);
		if (!index.isValid())
			return;

		QMenu menu(this);

		// Mute
		const auto state = get_mute();
		if ((state & mute_state::unmuted) == mute_state::unmuted)
			menu.addAction(m_mute_action);

		// Unmute
		if ((state & mute_state::muted) == mute_state::muted)
			menu.addAction(m_unmute_action);
		
		menu.addAction(m_chat_action);
		menu.addSeparator();

		menu.addAction(m_update_action);

		menu.exec(m_table->viewport()->mapToGlobal(pos));
	}

	// Get
	size_t client_client_widget::get_mute()
	{
		assert(thread_info::main());

		size_t state = mute_state::none;

		auto selected = m_table->selectionModel()->selectedRows();

		if (!selected.empty())
		{
			const auto proxy = m_table->get_proxy_model();

			for (const auto& index : selected)
			{
				if (index.isValid())
				{
					const auto sindex = proxy->mapToSource(index);
					if (sindex.isValid())
					{
						const auto entity = m_model->get_entity(sindex);
						if (entity)
						{
							const auto endpoint = entity->get<endpoint_component>();
							state |= endpoint->unmuted() ? mute_state::unmuted : mute_state::muted;
						}
					}
				}
			}
		}

		return state;
	}
}
