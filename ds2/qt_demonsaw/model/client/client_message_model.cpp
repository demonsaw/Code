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

#include <QIcon>
#include <QMediaPlayer>

#include "client_message_model.h"
#include "component/color_component.h"
#include "component/chat_component.h"
#include "component/time_component.h"
#include "component/client/client_component.h"
#include "component/client/client_option_component.h"
#include "resource/resource.h"
#include "window/client_window.h"
#include "window/main_window.h"
#include "window/window.h"

namespace eja
{
	// Interface
	void client_message_model::init()
	{
		entity_table_model::init();

		// Create
		m_player = new QMediaPlayer(this);

		// Signal
		connect(this, &client_message_model::size_hint, this, &client_message_model::on_size_hint);
		connect(this, &client_message_model::alert, this, &client_message_model::on_alert);
	}

	// Signal
	void client_message_model::add(const entity::ptr entity)
	{		
		client_component::ptr client;

		if (entity->has<client_component>())
		{
			// External
			client = entity->get<client_component>();
			if (!client)
				return;

			emit alert();
		}
		else
		{
			// Internal
			client = m_entity->get<client_component>();
			if (!client)
				return;

			entity->add(client);
		}

		// Color
		const auto& name = client->get_name();
		const auto color = color_component::create(name);
		entity->add(color);

		// Time
		entity->add<time_component>();
		
		emit added(entity);
	}

	// Slot
	void client_message_model::on_add(const entity::ptr entity)
	{
		assert(thread::main());

		const auto message_vector = m_entity->get<message_entity_vector_component>();
		if (!message_vector)
			return;

		{
			auto_lock_ptr(message_vector);
			
			const auto row = message_vector->size();

			// Add
			beginInsertRows(QModelIndex(), row, row);
			message_vector->add(entity);
			endInsertRows();
			
			emit size_hint(row);
		}

		set_status(client_statusbar::message, message_vector->size());
	}

	void client_message_model::on_remove(const entity::ptr entity)
	{
		assert(thread::main());

		const auto message_vector = m_entity->get<message_entity_vector_component>();
		if (!message_vector)
			return;

		{
			auto_lock_ptr(message_vector);

			const auto row = message_vector->get(entity);
			if (row == type::npos)
				return;

			beginRemoveRows(QModelIndex(), row, row);
			message_vector->remove(row);
			endRemoveRows();
		}

		set_status(client_statusbar::message, message_vector->size());
	}

	void client_message_model::on_remove(const size_t row)
	{
		assert(thread::main());

		const auto message_vector = m_entity->get<message_entity_vector_component>();
		if (!message_vector)
			return;

		{
			auto_lock_ptr(message_vector);

			beginRemoveRows(QModelIndex(), row, row);
			message_vector->remove(row);
			endRemoveRows();
		}

		set_status(client_statusbar::message, message_vector->size());
	}

	void client_message_model::on_clear()
	{
		assert(thread::main());

		const auto message_vector = m_entity->get<message_entity_vector_component>();
		if (!message_vector)
			return;

		beginResetModel();
		message_vector->clear();
		endResetModel();

		set_status(client_statusbar::message, message_vector->size());
	}

	void client_message_model::on_size_hint(const int row)
	{
		assert(thread::main());

		if (!m_resize_function.empty())
			m_resize_function.call(row);
	}

	void client_message_model::on_alert()
	{
		assert(thread::main());

		// Notifications			
		const auto option = m_entity->get<client_option_component>();
		if (option)
		{
			// Audio
			if (option->has_message_audio() && (m_player->state() != QMediaPlayer::PlayingState))
			{
				if (!option->has_message_sound())
					m_player->setMedia(QUrl(resource::audio::message));
				else
					m_player->setMedia(QUrl::fromLocalFile(QString::fromStdString(option->get_message_sound())));

				m_player->setVolume(option->get_message_volume());
				m_player->play();
			}

			// Visual
			if (option->has_message_visual())
			{
				const auto main = main_window::get_instance();
				main->flash();
			}
		}

		// Tab
		const auto mw = main_window::get_instance();
		const auto ew = mw->get_entity_window(m_entity);
		if (ew)
		{
			const auto tw = ew->get_main_tab();
			if (tw)
			{
				const auto index = static_cast<int>(client_statusbar::message);
				tw->setTabIcon(static_cast<int>(client_statusbar::message), QIcon(resource::tab::message_unread));
			}
		}
	}

