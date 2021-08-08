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

#include <QDataStream>
#include <QMimeData>

#include "entity/entity.h"
#include "entity/entity_model.h"
#include "resource/resource.h"
#include "thread/thread_info.h"

namespace eja
{
	// Constructor
	entity_model::entity_model(const entity::ptr& entity, QObject* parent /*= nullptr*/) : QAbstractItemModel(parent), entity_type(entity)
	{
		// Signal
		connect(this, static_cast<void (entity_model::*)()>(&entity_model::create), this, static_cast<void (entity_model::*)()>(&entity_model::on_create));
		connect(this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::create), this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::on_create));
		connect(this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::create), this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::on_create));
		connect(this, static_cast<void (entity_model::*)(const entity::ptr entity)>(&entity_model::create), this, static_cast<void (entity_model::*)(const entity::ptr entity)>(&entity_model::on_create));

		connect(this, static_cast<void (entity_model::*)()>(&entity_model::destroy), this, static_cast<void (entity_model::*)()>(&entity_model::on_destroy));
		connect(this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::destroy), this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::on_destroy));
		connect(this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::destroy), this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::on_destroy));
		connect(this, static_cast<void (entity_model::*)(const entity::ptr entity)>(&entity_model::destroy), this, static_cast<void (entity_model::*)(const entity::ptr entity)>(&entity_model::on_destroy));

		connect(this, static_cast<void (entity_model::*)()>(&entity_model::init), this, static_cast<void (entity_model::*)()>(&entity_model::on_init));
		connect(this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::init), this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::on_init));
		connect(this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::init), this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::on_init));
		connect(this, static_cast<void (entity_model::*)(const entity::ptr entity)>(&entity_model::init), this, static_cast<void (entity_model::*)(const entity::ptr entity)>(&entity_model::on_init));

		connect(this, static_cast<void (entity_model::*)()>(&entity_model::shutdown), this, static_cast<void (entity_model::*)()>(&entity_model::on_shutdown));
		connect(this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::shutdown), this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::on_shutdown));
		connect(this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::shutdown), this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::on_shutdown));
		connect(this, static_cast<void (entity_model::*)(const entity::ptr entity)>(&entity_model::shutdown), this, static_cast<void (entity_model::*)(const entity::ptr entity)>(&entity_model::on_shutdown));

		connect(this, static_cast<void (entity_model::*)()>(&entity_model::update), this, static_cast<void (entity_model::*)()>(&entity_model::on_update));
		connect(this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::update), this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::on_update));
		connect(this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::update), this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::on_update));
		connect(this, static_cast<void (entity_model::*)(const entity::ptr entity)>(&entity_model::update), this, static_cast<void (entity_model::*)(const entity::ptr entity)>(&entity_model::on_update));

		connect(this, static_cast<void (entity_model::*)()>(&entity_model::add), this, static_cast<void (entity_model::*)()>(&entity_model::on_add));
		connect(this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::add), this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::on_add));
		connect(this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::add), this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::on_add));
		connect(this, static_cast<void (entity_model::*)(const entity::ptr entity)>(&entity_model::add), this, static_cast<void (entity_model::*)(const entity::ptr entity)>(&entity_model::on_add));
		connect(this, static_cast<void (entity_model::*)(const entity::ptr entity, const size_t row)>(&entity_model::add), this, static_cast<void (entity_model::*)(const entity::ptr entity, const size_t row)>(&entity_model::on_add));

		connect(this, static_cast<void (entity_model::*)()>(&entity_model::remove), this, static_cast<void (entity_model::*)()>(&entity_model::on_remove));
		connect(this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::remove), this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::on_remove));
		connect(this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::remove), this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::on_remove));
		connect(this, static_cast<void (entity_model::*)(const entity::ptr entity)>(&entity_model::remove), this, static_cast<void (entity_model::*)(const entity::ptr entity)>(&entity_model::on_remove));

		connect(this, static_cast<void (entity_model::*)()>(&entity_model::clear), this, static_cast<void (entity_model::*)()>(&entity_model::on_clear));
		connect(this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::clear), this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::on_clear));
		connect(this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::clear), this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::on_clear));
		connect(this, static_cast<void (entity_model::*)(const entity::ptr entity)>(&entity_model::clear), this, static_cast<void (entity_model::*)(const entity::ptr entity)>(&entity_model::on_clear));

		connect(this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::click), this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::on_click));
		connect(this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::click), this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::on_click));
		connect(this, static_cast<void (entity_model::*)(const entity::ptr entity)>(&entity_model::click), this, static_cast<void (entity_model::*)(const entity::ptr entity)>(&entity_model::on_click));

		connect(this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::double_click), this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::on_double_click));
		connect(this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::double_click), this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::on_double_click));
		connect(this, static_cast<void (entity_model::*)(const entity::ptr entity)>(&entity_model::double_click), this, static_cast<void (entity_model::*)(const entity::ptr entity)>(&entity_model::on_double_click));

		connect(this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::execute), this, static_cast<void (entity_model::*)(const size_t row)>(&entity_model::on_execute));
		connect(this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::execute), this, static_cast<void (entity_model::*)(const QModelIndex& index)>(&entity_model::on_execute));
		connect(this, static_cast<void (entity_model::*)(const entity::ptr entity)>(&entity_model::execute), this, static_cast<void (entity_model::*)(const entity::ptr entity)>(&entity_model::on_execute));
	}

	entity_model::entity_model(column_names&& names, QObject* parent /*= nullptr*/) : entity_model(parent)
	{ 
		set_column_names(std::forward<column_names>(names));
	}
	
	entity_model::entity_model(column_names&& names, column_sizes&& sizes, QObject* parent /*= nullptr*/) : entity_model(parent)
	{
		set_column_names(std::forward<column_names>(names));
		set_column_sizes(std::forward<column_sizes>(sizes));
	}

	entity_model::entity_model(const column_names& names, QObject* parent /*= nullptr*/) : entity_model(parent)
	{
		m_column_names = names;
	}

	entity_model::entity_model(const column_names& names, const column_sizes& sizes, QObject* parent /*= nullptr*/) : entity_model(parent)
	{
		set_column_names(names);
		set_column_sizes(sizes);
	}

	entity_model::entity_model(const entity::ptr& entity, column_names&& names, QObject* parent /*= nullptr*/) : entity_model(entity, parent)
	{
		set_column_names(std::forward<column_names>(names));
	}

	entity_model::entity_model(const entity::ptr& entity, column_names&& names, column_sizes&& sizes, QObject* parent /*= nullptr*/) : entity_model(entity, parent)
	{
		set_column_names(std::forward<column_names>(names));
		set_column_sizes(std::forward<column_sizes>(sizes));
	}

	entity_model::entity_model(const entity::ptr& entity, const column_names& names, QObject* parent /*= nullptr*/) : entity_model(entity, parent)
	{
		set_column_names(names);
	}

	entity_model::entity_model(const entity::ptr& entity, const column_names& names, const column_sizes& sizes, QObject* parent /*= nullptr*/) : entity_model(entity, parent)
	{
		set_column_names(names);
		set_column_sizes(sizes);
	}

	// Interface
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

	void entity_model::on_update(const size_t row)
	{
		assert(thread_info::main());

		const auto start = createIndex(row, 0);
		const auto end = createIndex(row, columnCount() - 1);
		emit dataChanged(start, end);
	}

	void entity_model::on_update(const QModelIndex& index)
	{
		assert(thread_info::main());

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

	QVariant entity_model::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
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
							return m_column_names[col];
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

		return QAbstractItemModel::headerData(section, orientation, role);
	}


	QStringList entity_model::mimeTypes() const
	{
		assert(thread_info::main());

		QStringList types;
		types << "application/unknown";

		return types;
	}

	QMimeData* entity_model::mimeData(const QModelIndexList &indexes) const
	{
		assert(thread_info::main());

		QByteArray encoded;
		QDataStream stream(&encoded, QIODevice::WriteOnly);

		for (auto i = 0; i < indexes.size(); i += columnCount())
		{
			const auto index = indexes[i];

			if (index.isValid())
			{
				const auto entity = get_entity(index);
				if (entity)
				{
					const auto psz = reinterpret_cast<char*>(entity.get());
					stream.writeRawData(psz, sizeof(void*));
				}
			}
		}

		QMimeData* data = new QMimeData();
		data->setData("application/unknown", encoded);

		return data;
	}

	Qt::DropActions entity_model::supportedDropActions() const
	{
		assert(thread_info::main());

		return Qt::CopyAction | Qt::MoveAction;
	}

	bool entity_model::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
	{
		assert(thread_info::main());

		if (!data->hasFormat("application/unknown"))
			return false;

		if (action == Qt::IgnoreAction)
			return true;

		size_t endRow = 0;
		if (!parent.isValid() && (row < 0))
			endRow = rowCount();
		else if (!parent.isValid())
			endRow = std::min(row, rowCount());
		else
			endRow = parent.row();

		QByteArray encoded = data->data("application/unknown");
		QDataStream stream(&encoded, QIODevice::ReadOnly);

		while (!stream.atEnd())
		{
			entity* ptr = nullptr;
			stream.readRawData(reinterpret_cast<char*>(&ptr), sizeof(void*));
			if (ptr)
			{
				const auto entity = ptr->shared_from_this();
				on_add(entity, endRow++);
			}
		}

		return true;
	}

	// Set	
	void entity_model::set_entity(const entity::ptr& entity)
	{
		beginResetModel();

		entity_type::set_entity(entity);

		endResetModel();
	}

	void entity_model::set_entity()
	{
		beginResetModel();

		entity_type::clear();

		endResetModel();
	}

	void entity_model::set_column_sizes(column_sizes&& sizes)
	{ 
		m_column_sizes = std::forward<column_sizes>(sizes);

		// Ratio
		for (auto& size : m_column_sizes)
			size *= resource::get_ratio();
	}

	void entity_model::set_column_sizes(const column_sizes& sizes)
	{ 
		m_column_sizes = sizes; 

		// Ratio
		for (auto& size : m_column_sizes)
			size *= resource::get_ratio();
	}
}
