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

#include "component/error_component.h"
#include "component/time/time_component.h"

#include "entity/entity.h"
#include "model/error_model.h"
#include "thread/thread_info.h"
#include "utility/value.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Slot
	void error_model::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto error_list = m_entity->get<error_list_component>();
		const auto row = error_list->size();

		beginInsertRows(QModelIndex(), row, row);
		error_list->push_back(entity);
		endInsertRows();

		// Max rows
		if (error_list->size() > ui::errors)
		{
			const auto count = error_list->size() - ui::errors;
			const auto it = error_list->begin() + count;

			beginRemoveRows(QModelIndex(), 0, count - 1);
			error_list->erase(error_list->begin(), it);
			endRemoveRows();
		}
	}

	void error_model::on_clear()
	{
		assert(thread_info::main());
		
		const auto error_list = m_entity->get<error_list_component>();

		beginResetModel();
		error_list->clear();
		endResetModel();
	}

	// Utility
	bool error_model::empty() const
	{
		assert(thread_info::main());

		const auto error_list = m_entity->get<error_list_component>();
		return error_list->empty();
	}

	// Model
	bool error_model::removeRows(int row, int count, const QModelIndex& index /*= QModelIndex()*/)
	{
		assert(thread_info::main());
		
		const auto error_list = m_entity->get<error_list_component>();
		const auto it = error_list->begin() + row;

		beginRemoveRows(index, row, (row + count - 1));
		error_list->erase(it, it + count);
		endRemoveRows();

		return true;
	}	

	Qt::ItemFlags error_model::flags(const QModelIndex& index) const
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

	QVariant error_model::data(const QModelIndex& index, int role) const
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
					case column::time:
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
						const auto error = entity->get<error_component>();
						return QString::fromStdString(error->get_text());
					}
				}

				break;
			}
			case Qt::UserRole:
			{
				switch (index.column())
				{
					case column::row:
					{
						return QVariant::fromValue(index.row());
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

	int error_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		assert(thread_info::main());

		const auto error_list = m_entity->get<error_list_component>();
		return error_list->size();
	}

	// Get
	entity::ptr error_model::get_entity(const size_t row) const
	{
		assert(thread_info::main());

		const auto error_list = m_entity->get<error_list_component>();
		return (row < error_list->size()) ? error_list->at(row) : nullptr;
	}

	size_t error_model::get_row(const entity::ptr& entity) const
	{
		assert(thread_info::main());

		const auto error_list = m_entity->get<error_list_component>();
		const auto it = std::find(error_list->begin(), error_list->end(), entity);
		return (it != error_list->end()) ? (it - error_list->begin()) : npos;
	}
}
