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
#include <QLineEdit>
#include <QFontMetrics>
#include <QIcon>
#include <QSize>

#include "router_server_model.h"
#include "component/error/error_component.h"
#include "component/server/server_machine_component.h"
#include "component/server/server_option_component.h"
#include "component/server/server_security_component.h"
#include "resource/resource.h"
#include "utility/server/server_util.h" 
#include "window/router_window.h"
#include "window/window.h"

namespace eja
{
	// Interface
	void router_server_model::init()
	{
		entity_table_model::init();

		// Callback
		const auto lambda = [this](const entity::ptr entity) { on_refresh(entity); };
		m_function = function::create(lambda);

		const auto server_vector = m_entity->get<server_entity_vector_component>();
		if (!server_vector)
			return;

		for (const auto& entity : server_vector->copy())
		{
			const auto machine = entity->get<server_machine_component>();
			if (machine)
				machine->add(m_function);
		}
	}

	void router_server_model::shutdown()
	{
		entity_table_model::shutdown();

		const auto server_vector = m_entity->get<server_entity_vector_component>();
		if (!server_vector)
			return;

		for (const auto& entity : server_vector->copy())
		{
			const auto machine = entity->get<server_machine_component>();
			if (machine)
				machine->remove(m_function);
		}
	}

	void router_server_model::remove(const entity::ptr entity)
	{
		entity->shutdown();

		emit removed(entity);
	}

	void router_server_model::refresh()
	{
		const auto server_vector = m_entity->get<server_entity_vector_component>();
		if (!server_vector)
			return;

		for (const auto& entity : server_vector->copy())
		{
			const auto option = entity->get<server_option_component>();
			if (option && option->enabled())
			{
				const auto machine = entity->get<server_machine_component>();
				if (machine && machine->sleeping())
					machine->restart();
			}
		}

		emit refreshed();
	}

	void router_server_model::refresh(const QModelIndex& index)
	{
		const auto row = index.row();
		if (row == type::npos)
			return;

		const auto server_vector = m_entity->get<server_entity_vector_component>();
		if (!server_vector)
			return;

		const auto entity = server_vector->get(row);
		if (!entity)
			return;

		const auto option = entity->get<server_option_component>();
		if (option && option->enabled())
		{
			const auto machine = entity->get<server_machine_component>();
			if (machine)
				machine->restart();
		}

		emit on_refresh(index);
	}

	void router_server_model::clear()
	{
		const auto server_vector = m_entity->get<server_entity_vector_component>();
		if (!server_vector)
			return;

		for (const auto& entity : server_vector->copy())
			entity->shutdown();

		emit cleared();
	}

	// Slot
	void router_server_model::on_add(const entity::ptr entity)
	{
		assert(thread::main());

		const auto server_vector = m_entity->get<server_entity_vector_component>();
		if (!server_vector)
			return;

		{
			auto_lock_ptr(server_vector);

			const auto row = server_vector->size();

			beginInsertRows(QModelIndex(), row, row);
			server_vector->add(entity);
			endInsertRows();
		}

		set_status(router_statusbar::router, server_vector->size());
	}

	void router_server_model::on_remove(const entity::ptr entity)
	{
		assert(thread::main());

		const auto server_vector = m_entity->get<server_entity_vector_component>();
		if (!server_vector)
			return;

		{
			auto_lock_ptr(server_vector);

			const auto row = server_vector->get(entity);
			if (row == type::npos)
				return;

			beginRemoveRows(QModelIndex(), row, row);
			server_vector->remove(row);
			endRemoveRows();
		}

		set_status(router_statusbar::router, server_vector->size());
	}

	void router_server_model::on_clear()
	{
		assert(thread::main());

		const auto server_vector = m_entity->get<server_entity_vector_component>();
		if (!server_vector)
			return;

		beginResetModel();
		server_vector->clear();
		endResetModel();

		set_status(router_statusbar::router, server_vector->size());
	}

	// Utility
	bool router_server_model::empty() const
	{
		const auto server_vector = m_entity->get<server_entity_vector_component>();
		return !server_vector || server_vector->empty();
	}

	void router_server_model::post_create()
	{
		const auto server_vector = m_entity->get<server_entity_vector_component>();
		if (!server_vector)
			return;

		set_status(router_statusbar::router, server_vector->size());
	}

	// Model
	bool router_server_model::insertRows(int row, int count, const QModelIndex& parent /*= QModelIndex()*/)
	{
		assert(thread::main());

		const auto server_vector = m_entity->get<server_entity_vector_component>();
		if (!server_vector)
			return false;

		beginInsertRows(QModelIndex(), row, (row + count - 1));

		for (auto i = 0; i < count; ++i)
		{
			// Entity
			const auto entity = server_util::create(m_entity);

			// Server
			const auto server = entity->get<server_component>();
			if (server)
				server->set_name(default_router::name);

			// Machine
			const auto machine = entity->get<server_machine_component>();
			if (machine)
				machine->add(m_function);

			server_vector->add(entity);
		}
		
		endInsertRows();

		set_status(router_statusbar::router, server_vector->size());

		return true;
	}

	bool router_server_model::removeRows(int row, int count, const QModelIndex& parent /*= QModelIndex()*/)
	{
		assert(thread::main());

		const auto server_vector = m_entity->get<server_entity_vector_component>();
		if (!server_vector)
			return false;

		size_t status = 0;

		beginRemoveRows(QModelIndex(), row, (row + count - 1));

		for (auto i = (row + count - 1); i >= row; --i)
		{
			const auto entity = server_vector->get(row);
			if (entity)
			{
				entity->shutdown();
				status += server_vector->remove(row);
			}
		}

		endRemoveRows();

		set_status(router_statusbar::router, server_vector->size());
		
		return status > 0;
	}

