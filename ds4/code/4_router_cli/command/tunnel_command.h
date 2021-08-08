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

#ifndef _EJA_TUNNEL_COMMAND_H_
#define _EJA_TUNNEL_COMMAND_H_

#include <memory>

#include "command/acceptor_command.h"
#include "data/request/tunnel_request_data.h"
#include "message/request/router_request_message.h"
#include "system/type.h"

namespace eja
{
	class entity;
	class http_socket;

	class tunnel_command final : public acceptor_command
	{
		make_factory(tunnel_command);

	private:
		// Utility
		bool validate(const std::shared_ptr<router_request_message>& request_message, const std::shared_ptr<tunnel_request_data>& request_data) const;

	public:
		tunnel_command() { }
		tunnel_command(const std::shared_ptr<entity>& entity) : acceptor_command(entity) { }

		// Interface
		void execute(const std::shared_ptr<entity>& entity, const std::shared_ptr<router_request_message>& request_message, const std::shared_ptr<tunnel_request_data>& request_data, const std::shared_ptr<http_socket>& socket) const;

		// Static
		static ptr create(const std::shared_ptr<entity>& entity) { return std::make_shared<tunnel_command>(entity); }
	};
}

#endif
