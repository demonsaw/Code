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

#define _ENABLE_ATOMIC_ALIGNMENT_FIX

#include <algorithm>
#include <vector>

#include "command/group_command.h"
#include "component/mute_component.h"
#include "component/client/client_component.h"
#include "component/group/group_component.h"
#include "component/router/router_acceptor_component.h"

#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "entity/entity_util.h"
#include "factory/router_factory.h"
#include "message/request/client_request_message.h"
#include "security/filter/exclusive.h"
#include "security/filter/hex.h"

namespace eja
{
	// Interface
	void group_command::execute(const entity::ptr& entity, const group_request_message::ptr& request_message) const
	{
		// Validate
		if (!validate(entity, request_message))
			return;

		// Group
		const auto client = entity->get<client_component>();
		const auto group_entity = get_group(entity);

		if (likely(group_entity))
		{
			// Id
			if (request_message->has_id())
			{
				const auto message_map = group_entity->get<message_map_component>();
				{
					const auto e = entity_factory::create_timeout(entity);
					const auto pair = std::make_pair(request_message->get_id(), e);

					// NOTE: Allow reuse
					thread_lock(message_map);
					message_map->insert(pair);
				}
			}

			// Copy
			std::vector<entity::ptr> clients;

			if (!request_message->has_clients())
			{
				if (!request_message->has_room())
				{
					// Copy
					clients = entity_util::transform<client_map_component>(group_entity);
				}
				else
				{
					const auto room_entity = get_room(group_entity, request_message->get_room());
					if (likely(room_entity))
					{
						// Copy
						clients = entity_util::transform<client_map_component>(room_entity);

						// NOTE: Add if not in room
						const auto client_map = room_entity->get<client_map_component>();
						{
							thread_lock(client_map);
							const auto it = client_map->find(client->get_id());
							if (it == client_map->end())
							{
								const auto pair = std::make_pair(client->get_id(), entity);
								const auto result = client_map->insert(pair);
								assert(result.second);
							}
						}
					}
				}
			}
			else
			{
				const auto client_map = group_entity->get<client_map_component>();
				clients.reserve(request_message->get_clients().size());

				for (const auto& id : request_message->get_clients())
				{
					// XOR: Don't really need this
					//const auto idx = exclusive::compute(id, client->get_seed());

					thread_lock(client_map);
					const auto it = client_map->find(id);
					if (it != client_map->end())
					{
						const auto& e = it->second;
						clients.push_back(e);
					}
				}
			}

			// Remove
			if (!request_message->is_share())
			{
				// Muted
				clients.erase(std::remove_if(clients.begin(), clients.end(), [&](const entity::ptr e)
				{
					if (entity != e)
					{
						// Muted?
						const auto ms = e->get<mute_set_component>();
						{
							thread_lock(ms);
							return ms->find(e) != ms->end();
						}
					}

					// Send to self?
					return !request_message->is_self();

				}), clients.end());
			}
			else
			{
				// Muted or not sharing
				clients.erase(std::remove_if(clients.begin(), clients.end(), [&](const entity::ptr e)
				{
					// Sharing?
					const auto c = e->get<client_component>();
					if (!c->is_share())
						return true;

					if (entity != e)
					{
						// Muted?
						const auto ms = e->get<mute_set_component>();
						{
							thread_lock(ms);
							return ms->find(e) != ms->end();
						}
					}

					// Send to self?
					return !request_message->is_self();

				}), clients.end());
			}

			// Empty?
			if (!clients.empty())
			{
				// Request
				const auto client_request = client_request_message::create();
				client_request->set_id(request_message->get_id());
				client_request->set_data(request_message->get_data());
				client_request->set_room(request_message->get_room());
				client_request->set_origin(client->get_id());

				// Write
				const auto acceptor = m_entity->get<router_acceptor_component>();
				for (const auto& e : clients)
				{
					// XOR: Don't really need this
					//const auto client_request = client_request_message::create();
					//client_request->set_id(request_message->get_id());
					//client_request->set_data(request_message->get_data());
					//client_request->set_room(request_message->get_room());

					//const auto c = e->get<client_component>();
					///*const*/ auto idx = exclusive::compute(client->get_id(), c->get_seed());
					//client_request->set_origin(std::move(idx));

					acceptor->async_write(e, client_request);
				}
			}
		}
	}

	// Utility
	bool group_command::validate(const entity::ptr& entity, const group_request_message::ptr& request_message) const
	{
		// Id
		const auto& id = request_message->get_id();
		if (unlikely(id.size() > io::max_id_size))
			return false;

		// Clients
		for (const auto& client : request_message->get_clients())
		{
			if (unlikely(client.size() > security::get_max_size()))
				return false;
		}

		// Origin
		if (unlikely(request_message->has_origin()))
			return false;

		// Room
		const auto& room = request_message->get_room();
		if (unlikely(room.size() > io::max_id_size))
			return false;

		// Data
		if (request_message->has_data())
		{
			// Verified
			const auto client = entity->get<client_component>();
			if (client->is_verified())
				return true;

			// PM, Browse, Search, & Transfer (Don't spam)
			if (request_message->has_clients() || request_message->is_share())
				return true;

			// Group (Don't spam)
			const auto group = entity->get<group_component>();
			if (group->has_id())
				return true;
		}

		return false;
	}
}
