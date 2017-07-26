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

#include "command/socket_command.h"
#include "component/socket_component.h"
#include "component/tunnel_component.h"
#include "entity/entity.h"
#include "http/http_socket.h"
#include "message/request/request_message.h"
#include "message/response/response_message.h"

namespace eja
{
	// Client
	socket_request_message::ptr client_socket_command::execute(const entity::ptr router)
	{
		return socket_request_message::create();
	}

	void client_socket_command::execute(const entity::ptr router, const std::shared_ptr<socket_response_message> response)
	{
		// N/A
	}

	// Router
	socket_response_message::ptr router_socket_command::execute(const entity::ptr client, const http_socket::ptr socket, const socket_request_message::ptr request)
	{
		// Socket
		const auto socket_set = m_entity->get<socket_set_component>();
		{
			thread_lock(socket_set);
			socket_set->erase(socket);
		}

		// Tunnel
		const auto tunnel_list = client->get<tunnel_list_component>();
		{
			thread_lock(tunnel_list);
			tunnel_list->push_back(socket);
		}

		return socket_response_message::create();
	}
}
