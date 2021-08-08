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

#include <QDesktopServices>
#include <QUrl>

#include "component/callback/callback.h"
#include "component/client/client_component.h"
#include "component/client/client_service_component.h"
#include "component/io/file_component.h"
#include "component/io/folder_component.h"
#include "component/io/share_component.h"

#include "entity/entity.h"
#include "model/share_model.h"
#include "thread/thread_info.h"
#include "utility/io/file_util.h"
#include "utility/io/folder_util.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Interface
	void share_model::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		size_t row;
		const auto share_list = m_entity->get<share_list_component>();
		{
			row = share_list->size();

			beginInsertRows(QModelIndex(), row, row);
			share_list->push_back(entity);
			endInsertRows();
		}

		// Service
		const auto client_service = m_entity->get<client_service_component>();
		if (client_service->valid())
			client_service->async_share(entity);

		// Callback
		const auto size = std::count_if(share_list->begin(), share_list->end(), [&](const entity::ptr e) { return e->enabled(); });
		if (size == 1)
			m_entity->async_call(callback::window | callback::update);
	}

	void share_model::on_remove(const QModelIndex& index)
	{
		assert(thread_info::main());

		const auto entity = get_entity(index);
		if (entity && !entity->has_parent())
		{
			const auto row = index.row();
			const auto share_list = m_entity->get<share_list_component>();
			const auto it = share_list->begin() + row;

			beginRemoveRows(QModelIndex(), row, row);
			share_list->erase(it);
			endRemoveRows();

			// Service
			const auto client_service = m_entity->get<client_service_component>();
			if (client_service->valid())
				client_service->async_unshare(entity);

			// Callback
			const auto size = std::count_if(share_list->begin(), share_list->end(), [&](const entity::ptr e) { return e->enabled(); });
			if (size == 0)
				m_entity->async_call(callback::window | callback::update);
		}
	}

	void share_model::on_clear(const entity::ptr entity)
	{
		assert(thread_info::main());

		// Delete child nodes (DO NOT REMOVE)
		entity->remove<share_list_component>();

		if (entity->has<folder_component>())
		{
			const auto folder = entity->get<folder_component>();
			folder->clear_children();
		}
	}

	void share_model::on_clear()
	{
		assert(thread_info::main());

		const auto share_list = m_entity->get<share_list_component>();

		beginResetModel();

		// Delete child nodes (DO NOT REMOVE)
		for (const auto& e : *share_list)
		{
			e->remove<share_list_component>();

			if (e->has<folder_component>())
			{
				const auto folder = e->get<folder_component>();
				folder->clear_children();
			}
		}

		endResetModel();
	}

	// Utility
	bool share_model::empty() const
	{
		assert(thread_info::main());

		const auto share_list = m_entity->get<share_list_component>();
		return share_list->empty();
	}

	// Set
	void share_model::set_modified() const
	{
		assert(thread_info::main());
	}

	// Model
	Qt::ItemFlags share_model::flags(const QModelIndex& index) const
	{
		assert(thread_info::main());

		if (index.isValid())
		{
			auto flags = QAbstractItemModel::flags(index);

			switch (index.column())
			{
				case column::name:
				{
					flags |= Qt::ItemIsUserCheckable;
					break;
				}
			}

			return flags;
		}

		return Qt::ItemIsEnabled;
	}

	QVariant share_model::data(const QModelIndex& index, int role) const
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

						const auto folder = entity->get<folder_component>();
						return QString::fromStdString(folder_util::get_size(folder->get_size()));
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

						const auto folder = entity->get<folder_component>();

						if (folder->has_time())
						{
							char buffer[32];
							const auto time = folder->get_time();
							const auto tm = localtime((time_t*)&time);

							const auto client = m_entity->get<client_component>();
							strftime(buffer, 32, client->get_datestamp().c_str(), tm);

							return QString(buffer);
						}

						break;
					}
				}

				break;
			}
			case Qt::UserRole:
			{
				return QVariant::fromValue(entity);
			}
			case Qt::CheckStateRole:
			{
				switch (index.column())
				{
					case column::name:
					{
						if (!entity->has_parent())
							return entity->enabled() ? Qt::Checked : Qt::Unchecked;

						break;
					}
				}
				break;
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

	bool share_model::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
	{
		assert(thread_info::main());

		const auto entity = get_entity(index);
		if (!entity)
			return false;

		bool modified = false;

		switch (role)
		{
			case Qt::CheckStateRole:
			{
				switch (index.column())
				{
					case column::name:
					{
						switch (value.toInt())
						{
							case Qt::Checked:
							{
								if (entity->disabled())
								{
									entity->enable();

									entity_tree_model::on_update(index);

									// Service
									const auto client_service = m_entity->get<client_service_component>();
									if (client_service->valid())
									{
										client_service->async_share(entity);

										// Callback
										const auto share_list = m_entity->get<share_list_component>();
										const auto size = std::count_if(share_list->begin(), share_list->end(), [&](const entity::ptr e) { return e->enabled(); });
										if (size == 1)
											m_entity->async_call(callback::window | callback::update);
									}

									modified = true;
								}

								break;
							}
							case Qt::Unchecked:
							{
								if (entity->enabled())
								{
									entity->disable();

									entity_tree_model::on_update(index);

									// Service
									const auto client_service = m_entity->get<client_service_component>();
									if (client_service->valid())
									{
										client_service->async_unshare(entity);

										on_clear(entity);

										// Callback
										const auto share_list = m_entity->get<share_list_component>();
										const auto size = std::count_if(share_list->begin(), share_list->end(), [&](const entity::ptr e) { return e->enabled(); });
										if (size == 0)
											m_entity->async_call(callback::window | callback::update);
									}

									modified = true;
								}

								break;
							}
						}

						break;
					}
				}

				break;
			}
		}

		return modified;
	}

	bool share_model::hasChildren(const QModelIndex& index /*= QModelIndex()*/) const
	{
		assert(thread_info::main());

		if (index.isValid())
		{
			const auto entity = get_entity(index);
			if (entity)
			{
				if (entity->has<file_component>())
					return false;

				if (entity->has<share_list_component>())
				{
					const auto share_list = entity->get<share_list_component>();
					return !share_list->empty();
				}
			}
		}

		return true;
	}

	bool share_model::canFetchMore(const QModelIndex& index) const
	{
		assert(thread_info::main());

		if (index.isValid())
		{
			const auto entity = get_entity(index);
			return entity && !entity->has<share_list_component>();
		}

		return false;
	}

	void share_model::fetchMore(const QModelIndex& index)
	{
		assert(thread_info::main());

		const auto entity = get_entity(index);
		if (entity)
		{
			// Share
			const auto share_list = entity->add<share_list_component>();

			// Folders
			const auto folder = entity->get<folder_component>();
			const auto folders = folder->get_folders();
			for (const auto& e : folders)
			{
				e->set_owner(m_entity);
				share_list->push_back(e);
			}

			// Files
			const auto files = folder->get_files();
			for (const auto& e : files)
			{
				e->set_owner(m_entity);
				share_list->push_back(e);
			}
		}
	}

	QModelIndex share_model::index(int row, int column, const QModelIndex& index /*= QModelIndex()*/) const
	{
		assert(thread_info::main());

		const auto entity = get_entity(index, true);
		if (entity->has<share_list_component>())
		{
			const auto share_list = entity->get<share_list_component>();
			if (row < share_list->size())
			{
				const auto child = share_list->at(row);
				return createIndex(row, column, child.get());
			}
		}

		return QModelIndex();
	}

	QModelIndex share_model::parent(const QModelIndex& index) const
	{
		assert(thread_info::main());

		const auto parent = get_parent(index);
		if (parent)
		{
			const auto entity = get_parent(parent, true);
			if (entity->has<share_list_component>())
			{
				const auto share_list = entity->get<share_list_component>();
				const auto it = std::find(share_list->begin(), share_list->end(), parent);
				if (it != share_list->end())
				{
					const auto row = it - share_list->begin();
					return createIndex(row, 0, parent.get());
				}
			}
		}

		return QModelIndex();
	}

	int share_model::rowCount(const QModelIndex& index /*= QModelIndex()*/) const
	{
		assert(thread_info::main());

		const auto entity = get_entity(index, true);
		if (entity->has<share_list_component>())
		{
			const auto share_list = entity->get<share_list_component>();
			return share_list->size();
		}

		return 0;
	}

	// Get
	entity::ptr share_model::get_entity(const size_t row) const
	{
		assert(thread_info::main());

		const auto share_list = m_entity->get<share_list_component>();
		return (row < share_list->size()) ? share_list->at(row) : nullptr;
	}

	QModelIndex share_model::get_index(const entity::ptr& entity) const
	{
		assert(thread_info::main());

		const auto parent = get_parent(entity, true);
		if (parent->has<share_list_component>())
		{
			const auto share_list = parent->get<share_list_component>();
			const auto it = std::find(share_list->begin(), share_list->end(), entity);
			if (it != share_list->end())
			{
				const auto row = it - share_list->begin();
				return createIndex(row, 0, entity.get());
			}
		}

		return QModelIndex();
	}

	size_t share_model::get_row(const entity::ptr& entity) const
	{
		assert(thread_info::main() && entity->has_data());

		const auto parent = get_parent(entity, true);
		if (parent->has<share_list_component>())
		{
			// O(N)
			const auto share_list = parent->get<share_list_component>();
			const auto it = std::find(share_list->begin(), share_list->end(), entity);
			return (it != share_list->end()) ? (it - share_list->begin()) : npos;
		}

		return npos;
	}
}
