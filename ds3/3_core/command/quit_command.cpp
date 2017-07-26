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

#include "command/quit_command.h"
#include "component/endpoint_component.h"
#include "component/socket_component.h"
#include "component/tunnel_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/group/group_component.h"
#include "component/session/session_component.h"

#include "entity/entity.h"
#include "http/http_socket.h"
#include "message/request/request_message.h"
#include "message/response/response_message.h"
#include "utility/value.h"

namespace eja
{
	// Client
	quit_request_message::ptr client_quit_command::execute(const entity::ptr router)
	{
		return quit_request_message::create();
	}

	quit_response_message::ptr client_quit_command::execute(const entity::ptr router, const quit_request_message::ptr request)
	{
		return quit_response_message::create();
	}

	void client_quit_command::execute(const entity::ptr router, const quit_response_message::ptr response)
	{
		// Session
		const auto session = router->get<session_component>();
		const auto session_map = m_entity->get<session_map_component>();
		{
			thread_lock(session_map);
			session_map->erase(session->get_id());
		}
	}

	// Router
	quit_response_message::ptr router_quit_command::execute(const entity::ptr client, const quit_request_message::ptr request)
	{
		clear(client);

		return quit_response_message::create();
	}

	// Utility
	void router_quit_command::clear(const entity::ptr client)
	{
		// Session
		const auto session = client->get<session_component>();

		if (session->has_id())
		{
			const auto session_map = m_entity->get<session_map_component>();
			{
				thread_lock(session_map);
				session_map->erase(session->get_id());
			}
			
			m_entity->call(callback_type::session, callback_action::remove, client);
		}

		// Client
		const auto endpoint = client->get<endpoint_component>();

		if (endpoint->has_id())
		{
			const auto client_map = m_entity->get<client_map_component>();
			{
				thread_lock(client_map);
				client_map->erase(endpoint->get_id());
			}			
			m_entity->call(callback_type::client, callback_action::remove, client);
		}

		// Group
		const auto group = client->get<group_component>();
		const auto group_map = m_entity->get<group_map_component>();
		{
			thread_lock(group_map);
			auto range = group_map->equal_range(group->get_id());

			for (auto it = range.first; it != range.second; )
			{
				if (client == it->second)
				{
					group_map->erase(it++);
					break;
				}
				else
					++it;
			}
		}

		// Anti-troll
		if (endpoint->has_name() && (endpoint->get_name() != client::name))
		{
			const auto troll_map = m_entity->get<group_troll_map_component>();
			{
				thread_lock(troll_map);
				const auto it = troll_map->find(group->get_id());

				if (it != troll_map->end())
				{
					const auto entity_map = it->second;

					// Strip all non-printable, non-ASCII characters
					auto name = endpoint->get_name();
					name.erase(std::remove_if(name.begin(), name.end(), [](char ch) {return !(ch >= 33 && ch <= 126); }), name.end());

					thread_lock(entity_map);
					entity_map->erase(name);

					if (entity_map->empty())
						troll_map->erase(group->get_id());
				}
			}
		}		

		// Tunnels
		const auto tunnel_list = client->get<tunnel_list_component>();
		{
			thread_lock(tunnel_list);
			tunnel_list->clear();
		}
	}

}
