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
#include "command/ping_command.h"
#include "command/router_command.h"

#include "component/room_component.h"
#include "component/callback/callback.h"
#include "component/client/client_component.h"
#include "component/group/group_component.h"
#include "component/router/router_component.h"
#include "component/transfer/transfer_component.h"

#include "data/client_data.h"
#include "data/room_data.h"
#include "data/request/ping_request_data.h"
#include "data/response/ping_response_data.h"
#include "factory/client_factory.h"
#include "utility/value.h"

namespace eja
{
	// Interface
	router_request_message::ptr ping_command::execute() const
	{
		// Data
		const auto request_data = ping_request_data::create();
		request_data->set_client(true);
		request_data->set_room(true);
		request_data->set_router(true);

		// Command
		router_command cmd(m_entity);
		return cmd.get_request_message(request_data);
	}

	void ping_command::execute(const router_response_message::ptr& response_message, const ping_response_data::ptr& response_data) const
	{
		// Validate
		if (unlikely(!validate(response_message, response_data)))
			return;

		// 1) Rooms
		if (response_data->has_rooms())
		{
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

					// Update
					if (room_entity)
					{
						command_util::update(room_entity, rdata);

						// Callback
						m_entity->async_call(callback::client | callback::update, room_entity);
					}
				}
			}
		}

		// 2) Routers
		if (response_data->has_routers())
		{
			const auto transfer_map = m_entity->get<transfer_map_component>();
			const auto tlist = transfer_list_component::create();

			for (const auto& rdata : response_data->get_routers())
			{
				if (rdata->invalid())
					continue;

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
		}

		// 3) Clients
		if (response_data->has_clients())
		{
			// Copy
			std::vector<entity::ptr> clist;
			client_map_component::ptr cmap;
			const auto client_map = m_entity->get<client_map_component>();
			{
				thread_lock(client_map);
				cmap = client_map->copy();
			}

			// Self
			const auto client = m_entity->get<client_component>();
			cmap->erase(client->get_id());

			for (const auto& cdata : response_data->get_clients())
			{
				if (cdata->invalid())
					continue;

				// Entity
				entity::ptr client_entity;
				const auto it = cmap->find(cdata->get_id());
				if (it != cmap->end())
				{
					client_entity = it->second;

					// Update
					const auto c = client_entity->get<client_component>();
					if (c->is_share() != cdata->is_share())
					{
						// Callback
						const auto action = cdata->is_share() ? callback::add : callback::remove;
						m_entity->async_call(callback::browse | action, client_entity);
					}

					cmap->erase(it);
				}
				else
				{
					// Self
					if (cdata->get_id() == client->get_id())
					{
						command_util::update(m_entity, cdata);
						m_entity->async_call(callback::status | callback::update);
					}

					client_entity = client_factory::create_client(m_entity);
					const auto c = client_entity->get<client_component>();
					c->set_id(cdata->get_id());

					clist.push_back(client_entity);
				}

				// Update
				command_util::update(client_entity, cdata);
			}

			for (const auto& pair : *cmap)
			{
				const auto& e = pair.second;

				// Callback
				m_entity->async_call(callback::client | callback::remove, e);

				const auto c = e->get<client_component>();
				if (c->is_share())
					m_entity->async_call(callback::browse | callback::remove, e);
			}

			for (const auto& e : clist)
			{
				// Callback
				m_entity->async_call(callback::client | callback::add, e);

				const auto c = e->get<client_component>();
				if (c->is_share())
					m_entity->async_call(callback::browse | callback::add, e);
			}
		}
	}

	// Utility
	bool ping_command::validate(const router_response_message::ptr& response_message, const ping_response_data::ptr& response_data) const
	{
		return response_data->valid();
	}

}
