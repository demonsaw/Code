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

#include "component/browse_component.h"
#include "component/callback/callback.h"
#include "component/client/client_component.h"
#include "component/client/client_service_component.h"
#include "component/io/file_component.h"
#include "component/io/folder_component.h"

#include "entity/entity.h"
#include "model/browse_model.h"
#include "thread/thread_info.h"
#include "utility/io/file_util.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Slot
	void browse_model::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		// Parent
		QModelIndex index;
		entity::ptr parent;

		if (entity->has_parent())
		{
			parent = entity->get_parent();
			index = get_index(parent);
			if (!index.isValid())
				return;
		}
		else
		{
			parent = m_entity;
		}

		// Data
		const auto browse_list = parent->get<browse_list_component>();
		const auto row = browse_list->size();

		if (!entity->has<client_component>())
		{
			const auto list = entity->get<browse_list_component>();
			if (likely(!list->empty()))
			{
				if (browse_list->empty())
				{
					beginInsertRows(index, row, (list->size() - 1));
					browse_list->swap(*list);
					endInsertRows();
				}
				else
				{
					beginInsertRows(index, row, row + (list->size() - 1));
					browse_list->insert(browse_list->end(), list->begin(), list->end());
					endInsertRows();
				}
			}
		}
		else
		{
			beginInsertRows(index, row, row);
			browse_list->push_back(entity);
			endInsertRows();
		}

		// Callback
		m_entity->async_call(callback::statusbar | callback::add);
	}

	void browse_model::on_remove(const entity::ptr entity)
	{
		assert(thread_info::main());

		if (likely(!entity->has_parent()))
		{
			const auto row = get_row(entity);
			if (row != npos)
			{
				const auto browse_list = m_entity->get<browse_list_component>();
				const auto it = browse_list->begin() + row;

				beginRemoveRows(QModelIndex(), row, row);
				browse_list->erase(it);
				endRemoveRows();
			}
		}
	}

	void browse_model::on_clear()
	{
		assert(thread_info::main());

		reset();

		const auto browse_list = m_entity->get<browse_list_component>();

		beginResetModel();
		browse_list->clear();
		endResetModel();

		// Callback
		m_entity->async_call(callback::statusbar | callback::remove);
	}

	// Utility
	bool browse_model::empty() const
	{
		assert(thread_info::main());

		const auto browse_list = m_entity->get<browse_list_component>();
		return browse_list->empty();
	}

	void browse_model::reset() const
	{
		// Delete child nodes (DO NOT REMOVE)
		const auto browse_list = m_entity->get<browse_list_component>();
		for (const auto& e : *browse_list)
		{
			assert(m_entity != e);
			const auto b = e->get<browse_list_component>();
			b->clear();

			const auto c = e->get<client_component>();
			c->set_size();
		}
	}

	// Model
	QVariant browse_model::data(const QModelIndex& index, int role) const
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
					case column::name:
					{
						return QVariant::fromValue(entity);
					}
					case column::type:
					{
						if (entity->has<file_component>())
						{
							const auto file = entity->get<file_component>();
							return QString::fromStdString(file->get_extension());
						}

						break;
					}
					case column::size:
					{
						if (entity->has<file_component>())
						{
							const auto file = entity->get<file_component>();
							return QString::fromStdString(file_util::get_size(file->get_size()));
						}
						else if (entity->has<folder_component>())
						{
							const auto folder = entity->get<folder_component>();
							return QString::fromStdString(file_util::get_size(folder->get_size()));
						}
						else //if (entity->has<client_component>())
						{
							const auto client = entity->get<client_component>();
							if (client->has_size())
								return QString::fromStdString(file_util::get_size(client->get_size()));
						}

						break;
					}
					case column::time:
					{
						if (entity->has<file_component>())
						{
							const auto file = entity->get<file_component>();

							if (file->has_time())
							{
								char buffer[32];
								const auto time = file->get_time();
								const auto tm = localtime((time_t*)&time);

								const auto client = m_entity->get<client_component>();
								strftime(buffer, 32, client->get_datestamp().c_str(), tm);

								return QString(buffer);
							}
						}
						else if (entity->has<folder_component>())
						{
							const auto folder = entity->get<folder_component>();

							if (folder->has_time())
							{
								char buffer[32];
								const auto time = folder->get_time();
								const auto tm = localtime((time_t*) &time);

								const auto client = m_entity->get<client_component>();
								strftime(buffer, 32, client->get_datestamp().c_str(), tm);

								return QString(buffer);
							}
						}

						break;
					}
				}
				break;
			}
			case Qt::EditRole:
			{
				if (entity->has<file_component>())
				{
					const auto file = entity->get<file_component>();
#if _WSTRING
					return QString::fromStdWString(file->get_wstem());
#else
					return QString::fromStdString(file->get_stem());
#endif
				}
				else if (entity->has<folder_component>())
				{
					const auto folder = entity->get<folder_component>();
#if _WSTRING
					return QString::fromStdWString(folder->get_wname());
#else
					return QString::fromStdString(folder->get_name());
#endif
				}

				const auto client = entity->get<client_component>();
				return QString::fromStdString(client->get_name());
			}
			case Qt::UserRole:
			{
				return QVariant::fromValue(entity);
			}
			case Qt::TextAlignmentRole:
			{
				switch (index.column())
				{
					case column::type:
					case column::size:
					case column::time:
					{
						return static_cast<int>(Qt::AlignVCenter | Qt::AlignRight);
					}
				}

				break;
			}
		}

		return QVariant();
	}

	bool browse_model::hasChildren(const QModelIndex& index /*= QModelIndex()*/) const
	{
		assert(thread_info::main());

		if (index.isValid())
		{
			const auto entity = get_entity(index);
			if (!entity || entity->has<file_component>())
				return false;

			const auto browse_list = entity->get<browse_list_component>();
			if (browse_list->is_expanded())
				return !browse_list->empty();

			if (entity->has<folder_component>())
			{
				const auto folder = entity->get<folder_component>();
				return folder->has_size();
			}
		}

		return true;
	}

	bool browse_model::canFetchMore(const QModelIndex& index) const
	{
		assert(thread_info::main());

		if (index.isValid())
		{
			const auto entity = get_entity(index);
			if (entity && entity->has<browse_list_component>())
			{
				const auto browse_list = entity->get<browse_list_component>();
				return !browse_list->is_expanded();
			}
		}

		return false;
	}

	void browse_model::fetchMore(const QModelIndex& index)
	{
		assert(thread_info::main());

		// Entity
		const auto entity = get_entity(index);
		if (entity && entity->has<browse_list_component>())
		{
			const auto browse_list = entity->get<browse_list_component>();
			browse_list->set_expanded(true);

			// Service
			const auto client_service = m_entity->get<client_service_component>();
			client_service->async_browse(entity);
		}
	}

	QModelIndex browse_model::index(int row, int column, const QModelIndex& index) const
	{
		assert(thread_info::main());

		const auto entity = get_entity(index, true);
		if (entity->has<browse_list_component>())
		{
			const auto browse_list = entity->get<browse_list_component>();
			if (row < browse_list->size())
			{
				const auto child = browse_list->at(row);
				return createIndex(row, column, child.get());
			}
		}

		return QModelIndex();
	}

	QModelIndex browse_model::parent(const QModelIndex& index) const
	{
		assert(thread_info::main());

		const auto parent = get_parent(index);
		if (parent)
		{
			const auto entity = get_parent(parent, true);
			if (entity->has<browse_list_component>())
			{
				const auto browse_list = entity->get<browse_list_component>();
				const auto it = std::find(browse_list->begin(), browse_list->end(), parent);
				if (it != browse_list->end())
				{
					const auto row = it - browse_list->begin();
					return createIndex(row, 0, parent.get());
				}
			}
		}

		return QModelIndex();
	}

	int browse_model::rowCount(const QModelIndex& index /*= QModelIndex()*/) const
	{
		assert(thread_info::main());

		const auto entity = get_entity(index, true);
		if (entity->has<browse_list_component>())
		{
			const auto browse_list = entity->get<browse_list_component>();
			return browse_list->size();
		}

		return 0;
	}

	// Get
	entity::ptr browse_model::get_entity(const size_t row) const
	{
		assert(thread_info::main());

		const auto browse_list = m_entity->get<browse_list_component>();
		return (row < browse_list->size()) ? browse_list->at(row) : nullptr;
	}

	QModelIndex browse_model::get_index(const entity::ptr& entity) const
	{
		assert(thread_info::main());

		const auto parent = get_parent(entity, true);
		if (parent->has<browse_list_component>())
		{
			const auto browse_list = parent->get<browse_list_component>();
			const auto it = std::find(browse_list->begin(), browse_list->end(), entity);
			if (it != browse_list->end())
			{
				const auto row = it - browse_list->begin();
				return createIndex(row, 0, entity.get());
			}
		}

		return QModelIndex();
	}

	size_t browse_model::get_row(const entity::ptr& entity) const
	{
		assert(thread_info::main());

		const auto parent = get_parent(entity, true);
		if (parent->has<browse_list_component>())
		{
			// O(N)
			const auto browse_list = parent->get<browse_list_component>();
			const auto it = std::find(browse_list->begin(), browse_list->end(), entity);
			return (it != browse_list->end()) ? (it - browse_list->begin()) : npos;
		}

		return npos;
	}
}
