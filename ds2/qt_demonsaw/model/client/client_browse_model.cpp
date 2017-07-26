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

#include <thread>
#include <boost/format.hpp>

#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QUrl>

#include "client_browse_model.h"
#include "entity/entity.h"
#include "command/client/request/client_group_request_command.h"
#include "component/color_component.h"
#include "component/empty_component.h"
#include "component/function_component.h"
#include "component/search_component.h"
#include "component/session_component.h"
#include "component/timer_component.h"
#include "component/client/client_component.h"
#include "component/client/client_option_component.h"
#include "component/error/error_component.h"
#include "component/router/router_component.h"
#include "component/router/router_security_component.h"
#include "component/transfer/chunk_component.h"
#include "component/transfer/transfer_component.h"
#include "http/http_socket.h"
#include "resource/icon_provider.h"
#include "resource/resource.h"
#include "window/client_window.h"
#include "window/main_window.h"
#include "window/window.h"

namespace eja
{
	// Interface
	void client_browse_model::init()
	{
		entity_tree_model::init();

		// Meta
		qRegisterMetaType<client_browse_request_command::ptr>("client_browse_request_command::ptr");
		qRegisterMetaType<http_status>("http_status");

		// Callback
		add_callback(function_type::browse);
	}

	void client_browse_model::refresh()
	{
		const auto owner = m_entity->shared_from_this();

		const auto socket = http_socket::create();

		try
		{
			const auto option = owner->get<client_option_component>();
			if (!option)
				return;

			const auto router = owner->get<router_component>();
			if (!router)
				return;

			// Socket
			socket->set_timeout(option->get_socket_timeout());
			socket->open(router->get_address(), router->get_port());

			// Command
			const auto request_command = client_group_request_command::create(owner, socket);
			const auto request_status = request_command->execute();

			switch (request_status.get_code())
			{
				case http_code::ok:
				{
					break;
				}
				case http_code::none:
				{
					owner->log("HTTP Timeout (browse)");
					break;
				}
				case http_code::not_found:
				{
					owner->log("HTTP 404 Not Found (browse)");
					break;
				}
				case http_code::not_implemented:
				{
					owner->log("HTTP 501 Not Implemented (browse)");
					break;
				}
				case http_code::bad_request:
				{
					// If the router sends a bad_request this means that it's overloaded, try again
					owner->log("HTTP 400 bad Request (browse)");
					break;
				}
				case http_code::service_unavailable:
				{
					owner->log("HTTP 503 Service Unavailable (browse)");
					break;
				}
				case http_code::internal_server_error:
				{
					owner->log("HTTP 500 Internal Server Error (browse)");
					break;
				}
				case http_code::unauthorized:
				{
					owner->log("HTTP 401 Unauthorized (browse)");
					break;
				}
				default:
				{
					owner->log("HTTP Unknown Code (browse)");
					break;
				}
			}
		}
		catch (std::exception& e)
		{
			owner->log(e);
		}
		catch (...)
		{
			owner->log("Unknown Exception (browse)");
		}

		socket->close();		
	}

	void client_browse_model::double_click(const QModelIndex& index)
	{
		if (!index.isValid())
			return;

		const auto entity = get_node(index);
		if (!entity)
			return;

		if (entity->has<file_component>(false))
		{
			// Entity
			const auto e = entity::create(m_entity);

			// Option
			const auto option = m_entity->get<client_option_component>();
			if (!option)
				return;

			// Folder
			const auto folder = folder_component::create(option->get_download_path());
			e->add(folder);

			// File
			const auto file = entity->copy<file_component>();
			boost::filesystem::path path = folder->get_data() / file->get_data();
			file->set_path(path);
			e->add(file);

			// Temp
			/*const auto temp_file = temp_file_component::create();
			boost::filesystem::path temp_path = folder->get_data() / (file->get_name() + default_file::extension);
			temp_file->set_path(temp_path);
			e->add(temp_file);*/

			// Transfer
			const auto transfer = transfer_component::create(transfer_type::download);
			e->add(transfer);

			call(function_type::transfer, function_action::add, e);
		}
	}

