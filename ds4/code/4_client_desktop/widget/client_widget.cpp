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
#include <QHeaderView>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMenu>

#include "component/chat_component.h"
#include "component/room_component.h"
#include "component/callback/callback.h"
#include "component/callback/callback_service_component.h"
#include "component/client/client_component.h"
#include "component/client/client_service_component.h"

#include "delegate/client/client_delegate.h"
#include "entity/entity.h"
#include "entity/entity_tree_view_ex.h"
#include "font/font_awesome.h"
#include "model/client_model.h"
#include "model/proxy/client_proxy_model.h"
#include "resource/resource.h"
#include "thread/thread_info.h"
#include "widget/client_widget.h"

namespace eja
{
	// Constructor
	client_widget::client_widget(const entity::ptr& entity, QWidget* parent /*= nullptr*/) : entity_dock_widget(entity, "Users", fa::users, parent)
	{
		assert(thread_info::main());

		// Action
		m_join_action = make_action(" Join", fa::hash_tag);
		m_message_action = make_action(" Message", fa::envelope);
		m_mute_action = make_action(" Mute", fa::ban);
		m_unmute_action = make_action(" Unmute", fa::circle_o);
		m_refresh_action = make_action(" Refresh", fa::refresh);
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
		m_tree = new entity_tree_view_ex(m_entity, this);
		m_model = new client_model(m_entity, this);
		init_ex<client_proxy_model>(m_tree, m_model);

		const auto header = m_tree->header();
		header->setObjectName("stretch");
		header->setSortIndicator(client_model::column::name, Qt::SortOrder::AscendingOrder);
		header->setSectionResizeMode(client_model::column::name, QHeaderView::Stretch);
		header->setSectionsMovable(false);

		m_tree->setItemDelegateForColumn(client_model::column::name, new client_delegate(m_tree));
		m_tree->sortByColumn(client_model::column::name);
		m_tree->set_column_sizes();

		setWidget(m_tree);

		// Event
		m_tree->installEventFilter(this);

		// Callback
		add_callback(callback::client | callback::add, [&](const entity::ptr entity) { on_add(entity); });
		add_callback(callback::client | callback::remove, [&](const entity::ptr entity) { on_remove(entity); });
		add_callback(callback::client | callback::update, [&](const entity::ptr entity) { m_model->update(entity); });
		add_callback(callback::client | callback::update, [&]() { m_model->update(); });
		add_callback(callback::client | callback::clear, [&](const entity::ptr entity) { m_model->clear(entity); });
		add_callback(callback::client | callback::clear, [&]() { m_model->clear(); });

		// Signal
		connect(this, &client_widget::customContextMenuRequested, this, &client_widget::show_menu);
		connect(m_tree, &entity_tree_view_ex::customContextMenuRequested, this, &client_widget::show_menu);
		connect(m_tree, &entity_tree_view_ex::doubleClicked, this, &client_widget::on_double_click);

		connect(m_join_action, &QAction::triggered, this, &client_widget::on_join);
		connect(m_message_action, &QAction::triggered, this, &client_widget::on_message);
		connect(m_mute_action, &QAction::triggered, [&]() { on_mute(true); });
		connect(m_unmute_action, &QAction::triggered, [&]() { on_mute(false); });
		connect(m_refresh_action, &QAction::triggered, [&]() { update(); });
		connect(m_clear_action, &QAction::triggered, [&]() { clear(); });
		connect(m_close_action, &QAction::triggered, [&]() { close(); });
	}

	// Interface
	void client_widget::on_add(const entity::ptr entity)
	{
		if (entity->has<client_component>() && entity->is_owner(m_entity))
		{
			const auto client = entity->get<client_component>();
			const auto pair = std::make_pair(client->get_id(), entity);
			const auto client_map = m_entity->get<client_map_component>();
			{
				thread_lock(client_map);
				const auto result = client_map->insert(pair);
				if (!result.second)
					return;
			}
		}

		m_model->add(entity);
	}

	void client_widget::on_remove(const entity::ptr entity)
	{
		if (entity->has<client_component>() && entity->is_owner(m_entity))
		{
			const auto client = entity->get<client_component>();
			const auto client_map = m_entity->get<client_map_component>();
			{
				thread_lock(client_map);
				const auto result = client_map->erase(client->get_id());
				if (!result)
					return;
			}
		}

		m_model->remove(entity);
	}

	void client_widget::on_update()
	{
		assert(thread_info::main());

		// Service
		const auto client_service = m_entity->get<client_service_component>();
		if (client_service->valid())
			client_service->async_room();
	}

	void client_widget::on_clear()
	{
		assert(thread_info::main());

		m_tree->execute([&](const QModelIndex index)
		{
			const auto entity = m_model->get_entity(index);
			if (entity && entity->has<client_component>() && !is_owner(entity))
				m_entity->async_call(callback::pm | callback::clear, entity);
		});
	}

	void client_widget::on_double_click(const QModelIndex& index)
	{
		assert(thread_info::main());

		const auto proxy = m_tree->get_proxy();
		const auto sindex = proxy->mapToSource(index);
		const auto entity = m_model->get_entity(sindex);

		if (entity && entity->has<client_component>() && !is_owner(entity))
		{
			const auto client = entity->get<client_component>();
			if (!client->is_mute())
				m_entity->async_call(callback::pm | callback::create, entity);
		}
	}

