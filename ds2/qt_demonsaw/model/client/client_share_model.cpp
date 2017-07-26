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
#include <QFileDialog>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QUrl>

#include "client_share_model.h"
#include "component/client/client_option_component.h"
#include "entity/entity.h"
#include "component/io/file_component.h"
#include "component/io/folder_component.h"
#include "resource/icon_provider.h"
#include "utility/io/file_util.h"
#include "utility/io/folder_util.h"
#include "window/client_window.h"
#include "window/main_window.h"

namespace eja
{
	// Interface
	void client_share_model::init()
	{
		assert(thread::main());

		entity_tree_model::init();

		// Callback
		add_callback(function_type::share);

		const auto folder_map = m_entity->get<folder_map_component>();
		const auto share = m_entity->get<share_idle_component>();
		const auto folders = share->get_folders();

		const auto share_vector = m_entity->get<share_entity_vector_component>();
		if (!share_vector)
			return;

		// Model
		beginResetModel();
				
		auto_lock_ref(folders);

		for (const auto& folder : folders)
		{
			// Folder
			const auto e = entity::create(m_entity);
			const auto fc = std::static_pointer_cast<folder_component>(folder);
			e->add(fc);

			const auto fc2 = folder_map->get(folder->get_id());
			if (fc2)
				fc->set_size(fc2->get_size());

			share_vector->add(e);
		}

		endResetModel();
	}

	void client_share_model::add()
	{
		QFileDialog dialog;
		dialog.setWindowTitle("Share Path");
		dialog.setFileMode(QFileDialog::DirectoryOnly);
		dialog.setOptions(QFileDialog::ShowDirsOnly | QFileDialog::ReadOnly);
		dialog.setViewMode(QFileDialog::ViewMode::List);
		dialog.setDirectory(QDir::currentPath());

		if (dialog.exec())
		{
			const auto qpaths = dialog.selectedFiles();
			const auto& qpath = qpaths.at(0);
			const auto path = qpath.toStdString();

			// Model
			const auto folder = folder_component::create(path);
			const auto entity = entity::create(m_entity);
			entity->add(folder);

			emit added(entity);

			// Share
			const auto share = m_entity->get<share_idle_component>();
			if (!share)
				return;

			const auto hash = share->get_hash();
			const auto id = folder_util::get_id(folder, hash);
			folder->set_id(id);

			share->add(folder);
		}
	}

	// Slot
	void client_share_model::on_add(const entity::ptr entity)
	{
		assert(thread::main());

		const auto share_vector = m_entity->get<share_entity_vector_component>();
		if (!share_vector)
			return;

		{
			auto_lock_ptr(share_vector);

			const auto row = share_vector->size();

			beginInsertRows(QModelIndex(), row, row);
			share_vector->add(entity);
			endInsertRows();
		}

		set_status(client_statusbar::share, share_vector->size());
	}

	void client_share_model::on_remove(const QModelIndex& index)
	{
		assert(thread::main());

		if (!index.isValid())
			return;

		const auto share_vector = m_entity->get<share_entity_vector_component>();
		if (!share_vector)
			return;

		const auto entity = get_node(index);
		if (!entity)
			return;

		if (!share_vector->has(entity))
			return;

		{
			auto_lock_ptr(share_vector);

			const auto row = index.row();

			beginRemoveRows(QModelIndex(), row, row);
			share_vector->remove(row);
			endRemoveRows();
		}

		set_status(client_statusbar::share, share_vector->size());

		// Share
		const auto folder = entity->get<folder_component>();
		if (!folder)
			return;

		const auto share = m_entity->get<share_idle_component>();
		if (share)
			share->remove(folder);
	}

	void client_share_model::on_refresh()
	{
		assert(thread::main());

		// Model
		emit cleared();

		// Share
		const auto share = m_entity->get<share_idle_component>();
		if (share)
			share->update();
	}

	void client_share_model::on_clear()
	{
		assert(thread::main());

		const auto share_vector = m_entity->get<share_entity_vector_component>();
		if (!share_vector)
			return;

		beginResetModel();

		for (const auto& entity : share_vector->copy())
			entity->remove<share_entity_vector_component>();

		endResetModel();

		set_status(client_statusbar::share, share_vector->size());
	}

	void client_share_model::on_double_click(const QModelIndex& index)
	{
		assert(thread::main());

		if (!index.isValid())
			return;

		const auto entity = get_node(index);
		if (entity && entity->has<file_component>())
		{
			const auto file = entity->get<file_component>();
			if (file)
			{
				const auto url = QUrl::fromLocalFile(QString::fromStdString(file->get_path()));
				QDesktopServices::openUrl(url);
			}
		}
	}

	// Utility
	bool client_share_model::empty() const
	{
		const auto share_vector = m_entity->get<share_entity_vector_component>();
		return !share_vector || share_vector->empty();
	}

	void client_share_model::post_create()
	{
		const auto share_vector = m_entity->get<share_entity_vector_component>();
		if (!share_vector)
			return;

		set_status(client_statusbar::share, share_vector->size());
	}

