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

#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/client/client_service_component.h"
#include "component/io/file_component.h"
#include "component/io/folder_component.h"
#include "component/search/search_component.h"
#include "entity/entity.h"
#include "model/client/client_search_model.h"
#include "security/checksum/crc.h"
#include "security/filter/hex.h"
#include "thread/thread_info.h"
#include "utility/io/file_util.h"
#include "utility/io/folder_util.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Slot
	void client_search_model::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());
		
		const auto search_list = m_entity->get<search_list_component>();
		const auto row = search_list->size();

		const auto file = entity->get<file_component>();
		const auto search_map = m_entity->get<search_map_component>();
		const auto it = search_map->find(file->get_id());

		if (it == search_map->cend())
		{
			const auto row = search_list->size();
			const auto pair = std::make_pair(file->get_id(), entity);
			search_map->insert(pair);

			beginInsertRows(QModelIndex(), row, row);
			search_list->push_back(entity);
			endInsertRows();

			// High swarm
			const auto search = entity->get<search_component>();
			const auto weight = search->get_swarm() * search->get_weight();
			if (weight > m_weight)
				m_weight = weight;
		}
		else
		{
			const auto& entity = it->second;
			const auto search = entity->get<search_component>();

			const auto s = entity->get<search_component>();
			search->add_weight(s->get_weight());
			search->add_swarm(s->get_swarm());

			// High swarm
			const auto weight = search->get_swarm() * search->get_weight();
			if (weight > m_weight)
				m_weight = weight;

			on_update(entity);
		}
	}

	void client_search_model::on_clear()
	{
		assert(thread_info::main());

		const auto search_map = m_entity->get<search_map_component>();
		search_map->clear();

		const auto search_list = m_entity->get<search_list_component>();

		beginResetModel();
		search_list->clear();
		endResetModel();

		m_weight = 1;
	}

	void client_search_model::on_double_click(const QModelIndex& index)
	{
		assert(thread_info::main());

		if (!index.isValid())
			return;

		const auto entity = get_entity(index);
		if (entity)
		{
			const auto parent = m_entity->get_parent();
			const auto service = parent->get<client_service_component>();
			if (service->valid())
				service->async_file(entity);
		}			
	}

	// Utility
	bool client_search_model::empty() const
	{
		assert(thread_info::main());

		const auto search_list = m_entity->get<search_list_component>();
		return search_list->empty();
	}

	// Model
	bool client_search_model::removeRows(int row, int count, const QModelIndex& parent /*= QModelIndex()*/)
	{
		assert(thread_info::main());

		const auto search_list = m_entity->get<search_list_component>();
		const auto search_map = m_entity->get<search_map_component>();

		for (auto it = search_list->cbegin() + row; it != search_list->cbegin() + row + count; ++it)
		{
			const auto& e = *it;
			const auto file = e->get<file_component>();
			search_map->erase(file->get_id());
		}

		const auto it = search_list->cbegin() + row;
		const auto size = search_list->size();

		beginRemoveRows(QModelIndex(), row, (row + count - 1));
		search_list->erase(it, it + count);
		endRemoveRows();

		return search_list->size() < size;
	}

	QVariant client_search_model::data(const QModelIndex& index, int role) const
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
					case column::weight:
					case column::name:
					{
						return QVariant::fromValue(entity);
					}
					case column::type:
					{
						const auto file = entity->get<file_component>();
						return QString::fromStdString(file->get_extension());
					}
					case column::checksum:
					{
						const auto file = entity->get<file_component>();
						const auto file_id = file->has_id() ? hex::encode(static_cast<u32>(crc32()(file->get_id()))) : "";
						return QString::fromStdString(file_id);
					}
					case column::size:
					{
						const auto file = entity->get<file_component>();
						return QString::fromStdString(file_util::get_size(file->get_size()));
					}
				}
				break;
			}
			case Qt::UserRole:
			{
				switch (col)
				{
					case column::weight:
					{
						const auto search = entity->get<search_component>();
						const auto ratio = ((search->get_swarm() * search->get_weight()) / static_cast<float>(m_weight));
						return QVariant::fromValue<float>(ratio);
					}
					case column::name:
					{
						const auto file = entity->get<file_component>();
						return QString::fromStdString(file->get_stem()).toLower();
					}					
					case column::type:
					{
						const auto file = entity->get<file_component>();
						return QString::fromStdString(file->get_extension()).toLower();
					}
					case column::size:
					{
						const auto file = entity->get<file_component>();
						return QVariant::fromValue<qulonglong>(file->get_size());
					}
					default:
					{
						return data(index, Qt::DisplayRole);
					}
				}
				break;
			}
			case Qt::EditRole:
			{
				switch (col)
				{
					case column::weight:
					{
						return QVariant::fromValue<uint>(m_weight);
					}
					case column::name:
					{
						const auto file = entity->get<file_component>();
						return QString::fromStdString(file->get_stem());
					}					
				}

				break;
			}
			case Qt::TextAlignmentRole:
			{
				switch (col)
				{
					case column::weight:
					case column::type:
					case column::checksum:
					case column::size:
					{
						return static_cast<int>(Qt::AlignVCenter | Qt::AlignRight);
					}
				}

				break;
			}
		}

		return QVariant();
	}

	int client_search_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		assert(thread_info::main());

		const auto search_list = m_entity->get<search_list_component>();
		return search_list->size();
	}

	// Get
	entity::ptr client_search_model::get_entity(const size_t row) const
	{
		assert(thread_info::main());

		const auto search_list = m_entity->get<search_list_component>();
		return (row < search_list->size()) ? search_list->at(row) : nullptr;
	}

	size_t client_search_model::get_row(const entity::ptr entity) const
	{
		assert(thread_info::main());

		// O(N)
		const auto search_list = m_entity->get<search_list_component>();
		const auto it = std::find(search_list->cbegin(), search_list->cend(), entity);
		return (it != search_list->cend()) ? (it - search_list->cbegin()) : npos;
	}
}
