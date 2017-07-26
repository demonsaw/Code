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

#ifndef _EJA_ROUTER_TRANSFER_REQUEST_COMMAND_
#define _EJA_ROUTER_TRANSFER_REQUEST_COMMAND_

#include <memory>

#include "command/router/router_command.h"
#include "entity/entity.h"
#include "http/http_socket.h"
#include "http/http_status.h"

namespace eja
{
	class router_transfer_request_command final : public router_command
	{
	public:
		using ptr = std::shared_ptr<router_transfer_request_command>;

	private:
		router_transfer_request_command(const entity::ptr entity, const http_socket::ptr socket) : router_command(entity, socket) { }

	public:		
		virtual ~router_transfer_request_command() override { }

		// Utility
		virtual http_status execute(const entity::ptr entity, const json_packet::ptr packet) override;

		// Static
		static ptr create(const entity::ptr entity, const http_socket::ptr socket) { return ptr(new router_transfer_request_command(entity, socket)); }
	};
}

#endif
