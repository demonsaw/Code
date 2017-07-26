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

#include "component/chat_component.h"
#include "component/chat_model_component.h"
#include "component/endpoint_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"

#include "controller/application_controller.h"
#include "controller/chat_controller.h"
#include "thread/thread_info.h"
#include "utility/locale.h"
#include "utility/utf8.h"

namespace eja
{
	// Constructor
	chat_model_component::chat_model_component(QObject* parent /*= nullptr*/) : model_component(parent)
	{
		// Callback
		add_callback(callback_action::add, [&](const entity::ptr entity) { add(entity); });
		add_callback(callback_action::clear, [&](const entity::ptr entity) { clear(); });
	}

	// Interface
	void chat_model_component::init()
	{
		component::init();

		set_type(callback_type::chat);
	}

	void chat_model_component::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		// Chat only
		const auto chat = entity->get<chat_component>();
		if (chat->has_client())
			return;

		const auto owner = get_entity();
		const auto app = application_controller::get();

		if ((owner == app->get_entity()) /*&& !app->suspended()*/)
		{
			// Active
			const auto control = app->get_chat_controller();
			control->add(entity);
		}
		else
		{
			// Inactive
			const auto parent = entity->get_parent();
			const auto grandparent = parent->get_parent();
			assert(grandparent);

			if (grandparent)
			{
				const auto chat_list = grandparent->get<chat_list_component>();

				if (!chat_list->empty())
				{
					const auto e = chat_list->front();
					if (e->get_parent() == entity->get_parent())
					{
						// Same
						const auto c = e->get<chat_component>();
						auto& text = c->get_text();
						text += "\n";

						const auto chat = entity->get<chat_component>();
						text += chat->get_text();
					}
					else
					{
						chat_list->push_front(entity);
					}
				}
				else
				{
					chat_list->push_front(entity);
				}
			}
		}
	}

	void chat_model_component::on_clear()
	{
		assert(thread_info::main());

		const auto owner = get_entity();
		const auto app = application_controller::get();

		if (owner == app->get_entity())
		{
			// Active
			const auto app = application_controller::get();
			const auto control = app->get_chat_controller();
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