	void client_browse_model::double_click(const QModelIndex& index, const folder_component::ptr root)
	{
		if (!index.isValid())
			return;

		const auto entity = get_node(index);
		if (!entity)
			return;

		if (entity->has<file_component>(false))
		{
			// Entity
			const auto e = entity::create(m_entity);

			// Folder
			const auto folder = folder_component::create(root);
			e->add(folder);

			// File
			const auto file = entity->copy<file_component>();
			boost::filesystem::path path = folder->get_data() / file->get_data();
			file->set_path(path);
			e->add(file);

			// Temp
			/*const auto temp_file = temp_file_component::create();
			boost::filesystem::path temp_path = folder->get_data() / (file->get_name() + default_file::extension);
			temp_file->set_path(temp_path);
			e->add(temp_file);*/

			// Transfer
			const auto transfer = transfer_component::create(transfer_type::download);
			e->add(transfer);

			call(function_type::transfer, function_action::add, e);
		}
	}

	void client_browse_model::execute(const QModelIndex& index)
	{
		if (!index.isValid())
			return;

		const auto entity = get_node(index);
		if (!entity)
			return;

		if (entity->has<file_component>(false))
		{
			double_click(index);
		}
		else
		{
			const auto option = m_entity->get<client_option_component>();
			if (!option)
				return;

			const auto folder = entity->get<folder_component>();
			if (!folder)
				return;

			boost::filesystem::path path = option->get_download_path() / folder->get_data();
			if (!folder_util::exists(path))
				folder_util::create(path);

			const auto subfolder = folder_component::create(path);
			execute(index, subfolder);
		}
	}

	void client_browse_model::execute(const QModelIndex& index, const folder_component::ptr root)
	{
		if (!index.isValid())
			return;

		const auto entity = get_node(index);
		if (!entity)
			return;

		if (canFetchMore(index) && !entity->has<file_component>(false))
		{
			const auto browse_vector = browse_entity_vector_component::create();
			entity->add(browse_vector);
			fetchMore(entity, false);
		}

		const auto browse_vector = entity->get<browse_entity_vector_component>();
		if (!browse_vector)
			return;

		for (const auto& item : browse_vector->copy())
		{
			const auto index = this->index(item);

			if (item->has<file_component>(false))
			{
				double_click(index, root);
			}
			else
			{
				// Folder
				const auto folder = item->get<folder_component>();
				if (!folder)
					continue;

				boost::filesystem::path path = root->get_data() / folder->get_data();
				if (!folder_util::exists(path))
					folder_util::create(path);

				const auto subfolder = folder_component::create(path);

				execute(index, subfolder);
			}
		}
	}

	// Slot
	void client_browse_model::on_remove(const QModelIndex& index)
	{
		assert(thread::main());

		if (!index.isValid())
			return;

		const auto browse_vector = m_entity->get<browse_entity_vector_component>();
		if (!browse_vector)
			return;

		const auto entity = get_node(index);
		if (!entity)
			return;

		if (!browse_vector->has(entity))
			return;

		{
			auto_lock_ptr(browse_vector);

			const auto row = index.row();

			beginRemoveRows(QModelIndex(), row, row);
			browse_vector->remove(row);
			endRemoveRows();
		}

		set_status(client_statusbar::browse, browse_vector->size());
	}

	void client_browse_model::on_refresh(const entity::ptr entity)
	{
		assert(thread::main());

		const auto browse_vector = m_entity->get<browse_entity_vector_component>();
		if (!browse_vector)
			return;

		beginResetModel();

		browse_vector->clear();

		const auto client_vector = m_entity->get<client_entity_vector_component>();
		if (client_vector)
		{
			for (const auto& item : client_vector->copy())
			{
				const auto e = entity::create(m_entity);
				const auto client = item->get<client_component>();
				if (client)
				{
					e->add(client);

					const auto color = client ? color_component::create(client->get_name()) : color_component::create(QColor(default_chat::color));
					e->add(color);

					browse_vector->add(e);
				}
			}
		}

		endResetModel();

		set_status(client_statusbar::browse, browse_vector->size());
	}

