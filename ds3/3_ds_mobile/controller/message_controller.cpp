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
#include "component/message_wrapper_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/client/client_service_component.h"

#include "controller/application_controller.h"
#include "controller/envelope_controller.h"
#include "controller/message_controller.h"

#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "model/envelope_model.h"
#include "model/message_model.h"
#include "thread/thread_info.h"
#include "utility/locale.h"
#include "utility/utf8.h"
#include "utility/value.h"
#include "utility/client/client_util.h"

namespace eja
{
	// Interface
	void message_controller::add(const entity::ptr entity)
	{
		assert(thread_info::main() && has_entity());

		if (has_entity())
		{
			const auto message_comp = m_entity->get<message_wrapper_component>();
			const auto message_model = message_comp->get_model();
			message_model->add(entity);

			// Update envelope icon
			const auto app = application_controller::get();
			const auto envelope_control = app->get_envelope_controller();
			const auto envelope_model = envelope_control->get_model();
			envelope_model->update(m_entity);

			emit sizeChanged();
		}		
	}

	void message_controller::update()
	{
		//assert(thread_info::main() && has_entity());

		if (has_entity())
		{
			const auto comp = m_entity->get<message_wrapper_component>();
			const auto model = comp->get_model();
			model->update();

			emit sizeChanged();
		}		
	}

	void message_controller::clear()
	{
		assert(thread_info::main());

		if (has_entity())
		{
			const auto comp = m_entity->get<message_wrapper_component>();
			const auto model = comp->get_model();
			model->clear();

			// Update envelope icon
			const auto app = application_controller::get();
			const auto envelope_control = app->get_envelope_controller();
			const auto envelope_model = envelope_control->get_model();
			envelope_model->update(m_entity);

			emit sizeChanged();
		}
	}


	void message_controller::reset()
	{
		assert(thread_info::main() && has_entity());

		if (has_entity())
		{
			const auto comp = m_entity->get<message_wrapper_component>();
			const auto model = comp->get_model(); 
			model->reset();

			emit sizeChanged();
		}
	}

	// Utility
	bool message_controller::add(const QString& text)
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
		const auto service = m_entity->find<client_service_component>();
		if (service->valid())
		{
			const auto endpoint = m_entity->get<endpoint_component>();
			if (endpoint->has_id())
			{
				const auto parent = m_entity->get_parent();
				const auto entity = entity_factory::create_client_chat(parent);
				const auto chat = entity->get<chat_component>();
				chat->set_text(qtext.toStdString());

				// Callback
				const auto client = parent->get<endpoint_component>();
				if (client->unmuted())
				{
					chat->set_client(endpoint->get_id());
					m_entity->call(callback_type::chat, callback_action::add, entity);
					service->async_chat(entity, m_entity);
				}
				else
				{
					// Do not allow self-muted pm's to flood the network
					if (!m_entity->equals<endpoint_component>(parent))
						service->async_chat(entity, m_entity);
				}
			}
		}

		// Spam
		m_enabled = false;
		QTimer::singleShot(ui::enter, [this]() { m_enabled = true; });

		return true;
	}

	bool message_controller::empty() const
	{
		assert(thread_info::main() && has_entity());

		if (has_entity())
		{
			const auto comp = m_entity->get<message_wrapper_component>();
			const auto model = comp->get_model();
			return model->empty();
		}

		return true;
	}

	// Set
	void message_controller::set_entity(const entity::ptr entity)
	{
		assert(thread_info::main());

		entity_object::set_entity(entity);

		emit sizeChanged();
	}

	// Get
	size_t message_controller::get_size() const
	{
		assert(thread_info::main() && has_entity());

		if (has_entity())
		{
			const auto comp = m_entity->get<message_wrapper_component>();
			const auto model = comp->get_model();
			return model->size();
		}

		return 0;
	}

	const QObject* message_controller::get_model() const
	{
		if (has_entity())
		{
			const auto comp = m_entity->get<message_wrapper_component>();
			return comp->get_model();
		}

		return nullptr;
	}

	QObject* message_controller::get_model()
	{
		if (has_entity())
		{
			const auto comp = m_entity->get<message_wrapper_component>();
			return comp->get_model();
		}
		
		return nullptr;
	}

	QString message_controller::get_name() const
	{
		assert(thread_info::main() && has_entity());

		if (has_entity())
		{
			const auto endpoint = m_entity->get<endpoint_component>();
			return QString::fromStdString(endpoint->get_name());
		}

		return "";
	}
}
