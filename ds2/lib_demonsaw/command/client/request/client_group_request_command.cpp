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

#include <boost/algorithm/string.hpp> 
#include <boost/format.hpp>

#include "client_group_request_command.h"
#include "component/session_component.h"
#include "component/client/client_component.h"
#include "json/json_packet.h"
#include "message/request/group_request_message.h"
#include "message/response/group_response_message.h"
#include "security/base.h"
#include "utility/std.h"

namespace eja
{
	// Utility
	http_status client_group_request_command::execute()
	{
		// Request		
		const auto request_packet = json_packet::create();
		const auto request_message = group_request_message::create(request_packet);

		// Session
		const auto session = get_session();
		if (!session)
			return http_code::bad_request;

		// Json
		const auto json = request_packet->str();
		const auto json_encrypted = session->encrypt(json);
		const auto json_encoded = base64::encode(json_encrypted);

		// Http
		http_request request(session->get_id());
		request.set_host(get_address());
		request.set_body(json_encoded);
		m_socket->write(request);

		http_response response;
		m_socket->read(response);

		const auto& status = response.get_status();
		if (status.is_ok())
		{			
			// Body
			const auto& body = response.get_body();
			if (body.empty())
				return http_code::not_found;

			const auto body_decoded = base64::decode(body);
			const auto body_decypted = session->decrypt(body_decoded);

			// Response
			const auto response_packet = json_packet::create(body_decypted);
			const auto response_message = group_response_message::create(response_packet);
			const auto response_clients = response_message->get_clients();

			const auto temp_list = client_entity_vector_component::create();
			const auto client_vector = get_client_vector();
			if (!client_vector)
				return http_code::bad_request;

			for (const auto& value : *response_clients)
			{
				const auto response_value = json_value::create(value);
				const auto response_client = json_client::create(response_value);
				const auto client_id = response_client->get_id();

				// TODO: Optimize this!
				// TODO: Add client_entity_map_component
				auto entity = client_vector->find_if([client_id](const entity::ptr e)
				{
					const auto c = e->get<client_component>();
					return c && (c->get_id() == client_id);
				});

				const auto name = response_client->get_name();

				if (!entity)
				{					
					const auto client = client_component::create(response_client->get_id());
					client->set_name(name);
					client->set_join_name(name);

					entity = entity::create(); 
					entity->add(client);
				}
				else
				{
					const auto client = entity->get<client_component>();
					if (client)
					{
						client->set_name(name);
						client->set_join_name(name);
					}
				}

				temp_list->add(entity);
			}

			m_entity->add(temp_list);

			// Callback
			call(function_type::client, function_action::refresh, m_entity);
			call(function_type::browse, function_action::refresh, m_entity);			
			call(function_type::message, function_action::refresh, m_entity);
		}

		// Response
		return response.get_status();
	}
}
