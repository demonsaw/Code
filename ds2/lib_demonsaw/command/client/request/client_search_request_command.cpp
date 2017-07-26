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

#include <string.h>

#include "client_search_request_command.h"
#include "component/session_component.h"
#include "component/client/client_component.h"
#include "component/client/client_option_component.h"
#include "component/io/folder_component.h"
#include "data/request/search_request_data.h"
#include "data/response/search_response_data.h"
#include "json/value/json_file.h"
#include "json/value/json_folder.h"
#include "message/request/search_request_message.h"
#include "message/response/search_response_message.h"
#include "security/base.h"
#include "utility/boost.h"
#include "utility/default_value.h"

namespace eja
{
	// Utility
	http_status client_search_request_command::execute(const std::string& keyword, const file_filter filter)
	{
		// Data
		const auto request_data = json_data::create();
		const auto request_search = search_request_data::create(request_data, json::search);
		request_search->set_keyword(keyword);
		request_search->set_filter(filter);

		return execute(request_data);
	}

	http_status client_search_request_command::execute(const json_data::ptr data)
	{
		// Not thoroughly tested
		if (data->invalid())
			return http_code::bad_request;

		// Request
		const auto request_packet = json_packet::create();
		const auto request_message = search_request_message::create(request_packet);
		set_data(request_packet, data);

		// Session
		const auto session = get_session();
		if (!session)
			return http_code::bad_request;

		// Json
		const auto json = request_packet->str();
		const auto json_encrypted = session->encrypt(json);
		const auto json_encoded = base64::encode(json_encrypted);

		// Http
		http_request request(session->get_id());
		request.set_host(get_address());
		request.set_body(json_encoded);
		m_socket->write(request);

		http_response response;
		m_socket->read(response);

		const auto& status = response.get_status();
		if (status.is_ok())
		{
			// Body
			const auto& body = response.get_body();
			if (body.empty())
				return http_code::not_found;

			const auto body_decoded = base64::decode(body);
			const auto body_decypted = session->decrypt(body_decoded);

			// Response
			const auto response_packet = json_packet::create(body_decypted);
			const auto response_search = search_response_message::create(response_packet);
			const auto response_id = response_search->get_id();

			const auto search = get_search();
			if (search)
				search->set_id(response_id);
		}

		// Response
		return response.get_status();
	}

	http_status client_search_request_command::execute(const json_message::ptr message, const json_data::ptr data)
	{
		// Request
		const auto request_message = search_request_message::create(message);
		if (request_message->invalid())
		{
			m_socket->write(http_code::bad_request);
			return http_code::bad_request;
		}

		const auto request_data = get_data(data);
		const auto request_search = search_request_data::create(request_data, json::search);
		if (request_search->invalid())
		{
			m_socket->write(http_code::bad_request);
			return http_code::bad_request;
		}		

		// Data
		std::list<std::string> keywords;
		std::string keyword = request_search->get_keyword();
		const auto& filter = request_search->get_filter();

		if (keyword[0] == '"')
		{
			const auto offset = (keyword[keyword.size() - 1] == '"') ? 2 : 1;
			keyword = keyword.substr(1, keyword.size() - offset);
			std::transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);
			keywords.push_back(keyword);
		}
		else
		{
			boost::char_separator<char> separator(" ");
			boost::string_tokenizer tokens(keyword, separator);
			for (const auto& token : tokens)
			{
				auto keyword = token;
				std::transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);
				keywords.push_back(keyword);
			}				
		}

		std::list<file::ptr> files;
		const auto option = m_entity->get<client_option_component>();
		if (!option)
		{
			m_socket->write(http_code::internal_server_error);
			return http_code::internal_server_error;
		}

		const auto file_map = m_entity->get<file_map_component>();
		if (!file_map)
		{
			m_socket->write(http_code::internal_server_error);
			return http_code::internal_server_error;
		}

		{
			auto_lock_ptr(file_map);

			for (const auto& pair : *file_map)
			{
				const auto file = pair.second;

				auto ext = file->get_extension();
				std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
				if (!file_util::is_valid(filter, ext))
					continue;

				auto name = file->get_name();
				std::transform(name.begin(), name.end(), name.begin(), ::tolower);

				for (const auto& keyword : keywords)
				{
					if ((name.find(keyword) != std::string::npos))
					{
						const auto it = std::find(files.begin(), files.end(), file);
						if (it == files.end())
							files.push_back(file);
					}						
				}

				if (files.size() >= option->get_max_searches())
					break;
			}
		}

		if (files.empty())
		{
			m_socket->write(http_code::not_found);
			return http_code::not_found;
		}

		const auto response_data = json_data::create();
		const auto response_search = search_response_data::create(response_data);
		const auto response_files = response_search->get_files();

		for (const auto& file : files)
			response_files->add(file);

		// Response
		const auto response_packet = json_packet::create();
		const auto response_message = search_response_message::create(response_packet);
		set_data(response_packet, response_data);

		// Session
		const auto session = get_session();
		if (!session)
			return http_code::bad_request;

		const auto json = response_packet->str();
		const auto json_encrypted = session->encrypt(json);
		const auto json_encoded = base64::encode(json_encrypted);

		// Http
		http_ok_response response;
		response.set_body(json_encoded);
		m_socket->write(response);

		return response.get_status();
	}
}