	// Utility
	bool client_message_model::empty() const
	{
		const auto message_vector = m_entity->get<message_entity_vector_component>();
		return !message_vector || message_vector->empty();
	}

	// Model
	bool client_message_model::removeRows(int row, int count, const QModelIndex& parent /*= QModelIndex()*/)
	{
		assert(thread::main());

		const auto message_vector = m_entity->get<message_entity_vector_component>();
		if (!message_vector)
			return false;

		size_t status = 0;

		beginRemoveRows(QModelIndex(), row, (row + count - 1));

		for (auto i = (row + count - 1); i >= row; --i)
		{
			const auto entity = message_vector->get(row);
			if (entity)
				status += message_vector->remove(row);
		}

		endRemoveRows();

		set_status(client_statusbar::message, message_vector->size());

		return status > 0;
	}

	Qt::ItemFlags client_message_model::flags(const QModelIndex& index) const
	{
		if (!index.isValid())
			return Qt::ItemIsEnabled;

		Qt::ItemFlags flags = QAbstractItemModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemIsSelectable;

		switch (index.column())
		{
		case column::message:
		{
			flags |= Qt::ItemIsEditable;
			break;
		}
		}

		return flags;
	}

	QVariant client_message_model::data(const QModelIndex& index, int role) const
	{
		if (!index.isValid())
			return QVariant();

		const size_t row = static_cast<size_t>(index.row());
		const size_t col = static_cast<size_t>(index.column());

		const auto message_vector = m_entity->get<message_entity_vector_component>();
		if (!message_vector)
			return QVariant();

		const auto entity = message_vector->get(row);
		if (!entity)
			return QVariant();

		switch (role)
		{
		case Qt::DisplayRole:
		{
			switch (col)
			{
				case column::time:
				case column::name:
				case column::message:
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
				case column::time:
				{
					const auto time = entity->get<time_component>();
					if (time)
						return QVariant::fromValue<uint>(time->elapsed());

					break;
				}
				case column::name:
				{
					const auto client = entity->get<client_component>();
					if (client)
						return QString::fromStdString(client->get_join_name()).toLower();

					break;
				}
				case column::message:
				{
					const auto chat = entity->get<chat_component>();
					if (chat)
						return QString::fromStdString(chat->get_text()).toLower();

					break;
				}
			}

			break;
		}
		case Qt::EditRole:
		{
			switch (col)
			{
				case column::time:
				{
					// Component
					const auto time = entity->get<time_component>();
					if (time)
					{
						char buffer[64] = { 0 };
						const auto tm = localtime(&time->data());
						const auto client_option = m_entity->get<client_option_component>();
						strftime(buffer, 64, client_option ? client_option->get_message_timestamp().c_str() : default_message::timestamp, tm);
						return QString(buffer);
					}

					break;
				}
				case column::name:
				{
					const auto client = entity->get<client_component>();
					if (client)
						return QString::fromStdString(client->get_join_name());

					break;
				}
				case column::message:
				{
					const auto chat = entity->get<chat_component>();
					if (chat)
						return QString::fromStdString(chat->get_text());

					break;
				}
			}

			break;
		}
		case Qt::TextAlignmentRole:
		{
			switch (col)
			{
				case column::time:
				{
					return static_cast<int>(Qt::AlignTop | Qt::AlignLeft);
				}
			}

			break;
		}
		}

		return QVariant();
	}

	int client_message_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		const auto message_vector = m_entity->get<message_entity_vector_component>();
		return message_vector ? message_vector->size() : 0;
	}

	// Accessor
	size_t client_message_model::get_row(const entity::ptr entity) const
	{
		const auto message_vector = m_entity->get<message_entity_vector_component>();
		return message_vector ? message_vector->get(entity) : type::npos;
	}
}
