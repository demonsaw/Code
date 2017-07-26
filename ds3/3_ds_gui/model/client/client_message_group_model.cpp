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

#include "component/pixmap_component.h"
#include "component/client/client_service_component.h"
#include "component/group/group_security_component.h"
#include "component/message/message_service_component.h"
#include "component/status/status_component.h"

#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "model/client/client_message_group_model.h"
#include "thread/thread_info.h"
#include "utility/io/file_util.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Interface
	void client_message_group_model::on_init()
	{
		assert(thread_info::main());

		entity_table_model::on_init();

		// Signal
		connect(this, &client_message_group_model::set_status, [&](const entity::ptr entity) { update(entity); });

		// Status
		m_status_function = function::create([this](const entity::ptr entity) { emit set_status(entity); });
	}

	void client_message_group_model::on_shutdown()
	{
		assert(thread_info::main());

		entity_table_model::on_shutdown();

		// Status
		m_status_function.reset();
	}

	// Slot
	void client_message_group_model::on_clear()
	{
		assert(thread_info::main());

		if (!has_entity())
			return;

		const auto security_list = m_entity->get<group_security_list_component>();

		for (const auto& entity : *security_list)
		{
			const auto security = entity->get<group_security_component>();
			security->set_type(group_security_type::none);
			security->set_size(0);

			const auto status = entity->get<status_component>();
			status->set_status(eja::status::none);

			const auto pixmap = entity->get<pixmap_component>();
			pixmap->clear();
		}
	}

	void client_message_group_model::on_update()
	{
		assert(thread_info::main());

		if (!has_entity())
			return;

		const auto parent = m_entity->get_parent();
		const auto service = parent->get<client_service_component>();
		if (service->valid())
			service->async_message(m_entity);
	}

	// Utility
	bool client_message_group_model::empty() const
	{
		assert(thread_info::main());

		if (has_entity())
		{
			const auto security_list = m_entity->get<group_security_list_component>();
			return security_list->empty();
		}

		return true;
	}

	void client_message_group_model::set_modified(const entity::ptr entity /*= nullptr*/)
	{
		assert(thread_info::main());

		if (!has_entity())
			return;

		const auto parent = m_entity->get_parent();
		const auto service = parent->get<client_service_component>();
		if (service->invalid())
			return; 

		// Status
		const auto status = m_entity->get<status_component>();
		status->set_status(eja::status::pending);

		const auto security_list = m_entity->get<group_security_list_component>();

		for (const auto& e : *security_list)
		{
			if (e->enabled() || (e == entity))
			{
				const auto status = e->get<status_component>();
				status->set_status(eja::status::pending);
			}
		}
	}

	// Model
	bool client_message_group_model::insertRows(int row, int count, const QModelIndex& parent /*= QModelIndex()*/)
	{
		assert(thread_info::main());

		if (!has_entity())
			return false;

		const auto security_list = m_entity->get<group_security_list_component>();

		beginInsertRows(QModelIndex(), row, (row + count - 1));

		for (auto i = 0; i < count; ++i)
		{
			// Entity
			const auto entity = entity_factory::create_group(m_entity);
			entity->disable();

			// Pixmap
			entity->add<pixmap_component>();

			// Status
			const auto status = entity->get<status_component>();
			status->add(m_status_function);

			security_list->push_back(entity);
		}

		endInsertRows();

		return true;
	}

	bool client_message_group_model::removeRows(int row, int count, const QModelIndex& parent /*= QModelIndex()*/)
	{
		assert(thread_info::main());

		if (!has_entity())
			return false;

		bool updated = false;
		const auto security_list = m_entity->get<group_security_list_component>();

		for (auto i = row; i < (row + count); ++i)
		{
			const auto& entity = security_list->at(i);
			if (entity->enabled())
			{
				const auto security = entity->get<group_security_component>();
				if (security->has_entropy())
				{
					updated = true;
					break;
				}
			}
		}

		const auto it = security_list->cbegin() + row;
		const auto size = security_list->size();

		beginRemoveRows(QModelIndex(), row, (row + count - 1));
		security_list->erase(it, it + count);
		endRemoveRows();

		if (updated)
			on_update();

		return security_list->size() < size;
	}

	Qt::ItemFlags client_message_group_model::flags(const QModelIndex& index) const
	{
		assert(thread_info::main());

		if (!index.isValid())
			return Qt::ItemIsEnabled;

		Qt::ItemFlags flags = QAbstractItemModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemIsSelectable;

		switch (index.column())
		{
			case column::row:
			{
				flags |= Qt::ItemIsUserCheckable;
				break;
			}
			case column::size:
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

	QVariant client_message_group_model::data(const QModelIndex& index, int role) const
	{
		assert(thread_info::main());

		if (!index.isValid())
			return QVariant();

		const size_t row = static_cast<size_t>(index.row());
		const auto entity = get_entity(row);
		if (!entity)
			return QVariant();

		const size_t col = static_cast<size_t>(index.column());
		const auto security = entity->get<group_security_component>();

		switch (role)
		{
			case Qt::DisplayRole:
			{
				switch (col)
				{
					case column::entropy:
					case column::cipher:
					case column::hash:
					{
						return QVariant::fromValue(entity);
					}
					case column::size:
					{
						return security->has_size() ? QString::fromStdString(file_util::get_size(security->get_size())) : "";
					}
					case column::percent:
					{
						return QString().sprintf("%.02f", security->get_percent());
					}
					case column::bits:
					{
						return QString("%1").arg(security->get_key_size());
					}
					case column::salt:
					{
						return QString::fromStdString(security->get_salt());
					}
					case column::iterations:
					{
						return  QVariant::fromValue<uint>(security->get_iterations());
					}
				}

				break;
			}
			case Qt::EditRole:
			{
				switch (col)
				{
					case column::entropy:
					{
						return QString::fromStdString(security->get_entropy());
					}
					case column::percent:
					{
						return QVariant::fromValue<double>(security->get_percent());
					}
					case column::cipher:
					{
						return QString::fromStdString(security->get_cipher());
					}
					case column::bits:
					{
						return QVariant::fromValue<uint>(security->get_key_size());;
					}
					case column::hash:
					{
						return QString::fromStdString(security->get_hash());
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
					case column::entropy:
					{
						return QString::fromStdString(security->get_entropy()).toLower();
					}
					case column::size:
					{
						return QVariant::fromValue<qulonglong>(security->get_size());
					}
					case column::cipher:
					{
						return QString::fromStdString(security->get_cipher()).toLower();
					}
					case column::hash:
					{
						return QString::fromStdString(security->get_hash()).toLower();
					}
					case column::salt:
					{
						return QString::fromStdString(security->get_salt()).toLower();
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

	bool client_message_group_model::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
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
		const auto security = entity->get<group_security_component>();

		switch (role)
		{
		case Qt::EditRole:
		{
			switch (col)
			{
				case column::row:
				case column::entropy:
				{
					auto entropy = value.toString().toStdString();

					if (entropy != security->get_entropy())
					{
						if (!security->has_entropy())
						{
							const auto index = createIndex(row, column::row);
							setData(index, Qt::Checked, Qt::CheckStateRole);
						}
						else if (entropy.empty())
						{
							const auto index = createIndex(row, column::row);
							setData(index, Qt::Unchecked, Qt::CheckStateRole);
						}

						security->set_type(group_security_type::none);
						security->set_entropy(entropy);
						security->set_size(0);

						// Pixmap
						const auto pixmap = entity->get<pixmap_component>();

						if (security->get_type() == group_security_type::file)
							pixmap->set_path(entropy);
						else
							pixmap->clear();

						if (entity->enabled())
							modified = true;
					}

					break;
				}
				case column::percent:
				{
					// Validate
					const auto percent = value.toDouble();
					if ((percent <= 0.0) || (percent > 100.0))
						return false;

					if (percent != security->get_percent())
					{
						security->set_percent(percent);

						if (entity->enabled())
							modified = true;
					}

					break;
				}
				case column::cipher:
				{
					const auto cipher = value.toString().toStdString();
					if (cipher != security->get_cipher())
					{
						security->set_cipher(cipher);

						if (entity->enabled())
							modified = true;
					}

					break;
				}
				case column::bits:
				{
					const auto key_size = value.toUInt();
					if (key_size != security->get_key_size())
					{
						security->set_key_size(key_size);

						if (entity->enabled())
							modified = true;
					}

					break;
				}
				case column::hash:
				{
					const auto hash = value.toString().toStdString();
					if (hash != security->get_hash())
					{
						security->set_hash(hash);

						if (entity->enabled())
							modified = true;
					}

					break;
				}
				case column::salt:
				{
					const auto salt = value.toString().toStdString();
					if (salt != security->get_salt())
					{
						security->set_salt(salt);

						if (entity->enabled())
							modified = true;
					}

					break;
				}
				case column::iterations:
				{
					// Validate
					const auto iterations = value.toUInt();
					if (!iterations)
						return false;

					if (iterations != security->get_iterations())
					{
						security->set_iterations(iterations);

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

						if (m_entity->enabled() && security->has_entropy())
							on_update();
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

						if (m_entity->enabled() && security->has_entropy())
							on_update();
					}

					break;
				}
				}

				break;
			}
			}

			break;
		}
		}

		if (modified)
			set_modified(entity);

		return modified;
	}

	int client_message_group_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		assert(thread_info::main());

		if (has_entity())
		{
			const auto security_list = m_entity->get<group_security_list_component>();
			return security_list->size();
		}

		return 0;
	}

	// Set
	void client_message_group_model::set_entity(const std::shared_ptr<entity> entity)
	{
		assert(thread_info::main());
		
		if (has_entity())
		{
			// Remove
			const auto security_list = m_entity->get<group_security_list_component>();

			for (const auto& entity : *security_list)
			{
				const auto status = entity->get<status_component>();
				status->remove(m_status_function);
			}
		}

		entity_table_model::set_entity(entity);

		// Add
		const auto security_list = m_entity->get<group_security_list_component>();

		for (const auto& entity : *security_list)
		{
			const auto status = entity->get<status_component>();
			status->add(m_status_function);
		}
	}

	void client_message_group_model::set_entity()
	{
		assert(thread_info::main());

		if (has_entity())
		{
			// Remove
			const auto security_list = m_entity->get<group_security_list_component>();

			for (const auto& entity : *security_list)
			{
				const auto status = entity->get<status_component>();
				status->remove(m_status_function);
			}
		}

		entity_table_model::set_entity();
	}

	// Get
	entity::ptr client_message_group_model::get_entity(const size_t row) const
	{
		assert(thread_info::main());

		if (has_entity())
		{
			const auto security_list = m_entity->get<group_security_list_component>();
			return (row < security_list->size()) ? security_list->at(row) : nullptr;
		}

		return nullptr;
	}

	size_t client_message_group_model::get_row(const entity::ptr entity) const
	{
		assert(thread_info::main());

		if (has_entity())
		{
			// O(N)
			const auto security_list = m_entity->get<group_security_list_component>();
			const auto it = std::find(security_list->begin(), security_list->end(), entity);
			return (it != security_list->end()) ? (it - security_list->begin()) : npos;
		}

		return npos;
	}
}
