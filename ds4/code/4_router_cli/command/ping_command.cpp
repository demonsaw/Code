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

#include "command/ping_command.h"

#include "component/room_component.h"
#include "component/client/client_component.h"
#include "component/router/router_component.h"
#include "component/status/status_component.h"

#include "data/client_data.h"
#include "data/room_data.h"
#include "data/request/ping_request_data.h"
#include "data/response/ping_response_data.h"
#include "entity/entity.h"

namespace eja
{
	// Interface
	ping_response_data::ptr ping_command::execute(const entity::ptr& entity, const router_request_message::ptr& request_message, const ping_request_data::ptr& request_data) const
	{
		// Validate
		if (unlikely(!validate(request_message, request_data)))
			return nullptr;

		// Group
		const auto group_entity = get_group(entity);
		if (likely(group_entity))
		{
			const auto response_data = ping_response_data::create();

			// Clients
			if (request_data->is_client())
			{
				std::vector<entity::ptr> clients;
				const auto client_map = group_entity->get<client_map_component>();
				{
					thread_lock(client_map);
					clients.reserve(client_map->size());
					std::transform(client_map->begin(), client_map->end(), std::back_inserter(clients), [](client_map_component::value_type& pair) {return pair.second; });
				}

				if (likely(!clients.empty()))
				{
					for (const auto& e : clients)
					{
						// XOR: Don't really need this
						///*const*/ auto idx = exclusive::compute(c->get_id(), client->get_seed());
						//cdata->set_id(std::move(idx));

						// Client
						const auto cdata = client_data::create();
						const auto c = e->get<client_component>();
						cdata->set_id(c->get_id());

						cdata->set_name(c->get_name());
						cdata->set_color(c->get_color());

						cdata->set_share(c->is_share());
						cdata->set_troll(c->is_troll());
						cdata->set_verified(c->is_verified());

						response_data->add(cdata);
					}
				}
			}

			// Rooms
			if (request_data->is_room())
			{
				const auto room_map = group_entity->get<room_map_component>();

				for (const auto& pair : *room_map)
				{
					const auto& e = pair.second;
					const auto room = e->get<room_component>();

					const auto rdata = room_data::create();
					rdata->set_id(room->get_id());
					rdata->set_name(room->get_name());

					if (room->has_color())
						rdata->set_color(room->get_color());

					const auto client_map = e->get<client_map_component>();
					rdata->set_size(client_map->size());

					response_data->add(rdata);
				}
			}

			// Routers
			if (request_data->is_router())
			{
				const auto router_list = m_entity->get<router_list_component>();

				for (const auto& e : *router_list)
				{
					if (e->enabled())
					{
						const auto status = e->get<status_component>();
						if (status->is_success())
						{
							const auto router = e->get<router_component>();

							const auto rdata = router_data::create();
							rdata->set_name(router->get_name());
							rdata->set_address(router->get_address());
							rdata->set_password(router->get_password());
							rdata->set_port(router->get_port());

							response_data->add(rdata);
						}
					}
				}

				if (!response_data->has_routers())
				{
					// Set transfer router flag so that download/upload will work
					const auto router = m_entity->get<router_component>();
					router->set_transfer(true);

					const auto rdata = router_data::create();
					rdata->set_name(router->get_name());
					rdata->set_address(router->get_address());
					rdata->set_password(router->get_password());
					rdata->set_port(router->get_port());

					response_data->add(rdata);
				}
			}


			if (response_data->valid())
				return response_data;
		}

		return nullptr;
	}

	// Utility
	bool ping_command::validate(const router_request_message::ptr& request_message, const ping_request_data::ptr& request_data) const
	{
		return request_data->valid();
	}
}
