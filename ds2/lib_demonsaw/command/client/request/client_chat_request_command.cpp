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

#include "client_chat_request_command.h"
#include "component/chat_component.h"
#include "component/session_component.h"
#include "component/client/client_component.h"
#include "component/io/folder_component.h"
#include "data/request/chat_request_data.h"
#include "data/response/chat_response_data.h"
#include "json/value/json_file.h"
#include "json/value/json_folder.h"
#include "message/request/chat_request_message.h"
#include "message/response/chat_response_message.h"
#include "security/base.h"
#include "utility/boost.h"
#include "utility/default_value.h"

namespace eja
{
	// Utility
	http_status client_chat_request_command::execute(const entity::ptr entity, const std::string& message, const chat_type type)
	{
		// Client
		const auto client = get_client();
		if (!client)
			return http_code::bad_request;

		// Data
		const auto request_data = json_data::create();
		const auto request_chat = chat_request_data::create(request_data);
		request_chat->set_id(client->get_id());
		request_chat->set_name(client->get_join_name());		
		request_chat->set_text(message);
		request_chat->set_type(type);

		return execute(entity, request_data);
	}

	http_status client_chat_request_command::execute(const entity::ptr entity, const json_data::ptr data)
	{
		// Not thoroughly tested
		if (data->invalid())
			return http_code::bad_request;

		// Request
		const auto request_packet = json_packet::create();
		const auto request_message = chat_request_message::create(request_packet);
		set_data(request_packet, data);

		// PM
		if (entity)
		{
			const auto client = entity->get<client_component>();
			if (!client)
				return http_code::bad_request;

			// Client
			const auto request_client = request_message->get_client();
			request_client->set_id(client->get_id());
		}

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

		return response.get_status();
	}
}
