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

#include "router_download_response_command.h"
#include "component/session_component.h"
#include "component/timeout_component.h"
#include "json/json_packet.h"
#include "message/request/download_request_message.h"
#include "message/response/download_response_message.h"
#include "security/base.h"

namespace eja
{
	http_status router_download_response_command::execute(const entity::ptr entity, const json_message::ptr message, const json_data::ptr data)
	{
		// Option
		const auto option = get_router_option();
		if (!option)
		{
			m_socket->write(http_code::internal_server_error);
			return http_code::internal_server_error;
		}

		if (!option->get_transfer_router())
		{
			m_socket->write(http_code::not_implemented);
			return http_code::not_implemented;
		}

		// Request
		const auto request_message = download_request_message::create(message);
		if (request_message->invalid())
		{
			m_socket->write(http_code::bad_request);
			return http_code::bad_request;
		}

		// Entity
		const auto chunk_map = get_chunk_entity_map();
		if (!chunk_map)
		{
			m_socket->write(http_code::internal_server_error);
			return http_code::internal_server_error;
		}

		const auto request_id = request_message->get_id();
		const auto chunk_entity = chunk_map->get(request_id);
		if (!chunk_entity)
		{
			m_socket->write(http_code::not_found);
			return http_code::not_found;
		}

		// Chunk
		const auto upload_queue = chunk_entity->get<chunk_upload_queue_component>();
		if (!upload_queue)
		{
			m_socket->write(http_code::internal_server_error);
			return http_code::internal_server_error;
		}

		const auto download_queue = chunk_entity->get<chunk_download_queue_component>();
		if (!download_queue)
		{
			m_socket->write(http_code::internal_server_error);
			return http_code::internal_server_error;
		}
		
		// Chunk
		const auto has_chunk = request_message->has_chunk();
		if (has_chunk)
		{
			const auto request_chunk = request_message->get_chunk();
			if (request_chunk->has_offset())
			{
				const auto removed = download_queue->pop_if([request_chunk](const chunk::ptr chunk)
				{
					return chunk->get_offset() == request_chunk->get_offset();
				});
			}
		}

		if (upload_queue->done() && download_queue->done())
		{
			m_socket->write(http_code::not_found); 
			remove_transfer(chunk_entity);

			return http_code::not_found;
		}

		// Update
		const auto timeout = entity->get<timeout_component>();
		if (!timeout)
		{
			m_socket->write(http_code::internal_server_error);
			return http_code::internal_server_error;
		}

		timeout->update();

		// Session
		const auto session = entity->get<session_component>();
		if (!session)
		{
			m_socket->write(http_code::internal_server_error);
			return http_code::internal_server_error;
		}

		// Response		
		const auto response_packet = json_packet::create();
		const auto response_message = download_response_message::create(response_packet);

		// Chunk
		chunk::ptr chunk = nullptr;
		if (!download_queue->empty() && (chunk = download_queue->pop()))
		{
			if (has_chunk)
				download_queue->push(chunk, false);

			const auto request_chunk = response_message->get_chunk();
			request_chunk->set(chunk);

			const auto request_data = json_data::create(response_packet);
			request_data->set(chunk->get_data());

			// Update
			chunk_entity->update();
		}
		else
		{
			response_message->set_delay(default_timeout::router::download);
		}

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