	void client_browse_model::on_clear()
	{
		assert(thread::main());

		const auto browse_vector = m_entity->get<browse_entity_vector_component>();
		if (!browse_vector)
			return;

		beginResetModel();
		browse_vector->clear();
		endResetModel();

		set_status(client_statusbar::browse, browse_vector->size());
	}

	// Utility
	bool client_browse_model::empty() const
	{
		const auto browse_vector = m_entity->get<browse_entity_vector_component>();
		return !browse_vector || browse_vector->empty();
	}

	void client_browse_model::add_error(const entity::ptr parent, const std::string& message)
	{
		const auto browse_vector = parent->get<browse_entity_vector_component>();
		if (!browse_vector)
			return;

		const auto index = this->index(parent);
		if (!index.isValid())
			return;

		// Error
		const auto e = eja::entity::create(parent);
		const auto ec = error_component::create();
		ec->set_text(message);
		e->add(ec);

		// Browse
		const auto bv = browse_entity_vector_component::create();
		e->add(bv);

		// Modal
		beginInsertRows(index, 0, 0);
		browse_vector->add(e);
		endInsertRows();
	}

	void client_browse_model::add_empty(const entity::ptr parent)
	{
		const auto browse_vector = parent->get<browse_entity_vector_component>();
		if (!browse_vector)
			return;

		const auto index = this->index(parent);
		if (!index.isValid())
			return;

		const auto e = eja::entity::create(parent);
		const auto ec = empty_component::create();
		e->add(ec);

		const auto bv = browse_entity_vector_component::create();
		e->add(bv);

		// Modal
		beginInsertRows(index, 0, 0);
		browse_vector->add(e);
		endInsertRows();
	}

