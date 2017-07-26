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

#include "client_join_request_command.h"
#include "component/session_component.h"
#include "json/json_packet.h"
#include "message/request/join_request_message.h"
#include "message/response/join_response_message.h"
#include "object/version.h"
#include "security/base.h"

namespace eja
{
	http_status client_join_request_command::execute()
	{
		// Request		
		const auto request_packet = json_packet::create();
		const auto request_message = join_request_message::create(request_packet);

		const auto request_entity = request_message->get_entity();
		request_entity->set_id(m_entity->get_id());

		// Version
		request_message->set_version(default_version::app);

		// Client
		const auto client = get_client();
		if (!client)
			return http_code::bad_request;
		
		const auto request_client = request_message->get_client();
		request_client->set(client);

		// Option
		const auto option = get_client_option();
		if (!option)
			return http_code::bad_request;

		const auto request_option = request_message->get_option();
		request_option->set(option);

		// Group
		const auto group = get_group();
		if (!group)
			return http_code::bad_request;

		const auto request_group = request_message->get_group();
		request_group->set_id(group->get_id());

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
			const auto response_message = join_response_message::create(response_packet);

			// Info
			const auto info = get_router_info();
			if (!info)
				return http_code::bad_request;
			
			const auto response_info = response_message->get_info();
			info->set_sessions(response_info->get_sessions());
			info->set_clients(response_info->get_clients());
			info->set_groups(response_info->get_groups());

			// Name
			if (response_info->has_name())
				client->set_join_name(response_info->get_name());
			else
				client->set_join_name(client->get_name());

			// Message
			const auto motd = option->get_motd() && option->show_motd();			

			if (response_info->has_message() && motd)
			{
				option->show_motd(false);

				// Chat
				const auto entity = entity::create();
				const auto router = m_entity->copy<router_component>();
				entity->add(router);

				const auto chat = chat_component::create();
				chat->set_text(response_info->get_message());
				chat->set_type(chat_type::router);
				entity->add(chat);

				call(function_type::chat, function_action::add, entity);
			}

			// Version
			const auto router = get_router();
			if (router && motd)
			{
				option->show_motd(false);

				version client_version(client->get_version());
				version router_version(router->get_version());
				
				if (client_version < router_version)
				{
					// Chat
					const auto entity = entity::create();
					const auto router = m_entity->copy<router_component>();
					entity->add(router);

					const auto chat = chat_component::create();
					chat->set_text("A new version of demonsaw is available for download at www.demonsaw.com");
					chat->set_type(chat_type::router);
					entity->add(chat);

					call(function_type::chat, function_action::add, entity);
				}
			}
		}

		// Response
		return response.get_status();
	}
}
