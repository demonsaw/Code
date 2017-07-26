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

#include "command/tunnel_command.h"
#include "component/endpoint_component.h"
#include "component/socket_component.h"
#include "component/tunnel_component.h"
#include "component/client/client_network_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "entity/entity.h"
#include "http/http_socket.h"
#include "message/request/request_message.h"
#include "message/response/response_message.h"

namespace eja
{
	// Client
	tunnel_request_message::ptr client_tunnel_command::execute(const entity::ptr router)
	{
		const auto request = tunnel_request_message::create();

		// Sockets
		const auto network = m_entity->get<client_network_component>();
		request->set_sockets(static_cast<u8>(network->get_sockets()));

		// Server
		if (network->get_mode() == network_mode::server)
		{
			const auto endpoint = m_entity->get<endpoint_component>();

			if (endpoint->has_address() && endpoint->has_port())
			{
				request->set_address(endpoint->has_address_ext() ? endpoint->get_address_ext() : endpoint->get_address());
				request->set_port(endpoint->has_port_ext() ? endpoint->get_port_ext() : endpoint->get_port());
			}
			else
			{
				m_entity->log("Server Mode requires a valid address and port");
			}
		}
	
		return request;
	}

	void client_tunnel_command::execute(const entity::ptr router, const std::shared_ptr<tunnel_response_message> response)
	{
		// N/A
	}

	// Router
	tunnel_response_message::ptr router_tunnel_command::execute(const entity::ptr client, const http_socket::ptr socket, const tunnel_request_message::ptr request)
	{
		// Server
		if (request->has_endpoint())
		{
			const auto endpoint = client->get<endpoint_component>();
			endpoint->set_address(request->get_address());
			endpoint->set_port(request->get_port());
		}
		
		// Tunnel
		const auto tunnel_list = client->get<tunnel_list_component>();
		{
			thread_lock(tunnel_list);
			tunnel_list->clear();
		}

		return tunnel_response_message::create();
	}
}
