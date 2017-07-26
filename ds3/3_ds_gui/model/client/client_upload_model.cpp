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

#include <QDesktopServices>
#include <QTimer>
#include <QUrl>

#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/io/file_component.h"
#include "component/io/transfer_component.h"
#include "component/io/upload_component.h"
#include "component/status/status_component.h"
#include "component/time/timer_component.h"

#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "model/client/client_upload_model.h"
#include "security/checksum/crc.h"
#include "security/filter/hex.h"
#include "thread/thread_info.h"
#include "utility/io/file_util.h"

Q_DECLARE_METATYPE(eja::entity::ptr);
Q_DECLARE_METATYPE(eja::status);

namespace eja
{
	// Interface
	void client_upload_model::on_init()
	{
		assert(thread_info::main());

		entity_table_model::on_init();

		// Signal
		connect(this, &client_upload_model::set_status, [&](const entity::ptr entity) { update(entity); });

		// Status
		m_status_function = function::create([this](const entity::ptr entity) { emit set_status(entity); });

		// Timer
		m_timer = new QTimer(this);
		m_timer->setInterval(1000);

		connect(m_timer, &QTimer::timeout, [this]()
		{
			const auto upload_list = m_entity->get<upload_list_component>();

			QModelIndex start = createIndex(0, column::time);
			QModelIndex end = createIndex(upload_list->size() - 1, column::max - 1);
			emit dataChanged(start, end);

		});

		m_timer->start();
	}

	void client_upload_model::on_shutdown()
	{
		assert(thread_info::main());

		entity_table_model::on_shutdown();

		m_status_function.reset();
	}

	// Slot
	void client_upload_model::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		// NOTE: This might be buggy??
		//
		// Exist?
		//const auto upload_list = m_entity->get<upload_list_component>();
		//const auto it = std::find_if(upload_list->cbegin(), upload_list->cend(), [entity](const entity::ptr e) { return e->get_id() == entity->get_id(); });

		//if (it != upload_list->end())
		//{
		//	const auto e = *it;
		//	const auto t = e->get<transfer_component>();

		//	// Size
		//	const auto transfer = entity->get<transfer_component>();
		//	transfer->set_size(t->get_size());
		//	transfer->set_shards(0);

		//	// Remove
		//	const auto row = it - upload_list->begin();

		//	beginRemoveRows(QModelIndex(), row, row);
		//	upload_list->erase(it);
		//	endRemoveRows();
		//}
		
		// Status
		const auto status = entity->get<status_component>();
		status->add(m_status_function);

		const auto upload_list = m_entity->get<upload_list_component>();
		const auto row = upload_list->size();

		beginInsertRows(QModelIndex(), row, row);
		upload_list->push_back(entity);
		endInsertRows();

