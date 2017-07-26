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

#include "router_session_model.h"
#include "component/function_component.h"
#include "component/session_component.h"
#include "component/timeout_component.h"
#include "component/client/client_component.h"
#include "window/router_window.h"

namespace eja
{
	// Interface
	void router_session_model::init()
	{
		entity_table_model::init();

		// Callback
		add_callback(function_type::client);

		// Timer
		m_timer = new QTimer(this);
		m_timer->setInterval(1000);

		connect(m_timer, &QTimer::timeout, [this]()
		{
			const auto session_list = m_entity->get<session_entity_list_component>();
			if (!session_list || session_list->empty())
				return;
			
			QModelIndex start = createIndex(0, column::idle);
			QModelIndex end = createIndex(session_list->size() - 1, column::idle);
			emit dataChanged(start, end);
		});

		m_timer->start();
	}

	// Slot
	void router_session_model::on_add(const entity::ptr entity)
	{
		assert(thread::main());

		const auto session_list = m_entity->get<session_entity_list_component>();
		if (!session_list)
			return;

		{
			auto_lock_ptr(session_list);

			const auto row = session_list->size();

			beginInsertRows(QModelIndex(), row, row);
			session_list->add(entity);
			endInsertRows();
		}

		set_status(router_statusbar::session, session_list->size());
	}

	void router_session_model::on_remove(const entity::ptr entity)
	{
		assert(thread::main());

		const auto session_list = m_entity->get<session_entity_list_component>();
		if (!session_list)
			return;

		{
			auto_lock_ptr(session_list);

			const auto row = session_list->get(entity);
			if (row == type::npos)
				return;

			beginRemoveRows(QModelIndex(), row, row);
			session_list->remove(row);
			endRemoveRows();
		}

		set_status(router_statusbar::session, session_list->size());
	}

	void router_session_model::on_clear()
	{
		assert(thread::main());

		const auto session_list = m_entity->get<session_entity_list_component>();
		if (!session_list)
			return;

		beginResetModel();
		session_list->clear();
		endResetModel();

		set_status(router_statusbar::session, session_list->size());
	}

	// Utility
	bool router_session_model::empty() const
	{
		const auto session_list = m_entity->get<session_entity_list_component>();
		return !session_list || session_list->empty();
	}

	// Model
	QVariant router_session_model::data(const QModelIndex& index, int role) const
	{
		if (!index.isValid())
			return QVariant();

		const size_t row = static_cast<size_t>(index.row());
		const size_t col = static_cast<size_t>(index.column());

		const auto session_list = m_entity->get<session_entity_list_component>();
		if (!session_list)
			return QVariant();

		const auto entity = session_list->get(row);
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
						const auto session = entity->get<session_component>();
						if (session)
							return QString::fromStdString(session->get_id());

						break;
					}
					case column::idle:
					{
						const auto timeout = entity->get<timeout_component>();
						if (timeout)
							return QString::fromStdString(timeout->str());

						break;
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
						const auto session = entity->get<session_component>();
						if (session)
							return QString::fromStdString(session->get_id()).toLower();

						break;
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

	int router_session_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		const auto session_list = m_entity->get<session_entity_list_component>();
		return session_list ? session_list->size() : 0;
	}

	// Accessor
	size_t router_session_model::get_row(const entity::ptr entity) const
	{
		const auto session_list = m_entity->get<session_entity_list_component>();
		return session_list ? session_list->get(entity) : type::npos;
	}
}
