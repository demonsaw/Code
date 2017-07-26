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

#include <QApplication>

#include "client_search_model.h"
#include "component/function_component.h"
#include "component/search_component.h"
#include "component/session_component.h"
#include "component/timer_component.h"
#include "component/client/client_component.h"
#include "component/client/client_option_component.h"
#include "component/error/error_component.h"
#include "component/io/file_component.h"
#include "component/io/folder_component.h"
#include "component/router/router_component.h"
#include "component/router/router_security_component.h"
#include "component/transfer/chunk_component.h"
#include "component/transfer/transfer_component.h"
#include "resource/icon_provider.h"
#include "system/function/function_action.h"
#include "system/function/function_type.h"
#include "utility/io/file_util.h"
#include "utility/io/folder_util.h"
#include "window/client_window.h"
#include "window/main_window.h"

namespace eja
{
	// Interface
	void client_search_model::init()
	{
		entity_table_model::init();

		// Callback
		add_callback(function_type::search);
	}

	void client_search_model::add(const entity::ptr entity)
	{
		const auto found = find_entity<file_component, search_entity_vector_component>(entity);
		if (!found)
		{
			emit added(entity);
		}
		else
		{
			const auto search = found->get<search_component>();
			if (search)
			{
				search->add_swarm();
				emit refreshed(found);
			}				
		}
	}

	void client_search_model::double_click(const QModelIndex& index)
	{
		if (!index.isValid())
			return;

		const auto search_vector = m_entity->get<search_entity_vector_component>();
		if (!search_vector)
			return;

		const size_t row = static_cast<size_t>(index.row());
		const auto entity = search_vector->get(row);
		if (!entity)
			return;

		if (entity->has<file_component>())
		{
			// Folder
			const auto option = m_entity->get<client_option_component>();
			if (!option)
				return;

			const auto e = entity::create(m_entity);
			const auto folder = folder_component::create(option->get_download_path());
			e->add(folder);

			// File	
			const auto file = entity->copy<file_component>();
			boost::filesystem::path path = folder->get_data() / file->get_data();
			file->set_path(path);
			e->add(file);

			// Transfer
			const auto transfer = transfer_component::create(transfer_type::download);
			e->add(transfer);

			call(function_type::transfer, function_action::add, e);
		}
	}

	// Slot
	void client_search_model::on_add(const entity::ptr entity)
	{
		assert(thread::main());

		const auto search_vector = m_entity->get<search_entity_vector_component>();
		if (!search_vector)
			return;

		{
			auto_lock_ptr(search_vector);

			const auto row = search_vector->size();

			beginInsertRows(QModelIndex(), row, row);
			search_vector->add(entity);
			endInsertRows();
		}

		set_status(client_statusbar::search, search_vector->size());
	}

	void client_search_model::on_remove(const entity::ptr entity)
	{
		assert(thread::main());

		const auto search_vector = m_entity->get<search_entity_vector_component>();
		if (!search_vector)
			return;

		{
			auto_lock_ptr(search_vector);

			const auto row = search_vector->get(entity);
			if (row == type::npos)
				return;

			beginRemoveRows(QModelIndex(), row, row);
			search_vector->remove(row);
			endRemoveRows();
		}

		set_status(client_statusbar::search, search_vector->size());
	}

	void client_search_model::on_clear()
	{
		assert(thread::main());

		const auto search_vector = m_entity->get<search_entity_vector_component>();
		if (!search_vector)
			return;

		beginResetModel();
		search_vector->clear();
		endResetModel();

		set_status(client_statusbar::search, search_vector->size());
	}

	// Utility
	bool client_search_model::empty() const
	{
		const auto search_vector = m_entity->get<search_entity_vector_component>();
		return !search_vector || search_vector->empty();
	}

	// Model
	bool client_search_model::removeRows(int row, int count, const QModelIndex& parent /*= QModelIndex()*/)
	{
		assert(thread::main());

		const auto search_vector = m_entity->get<search_entity_vector_component>();
		if (!search_vector)
			return false;

		size_t status = 0;

		beginRemoveRows(QModelIndex(), row, (row + count - 1));

		for (auto i = (row + count - 1); i >= row; --i)
		{
			const auto entity = search_vector->get(row);
			if (entity)
				status += search_vector->remove(row);
		}

		endRemoveRows();

		set_status(client_statusbar::search, search_vector->size());

		return status > 0;
	}

	QVariant client_search_model::data(const QModelIndex& index, int role) const
	{
		if (!index.isValid())
			return QVariant();

		const size_t row = static_cast<size_t>(index.row());
		const size_t col = static_cast<size_t>(index.column());

		const auto search_vector = m_entity->get<search_entity_vector_component>();
		if (!search_vector)
			return QVariant();

		const auto entity = search_vector->get(row);
		if (!entity)
			return QVariant();

		switch (role)
		{
			case Qt::DisplayRole:
			{
				switch (col)
				{
					case column::number:
					{
						const auto search = entity->get<search_component>();
						if (search)
							return QVariant::fromValue<uint>(search->get_swarm());

						break;
					}
					case column::name:
					{
						return QVariant::fromValue(entity);
					}
					case column::size:
					{
						const auto file = entity->get<file_component>();
						if (file)
							return QString::fromStdString(file_util::get_size(file->get_size()));

						break;
					}					
					case column::type:
					{
						const auto file = entity->get<file_component>();
						if (file)
							return QString::fromStdString(file->get_description());

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
						const auto file = entity->get<file_component>();
						if (file)
							return QString::fromStdString(file->get_name()).toLower();

						break;
					}
					case column::size:
					{
						const auto file = entity->get<file_component>();
						if (file)
							return QVariant::fromValue<qulonglong>(file->get_size());

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
						const auto file = entity->get<file_component>();
						if (file)
							return QString::fromStdString(file->get_name());

						break;
					}
				}

				break;
			}
			case Qt::TextAlignmentRole:
			{
				switch (col)
				{
					case column::number:
					{
						return static_cast<int>(Qt::AlignVCenter | Qt::AlignHCenter);
					}
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

	int client_search_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		const auto search_vector = m_entity->get<search_entity_vector_component>();
		return search_vector ? search_vector->size() : 0;
	}

	// Accessor
	size_t client_search_model::get_row(const entity::ptr entity) const
	{
		const auto search_vector = m_entity->get<search_entity_vector_component>();
		return search_vector ? search_vector->get(entity) : type::npos;
	}
}
