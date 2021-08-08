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

#include "command/room_command.h"

#include "component/room_component.h"
#include "component/client/client_component.h"
#include "component/group/group_component.h"

#include "data/client_data.h"
#include "data/room_data.h"
#include "data/request/room_request_data.h"
#include "data/response/room_response_data.h"
#include "entity/entity.h"
#include "entity/entity_util.h"
#include "security/filter/exclusive.h"
#include "utility/value.h"

namespace eja
{
	// Interface
	room_response_data::ptr room_command::execute(const entity::ptr& entity, const router_request_message::ptr& request_message, const room_request_data::ptr& request_data) const
	{
		// Validate
		if (unlikely(!validate(request_message, request_data)))
			return nullptr;

		// Group
		const auto group_entity = get_group(entity);
		if (likely(group_entity))
		{
			switch (request_data->get_mode())
			{
				case room_request_data::browse:
				{
					if (likely(request_data->has_id()))
					{
						const auto room_entity = get_room(group_entity, request_data->get_id());
						if (unlikely(!room_entity))
							return nullptr;

						const auto clients = entity_util::transform<client_map_component>(room_entity);
						if (!clients.empty())
						{
							// Response
							const auto client = entity->get<client_component>();
							const auto response_data = room_response_data::create();

							// Clients
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

							return response_data;
						}
					}

					break;
				}
				case room_request_data::none:
				{
					const auto clients = entity_util::transform<client_map_component>(group_entity);
					if (!clients.empty())
					{
						// Response
						const auto client = entity->get<client_component>();
						const auto response_data = room_response_data::create();

						// Clients
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

						// Rooms
						const auto group = entity->get<group_component>();
						if (!group->has_id())
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

						return response_data;
					}

					break;
				}
				case room_request_data::join:
				{
					if (likely(request_data->has_id()))
					{
						const auto room_entity = get_room(group_entity, request_data->get_id());
						if (likely(room_entity))
						{
							const auto client_map = room_entity->get<client_map_component>();
							{
								const auto client = entity->get<client_component>();
								const auto pair = std::make_pair(client->get_id(), entity);

								thread_lock(client_map);
								const auto result = client_map->insert(pair);
								assert(result.second);
							}
						}
					}

					break;
				}
				case room_request_data::quit:
				{
					if (likely(request_data->has_id()))
					{
						const auto room_entity = get_room(group_entity, request_data->get_id());
						if (likely(room_entity))
						{
							const auto client_map = room_entity->get<client_map_component>();
							{
								const auto client = entity->get<client_component>();

								thread_lock(client_map);
								client_map->erase(client->get_id());
							}
						}
					}

					break;
				}
				default:
				{
					assert(false);
				}
			}
		}

		return nullptr;
	}

	// Utility
	bool room_command::validate(const router_request_message::ptr& request_message, const room_request_data::ptr& request_data) const
	{
		// Id
		const auto& id = request_data->get_id();
		if (id.size() > io::max_id_size)
			return false;

		return true;
	}
}
