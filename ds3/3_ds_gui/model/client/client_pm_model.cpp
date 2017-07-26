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
#include "component/endpoint_component.h"
#include "component/client/client_option_component.h"
#include "component/time/time_component.h"
#include "entity/entity.h"
#include "model/client/client_pm_model.h"
#include "thread/thread_info.h"
#include "utility/value.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Slot
	void client_pm_model::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto chat_list = m_entity->get<chat_list_component>();
		const auto row = chat_list->size();

		beginInsertRows(QModelIndex(), row, row);
		chat_list->push_back(entity);
		endInsertRows();
	}

	void client_pm_model::on_clear()
	{
		assert(thread_info::main());

		const auto chat_list = m_entity->get<chat_list_component>();

		beginResetModel();
		chat_list->clear();
		endResetModel();
	}

	// Utility
	bool client_pm_model::empty() const
	{
		assert(thread_info::main());

		const auto chat_list = m_entity->get<chat_list_component>();
		return chat_list->empty();
	}

	// Model
	bool client_pm_model::removeRows(int row, int count, const QModelIndex& parent /*= QModelIndex()*/)
	{
		assert(thread_info::main());

		const auto chat_list = m_entity->get<chat_list_component>();
		const auto it = chat_list->cbegin() + row;
		const auto size = chat_list->size();

		beginRemoveRows(QModelIndex(), row, (row + count - 1));
		chat_list->erase(it, it + count);
		endRemoveRows();

		return chat_list->size() < size;
	}

	Qt::ItemFlags client_pm_model::flags(const QModelIndex& index) const
	{
		assert(thread_info::main());

		if (!index.isValid())
			return Qt::ItemIsEnabled;

		Qt::ItemFlags flags = QAbstractItemModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemIsSelectable;

		switch (index.column())
		{
			case column::text:
			{
				flags |= Qt::ItemIsEditable;
				break;
			}
		}

		return flags;
	}

	QVariant client_pm_model::data(const QModelIndex& index, int role) const
	{
		assert(thread_info::main());

		if (!index.isValid())
			return QVariant();

		const size_t row = static_cast<size_t>(index.row());
		const auto entity = get_entity(row);
		if (!entity)
			return QVariant();

		const size_t col = static_cast<size_t>(index.column());
		const auto chat_list = m_entity->get<chat_list_component>();		

		switch (role)
		{
			case Qt::DisplayRole:
			{
				switch (col)
				{
					case column::time:
					case column::name:
					case column::text:
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
					case column::time:
					{
						const auto time = entity->get<time_component>();
						const auto option = m_entity->find<client_option_component>();

						char buff[32];
						const auto tm = localtime(&time->get_time());
						strftime(buff, 32, option->get_timestamp().c_str(), tm);
						return QString(buff);
					}
					case column::name:
					{
						const auto endpoint = entity->find<endpoint_component>();
						return QString::fromStdString(endpoint->get_name());
					}
					case column::text:
					{
						const auto chat = entity->get<chat_component>();
						return QString::fromStdString(chat->get_text());
					}
				}

				break;
			}
			case Qt::UserRole:
			{
				switch (col)
				{
					case column::time:
					{
						const auto time = entity->get<time_component>();
						return QVariant::fromValue<ulong>(time->get_time());
					}
					case column::name:
					{
						const auto endpoint = entity->find<endpoint_component>();
						return QString::fromStdString(endpoint->get_name()).toLower();
					}					
					case column::text:
					{
						const auto chat = entity->get<chat_component>();
						return QString::fromStdString(chat->get_text()).toLower();
					}
				}

				break;
			}
		}

		return QVariant();
	}

	int client_pm_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		assert(thread_info::main());

		const auto chat_list = m_entity->get<chat_list_component>();
		return chat_list->size();
	}

	// Get
	entity::ptr client_pm_model::get_entity(const size_t row) const
	{
		assert(thread_info::main());

		const auto chat_list = m_entity->get<chat_list_component>();
		return (row < chat_list->size()) ? chat_list->at(row) : nullptr;
	}

	size_t client_pm_model::get_row(const entity::ptr entity) const
	{
		assert(thread_info::main());

		// O(N)
		const auto chat_list = m_entity->get<chat_list_component>();
		const auto it = std::find(chat_list->begin(), chat_list->end(), entity);
		return (it != chat_list->end()) ? (it - chat_list->begin()) : npos;
	}
}
