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

#include "component/endpoint_component.h"
#include "component/version_component.h"
#include "component/group/group_security_component.h"
#include "component/message/message_acceptor_component.h"
#include "component/session/session_component.h"
#include "component/status/status_component.h"
#include "component/time/timeout_component.h"
#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "model/router/router_transfer_model.h"
#include "thread/thread_info.h"
#include "utility/value.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Interface
	void router_transfer_model::on_init()
	{
		assert(thread_info::main());

		entity_table_model::on_init();

		// Signal
		connect(this, &router_transfer_model::set_status, [&](const entity::ptr entity) { update(entity); });

		// Status
		m_status_function = function::create([this](const entity::ptr entity) { emit set_status(entity); });
		const auto transfer_list = m_entity->get<transfer_list_component>();

		for (const auto& entity : *transfer_list)
		{
			const auto status = entity->get<status_component>();
			status->add(m_status_function);
		}
	}

	void router_transfer_model::on_shutdown()
	{
		assert(thread_info::main());

		entity_table_model::on_shutdown();

		// Status
		const auto transfer_list = m_entity->get<transfer_list_component>();

		for (const auto& entity : *transfer_list)
		{
			const auto status = entity->get<status_component>();
			status->remove(m_status_function);
		}

		m_status_function.reset();
	}

	// Slot
	void router_transfer_model::on_clear()
	{
		assert(thread_info::main());

		const auto transfer_list = m_entity->get<transfer_list_component>();

		for (const auto& entity : *transfer_list)
		{
			const auto status = entity->get<status_component>();
			status->set_status(eja::status::none);
		}
	}

	void router_transfer_model::on_update()
	{
		assert(thread_info::main());

		const auto acceptor = m_entity->get<message_acceptor_component>();
		
		if (acceptor->valid())
		{
			acceptor->update();
			acceptor->async_ping();
		}
	}

	void router_transfer_model::on_update(const QModelIndex& index)
	{
		assert(thread_info::main());

		const auto acceptor = m_entity->get<message_acceptor_component>();
		if (acceptor->invalid())
			return;

		const auto entity = get_entity(index);
		
		if (entity && entity->enabled())
		{
			acceptor->update();
			acceptor->async_ping(entity);
		}
	}

	// Utility
	bool router_transfer_model::empty() const
	{
		assert(thread_info::main());

		const auto transfer_list = m_entity->get<transfer_list_component>();
		return transfer_list->empty();
	}

	void router_transfer_model::set_modified(const entity::ptr entity /*= nullptr*/)
	{
		assert(thread_info::main());

		const auto acceptor = m_entity->get<message_acceptor_component>();		
		if (acceptor->invalid())
			return;

		// Status
		const auto status = entity->get<status_component>();
		status->set_status(eja::status::pending);
	}

	// Model
	bool router_transfer_model::insertRows(int row, int count, const QModelIndex& parent /*= QModelIndex()*/)
	{
		assert(thread_info::main());

		const auto transfer_list = m_entity->get<transfer_list_component>();

		beginInsertRows(QModelIndex(), row, (row + count - 1));

		for (auto i = 0; i < count; ++i)
		{
			// Entity
			const auto entity = entity_factory::create_client_transfer(m_entity);
			entity->disable();

			const auto endpoint = entity->get<endpoint_component>();
			endpoint->set_name(router::name);
			endpoint->set_address(router::address);
			endpoint->set_port(router::port);

			// Status
			const auto status = entity->get<status_component>();
			status->add(m_status_function);

			transfer_list->push_back(entity);
		}

		endInsertRows();

		return true;
	}

	bool router_transfer_model::removeRows(int row, int count, const QModelIndex& parent /*= QModelIndex()*/)
	{
		assert(thread_info::main());

		const auto transfer_list = m_entity->get<transfer_list_component>();
		const auto it = transfer_list->cbegin() + row;
		const auto size = transfer_list->size();

		beginRemoveRows(QModelIndex(), row, (row + count - 1));
		transfer_list->erase(it, it + count);
		endRemoveRows();

		// Update
		const auto acceptor = m_entity->get<message_acceptor_component>();
		if (acceptor->valid())
			acceptor->update();

		return transfer_list->size() < size;
	}

	// Model
	Qt::ItemFlags router_transfer_model::flags(const QModelIndex& index) const
	{
		assert(thread_info::main());

		if (!index.isValid())
			return Qt::ItemIsEnabled;

		Qt::ItemFlags flags = QAbstractItemModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemIsSelectable /*| Qt::ItemIsDropEnabled*/;
		/*if (index.isValid())
		flags |= Qt::ItemIsDragEnabled;*/

		switch (index.column())
		{
			case column::row:
			{
				flags |= Qt::ItemIsUserCheckable;
				break;
			}
			case column::name:
			case column::address:
			case column::port:
			{
				flags |= Qt::ItemIsEditable;
				break;
			}
		}

		return flags;
	}

	QVariant router_transfer_model::data(const QModelIndex& index, int role) const
	{
		assert(thread_info::main());

		if (!index.isValid())
			return QVariant();

		const size_t row = static_cast<size_t>(index.row());
		const auto entity = get_entity(row);
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
					case column::address:
					{
						const auto endpoint = entity->get<endpoint_component>();
						return QString::fromStdString(endpoint->get_address());
					}
					case column::port:
					{
						const auto endpoint = entity->get<endpoint_component>();
						return QVariant::fromValue<uint>(endpoint->get_port());
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
						const auto endpoint = entity->get<endpoint_component>();
						return QString::fromStdString(endpoint->get_name());
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
					case column::row:
					{
						return QVariant::fromValue<uint>(row);
					}
					case column::name:
					{
						const auto endpoint = entity->get<endpoint_component>();
						return QString::fromStdString(endpoint->get_name()).toLower();
					}
					case column::address:
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
					case column::row:
					{
						return entity->enabled() ? Qt::Checked : Qt::Unchecked;
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

	bool router_transfer_model::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
	{
		assert(thread_info::main());

		if (!index.isValid())
			return false;

		const size_t row = static_cast<size_t>(index.row());
		const auto entity = get_entity(row);
		if (!entity)
			return false;

		bool modified = false;
		const size_t col = static_cast<size_t>(index.column());

		switch (role)
		{
			case Qt::EditRole:
			{
				switch (col)
				{
					case column::name:
					{
						const auto name = value.toString().trimmed().toStdString();
						const auto endpoint = entity->get<endpoint_component>();
						endpoint->set_name(name);
						break;
					}
					case column::address:
					{
						const auto address = value.toString().trimmed().toStdString();
						const auto endpoint = entity->get<endpoint_component>();
						if (address != endpoint->get_address())
						{
							endpoint->set_address(address);
							
							if (entity->enabled())
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

						const auto endpoint = entity->get<endpoint_component>();
						if (port != endpoint->get_port())
						{
							endpoint->set_port(port);
							
							if (entity->enabled())
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
					case column::row:
					{
						switch (value.toInt())
						{
							case Qt::Checked:
							{
								if (entity->disabled())
								{									
									entity->enable();

									entity_table_model::on_update(index);

									const auto acceptor = m_entity->get<message_acceptor_component>();
									if (acceptor->valid())
									{
										acceptor->update();
										acceptor->async_ping(entity);
									}
								}

								break;
							}
							case Qt::Unchecked:
							{
								if (entity->enabled())
								{
									entity->disable();

									entity_table_model::on_update(index);

									// Status
									const auto status = entity->get<status_component>();
									status->set_status(eja::status::none);

									// Update
									const auto acceptor = m_entity->get<message_acceptor_component>();
									if (acceptor->valid())
										acceptor->update();
								}

								break;
							}
						}
					}
				}

				break;
			}
		}

		if (modified)
			set_modified(entity);

		return modified;
	}

	int router_transfer_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		assert(thread_info::main());

		const auto transfer_list = m_entity->get<transfer_list_component>();
		return transfer_list->size();
	}

	// Get
	entity::ptr router_transfer_model::get_entity(const size_t row) const
	{
		assert(thread_info::main());

		const auto transfer_list = m_entity->get<transfer_list_component>();
		return (row < transfer_list->size()) ? transfer_list->at(row) : nullptr;
	}

	size_t router_transfer_model::get_row(const entity::ptr entity) const
	{
		assert(thread_info::main());

		const auto transfer_list = m_entity->get<transfer_list_component>();
		const auto it = std::find(transfer_list->begin(), transfer_list->end(), entity);
		return (it != transfer_list->end()) ? (it - transfer_list->begin()) : npos;
	}
}
