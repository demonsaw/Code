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

#include <algorithm>
#include <vector>
#include <boost/format.hpp>

#include "command/join_command.h"
#include "command/router_command.h"
#include "command/info_command.h"
#include "command/troll_command.h"

#include "component/chat_component.h"
#include "component/room_component.h"
#include "component/verified_component.h"
#include "component/client/client_component.h"
#include "component/group/group_component.h"
#include "component/io/share_component.h"
#include "component/router/router_component.h"
#include "component/router/router_acceptor_component.h"
#include "component/session/session_component.h"
#include "component/status/status_component.h"

#include "data/client_data.h"
#include "data/room_data.h"
#include "data/router_data.h"
#include "data/request/join_request_data.h"
#include "data/response/join_response_data.h"
#include "data/response/info_response_data.h"

#include "entity/entity.h"
#include "exception/not_implemented_exception.h"
#include "factory/router_factory.h"
#include "security/security.h"
#include "security/checksum/xxhash.h"
#include "security/filter/exclusive.h"
#include "security/filter/hex.h"
#include "utility/value.h"
#include "window/main_window.h"

namespace eja
{
	// Interface
	join_response_data::ptr join_command::execute(const entity::ptr& entity, const router_request_message::ptr& request_message, const join_request_data::ptr& request_data) const
	{
		// Validate
		if (unlikely(!validate(entity, request_message, request_data)))
			return nullptr;

		// 0) Variables
		const auto client = entity->get<client_component>();
		const auto group = entity->get<group_component>();

		// 1) Group (old)
		const auto group_map = m_entity->get<group_map_component>();

		if (group->get_id() != request_data->get_group())
		{
			entity::ptr group_entity;
			{
				thread_lock(group_map);

				const auto it = group_map->find(group->get_id());
				if (it != group_map->end())
					group_entity = it->second;
			}

			if (group_entity)
			{
				size_t removed;
				const auto client_map = group_entity->get<client_map_component>();
				{
					thread_lock(client_map);
					removed = client_map->erase(client->get_id());
				}

				// Anti-troll
				if (removed)
				{
					troll_command cmd(m_entity);
					cmd.remove(entity);
				}
			}
		}

		// 2) Client
		client->set_name(request_data->get_name());
		client->set_color(request_data->get_color());

		client->set_share(request_data->is_share());
		client->set_troll(false);
		client->set_verified(false);

		const auto& app = main_window::get_app();

		// Password
		if (request_data->has_password())
		{
			const auto& passphrase = request_data->get_password();
			const auto verified_map = m_entity->get<verified_map_component>();
			{
				thread_lock(verified_map);

				const auto it = verified_map->find(client->get_name());
				if (it != verified_map->end())
					client->set_verified(passphrase == it->second);
			}
		}

		// 3) Group (new)
		group->set_id(request_data->get_group());

		entity::ptr group_entity;
		{
			thread_lock(group_map);

			const auto it = group_map->find(group->get_id());
			if (it != group_map->end())
			{
				group_entity = it->second;
			}
			else
			{
				group_entity = router_factory::create_group(m_entity);
				const auto pair = std::make_pair(group->get_id(), group_entity);
				group_map->insert(pair);
			}
		}

		// 4) Anti-troll
		troll_command cmd(m_entity);
		cmd.add(entity);

		// 5) Response
		const auto response_data = join_response_data::create();
		response_data->set_id(client->get_id());

		const auto router = m_entity->get<router_component>();
		response_data->set_motd(router->get_motd());

		// XOR: Don't really need this
		///*const*/ auto idx = exclusive::compute(client->get_id(), client->get_seed());
		//response_data->set_id(std::move(idx));

		// 6) Clients
		std::vector<entity::ptr> clients;
		{
			const auto client_map = group_entity->get<client_map_component>();
			const auto pair = std::make_pair(client->get_id(), entity);

			thread_lock(client_map);

			const auto result = client_map->insert(pair);
			if (!result.second)
			{
				// Replace
				const auto it = result.first;
				it->second = entity;
			}

			// Copy
			clients.reserve(client_map->size());
			std::transform(client_map->begin(), client_map->end(), std::back_inserter(clients), [](client_map_component::value_type& pair) {return pair.second; });
		}

		if (likely(!clients.empty()))
		{
			// Status
			info_command cmd(entity);
			const auto response_message = cmd.execute();
			const auto acceptor = m_entity->get<router_acceptor_component>();

			for (const auto& e : clients)
			{
				// Status
				if (e != entity)
					acceptor->add_message(e, response_message);

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

		// 7) Routers
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

		// 8) Rooms
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

	// Utility
	bool join_command::validate(const entity::ptr& entity, const router_request_message::ptr& request_message, const join_request_data::ptr& request_data) const
	{
		// Data
		if (request_data->invalid())
			throw std::invalid_argument("Invalid request (join)");

		// Name
		const auto& name = request_data->get_name();
		if (name.size() > io::max_name_size)
			throw std::invalid_argument("Invalid name (size)");

		// Group
		const auto& group = request_data->get_group();
		if (group.size() > io::max_id_size)
			throw std::invalid_argument("Invalid group (size)");

		// Passphrase
		const auto& passphrase = request_data->get_password();
		if (passphrase.size() > io::max_passphrase_size)
			throw std::invalid_argument("Invalid passphrase (size)");

		return true;
	}
}
