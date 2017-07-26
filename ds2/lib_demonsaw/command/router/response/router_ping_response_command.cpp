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

#include "router_ping_response_command.h"
#include "command/router/request/router_ping_request_command.h"
#include "component/session_component.h"
#include "component/tunnel_component.h"
#include "json/json_packet.h"
#include "message/request/ping_request_message.h"
#include "message/response/ping_response_message.h"
#include "security/base.h"

namespace eja
{
	http_status router_ping_response_command::execute(const entity::ptr entity, const json_message::ptr message, const json_data::ptr data)
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
		const auto request_message = ping_request_message::create(message);
		if (request_message->invalid())
		{
			m_socket->write(http_code::bad_request);
			return http_code::bad_request;
		}

		// Tunnel		
		const auto tunnel = entity->get<tunnel_component>();
		if (!tunnel)
		{
			m_socket->write(http_code::not_found);
			return http_code::not_found;
		}

		// Socket
		const auto socket = tunnel->get_socket();
		if (!socket)
		{
			m_socket->write(http_code::not_found);
			return http_code::not_found;
		}

		try
		{
			// Request			
			const auto request_command = router_ping_request_command::create(m_entity, socket);
			const auto request_status = request_command->execute(entity);
			
			if (request_status.is_error())
			{
				m_socket->write(http_code::not_found);
				return request_status;
			}
		}
		catch (std::exception& e)
		{
			m_socket->write(http_code::not_found);
			//remove_client(entity);
			log(e);

			return http_code::not_found;
		}
		catch (...)
		{
			m_socket->write(http_code::not_found);
			//remove_client(entity);
			log();

			return http_code::not_found;
		}

		// Session
		const auto session = entity->get<session_component>();
		if (!session)
		{
			m_socket->write(http_code::internal_server_error);
			return http_code::internal_server_error;
		}

		// Response
		const auto response_packet = json_packet::create();
		const auto response_message = ping_response_message::create(response_packet);

		const auto json = response_packet->str();
		const auto json_encrypted = session->encrypt(json);
		const auto json_encoded = base64::encode(json_encrypted);

		// Http
		http_ok_response response;
		response.set_body(json_encoded);
		m_socket->write(response);

		// Update
		const auto timeout = entity->get<timeout_component>();
		if (timeout)
			timeout->update();

		return response.get_status();
	}
}
