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
#include <list>
#include <regex>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>

#include "command/join_command.h"
#include "command/quit_command.h"

#include "component/browse_component.h"
#include "component/chat_component.h"
#include "component/endpoint_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/client/client_service_component.h"
#include "component/client/client_network_component.h"
#include "component/group/group_component.h"
#include "component/io/share_component.h"
#include "component/message/message_acceptor_component.h"
#include "component/message/message_network_component.h"
#include "component/message/message_service_component.h"
#include "component/session/session_component.h"
#include "component/status/status_component.h"
#include "component/transfer/transfer_service_component.h"

#include "data/client_data.h"
#include "data/router_data.h"
#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "message/request/request_message.h"
#include "message/request/join_request_message.h"
#include "message/response/response_message.h"
#include "message/response/join_response_message.h"
#include "security/security.h"
#include "security/filter/hex.h"
#include "utility/value.h"

namespace eja
{
	// Client
	join_request_message::ptr client_join_command::execute(const entity::ptr router)
	{
		const auto request = join_request_message::create();

		// Client
		auto endpoint = m_entity->get<endpoint_component>();		
		request->set_name(endpoint->get_name());

		// Group
		const auto group = m_entity->get<group_component>();
		request->set_group(group->get_id());

		// Color
		request->set_color(endpoint->get_color());

		// Options
		const auto network = m_entity->get<client_network_component>();
		request->set_browse(network->has_browse());
		request->set_chat(network->has_chat());
		request->set_pm(network->has_pm());
		request->set_search(network->has_search());
		request->set_share(network->has_share());
		request->set_upload(network->has_upload());

		// Id
		endpoint = router->get<endpoint_component>();
		request->set_client(endpoint->get_id());

		return request;
	}

	void client_join_command::execute(const entity::ptr router, const std::shared_ptr<join_response_message> response)
	{
		// Id
		if (response->has_client())
		{
			/*const*/ auto endpoint = router->get<endpoint_component>();
			endpoint->set_id(response->get_client());

			// HACK: Since we only have 1 router now, we can fake the endpoint ID
			endpoint = m_entity->get<endpoint_component>();
			endpoint->set_id(response->get_client());
		}

		// Session
		const auto session_map = m_entity->get<session_map_component>();
		{
			const auto session = router->get<session_component>();
			const auto pair = std::make_pair(session->get_id(), router);

			thread_lock(session_map);
			session_map->insert(pair);
		}

		// MOTD
		if (response->has_motd())
		{
			const auto network = m_entity->get<client_network_component>();
			if (network->has_motd())
			{
				const auto service = router->get<message_service_component>();
				if (!service->has_motd())
				{
					// Only display once
					service->set_motd(true);

					const auto e = entity_factory::create_client_chat(router);
					e->add<endpoint_component>(router);

					const auto chat = e->get<chat_component>();
					chat->set_text(response->get_motd());
					chat->set_type(chat_type::system);

					// Callback
					m_entity->call(callback_type::chat, callback_action::add, e);
				}
			}			
		}

		// Clients
		const auto& clients = response->get_clients();
		const auto client_map = router->get<client_map_component>();

		const auto cmap = client_map_component::create();
		const auto clist = client_list_component::create();
		clist->reserve(clients.size());

		const auto blist = browse_list_component::create();
		blist->reserve(clients.size());

		// Clients
		for (const auto& data : clients)
		{
			entity::ptr e;
			{
				thread_lock(client_map);
				const auto it = client_map->find(data->get_id());
				if (it != client_map->end())
					e = it->second;
			}

			if (!e)
				e = entity_factory::create_client_client(m_entity);

			// Browse
			const auto browse = e->get<browse_component>();
			browse->clear();

			if (data->has_share())				
				blist->push_back(e);

			// Client
			const auto endpoint = e->get<endpoint_component>();
			endpoint->set_id(data->get_id());
			endpoint->set_name(data->get_name());
			endpoint->set_color(data->get_color());

			const auto colopr = data->get_color();

			clist->push_back(e);

			const auto pair = std::make_pair(endpoint->get_id(), e);
			cmap->insert(pair);
		}
		
		// Copy
		set_list(router, blist);
		set_list(router, clist);
		set_map(router, cmap);

		// Callback
		m_entity->call(callback_type::client, callback_action::clear);
		
		if (!clist->empty())
		{
			const auto e = entity::create(m_entity, clist);
			m_entity->call(callback_type::client, callback_action::add, e);
		}			

		m_entity->call(callback_type::browse, callback_action::clear);

		if (!blist->empty())
		{
			const auto e = entity::create(m_entity, blist);
			m_entity->call(callback_type::browse, callback_action::add, e);
		}

		// Routers (Only if we trust them)
		const auto network = m_entity->get<client_network_component>();

		if (network->get_trust() == network_trust::router)
		{
			const auto& routers = response->get_routers();
			const auto service = m_entity->get<client_service_component>();
			const auto transfer_list = service->get_transfer_list();
			const auto tlist = transfer_list_component::create();

			for (const auto& data : routers)
			{
				const auto it = std::find_if(transfer_list->cbegin(), transfer_list->cend(), [data](const entity::ptr e)
				{
					const auto endpoint = e->get<endpoint_component>();
					return (endpoint->get_address() == data->get_address()) && (endpoint->get_port() == data->get_port());
				});

				if (it == transfer_list->cend())
				{
					const auto e = entity_factory::create_client_transfer(m_entity);
					e->set_state(entity_state::temporary);

					// Service
					const auto transfer_service = e->get<transfer_service_component>();
					transfer_service->set_threads(network->get_threads());

					// Router
					const auto endpoint = e->get<endpoint_component>();
					endpoint->set_name(data->get_name());
					endpoint->set_address(data->get_address());
					endpoint->set_port(data->get_port());

					tlist->push_back(e);					
				}				
			}

			if (!tlist->empty())
			{
				// Callback
				const auto e = entity::create(m_entity, tlist);
				m_entity->call(callback_type::transfer, callback_action::add, e);
			}
		}
	}

