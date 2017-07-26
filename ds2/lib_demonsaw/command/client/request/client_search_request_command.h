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

#ifndef _EJA_CLIENT_SEARCH_REQUEST_COMMAND_
#define _EJA_CLIENT_SEARCH_REQUEST_COMMAND_

#include <memory>
#include <string>

#include "command/client/client_command.h"
#include "entity/entity.h"
#include "http/http_socket.h"
#include "http/http_status.h"

namespace eja
{
	class client_search_request_command final : public client_command
	{
	public:
		using ptr = std::shared_ptr<client_search_request_command>;

	private:
		client_search_request_command(const entity::ptr entity, const http_socket::ptr socket) : client_command(entity, socket) { }

		// Utility
		virtual http_status execute(const json_data::ptr data) override;

	public:
		virtual ~client_search_request_command() override { }

		// Utility		
		virtual http_status execute(const json_message::ptr message, const json_data::ptr data) override;
		http_status execute(const std::string& keyword, const file_filter filter);

		// Static
		static ptr create(const entity::ptr entity, const http_socket::ptr socket) { return ptr(new client_search_request_command(entity, socket)); }
	};
}

#endif
