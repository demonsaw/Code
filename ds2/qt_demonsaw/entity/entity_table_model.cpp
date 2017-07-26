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

#include <QSize>

#include "entity_table_model.h"

namespace eja
{
	// Slot
	void entity_table_model::on_add()
	{ 
		assert(thread::main());

		insertRows(rowCount(), 1); 
	}
	
	void entity_table_model::on_add(const size_t row)
	{ 
		assert(thread::main());
		
		insertRows(row, 1);
	}

	void entity_table_model::on_add(const QModelIndex& index) 
	{ 
		assert(thread::main());

		insertRows(index.row(), 1);
	}

	void entity_table_model::on_remove() 
	{ 
		assert(thread::main());

		removeRows(0, rowCount());
	}

	void entity_table_model::on_remove(const size_t row) 
	{ 
		assert(thread::main());

		removeRows(row, 1);
	}

	void entity_table_model::on_remove(const QModelIndex& index) 
	{ 
		assert(thread::main());

		removeRows(index.row(), 1);
	}

	void entity_table_model::on_refresh(const entity::ptr entity)
	{
		if (entity)
		{
			const auto row = get_row(entity);
			if (row == type::npos)
				return;

			const auto start = createIndex(row, 0);
			const auto end = createIndex(row, columnCount() - 1);
			emit dataChanged(start, end);
		}
		else
		{
			const auto start = createIndex(0, 0);
			const auto end = createIndex(rowCount(), columnCount() - 1);
			emit dataChanged(start, end);
		}
	}

	// Model
	QVariant entity_table_model::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
	{
		if (section >= 0)
		{
			switch (orientation)
			{
				case Qt::Horizontal:
				{
					switch (role)
					{
						case Qt::DisplayRole:
						case Qt::UserRole:
						{
							const size_t col = static_cast<size_t>(section);
							return QString::fromStdString(m_column_names[col]);
						}
						case Qt::TextAlignmentRole:
						{
							return static_cast<int>(Qt::AlignVCenter | Qt::AlignLeft);
						}
					}

					break;
				}
				case Qt::Vertical:
				{
					switch (role)
					{
						case Qt::TextAlignmentRole:
						{
							return static_cast<int>(Qt::AlignVCenter | Qt::AlignRight);
						}
					}

					break;
				}
			}
		}

		return entity_model::headerData(section, orientation, role);
	}

	// Mutator
	void entity_table_model::set_entity(const entity::ptr entity)
	{
		beginResetModel();

		entity_callback::set_entity(entity);
		
		endResetModel();
	}
}