		// Callback
		const auto e = entity_factory::create_statusbar(statusbar::upload);
		m_entity->call(callback_type::status, callback_action::add, e);
	}

	void client_upload_model::on_remove()
	{
		assert(thread_info::main());

		const auto upload_list = m_entity->get<upload_list_component>();

		// NOTE: We must iterate from the back to the front for iterator safety
		for (int row = upload_list->size() - 1; row >= 0; --row)
		{
			const auto it = upload_list->begin() + row;

			const auto e = *it;
			const auto transfer = e->get<transfer_component>();

			if (transfer->invalid())
			{
				// Status
				const auto status = e->get<status_component>();
				status->remove(m_status_function);

				// Model
				beginRemoveRows(QModelIndex(), row, row);
				upload_list->erase(it);
				endRemoveRows();
			}
		}

		// Callback
		const auto e = entity_factory::create_statusbar(statusbar::upload);
		m_entity->call(callback_type::status, callback_action::remove, e);
	}

	void client_upload_model::on_clear()
	{
		assert(thread_info::main());

		const auto upload_list = m_entity->get<upload_list_component>();

		for (const auto& e : *upload_list)
		{
			const auto transfer = e->get<transfer_component>();
			transfer->stop();

			// Status
			const auto status = e->get<status_component>();
			status->remove(m_status_function);
		}

		beginResetModel();
		upload_list->clear();
		endResetModel();

		// Callback
		const auto e = entity_factory::create_statusbar(statusbar::upload);
		m_entity->call(callback_type::status, callback_action::clear, e);
	}

	void client_upload_model::on_double_click(const QModelIndex& index)
	{
		assert(thread_info::main());

		if (!index.isValid())
			return;

		const size_t row = static_cast<size_t>(index.row());
		const auto entity = get_entity(row);
		if (!entity)
			return;

		const auto file = entity->get<file_component>();
		const auto url = QUrl::fromLocalFile(QString::fromStdString(file->get_path()));
		QDesktopServices::openUrl(url);
	}

	// Utility
	bool client_upload_model::empty() const
	{
		assert(thread_info::main());

		const auto upload_list = m_entity->get<upload_list_component>();
		return upload_list->empty();
	}

	// Model
	bool client_upload_model::removeRows(int row, int count, const QModelIndex& parent /*= QModelIndex()*/)
	{
		assert(thread_info::main() && (count == 1));

		const auto upload_list = m_entity->get<upload_list_component>();
		const auto size = upload_list->size();

		const auto it = upload_list->begin() + row;
		if (it == upload_list->end())
			return false;

		const auto& e = *it;
		const auto transfer = e->get<transfer_component>();

		if (transfer->invalid())
		{
			// Status
			const auto status = e->get<status_component>();
			status->remove(m_status_function);

			// Model
			beginRemoveRows(QModelIndex(), row, row);
			upload_list->erase(it);
			endRemoveRows();

			// Callback
			const auto e = entity_factory::create_statusbar(statusbar::upload);
			m_entity->call(callback_type::status, callback_action::remove, e);
		}

		return upload_list->size() < size;
	}

	QVariant client_upload_model::data(const QModelIndex& index, int role) const
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
					case column::index:
					{
						return QVariant::fromValue(index.row() + 1);
					}
					case column::status :
					case column::name:
					case column::progress:
					{
						return QVariant::fromValue(entity);
					}
					case column::type:
					{
						const auto file = entity->get<file_component>();
						return QString::fromStdString(file->get_extension());
					}
					case column::checksum:
					{
						const auto file = entity->get<file_component>();
						const auto file_id = file->has_id() ? hex::encode(static_cast<u32>(crc32()(file->get_id()))) : "";
						return QString::fromStdString(file_id);
					}
					case column::size:
					{
						const auto file = entity->get<file_component>();
						return QString::fromStdString(file_util::get_size(file->get_size()));
					}
					case column::shards:
					{
						const auto transfer = entity->get<transfer_component>();
						return QVariant::fromValue<uint>(transfer->get_shards());
					}
					case column::time:
					{
						const auto transfer = entity->get<transfer_component>();
						return QString::fromStdString(transfer->get_time());
					}
					case column::speed:
					{
						const auto transfer = entity->get<transfer_component>();
						return QString::fromStdString(file_util::get_speed(transfer->get_speed()));
					}
				}
				break;
			}
			case Qt::UserRole:
			{
				switch (col)
				{
					case column::status:
					{
						const auto status = entity->get<status_component>();
						return QVariant::fromValue<uint>(static_cast<uint>(status->get_status()));
					}
					case column::name:
					{
						const auto file = entity->get<file_component>();
						return QString::fromStdString(file->get_stem()).toLower();
					}
					case column::type:
					{
						const auto file = entity->get<file_component>();
						return QString::fromStdString(file->get_extension()).toLower();
					}
					case column::size:
					{
						const auto file = entity->get<file_component>();
						return QVariant::fromValue<qulonglong>(file->get_size());
					}
					case column::shards:
					{
						const auto transfer = entity->get<transfer_component>();
						return QVariant::fromValue<uint>(transfer->get_shards());
					}
					case column::time:
					{
						const auto timer = entity->get<timer_component>();
						return QVariant::fromValue<uint>(timer->get_time());
					}
					case column::speed:
					{
						const auto transfer = entity->get<transfer_component>();
						return QVariant::fromValue<uint>(transfer->get_speed());
					}
					case column::progress:
					{
						const auto transfer = entity->get<transfer_component>();
						return QVariant::fromValue<double>(transfer->get_ratio());
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
						const auto file = entity->get<file_component>();
						return QString::fromStdString(file->get_stem());
					}
				}
				break;
			}
			case Qt::TextAlignmentRole:
			{
				switch (col)
				{
					case column::index:
					case column::type:
					case column::checksum:
					case column::size:
					case column::shards:
					case column::time:
					case column::speed:
					{
						return static_cast<int>(Qt::AlignVCenter | Qt::AlignRight);
					}
				}

				break;
			}
		}

		return QVariant();
	}

	int client_upload_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		assert(thread_info::main());

		const auto upload_list = m_entity->get<upload_list_component>();
		return upload_list->size();
	}

	// Get
	entity::ptr client_upload_model::get_entity(const size_t row) const
	{
		assert(thread_info::main());

		const auto upload_list = m_entity->get<upload_list_component>();
		return (row < upload_list->size()) ? upload_list->at(row) : nullptr;
	}

	size_t client_upload_model::get_row(const entity::ptr entity) const
	{
		assert(thread_info::main());

		// O(N)
		const auto upload_list = m_entity->get<upload_list_component>();
		const auto it = std::find(upload_list->begin(), upload_list->end(), entity);
		return (it != upload_list->end()) ? (it - upload_list->begin()) : npos;
	}
}
