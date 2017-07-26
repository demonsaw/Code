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

#include "component/endpoint_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/client/client_service_component.h"

#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "model/client/client_client_model.h"
#include "thread/thread_info.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Slot
	void client_client_model::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());
		
		const auto client_list = m_entity->get<client_list_component>();
		const auto row = client_list->size();

		if (entity->has<client_list_component>())
		{
			const auto list = entity->get<client_list_component>();
			//client_list->reserve(client_list->size() + list->size());

			beginInsertRows(QModelIndex(), row, row + list->size() - 1);
			client_list->insert(client_list->end(), list->begin(), list->end());
			endInsertRows();

			const auto client_map = m_entity->get<client_map_component>();
			{
				thread_lock(client_map);

				for (const auto& e : *list)
				{
					const auto endpoint = e->get<endpoint_component>();
					const auto pair = std::make_pair(endpoint->get_id(), e);
					client_map->insert(pair);
				}
			}			
		}
		else
		{
			const auto client_map = m_entity->get<client_map_component>();
			{
				const auto endpoint = entity->get<endpoint_component>();
				const auto pair = std::make_pair(endpoint->get_id(), entity);

				thread_lock(client_map);
				client_map->insert(pair);
			}

			beginInsertRows(QModelIndex(), row, row);
			client_list->push_back(entity);
			endInsertRows();
		}		

		// Callback
		const auto e = entity_factory::create_statusbar(statusbar::client);
		m_entity->call(callback_type::status, callback_action::add, e);
	}

	void client_client_model::on_clear()
	{
		assert(thread_info::main());

		const auto client_map = m_entity->get<client_map_component>();
		{
			thread_lock(client_map);
			client_map->clear();
		}		

		const auto client_list = m_entity->get<client_list_component>();

		beginResetModel();
		client_list->clear();
		endResetModel();

		// Callback
		const auto e = entity_factory::create_statusbar(statusbar::client);
		m_entity->call(callback_type::status, callback_action::clear, e);
	}

	void client_client_model::on_double_click(const QModelIndex& index)
	{
		assert(thread_info::main());

		if (!index.isValid())
			return;

		const auto entity = get_entity(index);
		if (entity)
			m_entity->call(callback_type::chat, callback_action::create, entity);
	}

	// Utility
	bool client_client_model::empty() const
	{
		assert(thread_info::main());

		const auto client_list = m_entity->get<client_list_component>();
		return client_list->empty();
	}

	// Model
	QVariant client_client_model::data(const QModelIndex& index, int role) const
	{
		assert(thread_info::main());

		if (!index.isValid())
			return QVariant();

		const size_t row = static_cast<size_t>(index.row());
		const auto entity = get_entity(row);
		if (!entity)
			return QVariant();

		const size_t col = static_cast<size_t>(index.column());

		switch (role)
		{
			case Qt::DisplayRole:
			{
				switch (col)
				{					
					case column::name:
					case column::status:
					{
						return QVariant::fromValue(entity);
					}
				}

				break;
			}
			case Qt::EditRole:
			{
				switch (col)
				{
					case column::name:
					{
						const auto endpoint = entity->get<endpoint_component>();
						return QString::fromStdString(endpoint->get_name());
					}
				}

				break;
			}
			case Qt::UserRole:
			{
				switch (col)
				{
					case column::name:
					{
						const auto endpoint = entity->get<endpoint_component>();
						return QString::fromStdString(endpoint->get_name()).toLower();
					}
					case column::status:
					{
						const auto endpoint = entity->get<endpoint_component>();
						return QVariant::fromValue(endpoint->has_mute());
					}
				}

				break;
			}
		}

		return QVariant();
	}

	int client_client_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		assert(thread_info::main());

		const auto client_list = m_entity->get<client_list_component>();
		return client_list->size();
	}

	// Get
	entity::ptr client_client_model::get_entity(const size_t row) const
	{
		assert(thread_info::main());

		const auto client_list = m_entity->get<client_list_component>();
		return (row < client_list->size()) ? client_list->at(row) : nullptr;
	}

	size_t client_client_model::get_row(const entity::ptr entity) const
	{
		assert(thread_info::main());

		// O(N)
		const auto client_list = m_entity->get<client_list_component>();
		const auto it = std::find(client_list->begin(), client_list->end(), entity);
		return (it != client_list->end()) ? (it - client_list->begin()) : npos;
	}
}
