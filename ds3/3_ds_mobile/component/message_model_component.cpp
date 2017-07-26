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
#include <boost/format.hpp>

#include "component/chat_component.h"
#include "component/message_model_component.h"
#include "component/endpoint_component.h"
#include "component/message_wrapper_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/client/client_option_component.h"

#include "controller/application_controller.h"
#include "controller/envelope_controller.h"
#include "controller/message_controller.h"

#include "model/envelope_model.h"
#include "model/message_model.h"
#include "thread/thread_info.h"
#include "utility/locale.h"
#include "utility/utf8.h"
#include "utility/client/client_util.h"

namespace eja
{
	// Constructor
	message_model_component::message_model_component(QObject* parent /*= nullptr*/) : model_component(parent)
	{
		// Callback
		add_callback(callback_action::add, [&](const entity::ptr entity) { add(entity); });
		add_callback(callback_action::clear, [&](const entity::ptr entity) { clear(); });
	}

	// Interface
	void message_model_component::init()
	{
		component::init();

		// Type
		set_type(callback_type::chat);
	}

	void message_model_component::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());
		
		// PM Only
		const auto chat = entity->get<chat_component>();
		if (!chat->has_client())
			return;

		const auto owner = get_entity();
		const auto app = application_controller::get();		
		bool empty = false;

		if (owner == app->get_entity())
		{
			const auto control = app->get_message_controller();
			const auto e = control->get_entity();			

			if (e && (e->get<endpoint_component>()->get_id() == chat->get_client()) /*&& !app->suspended()*/)
			{
				// Envelope
				empty = control->empty();

				// Active
				control->add(entity);

#if defined(Q_OS_ANDROID)				
				if (!control->visible())
				{
					const auto option = owner->get<client_option_component>();
					if (option->has_flash())
					{
						const auto endpoint = e->get<endpoint_component>();
						if (endpoint->has_name())
						{
							const auto str = boost::str(boost::format("%s: %s") % endpoint->get_name() % chat->get_text());
							app->notify(str);
						}
						else
						{
							app->notify(chat->get_text());
						}
					}					
				}
#endif
			}
			else
			{
				// Envelope
				const auto parent = entity->get_parent();				
				const auto chat_list = parent->get<chat_list_component>();
				empty = chat_list->empty();

#if defined(Q_OS_ANDROID)
				// Notification
				const auto option = owner->get<client_option_component>();
				if (option->has_flash())
				{
					const auto endpoint = parent->get<endpoint_component>();
					if (endpoint->has_name())
					{
						const auto str = boost::str(boost::format("%s: %s") % endpoint->get_name() % chat->get_text());
						app->notify(str);
					}
					else
					{
						app->notify(chat->get_text());
					}
				}				
#endif
				// Inactive								
				const auto message_comp = parent->get<message_wrapper_component>();
				const auto message_model = message_comp->get_model();
				message_model->add(entity);
			}

			// Envelope
			const auto parent = entity->get_parent();

			if (owner == parent)
			{
				const auto& client_id = chat->get_client();
				const auto e = client_util::get_client(owner, client_id);
				if (e)
				{
					const auto control = app->get_envelope_controller();
					const auto chat_list = e->get<chat_list_component>();

					if (empty)
					{
						control->add(e);
					} 
					else
					{
						const auto model = control->get_model();
						model->update(e);
					}
				}
			}
			else
			{
				const auto control = app->get_envelope_controller();
				const auto chat_list = parent->get<chat_list_component>();

				if (empty)
				{					
					control->add(parent);
				}
				else
				{
					// Update
					const auto model = control->get_model();
					model->update(parent);
				}
			}
		}
		else
		{
			// Empty
			const auto parent = entity->get_parent();
			const auto chat_list = parent->get<chat_list_component>();
			empty = chat_list->empty();

			// Inactive			
			const auto message_comp = parent->get<message_wrapper_component>();
			const auto message_model = message_comp->get_model();
			message_model->add(entity);

			if (empty)
			{
				// Envelope
				const auto index_list = owner->get<index_list_component>();
				index_list->push_back(parent);

				// Sort
				std::sort(index_list->begin(), index_list->end(), [](const entity::ptr e1, const entity::ptr e2)
				{
					const auto ec1 = e1->get<endpoint_component>();
					const auto ec2 = e2->get<endpoint_component>();
					return boost::algorithm::to_lower_copy(ec1->get_name()) < boost::algorithm::to_lower_copy(ec2->get_name());
				});
			}			

			// Envelope
			const auto control = app->get_envelope_controller();
			const auto model = control->get_model();
			model->update(parent);
		}
	}

	void message_model_component::on_clear()
	{
		assert(thread_info::main());

		const auto owner = get_entity();
		const auto app = application_controller::get();

		if (owner == app->get_entity())
		{
			// Active
			const auto app = application_controller::get();
			const auto control = app->get_message_controller();
			control->clear();
		}
		else
		{
			// Inactive
			const auto chat_list = owner->get<chat_list_component>();
			chat_list->clear();
		}
	}
}
