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

#include <memory>

#include "entity/entity_command.h"
#include "message/request/client_request_message.h"
#include "message/request/group_request_message.h"
#include "message/response/client_response_message.h"
#include "system/type.h"

namespace eja
{
	class entity;
	class search_request_data;
	class search_response_data;

	class search_command final : public entity_command
	{
		make_factory(search_command);

	private:
		// Write
		void write(const std::shared_ptr<client_request_message>& request_message, const std::shared_ptr<search_response_data>& response_data) const;

		// Utility
		bool validate(const std::shared_ptr<client_request_message>& request_message, const std::shared_ptr<search_request_data>& request_data) const;
		bool validate(const std::shared_ptr<client_response_message>& response_message, const std::shared_ptr<search_response_data>& response_data) const;

	public:
		search_command() { }
		search_command(const std::shared_ptr<entity>& entity) : entity_command(entity) { }

		// Interface
		std::shared_ptr<group_request_message> execute(const std::shared_ptr<entity>& entity) const;
		std::shared_ptr<search_response_data> execute(const std::shared_ptr<client_request_message>& request_message, const std::shared_ptr<search_request_data>& request_data) const;
		void execute(const std::shared_ptr<client_response_message>& response_message, const std::shared_ptr<search_response_data>& response_data) const;

		// Static
		static ptr create(const std::shared_ptr<entity>& entity) { return std::make_shared<search_command>(entity); }
	};
}

#endif
