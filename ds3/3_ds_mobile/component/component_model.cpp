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

#include "component/component_model.h"
#include "entity/entity.h"
#include "thread/thread_info.h"

namespace eja
{
	// Constructor
	component_model::component_model(const entity::ptr entity, QObject* parent /*= 0*/) : QAbstractItemModel(parent), entity_type(entity)
	{
		// Signal
		connect(this, static_cast<void (component_model::*)()>(&component_model::create), this, static_cast<void (component_model::*)()>(&component_model::on_create));
		connect(this, static_cast<void (component_model::*)(const QModelIndex& index)>(&component_model::create), this, static_cast<void (component_model::*)(const QModelIndex& index)>(&component_model::on_create));
		connect(this, static_cast<void (component_model::*)(const entity::ptr)>(&component_model::create), this, static_cast<void (component_model::*)(const entity::ptr)>(&component_model::on_create));

		connect(this, static_cast<void (component_model::*)()>(&component_model::destroy), this, static_cast<void (component_model::*)()>(&component_model::on_destroy));
		connect(this, static_cast<void (component_model::*)(const QModelIndex& index)>(&component_model::destroy), this, static_cast<void (component_model::*)(const QModelIndex& index)>(&component_model::on_destroy));
		connect(this, static_cast<void (component_model::*)(const entity::ptr)>(&component_model::destroy), this, static_cast<void (component_model::*)(const entity::ptr)>(&component_model::on_destroy));

		connect(this, static_cast<void (component_model::*)()>(&component_model::init), this, static_cast<void (component_model::*)()>(&component_model::on_init));
		connect(this, static_cast<void (component_model::*)(const QModelIndex& index)>(&component_model::init), this, static_cast<void (component_model::*)(const QModelIndex& index)>(&component_model::on_init));
		connect(this, static_cast<void (component_model::*)(const entity::ptr)>(&component_model::init), this, static_cast<void (component_model::*)(const entity::ptr)>(&component_model::on_init));

		connect(this, static_cast<void (component_model::*)()>(&component_model::update), this, static_cast<void (component_model::*)()>(&component_model::on_update));
		connect(this, static_cast<void (component_model::*)(const QModelIndex& index)>(&component_model::update), this, static_cast<void (component_model::*)(const QModelIndex& index)>(&component_model::on_update));
		connect(this, static_cast<void (component_model::*)(const entity::ptr)>(&component_model::update), this, static_cast<void (component_model::*)(const entity::ptr)>(&component_model::on_update));

		connect(this, static_cast<void (component_model::*)()>(&component_model::shutdown), this, static_cast<void (component_model::*)()>(&component_model::on_shutdown));
		connect(this, static_cast<void (component_model::*)(const QModelIndex& index)>(&component_model::shutdown), this, static_cast<void (component_model::*)(const QModelIndex& index)>(&component_model::on_shutdown));
		connect(this, static_cast<void (component_model::*)(const entity::ptr)>(&component_model::shutdown), this, static_cast<void (component_model::*)(const entity::ptr)>(&component_model::on_shutdown));

		connect(this, static_cast<void (component_model::*)()>(&component_model::add), this, static_cast<void (component_model::*)()>(&component_model::on_add));
		connect(this, static_cast<void (component_model::*)(const QModelIndex& index)>(&component_model::add), this, static_cast<void (component_model::*)(const QModelIndex& index)>(&component_model::on_add));
		connect(this, static_cast<void (component_model::*)(const entity::ptr)>(&component_model::add), this, static_cast<void (component_model::*)(const entity::ptr)>(&component_model::on_add));

		connect(this, static_cast<void (component_model::*)()>(&component_model::remove), this, static_cast<void (component_model::*)()>(&component_model::on_remove));
		connect(this, static_cast<void (component_model::*)(const QModelIndex& index)>(&component_model::remove), this, static_cast<void (component_model::*)(const QModelIndex& index)>(&component_model::on_remove));
		connect(this, static_cast<void (component_model::*)(const entity::ptr)>(&component_model::remove), this, static_cast<void (component_model::*)(const entity::ptr)>(&component_model::on_remove));

		connect(this, static_cast<void (component_model::*)()>(&component_model::clear), this, static_cast<void (component_model::*)()>(&component_model::on_clear));
		connect(this, static_cast<void (component_model::*)(const QModelIndex& index)>(&component_model::clear), this, static_cast<void (component_model::*)(const QModelIndex& index)>(&component_model::on_clear));
		connect(this, static_cast<void (component_model::*)(const entity::ptr)>(&component_model::clear), this, static_cast<void (component_model::*)(const entity::ptr)>(&component_model::on_clear));

		connect(this, static_cast<void (component_model::*)(const QModelIndex& index)>(&component_model::click), this, static_cast<void (component_model::*)(const QModelIndex& index)>(&component_model::on_click));
		connect(this, static_cast<void (component_model::*)(const QModelIndex& index)>(&component_model::double_click), this, static_cast<void (component_model::*)(const QModelIndex& index)>(&component_model::on_double_click));
		connect(this, static_cast<void (component_model::*)(const QModelIndex& index)>(&component_model::execute), this, static_cast<void (component_model::*)(const QModelIndex& index)>(&component_model::on_execute));
	}

	// Utility
	void component_model::on_add(const entity::ptr entity)
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

	void component_model::on_remove(const entity::ptr entity)
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

	void component_model::on_update(const entity::ptr entity)
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

	void component_model::on_update()
	{
		assert(thread_info::main());

		const auto start = createIndex(0, 0);
		const auto end = createIndex(rowCount(), columnCount() - 1);
		emit dataChanged(start, end);
	}

	void component_model::on_update(const QModelIndex& index)
	{
		assert(thread_info::main());

		const auto start = createIndex(index.row(), 0);
		const auto end = createIndex(index.row(), columnCount() - 1);
		emit dataChanged(start, end);
	}

	// Model
	Qt::ItemFlags component_model::flags(const QModelIndex& index) const
	{
		if (!index.isValid())
			return Qt::ItemIsEnabled;

		return QAbstractItemModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	}	

	// Set
	void component_model::set_entity()
	{
		beginResetModel();

		entity_type::set_entity();

		endResetModel();
	}

	void component_model::set_entity(const entity::ptr entity)
	{
		beginResetModel();

		entity_type::set_entity(entity);

		endResetModel();
	}
}
