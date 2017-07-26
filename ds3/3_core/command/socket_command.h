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

#ifndef _EJA_SOCKET_COMMAND_H_
#define _EJA_SOCKET_COMMAND_H_

#include <memory>

#include "entity/entity_command.h"
#include "message/request/socket_request_message.h"
#include "message/response/socket_response_message.h"
#include "system/type.h"

namespace eja
{
	class entity;
	class http_socket;

	// Client
	class client_socket_command final : public entity_command
	{
		make_factory(client_socket_command);

	public:
		client_socket_command() { }
		client_socket_command(const client_socket_command& cmd) : entity_command(cmd) { }
		client_socket_command(const std::shared_ptr<entity> entity) : entity_command(entity) { }

		// Interface
		std::shared_ptr<socket_request_message> execute(const std::shared_ptr<entity> router);
		void execute(const std::shared_ptr<entity> router, const std::shared_ptr<socket_response_message> response);

		// Static
		static ptr create(const std::shared_ptr<entity> entity) { return ptr(new client_socket_command(entity)); }
	};

	// Router
	class router_socket_command final : public entity_command
	{
		make_factory(router_socket_command);

	public:
		router_socket_command() { }
		router_socket_command(const router_socket_command& cmd) : entity_command(cmd) { }
		router_socket_command(const std::shared_ptr<entity> entity) : entity_command(entity) { }

		// Interface
		std::shared_ptr<socket_response_message> execute(const std::shared_ptr<entity> client, const std::shared_ptr<http_socket> socket, const std::shared_ptr<socket_request_message> request);

		// Static
		static ptr create(const std::shared_ptr<entity> entity) { return ptr(new router_socket_command(entity)); }
	};
}

#endif
