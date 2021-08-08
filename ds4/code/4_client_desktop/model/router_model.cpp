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

#include "component/callback/callback.h"
#include "component/client/client_component.h"
#include "component/router/router_component.h"
#include "component/router/router_service_component.h"
#include "component/status/status_component.h"

#include "entity/entity.h"
#include "model/router_model.h"
#include "thread/thread_info.h"
#include "utility/value.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Slot	
	void router_model::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto router_list = m_entity->get<router_list_component>();
		const auto row = router_list->size();

		beginInsertRows(QModelIndex(), row, row);
		router_list->push_back(entity);
		endInsertRows();

		// Callback
		m_entity->async_call(callback::statusbar | callback::add);
	}

	void router_model::on_add(const entity::ptr entity, const size_t row)
	{
		assert(thread_info::main());

		const auto router_list = m_entity->get<router_list_component>();
		const auto it = router_list->begin() + row;

		beginInsertRows(QModelIndex(), row, row);
		router_list->insert(it, entity);
		endInsertRows();
	}

	void router_model::on_remove(const entity::ptr entity)
	{
		assert(thread_info::main());
		
		if (entity->disabled())
		{
			const auto row = get_row(entity);
			if (row != npos)
			{
				const auto router_list = m_entity->get<router_list_component>();
				const auto it = router_list->begin() + row;

				beginRemoveRows(QModelIndex(), row, row);
				router_list->erase(it);
				endRemoveRows();
			}
		}		
	}

	void router_model::on_remove(const size_t row)
	{
		const auto entity = get_entity(row);
		if (entity && entity->disabled())
		{
			const auto router_list = m_entity->get<router_list_component>();
			const auto it = router_list->begin() + row;

			beginRemoveRows(QModelIndex(), row, row);
			router_list->erase(it);
			endRemoveRows();
		}
	}

	// Utility
	bool router_model::empty() const
	{
		assert(thread_info::main());

		const auto router_list = m_entity->get<router_list_component>();
		return router_list->empty();
	}

	// Set
	void router_model::set_modified() const
	{
		assert(thread_info::main());

		// Update
		const auto client = m_entity->get<client_component>();
		client->set_update(true);

		// Callback
		m_entity->async_call(callback::window | callback::update);
	}

	// Model
	bool router_model::removeRows(int row, int count, const QModelIndex& index /*= QModelIndex()*/)
	{
		assert(thread_info::main());
		
		const auto router_list = m_entity->get<router_list_component>();
		const auto it = router_list->begin() + row;

		beginRemoveRows(index, row, (row + count - 1));
		router_list->erase(it, it + count);
		endRemoveRows();

		return true;
	}

	Qt::ItemFlags router_model::flags(const QModelIndex& index) const
	{
		assert(thread_info::main());

		if (index.isValid())
		{
			auto flags = QAbstractItemModel::flags(index) | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;

			switch (index.column())
			{
				case column::row:
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

		return Qt::ItemIsEnabled;
	}

	QVariant router_model::data(const QModelIndex& index, int role) const
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
					case column::row:					
					{
						return QVariant::fromValue(entity);
					}
					case column::name:
					{
						const auto router = entity->get<router_component>();
						return QString::fromStdString(router->get_name());
					}
					case column::address:
					{
						const auto router = entity->get<router_component>();
						return QString::fromStdString(router->get_address());
					}
					case column::port:
					{
						const auto router = entity->get<router_component>();
						return QVariant::fromValue<uint>(router->get_port());
					}
					case column::password:
					{
						const auto router = entity->get<router_component>();
						return QString::fromStdString(router->get_password());
					}					
				}
				break;
			}
			case Qt::EditRole:
			{
				return data(index, Qt::DisplayRole);
			}
			case Qt::UserRole:
			{
				switch (index.column())
				{
					case column::row:
					{
						return QVariant::fromValue(index.row());
					}
					default:
					{
						return data(index, Qt::DisplayRole);
					}
				}

				break;
			}
		}

		return QVariant();
	}

	bool router_model::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
	{
		assert(thread_info::main());

		if (!index.isValid())
			return false;

		const auto entity = get_entity(index);
		if (!entity)
			return false;

		bool modified = false;

		switch (role)
		{
			case Qt::EditRole:
			{
				switch (index.column())
				{
					case column::name:
					{
						const auto name = value.toString().simplified().toStdString();
						const auto router = entity->get<router_component>();
						router->set_name(name);
						break;
					}
					case column::address:
					{
						const auto address = value.toString().trimmed().toStdString();
						const auto router = entity->get<router_component>();
						if (address != router->get_address())
						{
							router->set_address(address);

							// Update
							const auto router_list = m_entity->get<router_list_component>();
							if (router_list->get_entity() == entity)
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

						const auto router = entity->get<router_component>();
						if (port != router->get_port())
						{
							router->set_port(port);

							// Update
							const auto router_list = m_entity->get<router_list_component>();
							if (router_list->get_entity() == entity)
								modified = true;
						}

						break;
					}
					case column::password:
					{
						const auto qpassword = value.toString().trimmed();

						if (qpassword.length() <= io::max_passphrase_size)
						{
							const auto router = entity->get<router_component>();
							const auto password = qpassword.toStdString();							

							if (password != router->get_password())
							{
								router->set_password(password);

								// Update
								const auto router_list = m_entity->get<router_list_component>();
								if (router_list->get_entity() == entity)
									modified = true;
							}
						}						

						break;
					}
				}

				break;
			}
		}

		if (modified)
			set_modified();

		return modified;
	}

	int router_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		assert(thread_info::main());

		const auto router_list = m_entity->get<router_list_component>();
		return router_list->size();
	}

	// Get
	entity::ptr router_model::get_entity(const size_t row) const
	{
		assert(thread_info::main());

		const auto router_list = m_entity->get<router_list_component>();
		return (row < router_list->size()) ? router_list->at(row) : nullptr;
	}

	size_t router_model::get_row(const entity::ptr& entity) const
	{
		assert(thread_info::main());

		const auto router_list = m_entity->get<router_list_component>();
		const auto it = std::find(router_list->begin(), router_list->end(), entity);
		return (it != router_list->end()) ? (it - router_list->begin()) : npos;
	}
}
