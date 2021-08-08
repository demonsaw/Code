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
#include "component/callback/callback.h"
#include "component/client/client_component.h"
#include "component/router/router_component.h"

#include "entity/entity.h"
#include "model/chat_model.h"
#include "thread/thread_info.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Slot
	void chat_model::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto chat_list = m_entity->get<chat_list_component>();
		chat_list->add();

		if (!chat_list->empty())
		{
			// Add
			const auto owner = entity->get_owner();
			const auto e = chat_list->back();

			if (!e->is_owner(owner) || e->has<router_component>() || entity->has<router_component>())
			{
				const auto row = chat_list->size();

				beginInsertRows(QModelIndex(), row, row);
				chat_list->push_back(entity);
				endInsertRows();

				// Size
				const auto chat = entity->get<chat_component>();
				chat->add();
			}
			else
			{
				// Remove
				const auto e = chat_list->back();
				const auto row = chat_list->size() - 1;
				on_remove(row);

				// Update		
				const auto c = e->get<chat_component>();
				auto& text = c->get_text();
				text += "\n";

				const auto chat = entity->get<chat_component>();
				text += chat->get_text();

				// Add
				beginInsertRows(QModelIndex(), row, row);
				chat_list->push_back(e);
				endInsertRows();

				// Size
				chat_list->add(c->get_size());
				c->add();
			}
		}
		else
		{
			beginInsertRows(QModelIndex(), 0, 0);
			chat_list->push_back(entity);
			endInsertRows();

			// Size
			const auto chat = entity->get<chat_component>();
			chat->add();
		}

		// Callback
		m_entity->async_call(callback::statusbar | callback::add);
	}

	void chat_model::on_clear()
	{
		assert(thread_info::main());

		const auto chat_list = m_entity->get<chat_list_component>();

		beginResetModel();
		chat_list->clear();
		endResetModel();

		// Callback
		m_entity->async_call(callback::statusbar | callback::remove);
	}

	// Utility
	bool chat_model::empty() const
	{
		assert(thread_info::main());

		const auto chat_list = m_entity->get<chat_list_component>();
		return chat_list->empty();
	}

	// Model
	bool chat_model::removeRows(int row, int count, const QModelIndex& index /*= QModelIndex()*/)
	{
		assert(thread_info::main());

		const auto chat_entity = get_entity(row);
		if (chat_entity)
		{			
			const auto chat_list = m_entity->get<chat_list_component>();
			const auto it = chat_list->begin() + row;

			beginRemoveRows(index, row, (row + count - 1));
			chat_list->erase(it, it + count);
			endRemoveRows();

			// Size
			const auto chat = chat_entity->get<chat_component>();
			chat_list->remove(chat->get_size());

			// Callback
			m_entity->async_call(callback::statusbar | callback::remove);

			return true;
		}

		return false;
	}

	Qt::ItemFlags chat_model::flags(const QModelIndex& index) const
	{
		assert(thread_info::main());

		if (index.isValid())
		{
			auto flags = QAbstractItemModel::flags(index);

			switch (index.column())
			{
				case column::message:
				{
					flags |= Qt::ItemIsEditable;
					break;
				}
			}

			return flags;
		}		

		return Qt::ItemIsEnabled;
	}

	QVariant chat_model::data(const QModelIndex& index, int role) const
	{
		assert(thread_info::main());

		const auto entity = get_entity(index);
		if (!entity)
			return QVariant();

		switch (role)
		{
			case Qt::DisplayRole:
			{
				switch (index.column())
				{
					case column::row:
					case column::message:
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
					case column::message:
					{
						const auto chat = entity->get<chat_component>();
						return QString::fromStdString(chat->get_text());
					}
				}

				break;
			}
		}

		return QVariant();
	}

	int chat_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		assert(thread_info::main());

		const auto chat_list = m_entity->get<chat_list_component>();
		return chat_list->size();
	}

	// Get
	entity::ptr chat_model::get_entity(const size_t row) const
	{
		assert(thread_info::main());

		const auto chat_list = m_entity->get<chat_list_component>();
		return (row < chat_list->size()) ? chat_list->at(row) : nullptr;
	}

	size_t chat_model::get_row(const entity::ptr& entity) const
	{
		assert(thread_info::main());

		// O(N) - Optimized to search back-to-front
		const auto chat_list = m_entity->get<chat_list_component>();
		const auto it = std::find(chat_list->crbegin(), chat_list->crend(), entity);
		return (it != chat_list->crend()) ? (chat_list->size() - 1 - (it - chat_list->crbegin())) : npos;
	}
}
