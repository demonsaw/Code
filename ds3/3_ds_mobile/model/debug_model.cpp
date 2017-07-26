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
#include "entity/entity.h"
#include "model/debug_model.h"
#include "thread/thread_info.h"
#include "utility/value.h"

namespace eja
{
	// Slot
	void debug_model::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto error_list = m_entity->get<error_list_component>();

		if (error_list->size() >= ui::errors)
		{
			const auto row = ui::errors - 1;
			const auto it = error_list->cbegin() + row;

			beginRemoveRows(QModelIndex(), row, ui::errors - 1);
			error_list->erase(it, error_list->end());
			endRemoveRows();
		}

		const auto row = error_list->size();

		beginInsertRows(QModelIndex(), row, row);
		error_list->push_back(entity);
		endInsertRows();
	}

	void debug_model::on_clear()
	{
		assert(thread_info::main());
		
		const auto error_list = m_entity->get<error_list_component>();

		beginResetModel();
		error_list->clear();
		endResetModel();
	}

	// Utility
	bool debug_model::empty() const
	{
		assert(thread_info::main());

		const auto error_list = m_entity->get<error_list_component>();
		return error_list->empty();
	}

	// Model
	QVariant debug_model::data(const QModelIndex& index, int role) const
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
					case column::message:
					{
						const auto error = entity->get<error_component>();
						return QString::fromStdString(error->get_text());
					}
					case column::time:
					{
						char buffer[16];
						const auto error = entity->get<error_component>();
						const auto tm = localtime(&error->get_time());
						strftime(buffer, 16, "%I:%M:%S %p", tm);
						return QString(buffer);
					}
				}
				break;
			}		
		}

		return QVariant();
	}

	int debug_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		assert(thread_info::main());

		const auto error_list = m_entity->get<error_list_component>();
		return error_list->size();
	}

	QHash<int, QByteArray> debug_model::roleNames() const
	{

		static QHash<int, QByteArray> roles;		
		roles[column::role_message] = "role_message";
		roles[column::role_time] = "role_time";

		return roles;
	}

	// Get
	entity::ptr debug_model::get_entity(const size_t row) const
	{
		assert(thread_info::main());

		const auto error_list = m_entity->get<error_list_component>();
		return (row < error_list->size()) ? error_list->at(row) : nullptr;
	}

	size_t debug_model::get_row(const entity::ptr entity) const
	{
		assert(thread_info::main());

		const auto error_list = m_entity->get<error_list_component>();
		const auto it = std::find(error_list->cbegin(), error_list->cend(), entity);
		return (it != error_list->end()) ? (it - error_list->cbegin()) : npos;
	}
}