	// Router
	join_response_message::ptr router_join_command::execute(const entity::ptr client, const join_request_message::ptr request)
	{		
		// Request
		const auto endpoint = client->get<endpoint_component>();

		// Id
		if (request->has_client())
		{
			const auto& client_id = request->get_client();

			if (client_id != endpoint->get_id())
			{
				entity::ptr e;

				const auto client_map = m_entity->get<client_map_component>();
				{
					thread_lock(client_map);
					const auto it = client_map->find(client_id);
					if (it != client_map->end())
						e = it->second;
				}

				if (e)
				{
					// TODO: clear_command
					//
					//
					router_quit_command quit(m_entity);
					quit.clear(e);
					
					endpoint->set_id(client_id);
					const auto pair = std::make_pair(client_id, client);
					{
						thread_lock(client_map);
						client_map->insert(pair);
					}					

					m_entity->call(callback_type::client, callback_action::add, client);
				}
			}
		}

		// Response
		const auto response = join_response_message::create();

		// MOTD
		const auto message_network = m_entity->get<message_network_component>();
		if (message_network->has_motd())
			response->set_motd(message_network->get_motd());

		// Client		
		if (!endpoint->has_id())
		{
			while (true)
			{
				const auto client_id = security::get_id();
				const auto client_map = m_entity->get<client_map_component>();
				const auto pair = std::make_pair(client_id, client);
				response->set_client(client_id);
				
				thread_lock(client_map);
				const auto it = client_map->find(client_id);

				if (it == client_map->end())
				{
					endpoint->set_id(client_id);
					client_map->insert(pair);
					m_entity->call(callback_type::client, callback_action::add, client);

					break;
				}
			}
		}

		// Group
		const auto group = client->get<group_component>();
		const auto group_map = m_entity->get<group_map_component>();
		{
			const auto pair = std::make_pair(request->get_group(), client);

			thread_lock(group_map);
			auto range = group_map->equal_range(group->get_id());

			for (auto it = range.first; it != range.second; )
			{
				const auto& e = it->second;				

				if (client != e)
				{
					++it;
				}
				else
				{
					group_map->erase(it++);
					break;
				}
			}

			group->set_id(request->get_group());
			group_map->insert(pair);
		}

		// Anti-troll (add)
		if (request->has_name() && (request->get_name() != client::name))
		{
			entity_map_component::ptr entity_map;
			const auto troll_map = m_entity->get<group_troll_map_component>();
			{
				thread_lock(troll_map);
				const auto it = troll_map->find(group->get_id());

				if (it != troll_map->end())
				{
					entity_map = it->second;
				}
				else
				{
					entity_map = entity_map_component::create();
					const auto pair = std::make_pair(group->get_id(), entity_map);
					troll_map->insert(pair);
				}
			}
			
			// Strip all non-printable, non-ASCII characters
			auto name = request->get_name();
			name.erase(std::remove_if(name.begin(), name.end(), [](char ch) {return !(ch >= 33 && ch <= 126); }), name.end());
			{
				thread_lock(entity_map);
				const auto it = entity_map->find(name);

				if (it == entity_map->end())
				{
					const auto pair = std::make_pair(name, client);
					entity_map->insert(pair);

					endpoint->set_name(request->get_name());
					endpoint->set_color(request->get_color());
				}
				else
				{
					const auto& e = it->second;

					if (e == client)
					{
						endpoint->set_name(request->get_name());
						endpoint->set_color(request->get_color());
					}
					else
					{
						name = boost::str(boost::format("%s [%s]") % request->get_name() % hex::random(2));
						endpoint->set_name(name);
						endpoint->set_color(0);
					}
				}
			}			
		}
		else
		{
			endpoint->set_name(request->get_name());
			endpoint->set_color(request->get_color());
		}

		// Options
		const auto client_network = client->get<client_network_component>();
		client_network->set_browse(request->has_browse());
		client_network->set_chat(request->has_chat());
		client_network->set_pm(request->has_pm());
		client_network->set_search(request->has_search());
		client_network->set_share(request->has_share());
		client_network->set_upload(request->has_upload());

		// Clients
		std::list<entity::ptr> group_list;
		{
			thread_lock(group_map);
			const auto range = group_map->equal_range(group->get_id());
			std::transform(range.first, range.second, std::back_inserter(group_list), [](std::multimap<std::string, entity::ptr>::value_type& value) {return value.second; });
		}

		for (const auto& e : group_list)
		{
			/*if (client == e)
				continue;*/
			
			// Ghosting
			const auto network = e->get<client_network_component>();
			if (!network->has_browse())
				continue;

			// TODO: XOR
			//
			const auto cdata = client_data::create();

			// Info
			const auto client = e->get<endpoint_component>();
			cdata->set_id(client->get_id());
			cdata->set_name(client->get_name());
			cdata->set_color(client->get_color());

			// Option
			cdata->set_share(network->has_share());

			response->add(cdata);
		}

		// Routers
		const auto acceptor = m_entity->get<message_acceptor_component>();
		const auto transfer_list = acceptor->get_transfer_list();

		for (const auto& e : *transfer_list)
		{
			if (e->disabled())
				continue;

			const auto status = e->get<status_component>();
			if (status->is_error())
				continue;

			// Data			
			const auto router = e->get<endpoint_component>();

			const auto rdata = router_data::create();
			rdata->set_name(router->get_name());
			rdata->set_address(router->get_address());
			rdata->set_port(router->get_port());

			response->add(rdata);
		}

		return response;
	}
}
