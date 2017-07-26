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

#ifndef _EJA_SEARCH_COMMAND_H_
#define _EJA_SEARCH_COMMAND_H_

#include <string>

#include "entity/entity_command.h"
#include "message/callback/search_callback_message.h"
#include "message/request/search_request_message.h"
#include "message/response/search_response_message.h"
#include "system/type.h"

namespace eja
{
	class entity;

	// Client
	class client_search_command final : public entity_command
	{
		make_factory(client_search_command);

	public:
		client_search_command() { }
		client_search_command(const client_search_command& cmd) : entity_command(cmd) { }
		client_search_command(const std::shared_ptr<entity> entity) : entity_command(entity) { }

		// Interface
		std::shared_ptr<search_request_message> execute(const std::shared_ptr<entity> router, const std::shared_ptr<entity> data);
		void execute(const std::shared_ptr<entity> router, const std::shared_ptr<entity> client, const std::shared_ptr<search_callback_message> callback);

		std::shared_ptr<search_response_message> execute(const std::shared_ptr<entity> router, const std::shared_ptr<search_request_message> request);
		void execute(const std::shared_ptr<entity> router, const std::shared_ptr<search_response_message> response);

		// Static
		static ptr create(const std::shared_ptr<entity> entity) { return ptr(new client_search_command(entity)); }
	};

	// Router
	class router_search_command final : public entity_command
	{
		make_factory(router_search_command);

	public:
		router_search_command() { }
		router_search_command(const router_search_command& cmd) : entity_command(cmd) { }
		router_search_command(const std::shared_ptr<entity> entity) : entity_command(entity) { }

		// Interface		
		std::shared_ptr<search_callback_message> execute(const std::shared_ptr<entity> client, const std::shared_ptr<search_request_message> request);
		void execute(const std::shared_ptr<entity> client, const std::shared_ptr<entity> data, const std::shared_ptr<search_response_message> response);

		// Static
		static ptr create(const std::shared_ptr<entity> entity) { return ptr(new router_search_command(entity)); }
	};
}

#endif
