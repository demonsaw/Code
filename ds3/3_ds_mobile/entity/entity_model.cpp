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

#include "entity/entity.h"
#include "entity/entity_model.h"
#include "thread/thread_info.h"

namespace eja
{
	// Constructor
	entity_model::entity_model(const entity::ptr entity, QObject* parent /*= 0*/) : QAbstractItemModel(parent), entity_type(entity)
	{		
		// Signal
		connect(this, static_cast<void (entity_model::*)()>(&entity_model::create), this, static_cast<void (entity_model::*)()>(&entity_model::on_create));
		connect(this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::create), this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::on_create));
		connect(this, static_cast<void (entity_model::*)(const entity::ptr)>(&entity_model::create), this, static_cast<void (entity_model::*)(const entity::ptr)>(&entity_model::on_create));

		connect(this, static_cast<void (entity_model::*)()>(&entity_model::destroy), this, static_cast<void (entity_model::*)()>(&entity_model::on_destroy));
		connect(this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::destroy), this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::on_destroy));
		connect(this, static_cast<void (entity_model::*)(const entity::ptr)>(&entity_model::destroy), this, static_cast<void (entity_model::*)(const entity::ptr)>(&entity_model::on_destroy));

		connect(this, static_cast<void (entity_model::*)()>(&entity_model::init), this, static_cast<void (entity_model::*)()>(&entity_model::on_init));
		connect(this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::init), this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::on_init));
		connect(this, static_cast<void (entity_model::*)(const entity::ptr)>(&entity_model::init), this, static_cast<void (entity_model::*)(const entity::ptr)>(&entity_model::on_init));

		connect(this, static_cast<void (entity_model::*)()>(&entity_model::update), this, static_cast<void (entity_model::*)()>(&entity_model::on_update));
		connect(this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::update), this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::on_update));
		connect(this, static_cast<void (entity_model::*)(const entity::ptr)>(&entity_model::update), this, static_cast<void (entity_model::*)(const entity::ptr)>(&entity_model::on_update));

		connect(this, static_cast<void (entity_model::*)()>(&entity_model::shutdown), this, static_cast<void (entity_model::*)()>(&entity_model::on_shutdown));
		connect(this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::shutdown), this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::on_shutdown));
		connect(this, static_cast<void (entity_model::*)(const entity::ptr)>(&entity_model::shutdown), this, static_cast<void (entity_model::*)(const entity::ptr)>(&entity_model::on_shutdown));

		connect(this, static_cast<void (entity_model::*)()>(&entity_model::add), this, static_cast<void (entity_model::*)()>(&entity_model::on_add));
		connect(this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::add), this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::on_add));
		connect(this, static_cast<void (entity_model::*)(const entity::ptr)>(&entity_model::add), this, static_cast<void (entity_model::*)(const entity::ptr)>(&entity_model::on_add));

		connect(this, static_cast<void (entity_model::*)()>(&entity_model::remove), this, static_cast<void (entity_model::*)()>(&entity_model::on_remove));
		connect(this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::remove), this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::on_remove));
		connect(this, static_cast<void (entity_model::*)(const entity::ptr)>(&entity_model::remove), this, static_cast<void (entity_model::*)(const entity::ptr)>(&entity_model::on_remove));

		connect(this, static_cast<void (entity_model::*)()>(&entity_model::clear), this, static_cast<void (entity_model::*)()>(&entity_model::on_clear));
		connect(this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::clear), this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::on_clear));
		connect(this, static_cast<void (entity_model::*)(const entity::ptr)>(&entity_model::clear), this, static_cast<void (entity_model::*)(const entity::ptr)>(&entity_model::on_clear));

		connect(this, static_cast<void (entity_model::*)()>(&entity_model::reset), this, static_cast<void (entity_model::*)()>(&entity_model::on_reset)); 
		connect(this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::click), this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::on_click));
		connect(this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::double_click), this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::on_double_click));
		connect(this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::execute), this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::on_execute));		
	}

	// Utility
	void entity_model::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		if (entity)
		{
			const auto index = get_index(entity);
			if (index.isValid())
				on_add(index);
		}
		else
		{
			on_add();
		}
	}

	void entity_model::on_remove(const entity::ptr entity)
	{
		assert(thread_info::main());

		if (entity)
		{
			const auto index = get_index(entity);
			if (index.isValid())
				on_remove(index);
		}
		else
		{
			on_remove();
		}
	}

	void entity_model::on_update(const entity::ptr entity)
	{
		assert(thread_info::main());

		if (entity)
		{
			const auto index = get_index(entity);
			if (index.isValid())
			{
				const auto start = createIndex(index.row(), 0);
				const auto end = createIndex(index.row(), columnCount() - 1);
				emit dataChanged(start, end);
			}
		}
		else
		{
			const auto start = createIndex(0, 0);
			const auto end = createIndex(rowCount(), columnCount() - 1);
			emit dataChanged(start, end);
		}
	}

	void entity_model::on_update()
	{
		assert(thread_info::main());

		const auto start = createIndex(0, 0);
		const auto end = createIndex(rowCount(), columnCount() - 1);
		emit dataChanged(start, end);
	}

	void entity_model::on_update(const QModelIndex& index)
	{
		assert(thread_info::main());

		const auto start = createIndex(index.row(), 0);
		const auto end = createIndex(index.row(), columnCount() - 1);
		emit dataChanged(start, end);
	}

	void entity_model::on_reset()
	{
		assert(thread_info::main());

		beginResetModel();

		//

		endResetModel();
	}

	// Model
	Qt::ItemFlags entity_model::flags(const QModelIndex& index) const
	{
		assert(thread_info::main());

		if (!index.isValid())
			return Qt::ItemIsEnabled;

		return QAbstractItemModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	}	

	// Set
	void entity_model::set_entity()
	{
		assert(thread_info::main());

		if (has_entity())
		{
			beginResetModel();

			entity_type::set_entity();

			endResetModel();
		}		
	}

	void entity_model::set_entity(const entity::ptr entity)
	{
		assert(thread_info::main());

		if (m_entity != entity)
		{
			beginResetModel();

			entity_type::set_entity(entity);

			endResetModel();
		}		
	}
}
