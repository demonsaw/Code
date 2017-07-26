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
#include "component/session/session_component.h"
#include "component/status/status_component.h"
#include "component/time/timeout_component.h"
#include "component/transfer/transfer_service_component.h"

#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "model/client/client_transfer_model.h"
#include "security/checksum/crc.h"
#include "security/filter/hex.h"
#include "thread/thread_info.h"
#include "utility/value.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Interface
	void client_transfer_model::on_init()
	{
		assert(thread_info::main());

		entity_table_model::on_init();

		// Signal
		connect(this, &client_transfer_model::set_status, [&](const entity::ptr entity) { update(entity); });

		// Status
		m_status_function = function::create([this](const entity::ptr entity) { emit set_status(entity); });
		const auto transfer_list = m_entity->get<transfer_list_component>();

		for (const auto& e : *transfer_list)
		{
			// Status
			const auto status = e->get<status_component>();
			status->add(m_status_function);

			// Transfer
			if (e->enabled())
			{
				const auto service = e->get<transfer_service_component>();
				if (service->invalid())
					service->start();
			}
		}

		// Client
		const auto service = m_entity->get<client_service_component>();
		if (service->valid())
			service->update();

		// Callback
		const auto e = entity_factory::create_statusbar(statusbar::transfer);
		m_entity->call(callback_type::status, callback_action::add, e);
	}

	void client_transfer_model::on_shutdown()
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
	void client_transfer_model::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto transfer_list = m_entity->get<transfer_list_component>();
		const auto row = transfer_list->size();

		if (entity->has<transfer_list_component>())
		{						
			const auto list = entity->get<transfer_list_component>();
			transfer_list->reserve(transfer_list->size() + list->size());

			beginInsertRows(QModelIndex(), row, row + list->size() - 1);
			transfer_list->insert(transfer_list->end(), list->begin(), list->end());
			endInsertRows();

			// Client
			const auto service = m_entity->get<client_service_component>();
			if (service->valid())
				service->update();

			for (const auto& e : *list)
			{
				// Status
				const auto status = e->get<status_component>();
				status->add(m_status_function);

				// Transfer
				if (e->enabled())
				{
					const auto service = e->get<transfer_service_component>();
					if (service->invalid())
						service->start();
				}
			}
		}
		else
		{
			beginInsertRows(QModelIndex(), row, row);
			transfer_list->push_back(entity);
			endInsertRows();

			// Client
			const auto service = m_entity->get<client_service_component>();
			if (service->valid())
				service->update();

			// Status
			const auto status = entity->get<status_component>();
			status->add(m_status_function);

			// Transfer
			if (entity->enabled())
			{
				const auto service = entity->get<transfer_service_component>();
				if (service->invalid())
					service->start();
			}
		}

		// Callback
		const auto e = entity_factory::create_statusbar(statusbar::transfer);
		m_entity->call(callback_type::status, callback_action::add, e);
	}

	void client_transfer_model::on_clear(const entity::ptr entity)
	{		
		assert(thread_info::main());

		const auto transfer_list = m_entity->get<transfer_list_component>();
		const auto list = transfer_list_component::create();

		for (const auto& e : *transfer_list)
		{
			if (e->get_state() == entity_state::temporary)
			{
				const auto service = e->get<transfer_service_component>();
				if (service->valid())
					service->async_quit();
			}
			else
			{
				if (!entity)
				{
					// Status
					const auto status = e->get<status_component>();
					status->set_status(eja::status::none);
				}

				list->push_back(e);
			}
		}

		// Copy
		beginResetModel();
		transfer_list->assign(list->begin(), list->end());
		endResetModel();

		// Callback
		const auto e = entity_factory::create_statusbar(statusbar::transfer);
		m_entity->call(callback_type::status, callback_action::clear, e);
	}

	void client_transfer_model::on_update()
	{
		assert(thread_info::main());

		// Client
		const auto service = m_entity->get<client_service_component>();
		if (service->valid())
			service->async_transfer();
	}

	void client_transfer_model::on_update(const QModelIndex& index)
	{
		assert(thread_info::main());

		// Client
		const auto service = m_entity->get<client_service_component>();
		if (service->invalid())
			return;

		// Transfer
		const auto entity = get_entity(index);
		if (entity && entity->enabled())
			service->async_transfer(entity);
	}

	// Utility
	bool client_transfer_model::empty() const
	{
		assert(thread_info::main());

		const auto transfer_list = m_entity->get<transfer_list_component>();
		return transfer_list->empty();
	}

	void client_transfer_model::set_modified(const entity::ptr entity /*= nullptr*/)
	{
		assert(thread_info::main());

		const auto service = m_entity->get<client_service_component>();
		if (service->invalid())
			return;

		// Status
		const auto status = entity->get<status_component>();
		status->set_status(eja::status::pending);
	}

	// Model
	bool client_transfer_model::insertRows(int row, int count, const QModelIndex& parent /*= QModelIndex()*/)
	{
		assert(thread_info::main());

		const auto transfer_list = m_entity->get<transfer_list_component>();

		beginInsertRows(QModelIndex(), row, (row + count - 1));

		for (auto i = 0; i < count; ++i)
		{
			// Entity
			const auto entity = entity_factory::create_client_transfer(m_entity);
			entity->disable();

			// Endpoint
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

		// Callback
		const auto e = entity_factory::create_statusbar(statusbar::transfer);
		m_entity->call(callback_type::status, callback_action::add, e);

		return true;
	}

	bool client_transfer_model::removeRows(int row, int count, const QModelIndex& parent /*= QModelIndex()*/)
	{
		assert(thread_info::main());

		const auto transfer_list = m_entity->get<transfer_list_component>();		
		const auto it = transfer_list->cbegin() + row;
		const auto size = transfer_list->size();

		for (auto it = transfer_list->cbegin() + row; it != transfer_list->cbegin() + row + count; ++it)
		{
			const auto& e = *it;
			const auto service = e->get<transfer_service_component>();
			if (service->valid())
				service->async_quit();
		}

		beginRemoveRows(QModelIndex(), row, (row + count - 1));
		transfer_list->erase(it, it + count);
		endRemoveRows();

		// Client
		const auto service = m_entity->get<client_service_component>();
		if (service->valid())
			service->update();

		// Callback
		const auto e = entity_factory::create_statusbar(statusbar::transfer);
		m_entity->call(callback_type::status, callback_action::remove, e);

		return transfer_list->size() < size;
	}

	// Model
	Qt::ItemFlags client_transfer_model::flags(const QModelIndex& index) const
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

	QVariant client_transfer_model::data(const QModelIndex& index, int role) const
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
					case column::type:
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
					case column::row:
					{
						return QVariant::fromValue<uint>(row);
					}
					case column::type:
					{
						return QVariant::fromValue<bool>(entity->has_parent());
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

	bool client_transfer_model::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
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

									// Client
									const auto client_service = m_entity->get<client_service_component>();
									if (client_service->valid())
										client_service->update();
									
									// Transfer
									const auto transfer_service = entity->get<transfer_service_component>();
									if (transfer_service->invalid())
										transfer_service->start();
								}

								break;
							}
							case Qt::Unchecked:
							{
								if (entity->enabled())
								{									
									// Status
									const auto status = entity->get<status_component>();
									status->set_status(eja::status::none);

									const auto group_list = entity->get<group_security_list_component>();
									for (const auto& entity : *group_list)
									{
										const auto status = entity->get<status_component>();
										status->set_status(eja::status::none);
									}

									// Client
									const auto client_service = m_entity->get<client_service_component>();
									if (client_service->valid())
										client_service->update();

									// Transfer
									const auto transfer_service = entity->get<transfer_service_component>();
									if (transfer_service->valid())
										transfer_service->async_quit();

									entity->disable();

									entity_table_model::on_update(index);
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

	int client_transfer_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		assert(thread_info::main());

		const auto transfer_list = m_entity->get<transfer_list_component>();
		return transfer_list->size();
	}

	// Get
	entity::ptr client_transfer_model::get_entity(const size_t row) const
	{
		assert(thread_info::main());

		const auto transfer_list = m_entity->get<transfer_list_component>();
		return (row < transfer_list->size()) ? transfer_list->at(row) : nullptr;
	}

	size_t client_transfer_model::get_row(const entity::ptr entity) const
	{
		assert(thread_info::main());

		const auto transfer_list = m_entity->get<transfer_list_component>();
		const auto it = std::find(transfer_list->cbegin(), transfer_list->cend(), entity);
		return (it != transfer_list->cend()) ? (it - transfer_list->cbegin()) : npos;
	}
}
