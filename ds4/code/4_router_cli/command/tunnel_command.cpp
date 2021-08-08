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

#include "component/client/client_component.h"
#include "component/group/group_component.h"
#include "component/service/socket_component.h"

#include "entity/entity.h"
#include "http/http_socket.h"

namespace eja
{	
	// Interface
	void tunnel_command::execute(const entity::ptr& entity, const router_request_message::ptr& request_message, const tunnel_request_data::ptr& request_data, const http_socket::ptr& socket) const
	{
		// Validate
		if (unlikely(!validate(request_message, request_data)))
			return;

		// Client
		const auto group = entity->get<group_component>();
		const auto client = entity->get<client_component>();		

		if (!group->has_id())
		{
			client->set_address(std::move(socket->get_remote_address()));
			client->set_port(socket->get_remote_port());
		}
		else
		{
			client->set_address();
			client->set_port();
		}

		// Socket
		auto socket_set = m_entity->get<socket_set_component>();
		{
			thread_lock(socket_set);
			socket_set->erase(socket);
		}

		// Tunnel
		socket_set = entity->get<socket_set_component>();
		{
			thread_lock(socket_set);
			socket_set->clear();

			socket_set->insert(socket);
		}
	}

	// Utility
	bool tunnel_command::validate(const router_request_message::ptr& request_message, const tunnel_request_data::ptr& request_data) const
	{
		return true;
	}
}
