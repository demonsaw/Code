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

#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>

#include <QDesktopServices>
#include <QDoubleSpinBox>
#include <QUrl>

#include "client_group_model.h"
#include "component/pixmap_component.h"
#include "component/client/client_component.h"
#include "component/client/client_machine_component.h"
#include "component/error/error_component.h"
#include "component/group/group_status_component.h"
#include "component/io/file_component.h"
#include "component/group/group_security_component.h"
#include "component/group/group_option_component.h"
#include "resource/resource.h"
#include "security/security.h"
#include "utility/group/group_util.h"
#include "utility/io/file_util.h"
#include "window/client_window.h"
#include "window/main_window.h"
#include "window/window.h"

namespace eja
{
	// Interface
	void client_group_model::init()
	{
		entity_table_model::init();

		// Callback
		add_callback(function_type::group);

		const auto lambda = [this](const entity::ptr entity) { on_refresh(entity); };
		m_function = function::create(lambda);

		const auto group_vector = m_entity->get<group_entity_vector_component>();
		if (!group_vector)
			return;

		for (const auto& entity : group_vector->copy())
		{
			const auto status = entity->get<group_status_component>();
			if (status)
				status->add(m_function);
		}
	}

	void client_group_model::shutdown()
	{
		entity_table_model::shutdown();

		const auto group_vector = m_entity->get<group_entity_vector_component>();
		if (!group_vector)
			return;

		for (const auto& entity : group_vector->copy())
		{
			const auto status = entity->get<group_status_component>();
			if (status)
				status->remove(m_function);
		}
	}

	void client_group_model::refresh()
	{		
		const auto group_vector = m_entity->get<group_entity_vector_component>();
		if (!group_vector)
			return;

		for (const auto& entity : group_vector->copy())
		{
			const auto security = entity->get<group_security_component>();
			if (security)
				security->set_modified(true);

			const auto status = entity->get<group_status_component>();
			if (status)
				status->set_status(eja::status::pending);
		}

		// Clear the client list
		const auto client_vector = m_entity->get<client_entity_vector_component>();
		if (client_vector)
			client_vector->clear();

		// Clear the chat/message
		m_entity->call(function_type::chat, function_action::clear, m_entity);
		m_entity->call(function_type::message, function_action::clear, m_entity);

		// Machine
		const auto machine = m_entity->get<client_machine_component>();
		if (machine->success())
			machine->restart();

		emit refreshed();
	}

	// Slot
	void client_group_model::on_add(const entity::ptr entity)
	{
		assert(thread::main());

		const auto group_vector = m_entity->get<group_entity_vector_component>();
		if (!group_vector)
			return;

		{
			auto_lock_ptr(group_vector);

			const auto row = group_vector->size();

			beginInsertRows(QModelIndex(), row, row);
			group_vector->add(entity);
			endInsertRows();
		}

		set_status(client_statusbar::group, group_vector->size());
	}

	void client_group_model::on_remove(const entity::ptr entity)
	{
		assert(thread::main());

		const auto group_vector = m_entity->get<group_entity_vector_component>();
		if (!group_vector)
			return;

		{
			auto_lock_ptr(group_vector);

			const auto row = group_vector->get(entity);
			if (row == type::npos)
				return;

			beginRemoveRows(QModelIndex(), row, row);
			group_vector->remove(row);
			endRemoveRows();
		}

		set_status(client_statusbar::group, group_vector->size());
	}

	// Utility
	bool client_group_model::empty() const
	{
		const auto group_vector = m_entity->get<group_entity_vector_component>();
		return !group_vector || group_vector->empty();
	}

	void client_group_model::post_create()
	{
		const auto group_vector = m_entity->get<group_entity_vector_component>();
		if (!group_vector)
			return;

		set_status(client_statusbar::group, group_vector->size());
	}

