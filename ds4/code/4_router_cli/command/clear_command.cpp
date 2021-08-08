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

#include "command/clear_command.h"
#include "command/troll_command.h"

#include "component/room_component.h"
#include "component/service/socket_component.h"
#include "component/client/client_component.h"
#include "component/group/group_component.h"
#include "component/router/router_component.h"
#include "component/session/session_component.h"
#include "component/transfer/chunk_component.h"
#include "component/transfer/transfer_component.h"

#include "entity/entity.h"

namespace eja
{
	// Interface
	void clear_command::execute(const entity::ptr& entity) const
	{
		// Session
		const auto session = entity->get<session_component>();
		if (session->has_id())
		{
			const auto& session_id = session->get_id();
			const auto session_map = m_entity->get<session_map_component>();
			{
				thread_lock(session_map);
				session_map->erase(session_id);
			}
		}		

		// Entity
		if (entity->has_id())
		{
			const auto& entity_id = entity->get_id();
			const auto entity_map = m_entity->get<entity_map_component>();
			{
				thread_lock(entity_map);
				entity_map->erase(entity_id);
			}
		}

		// Group
		const auto group_entity = get_group(entity);
		if (group_entity)
		{
			// Client
			const auto client = entity->get<client_component>();
			if (client->has_id())
			{
				size_t removed;
				const auto& client_id = client->get_id();
				const auto client_map = group_entity->get<client_map_component>();
				{
					thread_lock(client_map);
					removed = client_map->erase(client_id);
				}

				// Anti-troll
				if (removed)
				{
					troll_command cmd(m_entity);
					cmd.remove(entity);
				}

				// Rooms
				const auto room_map = group_entity->get<room_map_component>();
				for (const auto& pair : *room_map)
				{
					const auto& e = pair.second;
					const auto client_map = e->get<client_map_component>();
					{
						thread_lock(client_map);
						client_map->erase(client_id);
					}
				}
			}
		}

		// Socket
		const auto socket_set = entity->get<socket_set_component>();
		{
			thread_lock(socket_set);
			socket_set->clear();
		}
	}
}