	// Utility
	void client_widget::on_join()
	{
		assert(thread_info::main());

		m_tree->execute([&](const QModelIndex index)
		{
			const auto entity = m_model->get_entity(index);
			if (entity && entity->has<room_component>())
			{
				const auto client = m_entity->get<client_component>();
				const auto client_list = entity->get<client_list_component>();

				// Find
				const auto it = std::find_if(client_list->begin(), client_list->end(), [&](const entity::ptr e)
				{
					const auto c = e->get<client_component>();
					return client->get_id() == c->get_id();
				});

				if (it == client_list->end())
				{
					if (client_list->is_expanded())
					{
						// Client
						const auto e = entity::create(m_entity);
						e->set_parent(entity);

						e->add<client_component>(m_entity);
						m_model->add(e);
					}
				}

				// Callback
				m_entity->async_call(callback::room | callback::create, entity);
			}
		});
	}

	void client_widget::on_message()
	{
		assert(thread_info::main());

		m_tree->execute([&](const QModelIndex index)
		{
			const auto entity = m_model->get_entity(index);
			if (entity && entity->has<client_component>() && !is_owner(entity))
				m_entity->async_call(callback::pm | callback::create, entity);
		});
	}

	void client_widget::on_mute(const bool value)
	{
		assert(thread_info::main());

		m_tree->execute([&](const QModelIndex index)
		{
			const auto entity = m_model->get_entity(index);
			if (entity && entity->has<client_component>() && !is_owner(entity))
			{
				const auto client = entity->get<client_component>();
				client->set_mute(value);

				// Chat
				m_entity->async_call(callback::chat | callback::update);

				// Room
				m_entity->async_call(callback::room | callback::update);

				// Browse
				if (client->is_share())
					m_entity->async_call(callback::browse | callback::update, entity);

				// Pm
				m_entity->async_call(callback::pm | callback::update, entity);

				// Service
				const auto client_service = m_entity->get<client_service_component>();
				client_service->async_mute(entity, value);
			}
		});
	}

	// Event
	bool client_widget::eventFilter(QObject* object, QEvent* event)
	{
		if (event->type() == QEvent::KeyPress)
		{
			const auto ke = static_cast<QKeyEvent*>(event);

			switch (ke->key())
			{
				case Qt::Key_F5:
				{
					update();
					break;
				}
			}
		}

		return entity_dock_widget::eventFilter(object, event);
	}

	// Has
	bool client_widget::has_client() const
	{
		assert(thread_info::main());

		const auto selected = m_tree->selectionModel()->selectedRows();
		if (!selected.empty())
		{
			const auto proxy = m_tree->get_proxy();

			for (const auto& index : selected)
			{
				const auto sindex = proxy->mapToSource(index);
				const auto entity = m_model->get_entity(sindex);
				if (!entity || !entity->has<client_component>() || is_owner(entity))
					return false;
			}
		}

		return true;
	}

	bool client_widget::has_message() const
	{
		assert(thread_info::main());

		const auto selected = m_tree->selectionModel()->selectedRows();
		if (!selected.empty())
		{
			const auto proxy = m_tree->get_proxy();

			for (const auto& index : selected)
			{
				const auto sindex = proxy->mapToSource(index);
				const auto entity = m_model->get_entity(sindex);
				if (!entity || !entity->has<client_component>() || !entity->has<chat_list_component>() || is_owner(entity))
					return false;

				const auto chat_list = entity->get<chat_list_component>();
				if (chat_list->empty())
					return false;
			}
		}

		return true;
	}

	bool client_widget::has_room() const
	{
		assert(thread_info::main());

		const auto selected = m_tree->selectionModel()->selectedRows();
		if (!selected.empty())
		{
			const auto proxy = m_tree->get_proxy();

			for (const auto& index : selected)
			{
				const auto sindex = proxy->mapToSource(index);
				const auto entity = m_model->get_entity(sindex);
				if (!entity || !entity->has<room_component>())
					return false;
			}
		}

		return true;
	}

	// Is
	bool client_widget::is_owner(const entity::ptr& entity) const
	{
		if (m_entity != entity)
		{
			const auto client = m_entity->get<client_component>();
			const auto c = entity->get<client_component>();
			return client->get_id() == c->get_id();
		}

		return  true;
	}

	bool client_widget::is_muted() const
	{
		assert(thread_info::main());

		const auto selected = m_tree->selectionModel()->selectedRows();
		if (!selected.empty())
		{
			const auto proxy = m_tree->get_proxy();

			for (const auto& index : selected)
			{
				const auto sindex = proxy->mapToSource(index);
				const auto entity = m_model->get_entity(sindex);
				if (entity && entity->has<client_component>())
				{
					const auto client = entity->get<client_component>();
					if (client->is_mute())
						return true;
				}
			}
		}

		return false;
	}

	// Menu
	void client_widget::show_menu(const QPoint& pos)
	{
		assert(thread_info::main());

		QMenu menu;
		menu.setStyleSheet(QString("QMenu::item { height:%1px; width:%2px; }").arg(resource::get_row_height()).arg(resource::get_menu_width()));

		const auto client_service = m_entity->get<client_service_component>();

		if (client_service->valid())
		{
			if (!m_model->empty())
			{
				const auto index = m_tree->indexAt(pos);
				if (index.isValid())
				{
					if (has_room())
					{
						menu.addAction(m_join_action);
						menu.addSeparator();
					}
					else if (has_client())
					{
						if (!is_muted())
						{
							menu.addAction(m_message_action);
							menu.addAction(m_mute_action);
							menu.addSeparator();

							if (has_message())
							{
								menu.addAction(m_clear_action);
								menu.addSeparator();
							}
						}
						else
						{
							menu.addAction(m_unmute_action);
							menu.addSeparator();
						}
					}
				}
			}

			menu.addAction(m_refresh_action);
			menu.addSeparator();
		}

		menu.addAction(m_close_action);

		menu.exec(m_tree->viewport()->mapToGlobal(pos));
	}
}
