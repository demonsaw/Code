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

#include <cassert>
#include <string.h>
#include <boost/format.hpp>

#include "client_download_request_command.h"
#include "component/session_component.h"
#include "component/timer_component.h"
#include "component/client/client_component.h"
#include "component/transfer/chunk_component.h"
#include "component/transfer/transfer_component.h"
#include "data/request/download_request_data.h"
#include "data/response/download_response_data.h"
#include "json/value/json_file.h"
#include "message/request/download_request_message.h"
#include "message/response/download_response_message.h"
#include "security/base.h"
#include "utility/boost.h"
#include "utility/default_value.h"

namespace eja
{
	// Interface
	void client_download_request_command::clear()
	{
		client_command::clear();

		m_delay = 0;
	}

	// Utility
	http_status client_download_request_command::execute()
	{
		clear();

		// Request
		const auto request_packet = json_packet::create();				
		const auto request_message = download_request_message::create(request_packet);

		// Transfer
		const auto transfer = m_entity->get<transfer_component>();
		if (!transfer)
			return http_code::bad_request;

		request_message->set_id(transfer->get_id());

		// Chunk
		const auto chunk = m_entity->get<chunk_component>(false);
		if (!chunk)
			return http_code::bad_request;

		const auto request_chunk = request_message->get_chunk();
		if (chunk->has_ack())
			request_chunk->set_offset(chunk->get_offset());

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
			// Chunk
			chunk->set_ack(false);

			// Body
			const auto& body = response.get_body();
			if (body.empty())
				return http_code::not_found;

			const auto body_decoded = base64::decode(body);
			const auto body_decypted = session->decrypt(body_decoded);

			// Response
			const auto response_packet = json_packet::create(body_decypted);
			const auto response_message = download_response_message::create(response_packet);
			
			// Delay
			if (response_message->has_delay())
				m_delay = response_message->get_delay();

			// Chunk
			if (response_message->has_chunk())
			{
				// Data
				const auto response_data = get_data(response_packet);
				const auto response_download = download_response_data::create(response_data);
				const auto data = base64::decode(response_download->get_data());

				// Chunk	
				const auto response_chunk = response_message->get_chunk();
				chunk->set_offset(response_chunk->get_offset());
				chunk->set_size(response_chunk->get_size());
				chunk->set_data(data);

				// HACK
				const auto file = m_entity->get<file_component>();
				if (file && ((chunk->get_offset() + chunk->get_size()) < file->get_size()))
				{
					const auto parent = m_entity->get_entity();
					if (parent)
					{
						const auto parent_chunk = parent->get<chunk_component>();
						if (parent_chunk)
							parent_chunk->set_size(chunk->get_size());
					}
				}				

				assert(data.size() == response_chunk->get_size());
			}
		}

		// Response
		return response.get_status();
	}
}
