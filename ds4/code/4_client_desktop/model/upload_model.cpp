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

#include <QTimer>

#include "component/callback/callback.h"
#include "component/io/file_component.h"
#include "component/status/status_component.h"
#include "component/time/timer_component.h"
#include "component/transfer/transfer_component.h"
#include "component/transfer/upload_component.h"

#include "entity/entity.h"
#include "model/upload_model.h"
#include "thread/thread_info.h"
#include "utility/io/file_util.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Constructor
	upload_model::upload_model(const entity::ptr& entity, QObject* parent /*= nullptr*/) : entity_table_model(entity, { "", "Name", "Type", "Size", "Time", "Speed", "Progress" }, { 40, 504, 80, 80, 80, 80, 128 }, parent)
	{
		assert(thread_info::main());

		// Timer
		m_timer = new QTimer(this);
		m_timer->setInterval(sec_to_ms(1));

		connect(m_timer, &QTimer::timeout, [this]()
		{
			const auto upload_list = m_entity->get<upload_list_component>();

			QModelIndex start = createIndex(0, column::time);
			QModelIndex end = createIndex(upload_list->size() - 1, column::time);
			emit dataChanged(start, end);

		});

		m_timer->start();
	}

	// Slot
	void upload_model::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto upload_list = m_entity->get<upload_list_component>();
		const auto row = upload_list->size();

		beginInsertRows(QModelIndex(), row, row);
		upload_list->push_back(entity);
		endInsertRows();

		// Callback
		m_entity->async_call(callback::statusbar | callback::add);
	}

	void upload_model::on_add(const entity::ptr entity, const size_t row)
	{
		assert(thread_info::main());

		const auto upload_list = m_entity->get<upload_list_component>();
		const auto it = upload_list->begin() + row;

		beginInsertRows(QModelIndex(), row, row);
		upload_list->insert(it, entity);
		endInsertRows();

		// Callback
		m_entity->async_call(callback::statusbar | callback::add);
	}

	void upload_model::on_clear()
	{
		assert(thread_info::main());

		const auto upload_list = m_entity->get<upload_list_component>();

		beginResetModel();
		upload_list->clear();
		endResetModel();

		// Callback
		m_entity->async_call(callback::statusbar | callback::remove);
	}

	// Utility
	bool upload_model::empty() const
	{
		assert(thread_info::main());

		const auto upload_list = m_entity->get<upload_list_component>();
		return upload_list->empty();
	}

	// Model
	bool upload_model::removeRows(int row, int count, const QModelIndex& index /*= QModelIndex()*/)
	{
		assert(thread_info::main());
		
		const auto upload_list = m_entity->get<upload_list_component>();
		const auto it = upload_list->begin() + row;

		beginRemoveRows(index, row, (row + count - 1));
		upload_list->erase(it, it + count);
		endRemoveRows();

		// Callback
		m_entity->async_call(callback::statusbar | callback::remove);

		return true;
	}

	QVariant upload_model::data(const QModelIndex& index, int role) const
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
					case column::name:
					case column::progress:
					{
						return QVariant::fromValue(entity);
					}
					case column::type:
					{
						const auto file = entity->get<file_component>();
#if _WSTRING
						return QString::fromStdWString(file->get_wextension());
#else
						return QString::fromStdString(file->get_extension());
#endif
					}
					case column::size:
					{
						const auto file = entity->get<file_component>();
						return QString::fromStdString(file_util::get_size(file->get_size()));
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
				switch (index.column())
				{
					case column::row:
					{
						return QVariant::fromValue(index.row());
					}
					case column::name:
					{
						const auto file = entity->get<file_component>();
#if _WSTRING
						return QString::fromStdWString(file->get_wextension());
#else
						return QString::fromStdString(file->get_extension());
#endif
					}
					case column::size:
					{
						const auto file = entity->get<file_component>();
						return QVariant::fromValue<qulonglong>(file->get_size());
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
			case Qt::TextAlignmentRole:
			{
				switch (index.column())
				{
					case column::type:
					case column::size:
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

	int upload_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		assert(thread_info::main());

		const auto upload_list = m_entity->get<upload_list_component>();
		return upload_list->size();
	}

	// Get
	entity::ptr upload_model::get_entity(const size_t row) const
	{
		assert(thread_info::main());

		const auto upload_list = m_entity->get<upload_list_component>();
		return (row < upload_list->size()) ? upload_list->at(row) : nullptr;
	}

	size_t upload_model::get_row(const entity::ptr& entity) const
	{
		assert(thread_info::main());

		// O(N)
		const auto upload_list = m_entity->get<upload_list_component>();
		const auto it = std::find(upload_list->begin(), upload_list->end(), entity);
		return (it != upload_list->end()) ? (it - upload_list->begin()) : npos;
	}
}
