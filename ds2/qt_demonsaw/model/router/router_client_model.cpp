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

#include "router_client_model.h"
#include "component/function_component.h"
#include "component/group/group_component.h"
#include "component/session_component.h"
#include "component/timeout_component.h"
#include "component/client/client_component.h"
#include "window/router_window.h"

namespace eja
{
	// Interface
	void router_client_model::init()
	{
		entity_table_model::init();

		// Callback
		add_callback(function_type::client);

		// Timer
		m_timer = new QTimer(this);
		m_timer->setInterval(1000);

		connect(m_timer, &QTimer::timeout, [this]()
		{
			const auto client_vector = m_entity->get<client_entity_vector_component>();
			if (!client_vector || client_vector->empty())
				return;
			
			QModelIndex start = createIndex(0, column::idle);
			QModelIndex end = createIndex(client_vector->size() - 1, column::idle);
			emit dataChanged(start, end);
		});

		m_timer->start();
	}

	void router_client_model::remove(const entity::ptr entity)
	{
		entity->shutdown();

		emit removed(entity);
	}

	void router_client_model::clear()
	{
		const auto client_vector = m_entity->get<client_entity_vector_component>();
		if (!client_vector)
			return;

		for (const auto& entity : client_vector->copy())
			entity->shutdown();

		emit cleared();
	}

	// Slot
	void router_client_model::on_add(const entity::ptr entity)
	{
		assert(thread::main());

		const auto client_vector = m_entity->get<client_entity_vector_component>();
		if (!client_vector)
			return;

		{
			auto_lock_ptr(client_vector);

			const auto row = client_vector->size();

			beginInsertRows(QModelIndex(), row, row);
			client_vector->add(entity);
			endInsertRows();
		}

		set_status(router_statusbar::client, client_vector->size());
	}

	void router_client_model::on_remove(const entity::ptr entity)
	{
		assert(thread::main());

		const auto client_vector = m_entity->get<client_entity_vector_component>();
		if (!client_vector)
			return;

		{
			auto_lock_ptr(client_vector);

			const auto row = client_vector->get(entity);
			if (row == type::npos)
				return;

			beginRemoveRows(QModelIndex(), row, row);
			client_vector->remove(row);
			endRemoveRows();
		}

		set_status(router_statusbar::client, client_vector->size());
	}

	void router_client_model::on_clear()
	{
		assert(thread::main());

		const auto client_vector = m_entity->get<client_entity_vector_component>();
		if (!client_vector)
			return;

		beginResetModel();
		client_vector->clear();
		endResetModel();

		set_status(router_statusbar::client, client_vector->size());
	}

	// Utility
	bool router_client_model::empty() const
	{
		const auto client_vector = m_entity->get<client_entity_vector_component>();
		return !client_vector || client_vector->empty();
	}

	// Model
	QVariant router_client_model::data(const QModelIndex& index, int role) const
	{
		if (!index.isValid())
			return QVariant();
		
		const size_t row = static_cast<size_t>(index.row());
		const size_t col = static_cast<size_t>(index.column());

		const auto client_vector = m_entity->get<client_entity_vector_component>();
		if (!client_vector)
			return QVariant();

		const auto entity = client_vector->get(row);
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
						const auto client = entity->get<client_component>();
						if (client)
							return QString::fromStdString(client->get_name());

						break;
					}
					case column::version:
					{
						const auto client = entity->get<client_component>();
						if (client)
							return QString::fromStdString(client->get_version());

						break;
					}
					case column::session:
					{
						const auto session = entity->get<session_component>();
						if (session)
							return QString::fromStdString(session->get_id());

						break;
					}
					case column::group:
					{
						const auto group = entity->get<group_component>();
						if (group)
							return QString::fromStdString(group->get_name());

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
						const auto client = entity->get<client_component>();
						if (client)
							return QString::fromStdString(client->get_name()).toLower();

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

	int router_client_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		const auto session_list = m_entity->get<client_entity_vector_component>();
		return session_list ? session_list->size() : 0;
	}

	// Accessor
	size_t router_client_model::get_row(const entity::ptr entity) const
	{
		const auto client_vector = m_entity->get<client_entity_vector_component>();
		return client_vector ? client_vector->get(entity) : type::npos;
	}
}
