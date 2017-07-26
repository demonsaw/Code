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

#include "component/endpoint_component.h"
#include "component/version_component.h"
#include "component/group/group_component.h"
#include "component/session/session_component.h"
#include "component/time/timeout_component.h"

#include "entity/entity.h"
#include "model/router/router_session_model.h"
#include "security/checksum/crc.h"
#include "security/filter/hex.h"
#include "thread/thread_info.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Interface
	void router_session_model::on_init()
	{
		assert(thread_info::main());

		entity_table_model::on_init();

		// Timer
		m_timer = new QTimer(this);
		m_timer->setInterval(1000);

		connect(m_timer, &QTimer::timeout, [this]()
		{
			const auto session_list = m_entity->get<session_list_component>();

			QModelIndex start = createIndex(0, column::idle);
			QModelIndex end = createIndex(session_list->size() - 1, column::max - 1);
			emit dataChanged(start, end);

		});

		m_timer->start();
	}

	// Slot
	void router_session_model::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto session_list = m_entity->get<session_list_component>();
		const auto row = session_list->size();

		beginInsertRows(QModelIndex(), row, row);
		session_list->push_back(entity);
		endInsertRows();
	}

	void router_session_model::on_clear()
	{
		assert(thread_info::main());

		const auto session_list = m_entity->get<session_list_component>();

		beginResetModel();
		session_list->clear();
		endResetModel();
	}

	// Utility
	bool router_session_model::empty() const
	{
		assert(thread_info::main());

		const auto session_list = m_entity->get<session_list_component>();
		return session_list->empty();
	}

	// Model
	bool router_session_model::removeRows(int row, int count, const QModelIndex& parent /*= QModelIndex()*/)
	{
		assert(thread_info::main());

		// TODO: Remove data structures as well!
		//
		//

		const auto session_list = m_entity->get<session_list_component>();
		const auto it = session_list->cbegin() + row;
		const auto size = session_list->size();

		beginRemoveRows(QModelIndex(), row, (row + count - 1));
		session_list->erase(it, it + count);
		endRemoveRows();

		return session_list->size() < size;
	}

	QVariant router_session_model::data(const QModelIndex& index, int role) const
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
					case column::session:
					{
						const auto session = entity->get<session_component>();
						const auto session_id = hex::encode(static_cast<u32>(crc32()(session->get_id())));
						return QString::fromStdString(session_id);
					}
					case column::client:
					{
						const auto endpoint = entity->get<endpoint_component>();
						const auto client_id = endpoint->has_id() ? hex::encode(static_cast<u32>(crc32()(endpoint->get_id()))) : "";
						return QString::fromStdString(client_id);
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
					case column::idle:
					{
						const auto timeout = entity->get<timeout_component>();
						return QString::fromStdString(timeout->str());
					}
				}

				break;
			}
			case Qt::UserRole:
			{
				switch (col)
				{
					case column::session:
					{
						const auto session = entity->get<session_component>();
						return QString::fromStdString(session->get_id());
					}
					case column::client:
					{
						const auto client = entity->get<endpoint_component>();
						return QString::fromStdString(client->get_id());
					}
					case column::idle:
					{
						const auto timeout = entity->get<timeout_component>();
						return QVariant::fromValue<uint>(timeout->get_time());
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
				switch (col)
				{
					case column::session:
					case column::client:
					case column::group:
					case column::version:
					case column::idle:
					{
						return static_cast<int>(Qt::AlignVCenter | Qt::AlignRight);
					}
				}

				break;
			}
		}

		return QVariant();
	}

	int router_session_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		assert(thread_info::main());

		const auto session_list = m_entity->get<session_list_component>();
		return session_list->size();
	}

	// Get
	entity::ptr router_session_model::get_entity(const size_t row) const
	{
		assert(thread_info::main());

		const auto session_list = m_entity->get<session_list_component>();
		return (row < session_list->size()) ? session_list->at(row) : nullptr;
	}

	size_t router_session_model::get_row(const entity::ptr entity) const
	{
		assert(thread_info::main());

		const auto session_list = m_entity->get<session_list_component>();
		const auto it = std::find(session_list->begin(), session_list->end(), entity);
		return (it != session_list->end()) ? (it - session_list->begin()) : npos;
	}
}
