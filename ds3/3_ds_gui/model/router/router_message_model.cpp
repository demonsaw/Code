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
#include "component/session/session_component.h"
#include "component/time/timeout_component.h"
#include "entity/entity.h"
#include "model/router/router_message_model.h"
#include "thread/thread_info.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Slot
	void router_message_model::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto router_list = m_entity->get<message_list_component>();
		const auto row = router_list->size();

		beginInsertRows(QModelIndex(), row, row);
		router_list->push_back(entity);
		endInsertRows();
	}

	void router_message_model::on_clear()
	{
		assert(thread_info::main());

		const auto router_list = m_entity->get<message_list_component>();

		beginResetModel();
		router_list->clear();
		endResetModel();
	}

	// Utility
	bool router_message_model::empty() const
	{
		assert(thread_info::main());

		const auto router_list = m_entity->get<message_list_component>();
		return router_list->empty();
	}

	// Model
	bool router_message_model::insertRows(int row, int count, const QModelIndex& parent /*= QModelIndex()*/)
	{
		assert(thread_info::main());

		const auto router_list = m_entity->get<message_list_component>();

		beginInsertRows(QModelIndex(), row, (row + count - 1));

		for (auto i = 0; i < count; ++i)
		{
			// Entity
			/*const auto entity = entity_factory::create_group(m_entity);
			entity->add<pixmap_component>();

			// Status
			const auto status = entity->get<status_component>();
			status->add(m_status_function);

			security_list->push_back(entity);*/
		}

		endInsertRows();

		return true;
	}


	bool router_message_model::removeRows(int row, int count, const QModelIndex& parent /*= QModelIndex()*/)
	{
		assert(thread_info::main());

		const auto router_list = m_entity->get<message_list_component>();
		const auto it = router_list->cbegin() + row;
		const auto size = router_list->size();

		beginRemoveRows(QModelIndex(), row, (row + count - 1));
		router_list->erase(it, it + count);
		endRemoveRows();

		return router_list->size() < size;
	}

	// Model
	Qt::ItemFlags router_message_model::flags(const QModelIndex& index) const
	{
		assert(thread_info::main());

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

	QVariant router_message_model::data(const QModelIndex& index, int role) const
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
					case column::version:
					{
						/*const auto version = entity->get<version_component>();
						return QString::fromStdString(version->str());*/
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
					case column::name:
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

	/*bool router_server_model::moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild)
	{
		int i = 8;

		return true;
	}*/

	bool router_message_model::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
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
					case column::name:
					{
						switch (value.toInt())
						{
							case Qt::Checked:
							{
								if (entity->disabled())
								{
									entity->enable();

									entity_table_model::on_update(index);

									modified = true;
								}

								break;
							}
							case Qt::Unchecked:
							{
								if (entity->enabled())
								{
									entity->disable();

									entity_table_model::on_update(index);

									modified = true;
								}

								break;
							}
						}
					}
				}

				break;
			}
		}

		// TODO: Restart service for that message router
		//
		//
		if (modified)
		{
			// Machine
			/*const auto option = entity->get<server_option_component>();
			if (option && option->enabled())
			{
				const auto machine = entity->get<server_machine_component>();
				if (machine)
					machine->restart();
			}*/
		}

		return modified;
	}

	int router_message_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		assert(thread_info::main());

		const auto router_list = m_entity->get<message_list_component>();
		return router_list->size();
	}

	// Get
	entity::ptr router_message_model::get_entity(const size_t row) const
	{
		assert(thread_info::main());

		const auto router_list = m_entity->get<message_list_component>();
		return (row < router_list->size()) ? router_list->at(row) : nullptr;
	}

	size_t router_message_model::get_row(const entity::ptr entity) const
	{
		assert(thread_info::main());

		const auto router_list = m_entity->get<message_list_component>();
		const auto it = std::find(router_list->begin(), router_list->end(), entity);
		return (it != router_list->end()) ? (it - router_list->begin()) : npos;
	}
}