	// Model
	bool client_group_model::insertRows(int row, int count, const QModelIndex& parent /*= QModelIndex()*/)
	{
		assert(thread::main());

		const auto group_vector = m_entity->get<group_entity_vector_component>();
		if (!group_vector)
			return false;

		beginInsertRows(QModelIndex(), row, (row + count - 1));

		for (auto i = 0; i < count; ++i)
		{
			// Entity
			const auto entity = group_util::create(m_entity);
			entity->add<pixmap_component>();

			// Status
			const auto status = entity->get<group_status_component>();
			if (status)
				status->add(m_function);

			group_vector->add(entity);
		}

		endInsertRows();

		set_status(client_statusbar::group, group_vector->size());

		return true;
	}

	bool client_group_model::removeRows(int row, int count, const QModelIndex& parent /*= QModelIndex()*/)
	{
		assert(thread::main());

		const auto group_vector = m_entity->get<group_entity_vector_component>();
		if (!group_vector)
			return false;

		size_t status = 0;

		beginRemoveRows(QModelIndex(), row, (row + count - 1));

		for (auto i = (row + count - 1); i >= row; --i)
		{
			const auto entity = group_vector->get(row);
			if (entity)
			{
				const auto option = entity->get<group_option_component>();
				if (option && option->enabled())
					set_modified(true);

				status += group_vector->remove(row);				
			}
		}

		endRemoveRows();

		set_status(client_statusbar::group, group_vector->size());

		return status > 0;
	}

