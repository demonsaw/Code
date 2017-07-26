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
#include "component/endpoint_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/client/client_io_component.h"
#include "component/client/client_service_component.h"
#include "component/io/file_component.h"
#include "component/io/folder_component.h"
#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "model/client/client_browse_model.h"
#include "security/checksum/crc.h"
#include "security/filter/hex.h"
#include "thread/thread_info.h"
#include "utility/io/file_util.h"
#include "utility/io/folder_util.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Slot
	void client_browse_model::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto parent = entity->get_parent();
		if (!parent)
			return;

		if (entity->is_parent(m_entity))
		{
			const auto browse_list = m_entity->get<browse_list_component>();
			const auto row = browse_list->size();

			if (entity->has<browse_list_component>())
			{
				const auto list = entity->get<browse_list_component>();
				browse_list->reserve(browse_list->size() + list->size());

				beginInsertRows(QModelIndex(), row, row + list->size() - 1);
				browse_list->insert(browse_list->end(), list->begin(), list->end());
				endInsertRows();
			}
			else
			{
				beginInsertRows(QModelIndex(), row, row);
				browse_list->push_back(entity);
				endInsertRows();
			}
		}
		else
		{
			const auto index = get_index(parent);
			if (index.isValid())
			{
				const auto browse_list = parent->get<browse_list_component>();
				const auto row = browse_list->size();

				beginInsertRows(index, row, row);
				browse_list->push_back(entity);
				endInsertRows();
			}
		}

		// Callback
		const auto e = entity_factory::create_statusbar(statusbar::browse);
		m_entity->call(callback_type::status, callback_action::add, e);
	}

	void client_browse_model::on_update(const entity::ptr entity)
	{
		assert(thread_info::main());
		
		// Service
		const auto service = m_entity->get<client_service_component>();
		if (service->valid())
			service->async_client();
	}

	void client_browse_model::on_clear()
	{
		assert(thread_info::main());

		const auto browse_list = m_entity->get<browse_list_component>();		
		
		// Delete child nodes (DO NOT REMOVE)
		for (const auto& e : *browse_list)
		{
			const auto list = e->get<browse_list_component>();
			list->clear();
		}

		beginResetModel();
		browse_list->clear();
		endResetModel();

		// Callback
		const auto e = entity_factory::create_statusbar(statusbar::browse);
		m_entity->call(callback_type::status, callback_action::clear, e);
	}

	void client_browse_model::on_double_click(const QModelIndex& index)
	{
		assert(thread_info::main());

		if (!index.isValid())
			return;

		const auto entity = get_entity(index);
		if (entity && entity->has<file_component>())
		{
			const auto service = m_entity->get<client_service_component>();
			if (service->valid())
				service->async_file(entity);
		}
	}

	void client_browse_model::download(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto service = m_entity->get<client_service_component>();
		if (service->valid())
		{
			const auto io = m_entity->get<client_io_component>();
			const auto& path = io->get_path();
			download_to(entity, path);
		}
	}

	void client_browse_model::download_client(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto service = m_entity->get<client_service_component>();
		if (service->valid())
		{
			const auto io = m_entity->get<client_io_component>();
			const auto& path = io->get_path();
			download_to_client(entity, path);
		}
	}

	void client_browse_model::download_to(const entity::ptr entity, const std::string& root)
	{
		assert(thread_info::main());

		const auto service = m_entity->get<client_service_component>();
		if (service->invalid())
			return;

		if (entity->has<file_component>())
			service->async_file(entity, root);
		else if (entity->has<folder_component>())
			service->async_folder(entity, root);
	}

	void client_browse_model::download_to_client(const entity::ptr entity, const std::string& root)
	{
		assert(thread_info::main());

		const auto service = m_entity->get<client_service_component>();
		if (service->invalid())
			return;

		if (entity->has<file_component>())
		{
			const auto endpoint = entity->find<endpoint_component>();
			const auto client = endpoint->get_entity();
			service->async_file(entity, root, client);
		}
		else if (entity->has<folder_component>())
		{
			service->async_folder_client(entity, root);
		}
	}

	// Utility
	bool client_browse_model::empty() const
	{
		assert(thread_info::main());

		const auto browse_list = m_entity->get<browse_list_component>();
		return browse_list->empty();
	}

	// Model
	QVariant client_browse_model::data(const QModelIndex& index, int role) const
	{
		assert(thread_info::main());

		if (!index.isValid())
			return QVariant();

		const auto entity = get_entity(index);
		if (!entity)
			return QVariant();

		const size_t col = static_cast<size_t>(index.column());

		switch (role)
		{
			case Qt::DisplayRole:
			{
				switch (col)
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
					case column::checksum:
					{
						if (entity->has<file_component>())
						{
							const auto file = entity->get<file_component>();
							const auto file_id = file->has_id() ? hex::encode(static_cast<u32>(crc32()(file->get_id()))) : "";
							return QString::fromStdString(file_id);
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

						break;
					}
				}
				break;
			}
			case Qt::UserRole:
			{
				switch (col)
				{
					case column::name:
					{
						if (entity->has<file_component>())
						{
							const auto file = entity->get<file_component>();
							return QString("~%1").arg(file->get_stem().c_str()).toLower();
						}
						else if (entity->has<folder_component>())
						{
							const auto folder = entity->get<folder_component>();
							return folder->drive() ? QString::fromStdString(folder->get_path()).toLower() : QString::fromStdString(folder->get_name()).toLower();
						}
						else
						{
							const auto endpoint = entity->get<endpoint_component>();
							return QString::fromStdString(endpoint->get_name()).toLower();
						}

						break;
					}					
					case column::type:
					{
						if (entity->has<file_component>())
						{
							const auto file = entity->get<file_component>();
							return QString::fromStdString(file->get_extension()).toLower();
						}

						break;
					}
					case column::size:
					{
						if (entity->has<file_component>())
						{
							const auto file = entity->get<file_component>();
							return QVariant::fromValue<qulonglong>(file->get_size());
						}
						else if (entity->has<folder_component>())
						{
							const auto folder = entity->get<folder_component>();
							return QVariant::fromValue<qulonglong>(folder->get_size());
						}

						break;
					}
					default:
					{
						return data(index, Qt::DisplayRole);
					}
				}
				break;
			}
			case Qt::EditRole:
			{
				switch (col)
				{
					case column::name:
					{
						if (entity->has<file_component>())
						{
							const auto file = entity->get<file_component>();
							return QString("~%1").arg(file->get_stem().c_str());
						}
						else if (entity->has<folder_component>())
						{
							const auto folder = entity->get<folder_component>();
							return folder->drive() ? QString::fromStdString(folder->get_path()) : QString::fromStdString(folder->get_name());
						}
						else
						{
							const auto endpoint = entity->get<endpoint_component>();
							return QString::fromStdString(endpoint->get_name());
						}

						break;
					}
				}

				break;
			}
			case Qt::TextAlignmentRole:
			{
				switch (col)
				{
					case column::type:
					case column::checksum:
					case column::size:
					{
						return static_cast<int>(Qt::AlignVCenter | Qt::AlignRight);
					}
				}

				break;
			}
		}

		return QVariant();
	}

	bool client_browse_model::hasChildren(const QModelIndex& index /*= QModelIndex()*/) const
	{
		assert(thread_info::main());

		if (index.isValid())
		{
			const auto entity = get_entity(index);
			if (!entity || entity->has<file_component>())
				return false;

			const auto browse = entity->get<browse_component>();
			if (browse->expanded())
			{
				const auto browse_list = entity->get<browse_list_component>();
				return !browse_list->empty();
			}
			else if (entity->has<folder_component>())
			{
				const auto folder = entity->get<folder_component>();
				return folder->has_id() && folder->has_size();
			}
		}

		return true;
	}

	bool client_browse_model::canFetchMore(const QModelIndex& index) const
	{
		assert(thread_info::main());

		if (index.isValid())
		{
			const auto entity = get_entity(index);
			if (entity)
			{
				const auto browse = entity->get<browse_component>();
				return browse->empty();
			}
		}

		return false;
	}

	void client_browse_model::fetchMore(const QModelIndex& index)
	{
		assert(thread_info::main());

		// Entity
		const auto entity = get_entity(index);
		if (!entity)
			return;

		const auto browse = entity->get<browse_component>();
		browse->expand();

		// Service
		const auto service = m_entity->find<client_service_component>();
		if (service->valid())
			service->async_browse(entity);
	}

	QModelIndex client_browse_model::index(int row, int column, const QModelIndex& parent) const
	{
		assert(thread_info::main());

		if (row < 0 || column < 0)
			return QModelIndex();

		const auto entity = get_entity(parent);
		if (!entity)
			return QModelIndex();

		if (!entity->has<browse_list_component>())
			return QModelIndex();

		const auto browse_list = entity->get<browse_list_component>();
		if (!browse_list)
			return QModelIndex();

		if (row >= browse_list->size())
			return QModelIndex();

		const auto child = browse_list->at(row);

		return createIndex(row, column, child.get());
	}

	QModelIndex client_browse_model::parent(const QModelIndex& index) const
	{
		assert(thread_info::main());

		const auto parent = get_parent(index);
		if (!parent)
			return QModelIndex();

		const auto grandparent = get_parent(parent);
		if (!grandparent)
			return QModelIndex();

		if (!grandparent->has<browse_list_component>())
			return QModelIndex();

		const auto browse_list = grandparent->get<browse_list_component>();
		if (!browse_list)
			return QModelIndex();

		const auto it = std::find(browse_list->cbegin(), browse_list->cend(), parent);
		if (it == browse_list->end())
			return QModelIndex();

		const auto row = it - browse_list->cbegin();

		return createIndex(row, 0, parent.get());
	}

	int client_browse_model::rowCount(const QModelIndex& index /*= QModelIndex()*/) const
	{
		assert(thread_info::main());

		const auto entity = get_entity(index);
		if (entity && entity->has<browse_list_component>())
		{
			const auto browse_list = entity->get<browse_list_component>();
			return browse_list->size();
		}

		return 0;
	}

	// Get
	entity::ptr client_browse_model::get_entity(const size_t row) const
	{
		assert(thread_info::main());

		const auto browse_list = m_entity->get<browse_list_component>();
		return (row < browse_list->size()) ? browse_list->at(row) : nullptr;
	}

	QModelIndex client_browse_model::get_index(const entity::ptr entity) const
	{
		assert(thread_info::main());

		const auto parent = get_parent(entity);
		if (!parent)
			return QModelIndex();

		if (!parent->has<browse_list_component>())
			return QModelIndex();

		const auto browse_list = parent->get<browse_list_component>();
		if (!browse_list)
			return QModelIndex();

		const auto it = std::find(browse_list->begin(), browse_list->end(), entity);
		if (it == browse_list->end())
			return QModelIndex();

		const auto row = it - browse_list->begin();

		return createIndex(row, 0, entity.get());
	}

	size_t client_browse_model::get_row(const entity::ptr entity) const
	{
		assert(thread_info::main());

		const auto parent = get_parent(entity);
		if (!parent)
			return npos;

		if (!parent->has<browse_list_component>())
			return npos;

		// O(N)
		const auto browse_list = parent->get<browse_list_component>();
		const auto it = std::find(browse_list->cbegin(), browse_list->cend(), entity);
		return (it != browse_list->cend()) ? (it - browse_list->cbegin()) : npos;
	}
}
