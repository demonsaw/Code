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

#include "client_transfer_request_command.h"
#include "component/session_component.h"
#include "component/client/client_component.h"
#include "component/transfer/chunk_component.h"
#include "component/transfer/transfer_component.h"
#include "data/request/transfer_request_data.h"
#include "data/response/transfer_response_data.h"
#include "json/value/json_file.h"
#include "message/request/transfer_request_message.h"
#include "message/response/transfer_response_message.h"
#include "security/base.h"
#include "utility/boost.h"
#include "utility/default_value.h"

namespace eja
{
	// Utility
	http_status client_transfer_request_command::execute()
	{
		// Request
		const auto request_packet = json_packet::create();				
		const auto request_message = transfer_request_message::create(request_packet);

		// Transfer
		const auto transfer = m_entity->get<transfer_component>();
		if (!transfer)
			return http_code::bad_request;

		request_message->set_id(transfer->get_id());

		// Chunk
		const auto chunk = m_entity->get<chunk_component>();
		if (!chunk)
			return http_code::bad_request;

		const auto request_chunk = request_message->get_chunk();
		request_chunk->set_offset(chunk->get_offset());

		const auto file = m_entity->get<file_component>();
		if (!file)
			return http_code::bad_request;

		request_chunk->set_size(file->get_size());

		// Data
		const auto request_data = json_data::create();
		const auto request_transfer = transfer_request_data::create(request_data);
		
		const auto request_file = request_transfer->get_file();
		request_file->set_id(file->get_id());
		set_data(request_packet, request_data);

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
			const auto response_transfer = transfer_response_message::create(response_packet);
			const auto response_id = response_transfer->get_id();
			transfer->set_id(response_id);

			if (response_transfer->has_server())
			{
				const auto response_server = response_transfer->get_server();

				// Router
				const auto router = router_component::create();
				router->set_address(response_server->get_address());
				router->set_port(response_server->get_port());
				m_entity->add(router);

				// Security
				const auto security = router_security_component::create();
				if (response_server->has_passphrase())
					security->set_passphrase(response_server->get_passphrase());
				m_entity->add(security);
			}
		}

		// Response
		return response.get_status();
	}
}
