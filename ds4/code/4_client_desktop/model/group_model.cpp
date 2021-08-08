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
#include "component/callback/callback.h"
#include "component/client/client_component.h"
#include "component/group/group_component.h"
#include "component/group/group_option_component.h"
#include "component/status/status_component.h"

#include "entity/entity.h"
#include "factory/client_factory.h"
#include "model/group_model.h"
#include "thread/thread_info.h"
#include "utility/io/file_util.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Slot
	void group_model::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto group_list = m_entity->get<group_list_component>();
		{
			const auto row = group_list->size();

			beginInsertRows(QModelIndex(), row, row);
			group_list->push_back(entity);
			endInsertRows();
		}
	}

	void group_model::on_add(const entity::ptr entity, const size_t row)
	{
		assert(thread_info::main());

		const auto group_list = m_entity->get<group_list_component>();
		{
			const auto it = group_list->begin() + row;

			beginInsertRows(QModelIndex(), row, row);
			group_list->insert(it, entity);
			endInsertRows();
		}
	}

	void group_model::on_clear()
	{
		assert(thread_info::main());

		const auto group_list = m_entity->get<group_list_component>();

		for (const auto& e : *group_list)
		{
			const auto group_option = e->get<group_option_component>();
			group_option->set_type(entropy_type::none);
			group_option->set_size(0);

			const auto status = e->get<status_component>();
			status->set_status(eja::status::none);

			const auto pixmap = e->get<pixmap_component>();
			pixmap->clear();
		}
	}

	// Utility
	bool group_model::empty() const
	{
		assert(thread_info::main());

		const auto group_list = m_entity->get<group_list_component>();
		return group_list->empty();
	}

	// Set
	void group_model::set_modified() const
	{
		assert(thread_info::main());

		// Update
		const auto client = m_entity->get<client_component>();
		client->set_update(true);

		// Callback
		m_entity->async_call(callback::window | callback::update);
	}

	// Model
	bool group_model::removeRows(int row, int count, const QModelIndex& index /*= QModelIndex()*/)
	{
		assert(thread_info::main());

		const auto group_list = m_entity->get<group_list_component>();
		const auto it = group_list->begin() + row;

		for (auto i = row; i < (row + count); ++i)
		{
			const auto& e = group_list->at(i);
			if (e->enabled())
			{
				// TODO: Eventually only signal an update if the enabled row isn't really removed.
				// Will need to hook into remove/add in move (drag & drop)
				//
				set_modified();
				break;
			}
		}

		beginRemoveRows(index, row, (row + count - 1));
		group_list->erase(it, it + count);
		endRemoveRows();

		return true;
	}

	Qt::ItemFlags group_model::flags(const QModelIndex& index) const
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

		return Qt::ItemIsEnabled;
	}

	QVariant group_model::data(const QModelIndex& index, int role) const
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
					case column::entropy:
					case column::cipher:
					case column::hash:
					{
						return QVariant::fromValue(entity);
					}
					case column::size:
					{
						if (entity->enabled())
						{
							const auto group_option = entity->get<group_option_component>();
							return QString::fromStdString(file_util::get_size(group_option->get_size()));
						}

						break;
					}
					case column::percent:
					{
						const auto group_option = entity->get<group_option_component>();
						return QString().sprintf("%.02f", group_option->get_percent());
					}
					case column::bits:
					{
						const auto group_option = entity->get<group_option_component>();
						return QString::number(group_option->get_key_size());
					}
					case column::salt:
					{
						const auto group_option = entity->get<group_option_component>();
						return QString::fromStdString(group_option->get_salt());
					}
					case column::iterations:
					{
						const auto group_option = entity->get<group_option_component>();
						return  QVariant::fromValue<uint>(group_option->get_iterations());
					}
				}

				break;
			}
			case Qt::EditRole:
			{
				switch (index.column())
				{
					case column::entropy:
					{
						const auto group_option = entity->get<group_option_component>();
						return QString::fromStdString(group_option->get_entropy());
					}
					case column::percent:
					{
						const auto group_option = entity->get<group_option_component>();
						return QVariant::fromValue<double>(group_option->get_percent());
					}
					case column::cipher:
					{
						const auto group_option = entity->get<group_option_component>();
						return QString::fromStdString(group_option->get_cipher());
					}
					case column::bits:
					{
						const auto group_option = entity->get<group_option_component>();
						return QVariant::fromValue<uint>(group_option->get_key_size());;
					}
					case column::hash:
					{
						const auto group_option = entity->get<group_option_component>();
						return QString::fromStdString(group_option->get_hash());
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
				switch (index.column())
				{
					case column::row:
					{
						return QVariant::fromValue(index.row());
					}
					case column::entropy:
					{
						const auto group_option = entity->get<group_option_component>();
						return QString::fromStdString(group_option->get_entropy());
					}
					case column::size:
					{
						const auto group_option = entity->get<group_option_component>();
						return QVariant::fromValue<qulonglong>(group_option->get_size());
					}
					case column::cipher:
					{
						const auto group_option = entity->get<group_option_component>();
						return QString::fromStdString(group_option->get_cipher());
					}
					case column::hash:
					{
						const auto group_option = entity->get<group_option_component>();
						return QString::fromStdString(group_option->get_hash());
					}
					case column::salt:
					{
						const auto group_option = entity->get<group_option_component>();
						return QString::fromStdString(group_option->get_salt());
					}
					default:
					{
						return data(index, Qt::DisplayRole);
					}
				}

				break;
			}
			case Qt::TextAlignmentRole:
			{
				switch (index.column())
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

	bool group_model::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
	{
		assert(thread_info::main());

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
					case column::entropy:
					{
						const auto entropy = value.toString().toStdString();
						const auto group_option = entity->get<group_option_component>();

						if (entropy != group_option->get_entropy())
						{
							if (!group_option->has_entropy())
							{
								entity->enable();
							}
							else if (entropy.empty())
							{
								entity->disable();
								modified = true;
							}

							group_option->set_type(entropy_type::none);
							group_option->set_entropy(entropy);
							group_option->set_size(0);

							// Pixmap
							const auto pixmap = entity->get<pixmap_component>();
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

						const auto group_option = entity->get<group_option_component>();
						if (percent != group_option->get_percent())
						{
							group_option->set_percent(percent);

							if (entity->enabled())
								modified = true;
						}

						break;
					}
					case column::cipher:
					{
						const auto cipher = value.toString().toStdString();
						const auto group_option = entity->get<group_option_component>();

						if (cipher != group_option->get_cipher())
						{
							group_option->set_cipher(cipher);

							if (entity->enabled())
								modified = true;
						}

						break;
					}
					case column::bits:
					{
						const auto key_size = value.toUInt();
						const auto group_option = entity->get<group_option_component>();

						if (key_size != group_option->get_key_size())
						{
							group_option->set_key_size(key_size);

							if (entity->enabled())
								modified = true;
						}

						break;
					}
					case column::hash:
					{
						const auto hash = value.toString().toStdString();
						const auto group_option = entity->get<group_option_component>();

						if (hash != group_option->get_hash())
						{
							group_option->set_hash(hash);

							if (entity->enabled())
								modified = true;
						}

						break;
					}
					case column::salt:
					{
						const auto salt = value.toString().toStdString();
						const auto group_option = entity->get<group_option_component>();

						if (salt != group_option->get_salt())
						{
							group_option->set_salt(salt);

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

						const auto group_option = entity->get<group_option_component>();
						if (iterations != group_option->get_iterations())
						{
							group_option->set_iterations(iterations);

							if (entity->enabled())
								modified = true;
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

	int group_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		assert(thread_info::main());

		const auto group_list = m_entity->get<group_list_component>();
		return group_list->size();
	}

	// Get
	entity::ptr group_model::get_entity(const size_t row) const
	{
		assert(thread_info::main());

		const auto group_list = m_entity->get<group_list_component>();
		return (row < group_list->size()) ? group_list->at(row) : nullptr;
	}

	size_t group_model::get_row(const entity::ptr& entity) const
	{
		assert(thread_info::main());

		// O(N)
		const auto group_list = m_entity->get<group_list_component>();
		const auto it = std::find(group_list->begin(), group_list->end(), entity);
		return (it != group_list->end()) ? (it - group_list->begin()) : npos;
	}
}
