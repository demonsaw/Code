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

#include <cassert>

#include <QTimer>

#include "component/chat_component.h"
#include "component/endpoint_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/client/client_service_component.h"

#include "controller/application_controller.h"
#include "controller/chat_controller.h"
#include "controller/message_controller.h"

#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "model/chat_model.h"
#include "thread/thread_info.h"
#include "utility/locale.h"
#include "utility/utf8.h"
#include "utility/value.h"
#include "utility/client/client_util.h"

namespace eja
{
	// Constructor
	chat_controller::chat_controller(QObject* parent /*= nullptr*/) : entity_object(parent)
	{
		m_model = new chat_model(this);
	}
	
	// Interface	
	void chat_controller::add(const entity::ptr entity)
	{
		assert(thread_info::main() && has_entity());

		m_model->add(entity);

		emit sizeChanged();
	}

	void chat_controller::update()
	{
		assert(thread_info::main() && has_entity());

		m_model->update();

		emit sizeChanged();
	}

	void chat_controller::clear()
	{
		assert(thread_info::main() && has_entity());

		m_model->clear();

		emit sizeChanged();
	}

	void chat_controller::reset()
	{
		assert(has_entity());

		m_model->reset();

		emit sizeChanged();
	}

	// Utility
	bool chat_controller::add(const QString& text)
	{
		assert(thread_info::main() && has_entity());

		// Spam
		if (!m_enabled)
			return false;

		// Verify
		const auto qtext = text.trimmed();
		const auto qlength = static_cast<size_t>(qtext.length() - qtext.count(' '));
		if (qlength < chat::min_text)
			return false;

		// Service
		const auto service = m_entity->get<client_service_component>();
		if (service->valid())
		{
			const auto entity = entity_factory::create_client_chat(m_entity);
			const auto chat = entity->get<chat_component>();
			chat->set_text(qtext.toStdString());

			// Callback
			const auto client = m_entity->get<endpoint_component>();
			if (client->unmuted())
				m_entity->call(callback_type::chat, callback_action::add, entity);

			service->async_chat(entity);
		}

		// Spam
		m_enabled = false;
		QTimer::singleShot(ui::enter, [this]() { m_enabled = true; });

		return true;
	}

	bool chat_controller::empty() const
	{
		return m_model->size() <= 1;
	}

	void chat_controller::mute(const int row, const bool value /*= true*/)
	{
		const auto entity = m_model->get_entity(row);
		if (entity)
		{
			const auto parent = entity->get_parent();
			if (!parent)
				return;

			const auto endpoint = parent->get<endpoint_component>();
			endpoint->set_mute(value);

			// Self?
			if (m_entity->equals<endpoint_component>(parent))
			{
				// HACK: Associate m_entity and the client list version
				//
				const auto entity = client_util::get_client(m_entity, endpoint->get_id());
				if (entity)
				{
					const auto endpoint = entity->get<endpoint_component>();
					endpoint->set_mute(value);
				}
			}

			// UI
			const auto index = m_model->get_index(row);
			m_model->update(index);
		}
	}

	void chat_controller::message(const int row)
	{
		assert(thread_info::main());

		const auto entity = m_model->get_entity(row);
		if (entity)
		{
			const auto parent = entity->get_parent();
			if (parent)
			{
				// Self?
				if (!m_entity->equals<endpoint_component>(parent))
				{
					const auto app = application_controller::get();
					const auto control = app->get_message_controller();					
					control->set_entity(parent);
					control->set_visible(true);
				}
				else
				{
					// HACK: Associate m_entity and the client list version
					//
					const auto endpoint = parent->get<endpoint_component>();
					const auto entity = client_util::get_client(m_entity, endpoint->get_id());
					if (entity)
					{
						const auto app = application_controller::get();
						const auto control = app->get_message_controller();
						control->set_entity(entity);
						control->set_visible(true);
					}
				}					
			}
		}
	}

	// Set
	void chat_controller::set_entity(const entity::ptr entity)
	{
		entity_object::set_entity(entity);

		m_model->set_entity(entity);

		emit sizeChanged();
	}

	// Get
	size_t chat_controller::get_size() const
	{ 
		return m_model->size(); 
	}
}
