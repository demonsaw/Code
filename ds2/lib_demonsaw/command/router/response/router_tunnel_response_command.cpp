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

#include "router_tunnel_response_command.h"
#include "component/session_component.h"
#include "component/tunnel_component.h"
#include "json/json_packet.h"
#include "message/request/tunnel_request_message.h"
#include "message/response/tunnel_response_message.h"

namespace eja
{
	http_status router_tunnel_response_command::execute(const entity::ptr entity, const json_message::ptr message, const json_data::ptr data)
	{
		// Option
		const auto option = get_router_option();
		if (!option)
		{
			m_socket->write(http_code::internal_server_error);
			return http_code::internal_server_error;
		}

		if (!option->get_message_router())
		{
			m_socket->write(http_code::not_implemented);
			return http_code::not_implemented;
		}

		// Request
		const auto request_message = tunnel_request_message::create(message);
		if (request_message->invalid())
		{
			m_socket->write(http_code::bad_request);
			return http_code::bad_request;
		}

		// Tunnel
		const auto tunnel = tunnel_component::create(m_socket);
		entity->add(tunnel);

		// Session
		const auto session = entity->get<session_component>();
		if (!session)
		{
			m_socket->write(http_code::internal_server_error);
			return http_code::internal_server_error;
		}

		// Response
		const auto response_packet = json_packet::create();
		const auto response_message = tunnel_response_message::create(response_packet);

		const auto json = response_packet->str();
		const auto json_encrypted = session->encrypt(json);
		const auto json_encoded = base64::encode(json_encrypted);

		// Http
		http_ok_response response;
		response.set_body(json_encoded);
		m_socket->write(response);
		
		return response.get_status();
	}
}
