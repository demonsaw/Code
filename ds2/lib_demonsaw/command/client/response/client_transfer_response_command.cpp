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

#include "client_transfer_response_command.h"
#include "component/session_component.h"
#include "component/io/file_component.h"
#include "component/transfer/chunk_component.h"
#include "component/transfer/transfer_component.h"
#include "component/transfer/upload_machine_component.h"
#include "data/request/transfer_request_data.h"
#include "json/json_packet.h"
#include "message/request/transfer_request_message.h"

namespace eja
{
	http_status client_transfer_response_command::execute(const json_message::ptr message, const json_data::ptr data)
	{
		// Request
		const auto request_message = transfer_request_message::create(message);
		if (request_message->invalid())
		{
			m_socket->write(http_code::bad_request);
			return http_code::bad_request;
		}

		if (!request_message->has_id())
		{
			m_socket->write(http_code::bad_request);
			return http_code::bad_request;
		}

		// TODO: REMOVE ME FOR 2.7.0
		//
		// v1.0
		////////////////////////////////////////////////////////
		if (!request_message->has_chunk())
		{
			m_socket->write(http_code::bad_request);
			return http_code::bad_request;
		}
		////////////////////////////////////////////////////////

		// Data
		const auto request_data = get_data(data);
		const auto request_transfer = transfer_request_data::create(request_data);
		if (request_transfer->invalid())
		{
			m_socket->write(http_code::bad_request);
			return http_code::bad_request;
		}

		// File
		const auto file_map = get_file_map();
		if (!file_map)
		{
			m_socket->write(http_code::internal_server_error);
			return http_code::internal_server_error;
		}			

		// Response
		http_ok_response response;
		m_socket->write(response);

		const auto request_file = request_transfer->get_file();		
		const auto file = file_map->get(request_file->get_id());

		if (file)
		{
			// Entity
			const auto e = entity::create(m_entity);

			// File
			const auto fc = file_component::create(file);
			e->add(fc);

			// v1.0
			////////////////////////////////////////////////////////
			// Transfer
			const auto tc = transfer_component::create(transfer_type::upload);
			tc->set_id(request_message->get_id());
			e->add(tc);

			// TODO: REMOVE ME FOR 2.7.0
			//
			// Chunk
			const auto cc = chunk_component::create();
			const auto request_chunk = request_message->get_chunk();
			cc->set_offset(request_chunk->get_offset());
			cc->set_size(request_chunk->get_size());
			e->add(cc);
			////////////////////////////////////////////////////////

			if (request_message->has_server())
			{
				// Router
				const auto rc = router_component::create();
				const auto request_server = request_message->get_server();
				rc->set_address(request_server->get_address());
				rc->set_port(request_server->get_port());
				e->add(rc);

				// Security
				const auto security = router_security_component::create();
				if (request_server->has_passphrase())
					security->set_passphrase(request_server->get_passphrase());
				e->add(security);
			}

			call(function_type::transfer, function_action::add, e);
		}

		return response.get_status();
	}
}

