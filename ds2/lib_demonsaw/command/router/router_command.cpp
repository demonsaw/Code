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

#include "router_command.h"
#include "component/mutex_component.h"
#include "component/tunnel_component.h"
#include "json/json_type.h"
#include "system/function/function_action.h"
#include "system/function/function_type.h"

namespace eja
{
	// Client
	void router_command::remove_client(const std::string& id)
	{
		// Entity
		const auto entity_map = get_entity_map();
		if (entity_map)
		{
			const auto entity = entity_map->get(id);
			if (entity)
				remove_client(entity);
		}		
	}
	
	void router_command::remove_client(const entity::ptr entity)
	{
		// Mutex
		const auto mutex = entity->get<mutex_component>();
		auto_lock_ptr(mutex);

		// Entity
		const auto entity_map = get_entity_map();
		if (entity_map)
		{
			const std::string& entity_id = entity->get_id();
			entity_map->remove(entity_id);
		}			

		// Session
		const auto session = entity->get<session_component>();
		if (session)
		{
			const auto session_map = get_session_entity_map();
			if (session_map)
			{
				const std::string& session_id = session->get_id();
				session_map->remove(session_id);

				//call(function_type::session, function_action::remove, entity);
			}
		}

		// Client
		if (entity->has<client_component>())
		{
			const auto client = entity->get<client_component>();
			if (client)
			{
				const auto client_map = get_client_entity_map();
				if (client_map)
				{
					const std::string& client_id = client->get_id();
					client_map->remove(client_id);

					call(function_type::client, function_action::remove, entity);
				}
			}
		}

		// Tunnel
		if (entity->has<tunnel_component>())
		{
			const auto tunnel = entity->get<tunnel_component>();
			if (tunnel)
			{
				const auto socket = tunnel->get_socket();
				if (socket)
					socket->close();
			}
		}

		// Group
		if (entity->has<group_component>())
		{
			const auto group = entity->get<group_component>();
			if (group)
			{
				const auto group_map = get_group_entity_map();
				if (group_map)
				{
					const std::string& group_id = group->get_id();
					const auto group_entity = group_map->get(group_id);
					if (group_entity)
					{
						const auto group_entity_map = group_entity->get<entity_map_component>();
						if (group_entity_map)
						{							
							group_entity_map->remove(entity->get_id());
							function_action group_action = function_action::refresh;

							{
								// Mutex
								const auto mutex = get_mutex();
								auto_lock_ptr(mutex);

								if (group_entity_map->empty())
								{
									group_map->remove(group_id);
									group_action = function_action::remove;
								}
							}

							call(function_type::group, group_action, group_entity);
						}
					}
				}
			}
		}
	}

	// Session
	void router_command::remove_session(const std::string& id)
	{
		// Entity
		const auto session_map = get_session_entity_map();
		if (session_map)
			session_map->remove(id);
	}

	void router_command::remove_session(const entity::ptr entity)
	{
		assert(entity);

		const auto session = entity->get<session_component>();
		if (session)
		{
			const auto session_map = get_session_entity_map();
			if (session_map)
			{
				const std::string& session_id = session->get_id();
				session_map->remove(session_id);

				//call(function_type::session, function_action::remove, entity);
			}
		}
	}

	// Spam
	void router_command::remove_spam(const std::string& id)
	{
		// Entity
		const auto spam_map = get_spam_entity_map();
		if (spam_map)
			spam_map->remove(id);
	}

	void router_command::remove_spam(const entity::ptr entity)
	{
		assert(entity);

		const auto spam_map = get_spam_entity_map();
		if (spam_map)
		{
			const auto client = entity->get<client_component>();
			if (client)
				spam_map->remove(client->get_id());
		}
	}

	// Transfer
	void router_command::remove_transfer(const std::string& id)
	{
		// Entity
		const auto entity_map = get_entity_map();
		if (entity_map)
		{
			const auto entity = entity_map->get(id);
			if (entity)
				remove_transfer(entity);
		}
	}

	void router_command::remove_transfer(const entity::ptr entity)
	{
		assert(entity);
		
		// Chunk
		const auto chunk_map = get_chunk_entity_map();
		if (chunk_map)
		{
			const auto& id = entity->get_id();
			chunk_map->remove(id);

			call(function_type::transfer, function_action::remove, entity);
		}
	}

	// Accessors
	json_data::ptr router_command::get_data(const json_data::ptr data)
	{
		if (!data->empty())
		{
			// Request
			const auto json = data->get();

			// Response
			const auto response_data = json_data::create(json);

			return response_data;
		}

		return json_data::create();
	}

	json_data::ptr router_command::get_data(const json_packet::ptr packet)
	{
		const auto data = json_data::create(packet);

		return get_data(data);
	}
}
