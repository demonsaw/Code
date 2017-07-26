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

#include "router_group_model.h"
#include "window/router_window.h"

namespace eja
{
	// Interface
	void router_group_model::init()
	{
		entity_table_model::init();

		// Callback
		add_callback(function_type::group);
	}

	// Slot
	void router_group_model::on_add(const entity::ptr entity)
	{
		assert(thread::main());

		const auto group_vector = m_entity->get<group_entity_vector_component>();
		if (!group_vector)
			return;

		{
			auto_lock_ptr(group_vector);

			const auto row = group_vector->size();

			beginInsertRows(QModelIndex(), row, row);
			group_vector->add(entity);
			endInsertRows();
		}

		set_status(router_statusbar::group, group_vector->size());
	}

	void router_group_model::on_remove(const entity::ptr entity)
	{
		assert(thread::main());

		const auto group_vector = m_entity->get<group_entity_vector_component>();
		if (!group_vector)
			return;

		{
			auto_lock_ptr(group_vector);

			const auto row = group_vector->get(entity);
			if (row == type::npos)
				return;

			beginRemoveRows(QModelIndex(), row, row);
			group_vector->remove(row);
			endRemoveRows();
		}

		set_status(router_statusbar::group, group_vector->size());
	}

	void router_group_model::on_clear()
	{
		assert(thread::main());

		const auto group_vector = m_entity->get<group_entity_vector_component>();
		if (!group_vector)
			return;

		beginResetModel();
		group_vector->clear();
		endResetModel();

		set_status(router_statusbar::group, group_vector->size());
	}

	// Utility
	bool router_group_model::empty() const
	{
		const auto group_vector = m_entity->get<group_entity_vector_component>();
		return !group_vector || group_vector->empty();
	}

	// Model
	QVariant router_group_model::data(const QModelIndex& index, int role) const
	{
		if (!index.isValid())
			return QVariant();

		const size_t row = static_cast<size_t>(index.row());
		const size_t col = static_cast<size_t>(index.column());

		const auto group_vector = m_entity->get<group_entity_vector_component>();
		if (!group_vector)
			return QVariant();

		const auto entity = group_vector->get(row);
		if (!entity)
			return QVariant();

		switch (role)
		{
			case Qt::DisplayRole:
			{
				switch (col)
				{
					case column::name:
					{
						const auto group = entity->get<group_component>();
						if (group)
							return QString::fromStdString(group->get_name());

						break;
					}
					case column::clients:
					{
						const auto entity_map = entity->get<entity_map_component>();
						if (entity_map)
							return QVariant::fromValue<uint>(entity_map->size());

						break;
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
						const auto group = entity->get<group_component>();
						if (group)
							return QString::fromStdString(group->get_name()).toLower();

						break;
					}
					default:
					{
						return data(index, Qt::DisplayRole);
					}
				}
				break;
			}
		}

		return QVariant();
	}

	int router_group_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		const auto group_vector = m_entity->get<group_entity_vector_component>();
		return group_vector ? static_cast<int>(group_vector->size()) : 0;
	}

	// Accessor
	size_t router_group_model::get_row(const entity::ptr entity) const
	{
		const auto group = entity->get<group_component>();
		if (!group)
			return type::npos;

		const auto group_map = m_entity->get<group_entity_map_component>();
		if (!group_map)
			return type::npos;

		const auto group_entity = group_map->get(group->get_id());
		if (group_entity)
		{
			const auto group_vector = m_entity->get<group_entity_vector_component>();
			if (group_vector)
				return group_vector->get(group_entity);
		}

		return type::npos;
	}
}