	// Model
	QVariant client_browse_model::data(const QModelIndex& index, int role) const
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
					case column::name:
					{
						return QVariant::fromValue(entity);
					}
					case column::size:
					{
						if (entity->has<file_component>(false))
						{
							const auto file = entity->get<file_component>();
							if (file)
								return QString::fromStdString(file_util::get_size(file->get_size()));

							break;
						}
					}
					case column::type:
					{
						if (entity->has<file_component>(false))
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
					case column::name:
					{
						if (entity->has<file_component>(false))
						{
							const auto file = entity->get<file_component>();
							if (file)
								return QString("~%1").arg(file->get_name().c_str()).toLower();

							break;
						}
						else if (entity->has<folder_component>(false))
						{
							const auto folder = entity->get<folder_component>();
							if (folder)
								return folder->is_drive() ? QString::fromStdString(folder->get_path()).toLower() : QString::fromStdString(folder->get_name()).toLower();

							break;
						}
						else if (entity->has<client_component>(false))
						{
							const auto client = entity->get<client_component>();
							if (client)
								return QString::fromStdString(client->get_name()).toLower();

							break;
						}
					}
					case column::size:
					{
						if (entity->has<file_component>(false))
						{
							const auto file = entity->get<file_component>();
							if (file)
								return QVariant::fromValue<qulonglong>(file->get_size());

							break;
						}
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
						if (entity->has<file_component>(false))
						{
							const auto file = entity->get<file_component>();
							if (file)
								return QString("~%1").arg(file->get_name().c_str());

							break;
						}
						else if (entity->has<folder_component>(false))
						{
							const auto folder = entity->get<folder_component>();
							if (folder)
								return folder->is_drive() ? QString::fromStdString(folder->get_path()) : QString::fromStdString(folder->get_name());

							break;
						}
						else if (entity->has<client_component>(false))
						{
							const auto client = entity->get<client_component>();
							if (client)
								return QString::fromStdString(client->get_name());

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

	bool client_browse_model::hasChildren(const QModelIndex& index /*= QModelIndex()*/) const
	{
		if (!index.isValid())
			return true;

		const auto entity = get_node(index);
		if (entity)
		{
			if (entity->has<browse_entity_vector_component>(false))
			{
				const auto browse_vector = entity->get<browse_entity_vector_component>();
				return browse_vector && !browse_vector->empty();
			}

			return true;
		}

		return false;
	}

	bool client_browse_model::canFetchMore(const QModelIndex& index) const
	{
		if (!index.isValid())
			return false;

		const auto entity = get_node(index);
		return entity && !entity->has<browse_entity_vector_component>(false);
	}

	void client_browse_model::fetchMore(const entity::ptr entity, const bool insert)
	{
		const auto browse_vector = entity->get<browse_entity_vector_component>();
		if (!browse_vector)
			return;

		const auto index = this->index(entity);
		if (!index.isValid())
			return;

		const auto option = entity->get<client_option_component>();
		if (!option)
			return;

		const auto router = m_entity->get<router_component>();
		if (!router)
			return;

		const auto socket = http_socket::create();

		try
		{
			// Socket
			socket->set_timeout(option->get_socket_timeout());
			socket->open(router->get_address(), router->get_port());

			// Command				
			const auto request_command = client_browse_request_command::create(m_entity, socket);
			const auto request_status = request_command->execute(entity);

			switch (request_status.get_code())
			{
				case http_code::ok:
				{
					const auto& folders = request_command->get_folders();
					const auto& files = request_command->get_files();
					const auto rows = folders.size() + files.size();

					if (rows)
					{
						if (insert)
							beginInsertRows(index, 0, rows - 1);

						// Folders
						auto_lock_ref(folders);

						for (const auto& item : folders)
						{
							// Client
							const auto e = entity::create(entity);
							const auto cc = entity->get<client_component>();
							if (!cc)
								continue;

							e->add(cc);

							// Folder
							const auto fc = folder_component::create(item);
							e->add(fc);

							// TODO: v2.0 default size should be 0 (not -1)
							//
							//
							// Browse
							const auto size = item->get_size();
							if (!size || (size == static_cast<u64>(-1)))
							{
								const auto bc = browse_entity_vector_component::create();
								e->add(bc);
							}

							browse_vector->add(e);
						}

						// Files
						auto_lock_ref(files);

						for (const auto& item : files)
						{
							// File
							const auto e = entity::create(entity);
							const auto fc = file_component::create(item);
							e->add(fc);

							// Browse
							const auto bc = browse_entity_vector_component::create();
							e->add(bc);

							browse_vector->add(e);
						}

						if (insert)
							endInsertRows();
					}
					else
					{
						add_empty(entity);
					}

					break;
				}
				case http_code::none:
				{
					add_error(entity, "HTTP Timeout");
					m_entity->log("HTTP Timeout (browse)");
					break;
				}
				case http_code::not_found:
				{
					add_error(entity, "HTTP 404 Not Found");
					m_entity->log("HTTP 404 Not Found (browse)");
					break;
				}
				case http_code::not_implemented:
				{
					add_error(entity, "HTTP 501 Not Implemented");
					m_entity->log("HTTP 501 Not Implemented (browse)");
					break;
				}
				case http_code::bad_request:
				{
					// If the router sends a bad_request this means that it's overloaded, try again
					add_error(entity, "HTTP 400 Bad Request");
					m_entity->log("HTTP 400 Bad Request (browse)");
					break;
				}
				case http_code::service_unavailable:
				{
					add_error(entity, "HTTP 503 Service Unavailable");
					m_entity->log("HTTP 503 Service Unavailable (browse)");
					break;
				}
				case http_code::internal_server_error:
				{
					add_error(entity, "HTTP 500 Internal Server Error");
					m_entity->log("HTTP 500 Internal Server Error (browse)");
					break;
				}
				case http_code::unauthorized:
				{
					add_error(entity, "HTTP 401 Unauthorized");
					m_entity->log("HTTP 401 Unauthorized (browse)");
					break;
				}

				default:
				{
					add_error(entity, "HTTP Unknown Code");
					m_entity->log("HTTP Unknown Code (browse)");
					break;
				}
			}
		}
		catch (std::exception& e)
		{
			add_error(entity, e.what());
			m_entity->log(e);
		}
		catch (...)
		{
			add_error(entity, "Unknown Exception (browse)");
			m_entity->log();
		}

		socket->close();
	}

	void client_browse_model::fetchMore(const QModelIndex& index)
	{
		const auto entity = get_node(index);
		if (entity && !entity->has<file_component>(false))
		{
			const auto browse_vector = browse_entity_vector_component::create();
			entity->add(browse_vector);

			std::thread thread([this, entity]() { fetchMore(entity, true); });
			thread.detach();
		}
	}

	int client_browse_model::rowCount(const QModelIndex& index /*= QModelIndex()*/) const
	{
		const auto entity = get_node(index);

		if (entity && entity->has<browse_entity_vector_component>(false))
		{
			const auto browse_vector = entity->get<browse_entity_vector_component>();
			if (browse_vector)
				return browse_vector->size();
		}

		return 0;
	}

	QModelIndex client_browse_model::index(int row, int column, const QModelIndex& parent) const
	{
		if (row < 0 || column < 0)
			return QModelIndex();

		const auto entity = get_node(parent);
		if (!entity)
			return QModelIndex();

		if (!entity->has<browse_entity_vector_component>(false))
			return QModelIndex();

		const auto browse_vector = entity->get<browse_entity_vector_component>();
		if (!browse_vector)
			return QModelIndex();

		const auto child = browse_vector->get(row);
		if (!child)
			return QModelIndex();

		return createIndex(row, column, child.get());
	}

	QModelIndex client_browse_model::index(const entity::ptr entity) const
	{
		const auto parent = get_parent(entity);
		if (!parent)
			return QModelIndex();

		if (!parent->has<browse_entity_vector_component>(false))
			return QModelIndex();

		const auto browse_vector = parent->get<browse_entity_vector_component>();
		if (!browse_vector)
			return QModelIndex(); 
		
		const auto row = browse_vector->get(entity);
		if (row == type::npos)
			return QModelIndex();

		return createIndex(row, 0, entity.get());
	}

	QModelIndex client_browse_model::parent(const QModelIndex& index) const
	{
		const auto parent = get_parent(index);
		if (!parent)
			return QModelIndex();

		const auto grandparent = get_parent(parent);
		if (!grandparent)
			return QModelIndex();

		if (!grandparent->has<browse_entity_vector_component>(false))
			return QModelIndex();

		const auto browse_vector = grandparent->get<browse_entity_vector_component>();
		if (!browse_vector)
			return QModelIndex();
		
		const auto row = browse_vector->get(parent);
		if (row == type::npos)
			return QModelIndex();

		return createIndex(row, 0, parent.get());
	}

	// Accessor
	size_t client_browse_model::get_row(const entity::ptr entity) const
	{
		const auto parent = get_parent(entity);
		if (!parent)
			return type::npos;

		if (!parent->has<browse_entity_vector_component>(false))
			return type::npos;

		const auto share = parent->get<browse_entity_vector_component>();
		return share && share->get(entity);
	}

	folder_component::ptr client_browse_model::get_folder(const entity::ptr entity) const
	{
		const auto option = m_entity->get<client_option_component>();
		if (!option)
			return nullptr;

		boost::filesystem::path path = option->get_download_path();

		const auto parent = get_parent(entity);
		if (parent && parent->has<folder_component>(false))
		{
			const auto folder = parent->get<folder_component>();
			if (folder)
				path /= folder->get_data();
		}

		return folder_component::create(path);
	}
}
