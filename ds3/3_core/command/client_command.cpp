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

#include "command/client_command.h"
#include "component/browse_component.h"
#include "component/endpoint_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/client/client_network_component.h"
#include "component/group/group_component.h"
#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "message/request/request_message.h"
#include "message/response/response_message.h"

namespace eja
{
	// Client
	client_request_message::ptr client_client_command::execute(const entity::ptr router)
	{
		return client_request_message::create();
	}

	void client_client_command::execute(const entity::ptr router, const std::shared_ptr<client_response_message> response)
	{
		// Clients
		const auto& clients = response->get_clients();
		const auto client_map = router->get<client_map_component>();
		
		const auto cmap = client_map_component::create();
		const auto clist = client_list_component::create();
		clist->reserve(clients.size());

		const auto blist = browse_list_component::create();
		blist->reserve(clients.size());

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

			clist->push_back(e);

			const auto pair = std::make_pair(endpoint->get_id(), e);
			cmap->insert(pair);
		}

#if _ANDROID || DEBUG
		std::sort(clist->begin(), clist->end(), [](const entity::ptr e1, const entity::ptr e2)
		{
			const auto ec1 = e1->get<endpoint_component>();
			const auto ec2 = e2->get<endpoint_component>();
			return boost::algorithm::to_lower_copy(ec1->get_name()) < boost::algorithm::to_lower_copy(ec2->get_name());
		});
#endif
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
	}
	
	// Router
	client_response_message::ptr router_client_command::execute(const entity::ptr client, const client_request_message::ptr request)
	{
		// Response
		const auto response = client_response_message::create();

		// Clients
		std::list<entity::ptr> group_list;		
		{
			const auto group = client->get<group_component>();
			const auto group_map = m_entity->get<group_map_component>();

			thread_lock(group_map);
			const auto range = group_map->equal_range(group->get_id());
			std::transform(range.first, range.second, std::back_inserter(group_list), [](std::multimap<std::string, entity::ptr>::value_type& value) {return value.second; });
		}

		for (const auto& e : group_list)
		{
			// Self?
			/*if (client != e)
				continue;*/

			// Ghosting
			const auto network = e->get<client_network_component>();
			if (!network->has_browse())
				continue;

			const auto cdata = client_data::create();

			// Info
			const auto endpoint = e->get<endpoint_component>();
			cdata->set_id(endpoint->get_id());
			cdata->set_name(endpoint->get_name());
			cdata->set_color(endpoint->get_color());

			// Option
			cdata->set_share(network->has_share());

			response->add(cdata);
		}

		return response;
	}
}
