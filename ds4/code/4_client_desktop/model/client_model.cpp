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

#include "component/chat_component.h"
#include "component/room_component.h"
#include "component/callback/callback.h"
#include "component/client/client_component.h"
#include "component/client/client_service_component.h"

#include "entity/entity.h"
#include "model/client_model.h"
#include "thread/thread_info.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Slot
	void client_model::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		// Parent
		QModelIndex index;
		entity::ptr parent;
		
		if (entity->has_parent())
		{
			parent = entity->get_parent();
			index = get_index(parent);
			if (!index.isValid())
				return;
		}
		else
		{
			parent = m_entity;
		}

		// Data
		const auto client_list = parent->get<client_list_component>();
		const auto row = client_list->size();

		if (!entity->has<client_component>())
		{
			const auto list = entity->get<client_list_component>();
			if (likely(!list->empty()))
			{
				if (client_list->empty())
				{
					beginInsertRows(index, row, (list->size() - 1));
					client_list->swap(*list);
					endInsertRows();
				}
				else
				{
					beginInsertRows(index, row, row + (list->size() - 1));
					client_list->insert(client_list->end(), list->begin(), list->end());
					endInsertRows();
				}
			}
		}
		else
		{
			beginInsertRows(index, row, row);
			client_list->push_back(entity);
			endInsertRows();
		}

		// Callback
		m_entity->async_call(callback::statusbar | callback::add);
	}

	void client_model::on_remove(const entity::ptr entity)
	{
		assert(thread_info::main());

		if (likely(!entity->has_parent()))
		{
			const auto row = get_row(entity);
			if (row != npos)
			{
				const auto client_list = m_entity->get<client_list_component>();
				const auto it = client_list->begin() + row;

				beginRemoveRows(QModelIndex(), row, row);
				client_list->erase(it);
				endRemoveRows();
			}
		}
	}

	void client_model::on_clear(const entity::ptr entity)
	{
		assert(thread_info::main());

		if (entity->has<client_list_component>())
		{
			const auto client_list = entity->get<client_list_component>();

			beginResetModel();
			client_list->clear();
			endResetModel();
		}		
	}

	void client_model::on_clear()
	{
		assert(thread_info::main());

		reset();

		const auto client_list = m_entity->get<client_list_component>();

		beginResetModel();
		client_list->clear();
		endResetModel();

		// Callback
		m_entity->async_call(callback::statusbar | callback::remove);
	}

	// Utility
	bool client_model::empty() const
	{
		assert(thread_info::main());

		const auto client_list = m_entity->get<client_list_component>();
		return client_list->empty();
	}

	void client_model::reset() const
	{
		// Delete child nodes (DO NOT REMOVE)
		const auto client_list = m_entity->get<client_list_component>();

		for (const auto& e : *client_list)
		{
			if (e->has<room_component>())
			{
				assert(m_entity != e);
				const auto cl = e->get<client_list_component>();
				cl->clear();
			}
		}
	}

	// Model
	QVariant client_model::data(const QModelIndex& index, int role) const
	{
		assert(thread_info::main());

		const auto entity = get_entity(index);
		if (!entity)
			return QVariant();

		switch (role)
		{
			case Qt::DisplayRole:
			case Qt::UserRole:
			{
				switch (index.column())
				{					
					case column::name:
					{
						return QVariant::fromValue(entity);
					}
				}

				break;
			}
			case Qt::EditRole:
			{
				switch (index.column())
				{
					case column::name:
					{
						const auto client = entity->get<client_component>();
						return QString::fromStdString(client->get_name());
					}
				}

				break;
			}			
		}

		return QVariant();
	}

	bool client_model::hasChildren(const QModelIndex& index /*= QModelIndex()*/) const
	{
		assert(thread_info::main());

		if (index.isValid())
		{
			const auto entity = get_entity(index);
			if (!entity || !entity->has<room_component>())
				return false;
			
			const auto client_list = entity->get<client_list_component>();
			if (client_list->is_expanded())
				return !client_list->empty();
		}

		return true;
	}

	bool client_model::canFetchMore(const QModelIndex& index) const
	{
		assert(thread_info::main());

		if (index.isValid())
		{
			const auto entity = get_entity(index);
			if (entity && entity->has<room_component>())
			{
				const auto client_list = entity->get<client_list_component>();
				return !client_list->is_expanded();
			}
		}

		return false;
	}

	void client_model::fetchMore(const QModelIndex& index)
	{
		assert(thread_info::main());

		// Entity
		const auto entity = get_entity(index);
		if (entity && entity->has<room_component>())
		{
			const auto client_list = entity->get<client_list_component>();
			client_list->set_expanded(true);

			// Service
			const auto client_service = m_entity->get<client_service_component>();
			client_service->async_room_browse(entity);
		}
	}

	QModelIndex client_model::index(int row, int column, const QModelIndex& index /*= QModelIndex()*/) const
	{
		assert(thread_info::main());

		const auto entity = get_entity(index, true);
		if (entity->has<client_list_component>())
		{
			const auto client_list = entity->get<client_list_component>();
			if (row < client_list->size())
			{
				const auto child = client_list->at(row);
				return createIndex(row, column, child.get());
			}
		}

		return QModelIndex();
	}

	QModelIndex client_model::parent(const QModelIndex& index) const
	{
		assert(thread_info::main());

		const auto parent = get_parent(index);
		if (parent)
		{
			const auto entity = get_parent(parent, true);
			if (entity->has<client_list_component>())
			{
				const auto client_list = entity->get<client_list_component>();
				const auto it = std::find(client_list->begin(), client_list->end(), parent);
				if (it != client_list->end())
				{
					const auto row = it - client_list->begin();
					return createIndex(row, 0, parent.get());
				}
			}
		}

		return QModelIndex();
	}

	int client_model::rowCount(const QModelIndex& index /*= QModelIndex()*/) const
	{
		assert(thread_info::main());
		
		const auto entity = get_entity(index, true);
		if (entity->has<client_list_component>())
		{
			const auto client_list = entity->get<client_list_component>();
			return client_list->size();
		}

		return 0;
	}

	// Get
	entity::ptr client_model::get_entity(const size_t row) const
	{
		assert(thread_info::main());

		const auto client_list = m_entity->get<client_list_component>();
		return (row < client_list->size()) ? client_list->at(row) : nullptr;
	}

	QModelIndex client_model::get_index(const entity::ptr& entity) const
	{
		assert(thread_info::main());

		const auto parent = get_parent(entity, true);
		if (parent->has<client_list_component>())
		{
			const auto client_list = parent->get<client_list_component>();
			const auto it = std::find(client_list->begin(), client_list->end(), entity);
			if (it != client_list->end())
			{
				const auto row = it - client_list->begin();
				return createIndex(row, 0, entity.get());
			}
		}

		return QModelIndex();
	}

	size_t client_model::get_row(const entity::ptr& entity) const
	{
		assert(thread_info::main());

		const auto parent = get_parent(entity, true);
		if (parent->has<client_list_component>())
		{
			// O(N)
			const auto client_list = parent->get<client_list_component>();
			const auto it = std::find(client_list->begin(), client_list->end(), entity);
			return (it != client_list->end()) ? (it - client_list->begin()) : npos;
		}

		return npos;
	}
}
