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

#include <boost/locale.hpp>

#include <QColor>
#include <QTextCodec>

#include "component/chat_component.h"
#include "component/endpoint_component.h"
#include "component/group_ui_component.h"
#include "component/message_wrapper_component.h"
#include "component/client/client_service_component.h"

#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "model/envelope_model.h"
#include "model/message_model.h"
#include "thread/thread_info.h"
#include "utility/locale.h"
#include "utility/utf8.h"
#include "utility/value.h"

namespace eja
{
	// Constructor
	envelope_model::envelope_model(QObject* parent /*= nullptr*/) : entity_table_model(parent)
	{
		m_empty = entity_factory::create_client_client(m_entity);
	}

	// Interface
	void envelope_model::restart()
	{
		assert(thread_info::main());

		entity_list list;
		const auto index_list = m_entity->get<index_list_component>();		

		beginResetModel();

		const auto client_map = m_entity->get<client_map_component>();
		{
			thread_lock(client_map);

			std::copy_if(index_list->begin(), index_list->end(), std::back_inserter(list), [client_map](entity::ptr e)
			{
				const auto chat_list = e->get<chat_list_component>();

				if (!chat_list->empty())
				{
					const auto endpoint = e->get<endpoint_component>();
					return client_map->find(endpoint->get_id()) != client_map->end();
				}

				return false;
			});
		}

		index_list->assign(list.begin(), list.end());

		endResetModel();
	}

	// Slot
	void envelope_model::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto index_list = m_entity->get<index_list_component>();

		beginResetModel();

		index_list->push_back(entity);

		// Sort
		std::sort(index_list->begin(), index_list->end(), [](const entity::ptr e1, const entity::ptr e2)
		{
			const auto ec1 = e1->get<endpoint_component>();
			const auto ec2 = e2->get<endpoint_component>();
			return boost::algorithm::to_lower_copy(ec1->get_name()) < boost::algorithm::to_lower_copy(ec2->get_name());
		});

		endResetModel();
	}

	void envelope_model::on_clear()
	{
		assert(thread_info::main());

		const auto index_list = m_entity->get<index_list_component>();

		beginResetModel();
		index_list->clear();
		endResetModel();
	}

	// Utility
	bool envelope_model::empty() const
	{
		assert(thread_info::main());

		const auto index_list = m_entity->get<index_list_component>();
		return index_list->empty();
	}

	// Model
	bool envelope_model::removeRows(int row, int count, const QModelIndex& parent /*= QModelIndex()*/)
	{
		assert(thread_info::main());

		const auto index_list = m_entity->get<index_list_component>();
		const auto it = index_list->cbegin() + row;
		const auto size = index_list->size();

		beginRemoveRows(QModelIndex(), row, (row + count - 1));
		index_list->erase(it, it + count);
		endRemoveRows();

		return index_list->size() < size;
	}

	QVariant envelope_model::data(const QModelIndex& index, int role) const
	{
		assert(thread_info::main());

		if (!index.isValid())
			return QVariant();

		const size_t row = static_cast<size_t>(index.row());
		const auto entity = get_entity(row);
		if (!entity)
			return QVariant();

		size_t col;
		if (role <= Qt::UserRole)
		{
			col = static_cast<size_t>(index.column());
		}
		else
		{
			col = role - (Qt::UserRole + 1);
			role = Qt::DisplayRole;
		}

		switch (role)
		{
			case Qt::DisplayRole:
			{
				switch (col)
				{
					case column::name:
					{
						const auto endpoint = entity->get<endpoint_component>();
						return QString::fromStdString(endpoint->get_name());
					}
					case column::first:
					{
						const auto group_ui = entity->get<group_ui_component>();
						return group_ui->get_initial();
					}
					case column::color:
					{
						QColor color;
						const auto endpoint = entity->get<endpoint_component>();

						if (endpoint->get_alpha() >= 8)
							color.setRgba(endpoint->get_color());
						else
							color.setRgb(endpoint->get_red(), endpoint->get_green(), endpoint->get_blue());

						return QVariant::fromValue(color);
					}
					case column::color_inverse:
					{
						const auto endpoint = entity->get<endpoint_component>();

						if (endpoint->get_alpha() >= 8)
						{
							QColor color(255 - endpoint->get_red(), 255 - endpoint->get_green(), 255 - endpoint->get_blue(), endpoint->get_alpha());
							return QVariant::fromValue(color);
						}
						else
						{
							QColor color(255 - endpoint->get_red(), 255 - endpoint->get_green(), 255 - endpoint->get_blue());
							return QVariant::fromValue(color);
						}
					}
					case column::muted:
					{
						const auto endpoint = entity->get<endpoint_component>();
						return QVariant::fromValue(endpoint->has_mute());
					}
					case column::contrast:
					{
						const auto endpoint = entity->get<endpoint_component>();
						const double value = ((0.299 * endpoint->get_red()) + (0.587 * endpoint->get_green()) + 0.114 * endpoint->get_blue()) / 255.0;
						return QVariant::fromValue(value);
					}
					case column::message:
					{
						const auto comp = entity->get<message_wrapper_component>();
						const auto model = comp->get_model();
						return QVariant::fromValue(model->size());
					}
				}

				break;
			}
		}

		return QVariant();
	}

	int envelope_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		assert(thread_info::main());

		const auto index_list = m_entity->get<index_list_component>();
		return index_list->size();
	}

	QHash<int, QByteArray> envelope_model::roleNames() const
	{
		static QHash<int, QByteArray> roles;
		roles[column::role_name] = "role_name";
		roles[column::role_first] = "role_first";
		roles[column::role_color] = "role_color";
		roles[column::role_color_inverse] = "role_color_inverse";
		roles[column::role_muted] = "role_muted";
		roles[column::role_contrast] = "role_contrast";
		roles[column::role_message] = "role_message";

		return roles;
	}

	// Get
	entity::ptr envelope_model::get_entity(const size_t row) const
	{
		assert(thread_info::main());

		// HACK: Qml doesn't like something
		const auto index_list = m_entity->get<index_list_component>();
		return (row < index_list->size()) ? index_list->at(row) : m_empty;
	}

	size_t envelope_model::get_row(const entity::ptr entity) const
	{
		assert(thread_info::main());

		// O(N)
		const auto index_list = m_entity->get<index_list_component>();
		const auto it = std::find(index_list->begin(), index_list->end(), entity);
		return (it != index_list->end()) ? (it - index_list->begin()) : npos;
	}
}
