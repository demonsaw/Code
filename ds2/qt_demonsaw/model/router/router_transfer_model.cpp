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

#include "router_transfer_model.h"
#include "component/timer_component.h"
#include "component/transfer/chunk_component.h"
#include "window/router_window.h"

namespace eja
{
	// Interface
	void router_transfer_model::init()
	{
		entity_table_model::init();

		// Callback
		add_callback(function_type::transfer);

		// Timer
		m_timer = new QTimer(this);
		m_timer->setInterval(1000);

		connect(m_timer, &QTimer::timeout, [this]()
		{
			const auto chunk_list = m_entity->get<chunk_entity_list_component>();
			if (!chunk_list || chunk_list->empty())
				return;

			QModelIndex start = createIndex(0, column::chunks);
			QModelIndex end = createIndex(chunk_list->size() - 1, column::idle);
			emit dataChanged(start, end);
		});

		m_timer->start();
	}

	// Slot
	void router_transfer_model::on_add(const entity::ptr entity)
	{
		assert(thread::main());

		const auto chunk_list = m_entity->get<chunk_entity_list_component>();
		if (!chunk_list)
			return;

		{
			auto_lock_ptr(chunk_list);

			const auto row = chunk_list->size();

			beginInsertRows(QModelIndex(), row, row);
			chunk_list->add(entity);
			endInsertRows();
		}

		set_status(router_statusbar::transfer, chunk_list->size());
	}

	void router_transfer_model::on_remove(const entity::ptr entity)
	{
		assert(thread::main());

		const auto chunk_list = m_entity->get<chunk_entity_list_component>();
		if (!chunk_list)
			return;

		{
			auto_lock_ptr(chunk_list);

			const auto row = chunk_list->get(entity);
			if (row == type::npos)
				return;

			beginRemoveRows(QModelIndex(), row, row);
			chunk_list->remove(row);
			endRemoveRows();
		}

		set_status(router_statusbar::transfer, chunk_list->size());
	}

	void router_transfer_model::on_clear()
	{
		assert(thread::main());

		const auto chunk_list = m_entity->get<chunk_entity_list_component>();
		if (!chunk_list)
			return;

		beginResetModel();
		chunk_list->clear();
		endResetModel();

		set_status(router_statusbar::transfer, chunk_list->size());
	}

	// Utility
	bool router_transfer_model::empty() const
	{
		const auto chunk_list = m_entity->get<chunk_entity_list_component>();
		return !chunk_list || chunk_list->empty();
	}

	// Model
	QVariant router_transfer_model::data(const QModelIndex& index, int role) const
	{
		if (!index.isValid())
			return QVariant();

		const size_t row = static_cast<size_t>(index.row());
		const size_t col = static_cast<size_t>(index.column());

		const auto chunk_list = m_entity->get<chunk_entity_list_component>();
		if (!chunk_list)
			return QVariant();

		const auto entity = chunk_list->get(row);
		if (!entity)
			return QVariant();

		switch (role)
		{
			case Qt::DisplayRole:
			{				
				switch (col)
				{
					case column::name:
					{
						return QString::fromStdString(entity->get_id());
					}
					case column::chunks:
					{
						const auto download_queue = entity->get<chunk_download_queue_component>();
						if (download_queue)
							return QVariant::fromValue<uint>(download_queue->size());

						break;
					}
					case column::idle:
					{
						const auto timer = entity->get<timer_component>();
						if (timer)
							return QString::fromStdString(timer->str());

						break;
					}
				}
				break;
			}
			case Qt::UserRole:
			{
				return data(index, Qt::DisplayRole);
			}
		}

		return QVariant();
	}

	int router_transfer_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		const auto chunk_list = m_entity->get<chunk_entity_list_component>();
		return chunk_list ? chunk_list->size() : 0;
	}

	// Accessor
	size_t router_transfer_model::get_row(const entity::ptr entity) const
	{
		const auto chunk_list = m_entity->get<chunk_entity_list_component>();
		return chunk_list ? chunk_list->get(entity) : type::npos;
	}
}
