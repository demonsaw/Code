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

#include <QColor>

#include "component/chat_component.h"
#include "component/endpoint_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/client/client_option_component.h"
#include "component/time/time_component.h"

#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "model/message_model.h"
#include "thread/thread_info.h"
#include "utility/value.h"
#include "utility/client/client_util.h"

namespace eja
{
	// Slot
	void message_model::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto chat_list = m_entity->get<chat_list_component>();		

		if (!chat_list->empty())
		{
			const auto e = chat_list->front();
			if (e->get_parent() == entity->get_parent())
			{
				// Same
				const auto c = e->get<chat_component>();
				auto& text = c->get_text();
				text += "\n";

				const auto chat = entity->get<chat_component>();
				text += chat->get_text();				

				on_update(e);
			}
			else
			{
				beginInsertRows(QModelIndex(), 0, 0);
				chat_list->push_front(entity);
				endInsertRows();
			}
		}
		else
		{
			beginInsertRows(QModelIndex(), 0, 0);
			chat_list->push_front(entity);
			endInsertRows();
		}

		//++m_rows;
	}

	void message_model::on_clear()
	{
		assert(thread_info::main());

		const auto chat_list = m_entity->get<chat_list_component>();

		beginResetModel();
		chat_list->clear();
		endResetModel();

		//m_rows = 0;
	}

	// Utility
	bool message_model::empty() const
	{
		assert(thread_info::main());

		const auto chat_list = m_entity->get<chat_list_component>();
		return chat_list->empty();
	}

	// Model
	QVariant message_model::data(const QModelIndex& index, int role) const
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
						const auto endpoint = entity->find<endpoint_component>();
						return QString::fromStdString(endpoint->get_name());
					}
					case column::color:
					{
						QColor color;
						const auto endpoint = entity->find<endpoint_component>();

						if (endpoint->get_alpha() >= 8)
							color.setRgba(endpoint->get_color());
						else
							color.setRgb(endpoint->get_red(), endpoint->get_green(), endpoint->get_blue());

						return color;
					}
					case column::message:
					{
						const auto chat = entity->get<chat_component>();
						return QString::fromStdString(chat->get_text());
					}
					case column::time:
					{
						char buffer[16];
						const auto time = entity->get<time_component>();
						const auto tm = localtime(&time->get_time());
						strftime(buffer, 16, "%I:%M:%S %p", tm);
						return QString((buffer[0] == '0') ? &buffer[1] : buffer);
					}
					case column::self:
					{
						return !entity->has<endpoint_component>();
					}
				}

				break;
			}
		}

		return QVariant();
	}

	int message_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		assert(thread_info::main());

		const auto chat_list = m_entity->get<chat_list_component>();
		return chat_list->size();
	}

	QHash<int, QByteArray> message_model::roleNames() const
	{
		static QHash<int, QByteArray> roles;
		roles[column::role_name] = "role_name";
		roles[column::role_color] = "role_color";
		roles[column::role_message] = "role_message";
		roles[column::role_time] = "role_time";
		roles[column::role_self] = "role_self";

		return roles;
	}

	// Get
	entity::ptr message_model::get_entity(const size_t row) const
	{
		assert(thread_info::main());

		const auto chat_list = m_entity->get<chat_list_component>();
		return (row < chat_list->size()) ? chat_list->at(row) : nullptr;
	}

	size_t message_model::get_row(const entity::ptr entity) const
	{
		assert(thread_info::main());

		// O(N)
		const auto chat_list = m_entity->get<chat_list_component>();
		const auto it = std::find(chat_list->begin(), chat_list->end(), entity);
		return (it != chat_list->end()) ? (it - chat_list->begin()) : npos;
	}
}
