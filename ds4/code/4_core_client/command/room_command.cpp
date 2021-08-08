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

#include "command/command_util.h"
#include "command/room_command.h"
#include "command/router_command.h"

#include "component/browse_component.h"
#include "component/room_component.h"
#include "component/callback/callback.h"
#include "component/callback/callback_service_component.h"
#include "component/client/client_component.h"
#include "component/group/group_component.h"
#include "component/message/message_component.h"

#include "data/client_data.h"
#include "data/room_data.h"
#include "data/request/room_request_data.h"
#include "data/response/room_response_data.h"
#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "entity/entity_util.h"
#include "factory/client_factory.h"
#include "utility/value.h"

namespace eja
{
	// Interface
	router_request_message::ptr room_command::execute() const
	{
		// Data
		const auto request_data = room_request_data::create();

		return execute(request_data);
	}

	router_request_message::ptr room_command::execute(const room_request_data::ptr& request_data) const
	{
		// Command
		router_command cmd(m_entity);
		return cmd.get_request_message(request_data);
	}

	router_request_message::ptr room_command::execute(const entity::ptr& entity, const room_request_data::ptr& request_data) const
	{
		// Message
		const auto message_entity = entity_factory::create_timeout(m_entity);
		message_entity->set_data(entity);

		//const auto room = entity->get<room_component>();
		const auto message_id = entity_util::add<message_map_component>(m_entity, message_entity);
		//room->set_id(message_id);

		// Command
		router_command cmd(m_entity);
		const auto request_message = cmd.get_request_message(request_data);
		request_message->set_id(message_id);

		return request_message;
	}

	void room_command::execute(const router_response_message::ptr& response_message, const room_response_data::ptr& response_data) const
	{
		// Validate
		if (unlikely(!validate(response_message, response_data)))
			return;

		if (!response_message->has_id())
		{
			// 1) Entity
			const auto e = entity::create(m_entity);
			const auto client = m_entity->get<client_component>();
			const auto entity_map = m_entity->get<entity_map_component>();

			// 2) Rooms
			const auto clist = client_list_component::create();
			e->add(clist);

			const auto group = m_entity->get<group_component>();
			if (!group->has_id())
			{
				const auto room_map = m_entity->get<room_map_component>();

				for (const auto& rdata : response_data->get_rooms())
				{
					if (rdata->invalid())
						continue;

					// Entity
					entity::ptr room_entity;
					{
						thread_lock(room_map);
						const auto it = room_map->find(rdata->get_id());
						if (it != room_map->end())
							room_entity = it->second;
					}

					// Component
					if (!room_entity)
					{
						room_entity = client_factory::create_room(m_entity);
						const auto r = room_entity->get<room_component>();
						r->set_id(rdata->get_id());
					}

					// Update
					command_util::update(room_entity, rdata);

					// List
					clist->push_back(room_entity);
				}

				// Map
				{
					thread_lock(room_map);
					room_map->clear();
				}

				for (const auto& e : *clist)
				{
					const auto r = e->get<room_component>();
					const auto pair = std::make_pair(r->get_id(), e);
					{
						thread_lock(room_map);
						const auto result = room_map->insert(pair);
						assert(result.second);
					}
				}
			}

			// 3) Browse
			const auto browse_map = m_entity->get<browse_map_component>();
			const auto blist = browse_list_component::create();
			e->add(blist);

			// 4) Clients
			const auto room_size = clist->size();
			const auto client_map = m_entity->get<client_map_component>();
			{
				const auto pair = std::make_pair(client->get_id(), m_entity);
				{
					thread_lock(client_map);
					client_map->insert(pair);
				}
			}

			for (const auto& cdata : response_data->get_clients())
			{
				if (cdata->invalid())
					continue;

				// Entity
				entity::ptr client_entity;
				{
					thread_lock(client_map);
					const auto it = client_map->find(cdata->get_id());
					if (it != client_map->end())
						client_entity = it->second;
				}

				// Component
				if (!client_entity)
				{
					client_entity = client_factory::create_client(m_entity);
					const auto c = client_entity->get<client_component>();
					c->set_id(cdata->get_id());

					// Add
					const auto pair = std::make_pair(c->get_id(), client_entity);
					{
						thread_lock(entity_map);
						const auto result = entity_map->insert(pair);
						if (!result.second)
						{
							const auto& it = result.first;
							it->second = client_entity;
						}
					}
				}

				// Update
				command_util::update(client_entity, cdata);

				// List
				clist->push_back(client_entity);

				if (cdata->is_share())
				{
					if (m_entity == client_entity)
					{
						client_entity = command_util::get_browse_entity(client_entity);
						blist->push_back(client_entity);
					}
					else
					{
						blist->push_back(client_entity);
					}
				}
			}

			// Map
			{
				thread_lock(client_map);
				client_map->clear();
			}

			for (auto it = clist->begin() + room_size; it != clist->end(); ++it)
			{
				const auto& e = *it;
				const auto c = e->get<client_component>();
				const auto pair = std::make_pair(c->get_id(), e);
				{
					thread_lock(client_map);
					const auto result = client_map->insert(pair);
					assert(result.second);
				}
			}

			// Map
			{
				thread_lock(browse_map);
				browse_map->clear();
			}

			for (const auto& e : *blist)
			{
				const auto c = e->get<client_component>();
				const auto pair = std::make_pair(c->get_id(), e);
				{
					thread_lock(browse_map);
					const auto result = browse_map->insert(pair);
					assert(result.second);
				}
			}

			// 5) Callback
			m_entity->call(callback::client | callback::clear);
			if (!clist->empty())
				m_entity->async_call(callback::client | callback::add, e);

			m_entity->call(callback::browse | callback::clear);
			if (!blist->empty())
				m_entity->async_call(callback::browse | callback::add, e);

			m_entity->async_call(callback::status | callback::update);
			m_entity->async_call(callback::chat | callback::update);
			m_entity->async_call(callback::pm | callback::update);
			m_entity->async_call(callback::room | callback::update);
		}
		else
		{
			// 1) Message
			entity::ptr message_entity;
			const auto message_map = m_entity->get<message_map_component>();
			{
				thread_lock(message_map);
				const auto it = message_map->find(response_message->get_id());
				if (it != message_map->end())
					message_entity = it->second;
			}

			if (message_entity)
			{
				// 2) Entity
				const auto& room_entity = message_entity->get_data();
				if (!room_entity)
					return;

				const auto e = entity::create(m_entity);
				e->set_parent(room_entity);

				// 3) Clients
				const auto clist = client_list_component::create();
				e->add(clist);

				const auto client_map = m_entity->get<client_map_component>();

				for (const auto& cdata : response_data->get_clients())
				{
					if (cdata->invalid())
						continue;

					// Find
					entity::ptr client_entity;
					{
						thread_lock(client_map);
						const auto it = client_map->find(cdata->get_id());
						if (it != client_map->end())
							client_entity = it->second;
					}

					if (client_entity)
					{
						// Update
						command_util::update(client_entity, cdata);

						// Data
						clist->push_back(client_entity);
					}
				}

				// 4) Room
				const auto room = room_entity->get<room_component>();
				room->set_size(clist->size());

				// 5) Callback
				m_entity->async_call(callback::client | callback::add, e);
			}
		}
	}

	// Utility
	bool room_command::validate(const router_response_message::ptr& response_message, const room_response_data::ptr& response_data) const
	{
		return response_data->valid();
	}
}
