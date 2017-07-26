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

#include "client_search_response_command.h"
#include "component/search_component.h"
#include "component/session_component.h"
#include "component/io/file_component.h"
#include "data/response/search_response_data.h"
#include "json/json_packet.h"
#include "message/request/search_request_message.h"
#include "message/response/search_response_message.h"

namespace eja
{
	http_status client_search_response_command::execute(const json_message::ptr message, const json_data::ptr data)
	{
		// Request
		const auto request_message = search_response_message::create(message);
		if (request_message->invalid())
		{
			m_socket->write(http_code::bad_request);
			return http_code::bad_request;
		}

		// Id
		const auto search = get_search();
		if (!search)
		{
			m_socket->write(http_code::internal_server_error);
			return http_code::internal_server_error;
		}

		const auto& search_id = search->get_id();
		const auto& request_id = request_message->get_id();
		if (strcmp(request_id, search_id.c_str()))
		{
			m_socket->write(http_code::not_found);
			return http_code::not_found;
		}

		// Data
		const auto request_data = get_data(data);
		const auto request_search = search_response_data::create(request_data);
		if (request_search->invalid())
		{
			m_socket->write(http_code::bad_request);
			return http_code::bad_request;
		}

		// Response
		http_ok_response response;
		m_socket->write(response);

		// Files
		const auto response_files = request_search->get_files();

		if (!response_files->empty())
		{
			for (const auto& value : *response_files)
			{
				const auto response_value = json_value::create(value);
				const auto response_file = json_file::create(response_value);

				// Search
				const auto entity = entity::create();
				entity->add<search_component>();

				// File
				const auto file = file_component::create();
				file->set_id(response_file->get_id());
				file->set_path(response_file->get_name());
				file->set_size(response_file->get_size());
				file->clean();

				entity->add(file);

				// Callback
				call(function_type::search, function_action::add, entity);
			}
		}

		return response.get_status();
	}
}
