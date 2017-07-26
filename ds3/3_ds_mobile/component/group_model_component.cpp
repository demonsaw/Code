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

#include <QTextDecoder>

#include "component/chat_component.h"
#include "component/endpoint_component.h"
#include "component/group_model_component.h"
#include "component/group_ui_component.h"
#include "component/message_wrapper_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"

#include "controller/application_controller.h"
#include "controller/group_controller.h"
#include "thread/thread_info.h"
#include "utility/locale.h"
#include "utility/utf8.h"
#include "utility/value.h"

namespace eja
{
	// Constructor
	group_model_component::group_model_component(QObject* parent /*= nullptr*/) : model_component(parent)
	{
		// Callback
		add_callback(callback_action::add, [&](const entity::ptr entity) { add(entity); });
		add_callback(callback_action::clear, [&](const entity::ptr entity) { clear(); });
	}

	// Interface
	void group_model_component::init()
	{
		component::init();

		set_type(callback_type::client);
	}

	void group_model_component::on_add(const entity::ptr entity)
	{
		assert(thread_info::main() && has_entity());

		const auto owner = get_entity();
		const auto app = application_controller::get();

		if ((owner == app->get_entity()) /*&& !app->suspended()*/)
		{
			// Active
			const auto control = app->get_group_controller();
			control->add(entity);
		}
		else
		{
			// Inactive
			const auto client_list = owner->get<client_list_component>();

			if (entity->has<client_list_component>())
			{				
				const auto list = entity->get<client_list_component>();
				client_list->reserve(client_list->size() + list->size());				
				client_list->insert(client_list->end(), list->begin(), list->end());

				// Sort
				std::sort(client_list->begin(), client_list->end(), [](const entity::ptr e1, const entity::ptr e2)
				{
					const auto ec1 = e1->get<endpoint_component>();
					const auto ec2 = e2->get<endpoint_component>();
					return boost::algorithm::to_lower_copy(ec1->get_name()) < boost::algorithm::to_lower_copy(ec2->get_name());
				});

				// Map
				const auto client_map = owner->get<client_map_component>();
				{
					thread_lock(client_map);

					for (const auto& e : *list)
					{
						// Model
						application_controller::set_initial(e);

						if (!e->has<message_wrapper_component>())
							e->add<message_wrapper_component>();

						const auto endpoint = e->get<endpoint_component>();
						const auto pair = std::make_pair(endpoint->get_id(), e);
						client_map->insert(pair);
					}
				}
			}
			else
			{
				// Model
				application_controller::set_initial(entity);

				if (!entity->has<message_wrapper_component>())
					entity->add<message_wrapper_component>();

				const auto client_map = owner->get<client_map_component>();
				{
					const auto endpoint = entity->get<endpoint_component>();
					const auto pair = std::make_pair(endpoint->get_id(), entity);

					thread_lock(client_map);
					client_map->insert(pair);
				}

				// 
				// NOTE: DO NOT ADD TO END
				//
				//client_list->push_back(entity);

				//// Sort
				//std::sort(client_list->begin(), client_list->end(), [](const entity::ptr e1, const entity::ptr e2)
				//{
				//	const auto ec1 = e1->get<endpoint_component>();
				//	const auto ec2 = e2->get<endpoint_component>();
				//	return boost::algorithm::to_lower_copy(ec1->get_name()) < boost::algorithm::to_lower_copy(ec2->get_name());
				//});
			}
		}
	}

	void group_model_component::on_clear()
	{
		assert(thread_info::main());

		const auto owner = get_entity();
		const auto app = application_controller::get();

		if (owner == app->get_entity())
		{
			// Active
			const auto app = application_controller::get();
			const auto control = app->get_group_controller();
			control->clear();
		}
		else
		{
			// Inactive
			const auto client_list = owner->get<client_list_component>();
			client_list->clear();
		}
	}
}
