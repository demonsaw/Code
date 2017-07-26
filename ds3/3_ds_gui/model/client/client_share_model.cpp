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
#include <QDir>
#include <QUrl>

#include "component/client/client_service_component.h"
#include "component/io/file_component.h"
#include "component/io/folder_component.h"
#include "component/io/share_component.h"

#include "dialog/archive_dialog.h"
#include "entity/entity.h"
#include "model/client/client_share_model.h"
#include "security/checksum/crc.h"
#include "security/filter/hex.h"
#include "thread/thread_info.h"
#include "utility/io/file_util.h"
#include "utility/io/folder_util.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Slot
	void client_share_model::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto share_list = m_entity->get<share_list_component>();
		const auto row = share_list->size();

		beginInsertRows(QModelIndex(), row, row);
		share_list->push_back(entity);
		endInsertRows();

		// Service
		const auto service = m_entity->get<client_service_component>();
		if (service->valid())
			service->async_share(entity);
	}

	void client_share_model::on_clear()
	{
		assert(thread_info::main());

		const auto share_list = m_entity->get<share_list_component>();

		beginResetModel();

		for (const auto& entity : *share_list)
		{
			entity->remove<share_list_component>();

			if (entity->has<folder_component>())
			{
				const auto folder = entity->get<folder_component>();
				folder->clear_children();
			}
		}

		endResetModel();
	}

	void client_share_model::on_add()
	{
		assert(thread_info::main());

		archive_dialog dialog;
		dialog.setWindowTitle("Share Files & Folders");
		dialog.setDirectory(QDir::currentPath());

		if (dialog.exec())
		{
			for (const auto& qpath : dialog.selectedFiles())
			{
				const auto path = qpath.toStdString();
				entity::ptr entity;

				if (folder_util::exists(path))
				{
					const auto folder = folder_component::create(path);
					entity = entity::create(m_entity, folder);
				}
				else
				{
					const auto file = file_component::create(path);
					entity = entity::create(m_entity, file);
				}

				emit add(entity);
			}
		}
	}

	void client_share_model::on_update()
	{
		assert(thread_info::main());

		// Service
		const auto service = m_entity->get<client_service_component>();
		if (service->valid())
			service->async_share();
	}

	void client_share_model::on_double_click(const QModelIndex& index)
	{
		assert(thread_info::main());

		if (!index.isValid())
			return;

		const auto entity = get_entity(index);
		if (entity && entity->has<file_component>())
		{
			const auto file = entity->get<file_component>();
			const auto url = QUrl::fromLocalFile(QString::fromStdString(file->get_path()));
			QDesktopServices::openUrl(url);
		}
	}

	// Utility
	bool client_share_model::empty() const
	{
		assert(thread_info::main());

		const auto share_list = m_entity->get<share_list_component>();
		return share_list->empty();
	}

	// Model
	bool client_share_model::removeRows(int row, int count, const QModelIndex& parent /*= QModelIndex()*/)
	{
		assert(thread_info::main());
		
		const auto entity = get_entity(row);

		const auto share_list = m_entity->get<share_list_component>();
		const auto it = share_list->cbegin() + row;
		const auto size = share_list->size();

		beginRemoveRows(QModelIndex(), row, (row + count - 1));
		share_list->erase(it, it + count);
		endRemoveRows();

		if (entity)
		{
			// Service
			const auto service = m_entity->get<client_service_component>();
			if (service->valid())
				service->async_unshare(entity);
		}

		return share_list->size() < size;
	}

	QVariant client_share_model::data(const QModelIndex& index, int role) const
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
						else
						{
							const auto folder = entity->get<folder_component>();
							return QString::fromStdString(folder_util::get_size(folder->get_size()));
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
						else
						{
							const auto folder = entity->get<folder_component>();
							return folder->drive() ? QString::fromStdString(folder->get_path()).toLower() : QString::fromStdString(folder->get_name()).toLower();
						}

						break;
					}
					case column::type:
					{
						const auto file = entity->get<file_component>();
						return QString::fromStdString(file->get_extension()).toLower();
					}
					case column::size:
					{
						if (entity->has<file_component>())
						{
							const auto file = entity->get<file_component>();
							return QVariant::fromValue<qulonglong>(file->get_size());
						}
						else
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
						else
						{
							const auto folder = entity->get<folder_component>();
							return folder->drive() ? QString::fromStdString(folder->get_path()) : QString::fromStdString(folder->get_name());
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

	bool client_share_model::hasChildren(const QModelIndex& index /*= QModelIndex()*/) const
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

	bool client_share_model::canFetchMore(const QModelIndex& index) const
	{
		assert(thread_info::main());

		if (index.isValid())
		{
			const auto entity = get_entity(index);
			return entity && !entity->has<share_list_component>();
		}		
		
		return false;
	}

	void client_share_model::fetchMore(const QModelIndex& index)
	{
		assert(thread_info::main());
		
		// Entity
		const auto entity = get_entity(index);
		if (!entity)
			return;

		// Share
		const auto share_list = share_list_component::create();
		entity->add(share_list);

		// Folders
		const auto folder = entity->get<folder_component>();
		const auto folders = folder->get_folders();
		
		for (const auto& e : folders)
		{
			const auto f = e->get<folder_component>();
			if (!f->has_children())
			{
				const auto sc = share_list_component::create();
				e->add(sc);
			}

			share_list->push_back(e);
		}

		// Files
		const auto files = folder->get_files();

		for (const auto& e : files)
		{
			const auto f = e->get<file_component>();
			const auto sc = share_list_component::create();
			e->add(sc);

			share_list->push_back(e);
		}
	}

	QModelIndex client_share_model::index(int row, int column, const QModelIndex& parent) const
	{
		assert(thread_info::main());

		if (row < 0 || column < 0)
			return QModelIndex();

		const auto entity = get_entity(parent);
		if (!entity)
			return QModelIndex();

		if (!entity->has<share_list_component>())
			return QModelIndex();

		const auto share_list = entity->get<share_list_component>();
		if (!share_list)
			return QModelIndex();
		
		if (row >= share_list->size())
			return QModelIndex();

		const auto child = share_list->at(row);

		return createIndex(row, column, child.get());
	}
	
	QModelIndex client_share_model::parent(const QModelIndex& index) const
	{
		assert(thread_info::main());

		const auto parent = get_parent(index);
		if (!parent)
			return QModelIndex();

		const auto grandparent = get_parent(parent);
		if (!grandparent)
			return QModelIndex();

		if (!grandparent->has<share_list_component>())
			return QModelIndex();

		const auto share_list = grandparent->get<share_list_component>();
		if (!share_list)
			return QModelIndex();

		const auto it = std::find(share_list->begin(), share_list->end(), parent);
		if (it == share_list->end())
			return QModelIndex();

		const auto row = it - share_list->begin();

		return createIndex(row, 0, parent.get());
	}

	int client_share_model::rowCount(const QModelIndex& index /*= QModelIndex()*/) const
	{
		assert(thread_info::main());

		const auto entity = get_entity(index);
		if (entity && entity->has<share_list_component>())
		{
			const auto share_list = entity->get<share_list_component>();
			return share_list->size();
		}

		return 0;
	}

	// Get
	entity::ptr client_share_model::get_entity(const size_t row) const
	{
		assert(thread_info::main());

		const auto share_list = m_entity->get<share_list_component>();
		return (row < share_list->size()) ? share_list->at(row) : nullptr;
	}

	QModelIndex client_share_model::get_index(const entity::ptr entity) const
	{
		assert(thread_info::main());

		const auto parent = get_parent(entity);
		if (!parent)
			return QModelIndex();

		if (!parent->has<share_list_component>())
			return QModelIndex();

		const auto share_list = parent->get<share_list_component>();
		if (!share_list)
			return QModelIndex();

		const auto it = std::find(share_list->begin(), share_list->end(), entity);
		if (it == share_list->end())
			return QModelIndex();

		const auto row = it - share_list->begin();

		return createIndex(row, 0, entity.get());
	}


	size_t client_share_model::get_row(const entity::ptr entity) const
	{
		assert(thread_info::main());

		const auto parent = get_parent(entity);
		if (!parent)
			return npos;

		if (!parent->has<share_list_component>())
			return npos;

		// O(N)
		const auto share_list = parent->get<share_list_component>();
		const auto it = std::find(share_list->begin(), share_list->end(), entity);
		return (it != share_list->end()) ? (it - share_list->begin()) : npos;
	}
}
