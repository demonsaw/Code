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

#ifndef _EJA_FILE_COMMAND_H_
#define _EJA_FILE_COMMAND_H_

#include <memory>

#include "entity/entity_command.h"
#include "message/callback/file_callback_message.h"
#include "message/request/file_request_message.h"
#include "message/response/file_response_message.h"
#include "system/type.h"

namespace eja
{
	class entity;
	class service_data;

	// Client
	class client_file_command final : public entity_command
	{
		make_factory(client_file_command);

	public:
		client_file_command() { }
		client_file_command(const client_file_command& cmd) : entity_command(cmd) { }
		client_file_command(const std::shared_ptr<entity> entity) : entity_command(entity) { }

		// Interface
		std::shared_ptr<file_request_message> execute(const std::shared_ptr<entity> router, const std::shared_ptr<entity> data, const std::shared_ptr<entity> client = nullptr);
		void execute(const std::shared_ptr<entity> router, const std::shared_ptr<entity> client, const std::shared_ptr<file_callback_message> callback);

		std::shared_ptr<file_response_message> execute(const std::shared_ptr<entity> router, const std::shared_ptr<file_request_message> request);
		void execute(const std::shared_ptr<entity> router, const std::shared_ptr<file_response_message> response);

		// Static
		static ptr create(const std::shared_ptr<entity> entity) { return ptr(new client_file_command(entity)); }
	};

	// Router
	class router_file_command final : public entity_command
	{
		make_factory(router_file_command);

	public:
		router_file_command() { }
		router_file_command(const router_file_command& cmd) : entity_command(cmd) { }
		router_file_command(const std::shared_ptr<entity> entity) : entity_command(entity) { }

		// Interface
		std::shared_ptr<file_callback_message> execute(const std::shared_ptr<entity> client, const std::shared_ptr<file_request_message> request);
		void execute(const std::shared_ptr<entity> client, const std::shared_ptr<entity> data, const std::shared_ptr<file_response_message> response);

		// Static
		static ptr create(const std::shared_ptr<entity> entity) { return ptr(new router_file_command(entity)); }
	};
}

#endif
