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

#include "entity_model.h"

namespace eja
{
	// Interface
	void entity_model::init()
	{
		// Signal
		connect(this, static_cast<void (entity_model::*)()>(&entity_model::added), this, static_cast<void (entity_model::*)()>(&entity_model::on_add));
		connect(this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::added), this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::on_add));
		connect(this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::added), this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::on_add));
		connect(this, static_cast<void (entity_model::*)(const entity::ptr)>(&entity_model::added), this, static_cast<void (entity_model::*)(const entity::ptr)>(&entity_model::on_add));

		connect(this, static_cast<void (entity_model::*)()>(&entity_model::removed), this, static_cast<void (entity_model::*)()>(&entity_model::on_remove));
		connect(this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::removed), this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::on_remove));
		connect(this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::removed), this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::on_remove));
		connect(this, static_cast<void (entity_model::*)(const entity::ptr)>(&entity_model::removed), this, static_cast<void (entity_model::*)(const entity::ptr)>(&entity_model::on_remove));
		
		connect(this, static_cast<void (entity_model::*)()>(&entity_model::refreshed), this, static_cast<void (entity_model::*)()>(&entity_model::on_refresh));
		connect(this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::refreshed), this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::on_refresh));
		connect(this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::refreshed), this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::on_refresh));
		connect(this, static_cast<void (entity_model::*)(const entity::ptr)>(&entity_model::refreshed), this, static_cast<void (entity_model::*)(const entity::ptr)>(&entity_model::on_refresh));

		connect(this, static_cast<void (entity_model::*)()>(&entity_model::cleared), this, static_cast<void (entity_model::*)()>(&entity_model::on_clear));
		connect(this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::cleared), this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::on_clear));
		connect(this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::cleared), this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::on_clear));
		connect(this, static_cast<void (entity_model::*)(const entity::ptr)>(&entity_model::cleared), this, static_cast<void (entity_model::*)(const entity::ptr)>(&entity_model::on_clear));

		connect(this, static_cast<void (entity_model::*)()>(&entity_model::clicked), this, static_cast<void (entity_model::*)()>(&entity_model::on_click)); 
		connect(this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::clicked), this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::on_click));
		connect(this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::clicked), this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::on_click));
		connect(this, static_cast<void (entity_model::*)(const entity::ptr)>(&entity_model::clicked), this, static_cast<void (entity_model::*)(const entity::ptr)>(&entity_model::on_click));

		connect(this, static_cast<void (entity_model::*)()>(&entity_model::double_clicked), this, static_cast<void (entity_model::*)()>(&entity_model::on_double_click)); 
		connect(this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::double_clicked), this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::on_double_click));
		connect(this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::double_clicked), this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::on_double_click));
		connect(this, static_cast<void (entity_model::*)(const entity::ptr)>(&entity_model::double_clicked), this, static_cast<void (entity_model::*)(const entity::ptr)>(&entity_model::on_double_click));

		connect(this, static_cast<void (entity_model::*)()>(&entity_model::executed), this, static_cast<void (entity_model::*)()>(&entity_model::on_execute));
		connect(this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::executed), this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::on_execute));
		connect(this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::executed), this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::on_execute));
		connect(this, static_cast<void (entity_model::*)(const entity::ptr)>(&entity_model::executed), this, static_cast<void (entity_model::*)(const entity::ptr)>(&entity_model::on_execute));
	}

	// Slot
	void entity_model::on_refresh()
	{
		assert(thread::main());

		const auto start = createIndex(0, 0);
		const auto end = createIndex(rowCount(), columnCount() - 1);
		emit dataChanged(start, end);
	}

	void entity_model::on_refresh(const size_t row)
	{
		assert(thread::main());

		const auto start = createIndex(row, 0);
		const auto end = createIndex(row, columnCount() - 1);
		emit dataChanged(start, end);
	}

	void entity_model::on_refresh(const QModelIndex& index)
	{
		assert(thread::main());

		const auto start = createIndex(index.row(), 0);
		const auto end = createIndex(index.row(), columnCount() - 1);
		emit dataChanged(start, end);
	}

	// Model
	Qt::ItemFlags entity_model::flags(const QModelIndex& index) const
	{
		if (!index.isValid())
			return Qt::ItemIsEnabled;

		return QAbstractItemModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	}
}