	Qt::ItemFlags router_server_model::flags(const QModelIndex& index) const
	{
		if (!index.isValid())
			return Qt::ItemIsEnabled;

		Qt::ItemFlags flags = QAbstractItemModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemIsSelectable /*| Qt::ItemIsDropEnabled*/;
		/*if (index.isValid())
			flags |= Qt::ItemIsDragEnabled;*/

		switch (index.column())
		{
			case column::name:
			{
				flags |= Qt::ItemIsEditable | Qt::ItemIsUserCheckable;
				break;
			}
			case column::version:
			{
				break;
			}
			default:
			{
				flags |= Qt::ItemIsEditable;
				break;
			}
		}

		return flags;
	}

	QVariant router_server_model::data(const QModelIndex& index, int role) const
	{
		if (!index.isValid())
			return QVariant();

		const size_t row = static_cast<size_t>(index.row());
		const size_t col = static_cast<size_t>(index.column());

		const auto server_vector = m_entity->get<server_entity_vector_component>();
		if (!server_vector)
			return QVariant();

		const auto entity = server_vector->get(row);
		if (!entity)
			return QVariant();

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
					case column::version:
					{
						const auto server = entity->get<server_component>();
						if (server)
							return QString::fromStdString(server->get_version());

						break;
					}
					case column::address:
					{
						const auto server = entity->get<server_component>();
						if (server)
							return QString::fromStdString(server->get_address());

						break;
					}
					case column::port:
					{
						const auto server = entity->get<server_component>();
						if (server)
							return QVariant::fromValue<uint>(server->get_port());

						break;
					}
					case column::passphrase:
					{
						const auto security = entity->get<server_security_component>();
						if (security)
							return QString::fromStdString(security->get_passphrase());

						break;
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
						const auto server = entity->get<server_component>();
						if (server)
							return QString::fromStdString(server->get_name());

						break;
					}
					default:
					{
						return data(index, Qt::DisplayRole);
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
						const auto server = entity->get<server_component>();
						if (server)
							return QString::fromStdString(server->get_name()).toLower();

						break;
					}
					case column::address:
					case column::passphrase:
					{
						const auto variant = data(index, Qt::EditRole);
						return variant.toString().toLower();
					}
					default:
					{
						return data(index, Qt::DisplayRole);
					}
				}
				break;
			}
			case Qt::CheckStateRole:
			{
				switch (col)
				{
					case column::name:
					{
						const auto entity = server_vector->get(row);
						if (entity)
						{
							const auto option = entity->get<server_option_component>();
							if (option)
								return option->enabled() ? Qt::Checked : Qt::Unchecked;

							break;
						}
					}
				}
				break;
			}
		}

		return QVariant();
	}

	/*bool router_server_model::moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild)
	{
		int i = 8;

		return true;
	}*/

	bool router_server_model::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
	{
		if (!index.isValid())
			return false;

		const size_t row = static_cast<size_t>(index.row());
		const size_t col = static_cast<size_t>(index.column());
		bool modified = false;

		const auto server_vector = m_entity->get<server_entity_vector_component>();
		if (!server_vector)
			return false;

		const auto entity = server_vector->get(row);
		if (!entity)
			return false;

		switch (role)
		{
			case Qt::EditRole:
			{
				switch (col)
				{
					case column::name:
					{
						const auto name = value.toString().toStdString();
						const auto server = entity->get<server_component>();
						if (server)
							server->set_name(name);

						break;
					}
					case column::address:
					{
						const auto address = value.toString().toStdString();
						const auto server = entity->get<server_component>();
						if (server && address != server->get_address())
						{
							server->set_address(address);
							modified = true;
						}

						break;
					}
					case column::port:
					{
						// Validate
						const auto port = value.toUInt();
						if ((port < 0) || (port > u16_max))
							return false;

						const auto server = entity->get<server_component>();
						if (server && port != server->get_port())
						{
							server->set_port(port);
							modified = true;
						}

						break;
					}
					case column::passphrase:
					{
						const auto passphrase = value.toString().toStdString();
						const auto security = entity->get<server_security_component>();
						if (security && passphrase != security->get_passphrase())
						{
							security->set_passphrase(passphrase);
							modified = true;
						}

						break;
					}
				}

				break;
			}
			case Qt::CheckStateRole:
			{
				switch (col)
				{
					case column::name:
					{
						const auto option = entity->get<server_option_component>();
						if (option)
						{
							switch (value.toInt())
							{
								case Qt::Checked:
								{
									if (option->disabled())
									{
										option->set_enabled();
										entity_table_model::refresh(index);

										const auto machine = entity->get<server_machine_component>();
										if (machine)
											machine->start();
									}
									break;
								}
								case Qt::Unchecked:
								{
									if (option->enabled())
									{
										option->set_disabled();
										entity_table_model::refresh(index);

										const auto machine = entity->get<server_machine_component>();
										if (machine)
											machine->stop();
									}
									break;
								}
							}
						}

						break;
					}
				}

				break;
			}
		}

		if (modified)
		{
			// Machine
			const auto option = entity->get<server_option_component>();
			if (option && option->enabled())
			{
				const auto machine = entity->get<server_machine_component>();
				if (machine)
					machine->restart();
			}
		}

		return modified;
	}


	int router_server_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		const auto session_list = m_entity->get<server_entity_vector_component>();
		return session_list ? session_list->size() : 0;
	}

	size_t router_server_model::get_row(const entity::ptr entity) const
	{
		const auto server_vector = m_entity->get<server_entity_vector_component>();
		return server_vector ? server_vector->get(entity) : type::npos;
	}
}
