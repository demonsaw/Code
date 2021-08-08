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
#include "command/join_command.h"

#include "component/browse_component.h"
#include "component/chat_component.h"
#include "component/room_component.h"
#include "component/callback/callback.h"
#include "component/callback/callback_service_component.h"
#include "component/client/client_component.h"
#include "component/group/group_component.h"
#include "component/router/router_component.h"
#include "component/session/session_component.h"
#include "component/transfer/transfer_component.h"
#include "component/verified/verified_option_component.h"

#include "data/data_packer.h"
#include "data/request/join_request_data.h"
#include "data/request/tunnel_request_data.h"
#include "data/response/join_response_data.h"
#include "entity/entity.h"
#include "factory/client_factory.h"
#include "security/filter/hex.h"
#include "security/hash/sha.h"
#include "utility/value.h"

namespace eja
{
	// Interface
	router_request_message::ptr join_command::execute() const
	{
		// Packer
		data_packer packer;

		// Join
		{
			// Data
			const auto request_data = join_request_data::create();

			// Client
			const auto client = m_entity->get<client_component>();
			request_data->set_name(client->get_name());
			request_data->set_color(client->get_color());
			request_data->set_share(client->is_share());

			// Hash
			const auto verified_option = m_entity->get<verified_option_component>();
			if (verified_option->valid())
			{
				const auto pbkdf = pbkdf::create(verified_option->get_hash());
				pbkdf->set_salt(verified_option->get_salt());
				pbkdf->set_iterations(verified_option->get_iterations());

				const auto binary = pbkdf->compute(verified_option->get_key(), verified_option->get_key_size());
				const auto hex = hex::encode(binary);
				request_data->set_password(hex);
			}

			// Group
			const auto group = m_entity->get<group_component>();
			request_data->set_group(group->get_id());

			packer.push_back(request_data);
		}

		// TODO: If we want to tunnel...
		//
		//
		// Tunnel
		{
			// Data
			const auto request_data = tunnel_request_data::create();

			packer.push_back(request_data);
		}

		// Message
		const auto request_message = router_request_message::create();
		/*const*/ auto data = packer.pack();
		request_message->set_data(std::move(data));

		return request_message;
	}

	void join_command::execute(const router_response_message::ptr& response_message, const join_response_data::ptr& response_data) const
	{
		// Validate
		if (unlikely(!validate(response_message, response_data)))
			return;

		// 1) Entity
		const auto e = entity::create(m_entity);
		const auto client = m_entity->get<client_component>();
		const auto entity_map = m_entity->get<entity_map_component>();

		if (client->get_id() != response_data->get_id())
		{
			client->set_id(response_data->get_id());
			const auto pair = std::make_pair(client->get_id(), m_entity);
			{
				thread_lock(entity_map);
				entity_map->insert(pair);
			}
		}

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
		const auto browse_map = m_entity->get<browse_map_component>();
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

		// 6) Routers
		const auto transfer_map = m_entity->get<transfer_map_component>();
		const auto tlist = transfer_list_component::create();

		for (const auto& rdata : response_data->get_routers())
		{
			if (rdata->invalid())
				return;

			// Find
			entity::ptr router_entity;
			const auto router_id = rdata->get_endpoint();
			{
				thread_lock(transfer_map);
				const auto it = transfer_map->find(router_id);
				if (it != transfer_map->end())
					router_entity = it->second;
			}

			if (!router_entity)
			{
				router_entity = client_factory::create_router(m_entity);
				const auto r = router_entity->get<router_component>();
				r->set_id(router_id);
			}

			// Update
			command_util::update(router_entity, rdata);

			tlist->push_back(router_entity);
		}

		// Map
		{
			thread_lock(transfer_map);
			transfer_map->clear();

			for (const auto& e : *tlist)
			{
				const auto r = e->get<router_component>();

				const auto pair = std::make_pair(r->get_id(), e);
				const auto result = transfer_map->insert(pair);
				assert(result.second);
			}
		}

		// List
		{
			const auto transfer_list = m_entity->get<transfer_list_component>();

			thread_lock(transfer_list);
			transfer_list->swap(*tlist);
		}

		// 7) MOTD
		if (response_data->has_motd() && client->is_motd() && !client->is_join())
		{
			client->set_join(true);

			// Chat
			const auto e = client_factory::create_motd(m_entity);
			const auto chat = e->get<chat_component>();
			chat->set_text(std::move(response_data->get_motd()));

			// Callback
			m_entity->async_call(callback::chat | callback::add, e);
		}
	}

	// Utility
	bool join_command::validate(const router_response_message::ptr& response_message, const join_response_data::ptr& response_data) const
	{
		return response_data->valid();
	}
}
