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
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/client/client_service_component.h"
#include "component/group/group_component.h"
#include "component/group/group_security_component.h"
#include "component/message/message_service_component.h"
#include "component/session/session_component.h"
#include "component/status/status_component.h"
#include "component/time/timeout_component.h"

#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "model/client/client_message_model.h"
#include "security/checksum/crc.h"
#include "security/filter/hex.h"
#include "thread/thread_info.h"
#include "utility/value.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Interface
	void client_message_model::on_init()
	{
		assert(thread_info::main());

		entity_table_model::on_init();

		// Signal
		connect(this, &client_message_model::set_status, [&](const entity::ptr entity) { update(entity); });

		// Status
		m_status_function = function::create([this](const entity::ptr entity) { emit set_status(entity); });
		const auto message_list = m_entity->get<message_list_component>();

		for (const auto& e : *message_list)
		{
			const auto status = e->get<status_component>();
			status->add(m_status_function);
		}

		// Callback
		const auto e = entity_factory::create_statusbar(statusbar::message);
		m_entity->call(callback_type::status, callback_action::add, e);
	}

	void client_message_model::on_shutdown()
	{
		assert(thread_info::main());

		entity_table_model::on_shutdown();

		// Status
		const auto message_list = m_entity->get<message_list_component>();

		for (const auto& e : *message_list)
		{
			const auto status = e->get<status_component>();
			status->remove(m_status_function);
		}

		m_status_function.reset();
	}

	// Slot
	void client_message_model::on_remove(const QModelIndex& index)
	{
		assert(thread_info::main());

		entity_table_model::on_remove(index);

		const auto message_list = m_entity->get<message_list_component>();

		if (message_list->empty() && m_entity->enabled())
		{
			const auto service = m_entity->get<client_service_component>();
			if (service->valid())
				service->stop();

			m_entity->disable();
		}

		// Callback
		const auto e = entity_factory::create_statusbar(statusbar::message);
		m_entity->call(callback_type::status, callback_action::remove, e);
	}

	void client_message_model::on_clear()
	{
		assert(thread_info::main());

		const auto message_list = m_entity->get<message_list_component>();

		for (const auto& e : *message_list)
		{
			const auto status = e->get<status_component>();
			status->set_status(eja::status::none);

			const auto group_list = e->get<group_security_list_component>();
			for (const auto& e : *group_list)
			{
				const auto status = e->get<status_component>();
				status->set_status(eja::status::none);
			}
		}
	}

	void client_message_model::on_update()
	{
		assert(thread_info::main());

		// Client
		const auto service = m_entity->get<client_service_component>();
		if (service->valid())
			service->async_message();
	}

	void client_message_model::on_update(const QModelIndex& index)
	{
		assert(thread_info::main());

		const auto service = m_entity->get<client_service_component>();
		if (service->invalid())
			return;

		const auto entity = get_entity(index);
		if (entity && entity->enabled())
			service->async_message(entity);
	}

	// Utility
	bool client_message_model::empty() const
	{
		assert(thread_info::main());

		const auto message_list = m_entity->get<message_list_component>();
		return message_list->empty();
	}

	void client_message_model::set_modified(const entity::ptr entity /*= nullptr*/)
	{
		assert(thread_info::main());

		const auto service = entity->get<message_service_component>();
		if (service->invalid())
			return;

		// Status
		const auto status = entity->get<status_component>();
		status->set_status(eja::status::pending);

		const auto group_list = entity->get<group_security_list_component>();
		for (const auto& e : *group_list)
		{
			if (e->enabled())
			{
				const auto status = e->get<status_component>();
				status->set_status(eja::status::pending);
			}			
		}
	}

	// Model
	bool client_message_model::insertRows(int row, int count, const QModelIndex& parent /*= QModelIndex()*/)
	{
		assert(thread_info::main());

		const auto message_list = m_entity->get<message_list_component>();

		beginInsertRows(QModelIndex(), row, (row + count - 1));

		for (auto i = 0; i < count; ++i)
		{
			// Entity
			const auto entity = entity_factory::create_client_message(m_entity);
			entity->disable();

			// Endpoint
			const auto endpoint = entity->get<endpoint_component>();
			endpoint->set_name(router::name);
			endpoint->set_address(router::address);
			endpoint->set_port(router::port);

			// Status
			const auto status = entity->get<status_component>();
			status->add(m_status_function);

			message_list->push_back(entity);
		}

		endInsertRows();

		// Callback
		const auto e = entity_factory::create_statusbar(statusbar::message);
		m_entity->call(callback_type::status, callback_action::add, e);

		return true;
	}


	bool client_message_model::removeRows(int row, int count, const QModelIndex& parent /*= QModelIndex()*/)
	{
		assert(thread_info::main());

		const auto message_list = m_entity->get<message_list_component>();
		const auto it = message_list->cbegin() + row;
		const auto size = message_list->size();

		for (auto it = message_list->cbegin() + row; it != message_list->cbegin() + row + count; ++it)
		{
			const auto& e = *it;
			const auto service = e->get<message_service_component>();
			if (service->valid())
				service->async_quit();
		}

		beginRemoveRows(QModelIndex(), row, (row + count - 1));
		message_list->erase(it, it + count);
		endRemoveRows();

		// Client
		const auto service = m_entity->get<client_service_component>();
		if (service->valid())
			service->update();

		// Callback
		const auto e = entity_factory::create_statusbar(statusbar::message);
		m_entity->call(callback_type::status, callback_action::remove, e);

		return message_list->size() < size;
	}

	// Model
	Qt::ItemFlags client_message_model::flags(const QModelIndex& index) const
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

	QVariant client_message_model::data(const QModelIndex& index, int role) const
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
					case column::row:
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
					case column::group:
					{
						const auto group = entity->get<group_component>();
						const auto group_id = group->has_id() ? hex::encode(static_cast<u32>(crc32()(group->get_id()))) : "";
						return QString::fromStdString(group_id);
					}
					case column::version:
					{
						const auto version = entity->get<version_component>();
						return QString::fromStdString(version->str());
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
					/*case column::row:
					{
						return QVariant::fromValue<uint>(row);
					}*/
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
			case Qt::TextAlignmentRole:
			{
				switch (col)
				{
					case column::version:
					{
						return static_cast<int>(Qt::AlignVCenter | Qt::AlignRight);
					}
				}

				break;
			}
		}

		return QVariant();
	}

	/*bool client_server_model::moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild)
	{
		int i = 8;

		return true;
	}*/

	bool client_message_model::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
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

									// Client
									const auto client_service = m_entity->get<client_service_component>();
									if (client_service->valid())
									{
										client_service->update();
										client_service->async_message(entity);
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

									const auto group_list = entity->get<group_security_list_component>();
									for (const auto& e : *group_list)
									{
										const auto status = e->get<status_component>();
										status->set_status(eja::status::none);
									}

									// Client
									const auto client_service = m_entity->get<client_service_component>();
									if (client_service->valid())
									{
										client_service->update();
										client_service->async_message(entity);
									}										
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

	int client_message_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		assert(thread_info::main());

		const auto message_list = m_entity->get<message_list_component>();
		return message_list->size();
	}

	// Get
	entity::ptr client_message_model::get_entity(const size_t row) const
	{
		assert(thread_info::main());

		const auto message_list = m_entity->get<message_list_component>();
		return (row < message_list->size()) ? message_list->at(row) : nullptr;
	}

	size_t client_message_model::get_row(const entity::ptr entity) const
	{
		assert(thread_info::main());

		const auto message_list = m_entity->get<message_list_component>();
		const auto it = std::find(message_list->begin(), message_list->end(), entity);
		return (it != message_list->end()) ? (it - message_list->begin()) : npos;
	}
}
