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

#include <QColor>
#include <QIcon>

#include "client_client_model.h"
#include "component/color_component.h"
#include "component/chat_component.h"
#include "component/pixmap_component.h"
#include "component/client/client_option_component.h"
#include "http/http_code.h"
#include "http/http_socket.h"
#include "resource/resource.h"
#include "window/window.h"

namespace eja
{
	// Interface
	void client_client_model::init()
	{
		entity_table_model::init();

		// Callback
		add_callback(function_type::client);
	}

	void client_client_model::add(const entity::ptr entity)
	{
		const auto client = entity->get<client_component>();
		if (!client)
			return;

		const auto client_vector = m_entity->get<client_entity_vector_component>();
		if (!client_vector)
			return;

		const auto found = client_vector->find_if([&](const entity::ptr e)
		{
			const auto c = e->get<client_component>();
			return c && (c->get_id() == client->get_id());
		});

		if (!found)
			emit added(entity);
	}

	// Slot
	void client_client_model::on_add(const entity::ptr entity)
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
	}

	void client_client_model::on_refresh(const entity::ptr entity)
	{
		assert(thread::main());

		const auto client_vector = m_entity->get<client_entity_vector_component>();
		if (!client_vector)
			return;

		{
			auto_lock_ptr(client_vector);

			beginResetModel();

			for (auto& item : client_vector->copy())
			{
				// Option
				if (!item->has<client_option_component>())
					item->add<client_option_component>(m_entity);

				// Message
				if (!item->has<message_entity_vector_component>())
					item->add<message_entity_vector_component>();

				// Color
				const auto client = item->get<client_component>();
				if (client)
				{
					const auto color = client ? color_component::create(client->get_name()) : color_component::create(QColor(default_chat::color));
					item->add(color);
				}

				// Chat
				// HACK: This isn't really a good design - move to a chat_options_component?
				if (!item->has<chat_component>())
					item->add<chat_component>();
			}

			endResetModel();
		}
	}

	void client_client_model::on_clear()
	{
		assert(thread::main());

		const auto client_vector = m_entity->get<client_entity_vector_component>();
		if (!client_vector)
			return;

		beginResetModel();
		client_vector->clear();
		endResetModel();
	}

	// Utility
	bool client_client_model::empty() const
	{
		const auto client_vector = m_entity->get<client_entity_vector_component>();
		return !client_vector || client_vector->empty();
	}

	// Model
	QVariant client_client_model::data(const QModelIndex& index, int role) const
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
					case column::number:
					{
						return QVariant::fromValue(entity);
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
							return QString::fromStdString(client->get_join_name()).toLower();
												
						break;
					}
					case column::number:
					{
						const auto message_vector = entity->get<message_entity_vector_component>();
						if (message_vector)
							return QVariant::fromValue<uint>(message_vector->size());
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
						const auto client = entity->get<client_component>();
						if (client)
							return QString::fromStdString(client->get_join_name());
												
						break;
					}
					default:
					{
						return data(index, Qt::DisplayRole);
					}
				}

				break;
			}
			case Qt::DecorationRole:
			{
				switch (col)
				{
					case column::number:
					{
						return static_cast<int>(Qt::AlignVCenter | Qt::AlignRight);
					}
				}
			}
		}

		return QVariant();
	}

	int client_client_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		const auto client_vector = m_entity->get<client_entity_vector_component>();
		return client_vector ? client_vector->size() : 0;
	}

	// Accessor
	size_t client_client_model::get_row(const entity::ptr entity) const
	{
		const auto client_vector = m_entity->get<client_entity_vector_component>();
		return client_vector ? client_vector->get(entity) : type::npos;
	}
}
