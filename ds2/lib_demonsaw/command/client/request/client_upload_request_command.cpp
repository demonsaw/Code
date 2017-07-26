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

#include "client_upload_request_command.h"
#include "component/session_component.h"
#include "component/timer_component.h"
#include "component/client/client_component.h"
#include "component/transfer/chunk_component.h"
#include "component/transfer/transfer_component.h"
#include "component/transfer/upload_thread_component.h"
#include "data/request/upload_request_data.h"
#include "data/response/upload_response_data.h"
#include "json/value/json_file.h"
#include "message/request/upload_request_message.h"
#include "message/response/upload_response_message.h"
#include "security/base.h"
#include "utility/boost.h"
#include "utility/default_value.h"

namespace eja
{
	// Interface
	void client_upload_request_command::clear()
	{
		client_command::clear();

		m_delay = 0;
	}

	// Utility
	http_status client_upload_request_command::execute()
	{
		clear();

		// Request
		const auto request_packet = json_packet::create();
		const auto request_message = upload_request_message::create(request_packet);

		const auto transfer = m_entity->get<transfer_component>();
		if (!transfer)
			return http_code::bad_request;

		request_message->set_id(transfer->get_id());
		assert(request_message->has_id());

		const auto chunk = m_entity->get<chunk_component>();
		if (!chunk)
			return http_code::bad_request;

		if (chunk->has_data())
		{
			// Chunk
			const auto request_chunk = request_message->get_chunk();
			request_chunk->set(chunk);

			// Data
			const auto request_data = json_data::create();
			const auto request_upload = upload_request_data::create(request_data);
			request_upload->set_data(chunk->get_data());
			set_data(request_packet, request_upload);
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

		const auto& status = response.get_status();
		if (status.is_ok())
		{
			if (chunk->has_data())
			{
				// Transfer
				const auto thread = m_entity->get<upload_thread_component>();
				if (!thread || thread->add_chunk(chunk->get_offset()))
					transfer->update(chunk->get_size());

				chunk->clear();
			}

			// Body
			const auto& body = response.get_body();
			if (body.empty())
				return http_code::not_found;

			const auto body_decoded = base64::decode(body);
			const auto body_decypted = session->decrypt(body_decoded);

			// Response
			const auto response_packet = json_packet::create(body_decypted);
			const auto response_message = upload_response_message::create(response_packet);

			// Delay
			if (response_message->has_delay())
				m_delay = response_message->get_delay();

			if (response_message->has_chunk())
			{
				const auto response_chunk = response_message->get_chunk();
				chunk->set_offset(response_chunk->get_offset());
				chunk->set_size(response_chunk->get_size());
			}
		}

		// Response
		return response.get_status();
	}
}