	// Model
	QVariant client_share_model::data(const QModelIndex& index, int role) const
	{
		if (!index.isValid())
			return QVariant();

		const auto entity = get_node(index);
		if (!entity)
			return QVariant();

		const size_t col = static_cast<size_t>(index.column());

		switch (role)
		{
			case Qt::DisplayRole:			
			{
				switch (col)
				{					
					case column::file:
					{
						return QVariant::fromValue(entity);
					}
					case column::size:
					{
						if (entity->has<file_component>())
						{
							const auto file = entity->get<file_component>();
							if (file)
								return QString::fromStdString(file_util::get_size(file->get_size()));
						}

						break;
					}
					case column::type:
					{
						if (entity->has<file_component>())
						{
							const auto file = entity->get<file_component>();
							if (file)
								return QString::fromStdString(file->get_description());

							break;
						}
					}
				}
				break;
			}
			case Qt::UserRole:
			{
				switch (col)
				{					
					case column::file:
					{
						if (entity->has<file_component>())
						{
							const auto file = entity->get<file_component>();
							if (file)
								return QString("~%1").arg(file->get_name().c_str()).toLower();

							break;
						}
						else
						{
							const auto folder = entity->get<folder_component>();
							if (folder)
								return folder->is_drive() ? QString::fromStdString(folder->get_path()).toLower() : QString::fromStdString(folder->get_name()).toLower();

							break;
						}
					}
					case column::size:
					{
						if (entity->has<file_component>())
						{
							const auto file = entity->get<file_component>();
							if (file)
								return QVariant::fromValue<qulonglong>(file->get_size());
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
					case column::file:
					{
						if (entity->has<file_component>())
						{
							const auto file = entity->get<file_component>();
							if (file)
								return QString("~%1").arg(file->get_name().c_str());

							break;
						}
						else
						{
							const auto folder = entity->get<folder_component>();
							if (folder)
								return folder->is_drive() ? QString::fromStdString(folder->get_path()) : QString::fromStdString(folder->get_name());

							break;
						}
					}
				}

				break;
			}
			case Qt::TextAlignmentRole:
			{
				switch (col)
				{
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
		if (index.isValid())
		{
			const auto entity = get_node(index);
			if (entity && entity->has<share_entity_vector_component>(false))
			{
				const auto share_vector = entity->get<share_entity_vector_component>();
				return share_vector && !share_vector->empty();
			}
		}

		return true;
	}

	bool client_share_model::canFetchMore(const QModelIndex& index) const
	{
		if (!index.isValid())
			return false;

		const auto entity = get_node(index);
		return entity && !entity->has<share_entity_vector_component>(false);
	}

	void client_share_model::fetchMore(const QModelIndex& index)
	{
		// Share
		const auto entity = get_node(index);
		if (!entity)
			return;

		const auto share_vector = share_entity_vector_component::create();
		entity->add(share_vector);

		// Folders
		const auto folder = entity->get<folder_component>();
		if (!folder)
			return;

		const auto folder_map = m_entity->get<folder_map_component>();
		if (!folder_map)
			return;

		const auto folders = folder->get_folders();
		auto_lock_ref(folders);

		for (const auto& item : folders)
		{
			// Folder
			const auto e = entity::create(entity);
			const auto fc = folder_component::create(item);
			e->add(fc);

			// Share
			if (!item->has_children())
			{
				const auto sc = share_entity_vector_component::create();
				e->add(sc);
			}

			share_vector->add(e);
		}

		// Files
		const auto files = folder->get_files();
		auto_lock_ref(files);

		for (const auto& item : files)
		{
			// Folder
			const auto e = entity::create(entity);
			const auto fc = file_component::create(item);
			e->add(fc);

			// Share
			const auto sc = share_entity_vector_component::create();
			e->add(sc);

			share_vector->add(e);
		}
	}

	int client_share_model::rowCount(const QModelIndex& index /*= QModelIndex()*/) const
	{
		const auto entity = get_node(index);
		if (entity && entity->has<share_entity_vector_component>(false))
		{			
			const auto share_vector = entity->get<share_entity_vector_component>();
			if (share_vector)
				return share_vector->size();
		}

		return 0;
	}

	QModelIndex client_share_model::index(int row, int column, const QModelIndex& parent) const
	{
		if (row < 0 || column < 0)
			return QModelIndex();

		const auto entity = get_node(parent);
		if (!entity)
			return QModelIndex();

		if (!entity->has<share_entity_vector_component>(false))
			return QModelIndex();

		const auto share_vector = entity->get<share_entity_vector_component>();
		if (!share_vector)
			return QModelIndex();

		const auto child = share_vector->get(row);
		if (!child)
			return QModelIndex();

		return createIndex(row, column, child.get());
	}

	QModelIndex client_share_model::index(const entity::ptr entity) const
	{
		const auto parent = get_parent(entity);
		if (!parent)
			return QModelIndex();

		if (!parent->has<share_entity_vector_component>(false))
			return QModelIndex();

		const auto share_vector = parent->get<share_entity_vector_component>();
		if (!share_vector)
			return QModelIndex();

		const auto row = share_vector->get(entity);
		if (row == type::npos)
			return QModelIndex();

		return createIndex(row, 0, entity.get());
	}

	QModelIndex client_share_model::parent(const QModelIndex& index) const
	{
		const auto parent = get_parent(index);
		if (!parent)
			return QModelIndex();

		const auto grandparent = get_parent(parent);
		if (!grandparent)
			return QModelIndex();

		if (!grandparent->has<share_entity_vector_component>(false))
			return QModelIndex();

		const auto share_vector = grandparent->get<share_entity_vector_component>();
		if (!share_vector)
			return QModelIndex();

		const auto row = share_vector->get(parent);
		if (row == type::npos)
			return QModelIndex();

		return createIndex(row, 0, parent.get());
	}

	// Accessor
	size_t client_share_model::get_row(const entity::ptr entity) const
	{
		const auto parent = get_parent(entity);
		if (!parent)
			return type::npos;

		if (!parent->has<share_entity_vector_component>(false))
			return type::npos;

		const auto share_vector = parent->get<share_entity_vector_component>();
		if (share_vector)
			share_vector->get(entity);

		return type::npos;
	}
}
