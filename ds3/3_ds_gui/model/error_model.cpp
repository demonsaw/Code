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
	bool error_model::removeRows(int row, int count, const QModelIndex& parent /*= QModelIndex()*/)
	{
		assert(thread_info::main());
		
		const auto error_list = m_entity->get<error_list_component>();
		const auto it = error_list->cbegin() + row;
		const auto size = error_list->size();

		beginRemoveRows(QModelIndex(), row, (row + count - 1));
		error_list->erase(it, it + count);
		endRemoveRows();

		return error_list->size() < size;
	}
	

	Qt::ItemFlags error_model::flags(const QModelIndex& index) const
	{
		assert(thread_info::main());

		if (!index.isValid())
			return Qt::ItemIsEnabled;

		Qt::ItemFlags flags = QAbstractItemModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemIsSelectable;

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

	QVariant error_model::data(const QModelIndex& index, int role) const
	{
		assert(thread_info::main());

		if (!index.isValid())
			return QVariant();

		const size_t row = static_cast<size_t>(index.row());
		const auto entity = get_entity(row);
		if (!entity)
			return QVariant();

		const size_t col = static_cast<size_t>(index.column());
		const auto error = entity->get<error_component>();

		switch (role)
		{
			case Qt::DisplayRole:
			{				
				switch (col)
				{
					case column::index:
					{
						return QVariant::fromValue(index.row() + 1);
					}
					case column::time:
					{
						char buffer[16];
						const auto tm = localtime(&error->get_time());
						strftime(buffer, 16, "%I:%M:%S %p", tm);
						return QString(buffer);
					}
					case column::message:
					{
						return QString::fromStdString(error->get_text());
					}
				}
				break;
			}
			case Qt::EditRole:
			{
				switch (col)
				{
					case column::message:
					{
						return QString::fromStdString(error->get_text());
					}
				}

				break;
			}
			case Qt::UserRole:
			{
				switch (col)
				{
					case column::message:
					{
						const auto variant = data(index, Qt::DisplayRole);
						return variant.toString().toLower();
					}
					default:
					{
						return data(index, Qt::DisplayRole);
					}
				}
			}
			case Qt::TextAlignmentRole:
			{
				switch (col)
				{
					case column::index:
					{
						return static_cast<int>(Qt::AlignVCenter | Qt::AlignRight);
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

	size_t error_model::get_row(const entity::ptr entity) const
	{
		assert(thread_info::main());

		const auto error_list = m_entity->get<error_list_component>();
		const auto it = std::find(error_list->cbegin(), error_list->cend(), entity);
		return (it != error_list->end()) ? (it - error_list->cbegin()) : npos;
	}
}
