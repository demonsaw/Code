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

#include <thread>
#include <boost/format.hpp>

#include "router_component.h"
#include "router_idle_component.h"
#include "router_option_component.h"
#include "component/session_component.h"
#include "component/spam_component.h"
#include "component/timeout_component.h"
#include "component/client/client_component.h"
#include "component/group/group_component.h"
#include "component/transfer/chunk_component.h"
#include "entity/entity.h"
#include "utility/default_value.h"

namespace eja
{
	// Callback
	bool router_idle_component::on_run()
	{		
		on_client();
		on_spam();
		on_transfer();

		return idle_component::on_run();
	}

	// Client
	void router_idle_component::on_client()
	{
		try
		{
			const auto owner = get_entity();
			const auto option = owner->get<router_option_component>();
			if (!option)
				return;

			const auto session_map = owner->get<session_entity_map_component>();
			if (!session_map)
				return;

			for (const auto& entity : session_map->get_list())
			{
				const auto timeout = entity->get<timeout_component>();				
				if (timeout && timeout->expired(option->get_client_timeout()))
				{
					if (entity->has<client_component>())
						remove_client(entity);
					else
						remove_transfer(entity);
				}
			}			
		}
		catch (std::exception& e)
		{
			log(e);
		}
		catch (...)
		{
			log();
		}
	}

	void router_idle_component::remove_client(const entity::ptr entity)
	{
		try
		{
			// Entity
			const auto owner = get_entity();
			const auto entity_map = owner->get<entity_map_component>();
			if (entity_map)
			{
				const std::string& entity_id = entity->get_id();
				entity_map->remove(entity_id);
			}

			// Session
			const auto session_map = owner->get<session_entity_map_component>();
			if (session_map)
			{
				const auto session = entity->get<session_component>();
				if (session)
				{
					const std::string& session_id = session->get_id();
					session_map->remove(session_id);
				}
			}

			// Client
			const auto client_map = owner->get<client_entity_map_component>();
			if (client_map)
			{
				const auto client = entity->get<client_component>();
				if (client)
				{
					const std::string& client_id = client->get_id();
					client_map->remove(client_id);

					call(function_type::client, function_action::remove, entity);
				}
			}

			// Group
			const auto group_map = owner->get<group_entity_map_component>();
			if (group_map)
			{
				const auto group = entity->get<group_component>();
				if (group)
				{
					const std::string& group_id = group->get_id();
					const auto group_entity = group_map->get(group_id);
					if (group_entity)
					{
						const auto entity_map = group_entity->get<entity_map_component>();
						if (entity_map)
						{
							entity_map->remove(entity->get_id());

							if (entity_map->empty())
							{
								group_map->remove(group_id);
								call(function_type::group, function_action::remove, group_entity);
							}
							else
							{
								call(function_type::group, function_action::refresh, group_entity);
							}
						}
					}
				}
			}

			// Spam
			remove_spam(entity);
		}
		catch (std::exception& e)
		{
			log(e);
		}
		catch (...)
		{
			log();
		}
	}

	// Spam
	void router_idle_component::on_spam()
	{
		try
		{
			const auto owner = get_entity();
			const auto option = owner->get<router_option_component>();
			if (!option)
				return;

			const auto spam_map = owner->get<spam_entity_map_component>();
			if (!spam_map)
				return;

			for (const auto& entity : spam_map->get_list())
			{
				// Spam timeout
				const auto spam_timeout = entity->get<spam_timeout_component>();
				if (spam_timeout && spam_timeout->expired(option->get_spam_timeout()))
					remove_spam(entity);
			}
		}
		catch (std::exception& e)
		{
			log(e);
		}
		catch (...)
		{
			log();
		}
	}

	void router_idle_component::remove_spam(const entity::ptr entity)
	{
		try
		{
			// Chunk
			const auto owner = get_entity();
			const auto spam_map = owner->get<spam_entity_map_component>();
			if (spam_map)
			{
				const auto client = entity->get<client_component>();
				if (client)
					spam_map->remove(client->get_id());
			}
		}
		catch (std::exception& e)
		{
			log(e);
		}
		catch (...)
		{
			log();
		}
	}

	// Transfer
	void router_idle_component::on_transfer()
	{
		try
		{
			const auto owner = get_entity();
			const auto option = owner->get<router_option_component>();
			if (!option)
				return;

			const auto chunk_map = owner->get<chunk_entity_map_component>();
			if (!chunk_map)
				return;

			for (const auto& entity : chunk_map->get_list())
			{
				// Transfer timeout (>= 1 chunks)
				const auto timeout = entity->get<timeout_component>();
				if (timeout && timeout->expired(option->get_transfer_timeout()))
				{
					// Queue timeout (0 chunks)
					const auto download_queue = entity->get<chunk_download_queue_component>();
					if (download_queue && !download_queue->get_chunk_size() && !timeout->expired(option->get_queue_timeout()))
						continue;

					remove_transfer(entity);
				}
			}
		}
		catch (std::exception& e)
		{
			log(e);
		}
		catch (...)
		{
			log();
		}
	}

	void router_idle_component::remove_transfer(const entity::ptr entity)
	{
		try
		{
			// Chunk
			const auto owner = get_entity();
			const auto chunk_map = owner->get<chunk_entity_map_component>();
			if (chunk_map)
			{
				const auto& entity_id = entity->get_id();
				chunk_map->remove(entity_id);

				call(function_type::transfer, function_action::remove, entity);
			}
		}
		catch (std::exception& e)
		{
			log(e);
		}
		catch (...)
		{
			log();
		}
	}
}