	Qt::ItemFlags client_group_model::flags(const QModelIndex& index) const
	{
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

	QVariant client_group_model::data(const QModelIndex& index, int role) const
	{
		if (!index.isValid())
			return QVariant();

		const size_t row = static_cast<size_t>(index.row());
		const size_t col = static_cast<size_t>(index.column());

		const auto group_vector = m_entity->get<group_entity_vector_component>();
		if (!group_vector)
			return QVariant();

		const auto entity = group_vector->get(row);
		if (!entity)
			return QVariant();

		switch (role)
		{
			case Qt::DisplayRole:			
			{
				switch (col)
				{					
					case column::file:
					case column::cipher:
					case column::hash:
					{
						return QVariant::fromValue(entity);
					}
					case column::size:
					{
						const auto file = entity->get<file_component>();
						if (file)
							return (file->get_size() > 0) ? QString::fromStdString(file_util::get_size(file->get_size())) : "";

						break;
					}
					case column::entropy:
					{
						const auto security = entity->get<group_security_component>();
						if (security)
							return QString().sprintf("%.02f%%", security->get_entropy());

						break;
					}
					case column::bits:
					{
						const auto security = entity->get<group_security_component>();
						if (security)
							return QString().sprintf("%u bits", security->get_key_size());

						break;
					}
					case column::salt:
					{
						const auto security = entity->get<group_security_component>();
						if (security)
							return QString::fromStdString(security->get_salt());

						break;
					}
					case column::iterations:
					{
						const auto security = entity->get<group_security_component>();
						if (security)
						{
							const auto iterations = static_cast<size_t>(security->get_iterations());
							return  QVariant::fromValue<uint>(iterations);
						}

						break;
					}
				}

				break;
			}
			case Qt::EditRole:
			{
				switch (col)
				{
					case column::row:
					{
						return QString().sprintf("%02d", row + 1);
					}
					case column::file:
					{
						const auto file = entity->get<file_component>();
						if (file)
							return QString::fromStdString(file->get_path());

						break;
					}
					case column::entropy:
					{
						const auto security = entity->get<group_security_component>();
						if (security)
							return QVariant::fromValue<double>(security->get_entropy());

						break;
					}
					case column::cipher:
					{
						const auto security = entity->get<group_security_component>();
						if (security)
							return QVariant::fromValue<uint>(security->get_cipher_type());

						break;
					}
					case column::bits:
					{
						const auto security = entity->get<group_security_component>();
						if (security)
							return QVariant::fromValue<uint>(security->get_key_size());;

						break;
					}
					case column::hash:
					{
						const auto security = entity->get<group_security_component>();
						if (security)
							return QVariant::fromValue<uint>(security->get_hash_type());

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
					case column::row:
					{
						return QVariant::fromValue<uint>(row);
					}
					case column::file:
					{
						const auto file = entity->get<file_component>();
						if (file)
							return QString::fromStdString(file->get_path()).toLower();

						break;
					}
					case column::size:
					{
						const auto file = entity->get<file_component>();
						if (file)
							return QVariant::fromValue<qulonglong>(file->get_size());

						break;
					}
					case column::cipher:
					{
						const auto security = entity->get<group_security_component>();
						if (security)
							return QString::fromStdString(security->get_cipher_name()).toLower();

						break;
					}
					case column::hash:
					{
						const auto security = entity->get<group_security_component>();
						if (security)
							return QString::fromStdString(security->get_hash_name()).toLower();

						break;
					}
					case column::salt:
					{
						const auto security = entity->get<group_security_component>();
						if (security)
							return QString::fromStdString(security->get_salt()).toLower();

						break;
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
						const auto option = entity->get<group_option_component>();
						if (option)
							return option->enabled() ? Qt::Checked : Qt::Unchecked;

						break;
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

	bool client_group_model::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
	{
		if (!index.isValid())
			return false;

		const size_t row = static_cast<size_t>(index.row());
		const size_t col = static_cast<size_t>(index.column());
		bool modified = false;

		const auto group_vector = m_entity->get<group_entity_vector_component>();
		if (!group_vector)
			return false;

		const auto entity = group_vector->get(row);
		if (!entity)
			return false;

		const auto security = entity->get<group_security_component>();
		if (!security)
			return false;

		switch (role)
		{
		case Qt::EditRole:
		{
			switch (col)
			{
			case column::row:
			case column::file:
			{
				auto path = value.toString().toStdString();
				const auto file = entity->get<file_component>();
				if (file)
				{
					if (path != file->get_path())
					{
						if (!file->has_path())
						{
							const auto index = createIndex(row, column::row);
							setData(index, Qt::Checked, Qt::CheckStateRole);
						}
						else if (path.empty())
						{
							const auto index = createIndex(row, column::row);
							setData(index, Qt::Unchecked, Qt::CheckStateRole);
						}

						// Add http prefix
						if (!path.empty() && !file_util::exists(path))
						{
							if (!boost::istarts_with(path, "http"))
								path = boost::str(boost::format("http://%s") % path);
						}

						file->set_path(path);
						file->set_size(0);

						const auto option = entity->get<group_option_component>();
						if (option && option->enabled())
							modified = true;

						// Pixmap
						const auto pixmap = entity->get<pixmap_component>();
						if (pixmap)
							pixmap->set_path(path);
					}
				}

				break;
			}
			case column::entropy:
			{
				// Validate
				const auto entropy = value.toDouble();
				if ((entropy <= 0.0) || (entropy > 100.0))
					return false;

				if (entropy != security->get_entropy())
				{
					security->set_entropy(entropy);
					modified = true;
				}

				break;
			}
			case column::cipher:
			{
				const auto cipher_type = value.toUInt();
				if (cipher_type != security->get_cipher_type())
				{
					security->set_cipher_type(cipher_type);
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
					modified = true;
				}

				break;
			}
			case column::hash:
			{
				const auto hash_type = value.toUInt();
				if (hash_type != security->get_hash_type())
				{
					security->set_hash_type(hash_type);
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
					const auto option = entity->get<group_option_component>();
					if (option && option->disabled())
					{
						option->set_enabled();
						entity_table_model::refresh(index);
						modified = true;
					}

					break;
				}
				case Qt::Unchecked:
				{
					const auto option = entity->get<group_option_component>();
					if (option && option->enabled())
					{
						option->set_disabled();
						entity_table_model::refresh(index);
						modified = true;
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
		{
			// Set status to pending
			const auto option = entity->get<group_option_component>();
			if (option && (option->enabled() || (role == Qt::CheckStateRole)))
			{
				const auto status = entity->get<group_status_component>();
				if (status)
					status->set_status(eja::status::pending);
			}

			security->set_modified(true);
			set_modified(true);
		}

		return modified;
	}

	int client_group_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		const auto group_vector = m_entity->get<group_entity_vector_component>();
		return group_vector ? static_cast<int>(group_vector->size()) : 0;
	}

	// Accessor
	size_t client_group_model::get_row(const entity::ptr entity) const
	{
		const auto group_vector = m_entity->get<group_entity_vector_component>();
		return group_vector ? group_vector->get(entity) : type::npos;
	}
}
