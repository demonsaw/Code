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

#include "component/io/download_component.h"
#include "component/io/file_component.h"
#include "component/io/upload_component.h"
#include "component/time/timer_component.h"

#include "entity/entity.h"
#include "model/router/router_download_model.h"
#include "security/checksum/crc.h"
#include "security/filter/hex.h"
#include "thread/thread_info.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Interface
	void router_download_model::on_init()
	{
		assert(thread_info::main());

		entity_table_model::on_init();

		// Timer
		m_timer = new QTimer(this);
		m_timer->setInterval(1000);

		connect(m_timer, &QTimer::timeout, [this]()
		{
			const auto download_list = m_entity->get<download_list_component>();

			QModelIndex start = createIndex(0, column::queue);
			QModelIndex end = createIndex(download_list->size() - 1, column::max - 1);
			emit dataChanged(start, end);

		});

		m_timer->start();
	}

	// Slot
	void router_download_model::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto download_list = m_entity->get<download_list_component>();
		const auto row = download_list->size();

		beginInsertRows(QModelIndex(), row, row);
		download_list->push_back(entity);
		endInsertRows();
	}

	void router_download_model::on_clear()
	{
		assert(thread_info::main());

		const auto download_list = m_entity->get<download_list_component>();

		beginResetModel();
		download_list->clear();
		endResetModel();
	}

	// Utility
	bool router_download_model::empty() const
	{
		assert(thread_info::main());

		const auto download_list = m_entity->get<download_list_component>();
		return download_list->empty();
	}

	// Model
	bool router_download_model::removeRows(int row, int count, const QModelIndex& parent /*= QModelIndex()*/)
	{
		assert(thread_info::main());

		const auto download_list = m_entity->get<download_list_component>();
		const auto it = download_list->cbegin() + row;
		const auto size = download_list->size();

		beginRemoveRows(QModelIndex(), row, (row + count - 1));
		download_list->erase(it, it + count);
		endRemoveRows();

		return download_list->size() < size;
	}

	// Model
	QVariant router_download_model::data(const QModelIndex& index, int role) const
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
					case column::file:
					{
						const auto file = entity->get<file_component>();
						const auto file_id = hex::encode(static_cast<u32>(crc32()(file->get_id())));
						return QString::fromStdString(file_id);
					}
					case column::queue:
					{
						// NOTE: No need to lock
						const auto download_list = entity->get<download_index_list_component>();
						return QVariant::fromValue<uint>(download_list->size());
					}
					case column::buffer:
					{
						// NOTE: No need to lock
						const auto upload_list = entity->get<upload_data_list_component>();
						return QVariant::fromValue<uint>(upload_list->size());
					}
				}
				break;
			}
			case Qt::UserRole:
			{
				switch (col)
				{
					default:
					{
						return data(index, Qt::DisplayRole);
					}
				}

				break;
			}
			case Qt::TextAlignmentRole:
			{
				switch (col)
				{
					case column::file:
					case column::queue:
					case column::buffer:
					{
						return static_cast<int>(Qt::AlignVCenter | Qt::AlignRight);
					}
				}

				break;
			}
		}

		return QVariant();
	}

	int router_download_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		assert(thread_info::main());

		const auto download_list = m_entity->get<download_list_component>();
		return download_list->size();
	}

	// Get
	entity::ptr router_download_model::get_entity(const size_t row) const
	{
		assert(thread_info::main());

		const auto download_list = m_entity->get<download_list_component>();
		return (row < download_list->size()) ? download_list->at(row) : nullptr;
	}

	size_t router_download_model::get_row(const entity::ptr entity) const
	{
		assert(thread_info::main());

		const auto download_list = m_entity->get<download_list_component>();
		const auto it = std::find(download_list->begin(), download_list->end(), entity);
		return (it != download_list->end()) ? (it - download_list->begin()) : npos;
	}
}
