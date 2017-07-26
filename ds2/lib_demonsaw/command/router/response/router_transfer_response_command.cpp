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

#include "router_transfer_response_command.h"
#include "command/router/request/router_transfer_request_command.h"
#include "component/session_component.h"
#include "component/timeout_component.h"
#include "component/timer_component.h"
#include "component/tunnel_component.h"
#include "component/client/client_option_component.h"
#include "component/group/group_component.h"
#include "component/router/router_component.h"
#include "component/server/server_component.h"
#include "component/transfer/chunk_component.h"
#include "json/json_packet.h"
#include "message/request/transfer_request_message.h"
#include "message/response/transfer_response_message.h"
#include "security/base.h"
#include "system/function/function_action.h"
#include "system/function/function_type.h"

namespace eja
{
	// Message Router
	http_status router_transfer_response_command::execute(const entity::ptr entity, const json_message::ptr message, const json_data::ptr data)
	{
		// Transfer
		if (!entity->has<group_component>())
			return execute(entity, message);

		// Option
		const auto option = get_router_option();
		if (!option->get_message_router())
		{
			m_socket->write(http_code::not_implemented);
			return http_code::not_implemented;
		}

		// Request
		const auto request_message = transfer_request_message::create(message);
		if (request_message->invalid())
		{
			m_socket->write(http_code::bad_request);
			return http_code::bad_request;
		}

		const auto request_chunk = request_message->get_chunk();
		if (request_chunk->invalid())
		{
			m_socket->write(http_code::bad_request);
			return http_code::bad_request;
		}

		// Server		
		const auto server_entity = get_active_server();
		if (!server_entity)
		{
			if (!option->get_transfer_router())
			{
				m_socket->write(http_code::not_implemented);
				return http_code::not_implemented;
			}
		}

		// Group
		const auto group_map = get_group_entity_map();
		const auto group = entity->get<group_component>();
		const auto group_entity = group_map->get(group->get_id());
		if (!group_entity)
		{
			m_socket->write(http_code::not_found);
			return http_code::not_found;
		}

		// Chunk
		const auto file_size = request_chunk->get_size();
		const auto file_offset = request_chunk->get_offset();
		const auto upload_queue = chunk_upload_queue_component::create(file_size, file_offset);
		std::string request_id = request_message->has_id() ? request_message->get_id() : hex::random();

		if (!server_entity)
		{
			// Have we exceed max transfer requests?
			const auto chunk_map = get_chunk_entity_map();
			if (chunk_map->size() >= option->get_max_transfers())
			{
				m_socket->write(http_code::service_unavailable);
				return http_code::service_unavailable;
			}

			// TODO: Protect against duplicate keys			
			while (request_id.empty() || chunk_map->has(request_id))
				request_id = hex::random();

			auto chunk_entity = chunk_map->get(request_id);

			if (!chunk_entity)
			{
				// Entity
				chunk_entity = entity::create(request_id);

				// Timer
				chunk_entity->add<timeout_component>();
				chunk_entity->add<timer_component>();

				// Chunk				
				const auto chunk = chunk_component::create(file_size, file_offset);
				chunk_entity->add(chunk);

				// Option			
				const auto chunk_size = option->get_chunk_size();
				const auto chunk_buffer = option->get_chunk_buffer();
				const auto chunk_drift = option->get_chunk_drift();

				// Upload				
				upload_queue->set_chunk_size(chunk_size);
				upload_queue->set_chunk_drift(chunk_drift);
				chunk_entity->add(upload_queue);

				// Download
				const auto download_queue = chunk_download_queue_component::create(file_size, file_offset);
				download_queue->set_chunk_buffer(chunk_buffer);
				chunk_entity->add(download_queue);

				chunk_map->add(request_id, chunk_entity);

				call(function_type::transfer, function_action::add, chunk_entity);
			}
		}

		// Session
		const auto session = entity->get<session_component>();
		if (!session)
		{
			m_socket->write(http_code::internal_server_error);
			return http_code::internal_server_error;
		}

		// Response
		const auto response_id = request_id;
		const auto response_packet = json_packet::create();
		const auto response_message = transfer_response_message::create(response_packet);
		response_message->set_id(response_id);

		// Server
		if (server_entity)
		{
			const auto response_server = response_message->get_server();
			response_server->set(server_entity);
		}

		const auto json = response_packet->str();
		const auto json_encrypted = session->encrypt(json);
		const auto json_encoded = base64::encode(json_encrypted);

		// Http
		http_ok_response response;
		response.set_body(json_encoded);
		m_socket->write(response);

		// Limit the max number of threads
		const auto entity_map = group_entity->get<entity_map_component>();
		const auto entity_queue = std::make_shared<mutex_queue<eja::entity>>(entity_map->get_queue());		
		const auto max_threads = std::min(entity_queue->size() - 1, option->get_max_threads());
		const auto self = shared_from_this();

		for (size_t i = 0; i < max_threads; ++i)
		{
			std::thread thread([this, self, data, entity, server_entity, entity_queue, upload_queue, response_id]()
			{
				entity::ptr item = nullptr;

				while (item = entity_queue->pop())
				{
					// Ignore self
					if (item == entity)
						continue;

					// Ignore ghosts
					const auto option = item->get<client_option_component>();
					if (option && !option->get_transfer())
						continue;

					// Tunnel			
					if (!item->has<tunnel_component>())
						continue;					

					const auto item_tunnel = item->get<tunnel_component>();
					const auto item_socket = item_tunnel->get_socket();

					try
					{
						// Request
						const auto request_packet = json_packet::create();
						const auto request_message = transfer_request_message::create(request_packet);
						request_message->set_id(response_id);

						// TODO: REMOVE ME FOR 2.7.0
						//
						// v1.0
						////////////////////////////////////////////////////////
						const auto chunk = upload_queue->push();
						if (!chunk)
							break;

						const auto request_chunk = request_message->get_chunk();
						request_chunk->set(chunk);
						////////////////////////////////////////////////////////

						if (server_entity)
						{
							const auto request_server = request_message->get_server();
							request_server->set(server_entity);
						}

						const auto request_data = json_data::create(request_packet);
						request_data->set(data);

						// Command						
						const auto request_command = router_transfer_request_command::create(self->get_entity(), item_socket);
						const auto request_status = request_command->execute(item, request_packet);
						
						// CORE DUMP
						if (request_status.is_none())
							item_socket->close();
					}
					catch (const std::exception& ex)
					{
						// CORE DUMP						
						item_socket->close();
						log(ex);
						//remove_client(item);
					}
					catch (...)
					{
						// CORE DUMP						
						item_socket->close();
						log();
						//remove_client(item);
					}
				}
			});

			thread.detach();
		}

		return response.get_status();
	}
	
	// Data Router
	http_status router_transfer_response_command::execute(const entity::ptr entity, const json_message::ptr message)
	{
		// Option
		const auto option = get_router_option();
		if (!option)
		{
			m_socket->write(http_code::service_unavailable);
			return http_code::service_unavailable;
		}

		if (!option->get_transfer_router())
		{
			m_socket->write(http_code::not_implemented);
			return http_code::not_implemented;
		}

		// Request
		const auto request_message = transfer_request_message::create(message);
		if (request_message->invalid())
		{
			m_socket->write(http_code::bad_request);
			return http_code::bad_request;
		}

		const auto request_chunk = request_message->get_chunk();
		if (request_chunk->invalid())
		{
			m_socket->write(http_code::bad_request);
			return http_code::bad_request;
		}

		// Have we exceed max transfer requests?
		const auto chunk_map = get_chunk_entity_map();
		if (!chunk_map)
		{
			m_socket->write(http_code::service_unavailable);
			return http_code::service_unavailable;
		}

		if (chunk_map->size() >= option->get_max_transfers())
		{
			m_socket->write(http_code::service_unavailable);
			return http_code::service_unavailable;
		}

		// Chunk
		std::string request_id = request_message->has_id() ? request_message->get_id() : hex::random();

		if (request_id.empty())
		{
			// TODO: Protect against duplicate keys
			while (chunk_map->has(request_id))
				request_id = hex::random();
		}

		auto chunk_entity = chunk_map->get(request_id);
		if (!chunk_entity)
		{
			// Entity
			chunk_entity = entity::create(request_id);

			// Timer
			chunk_entity->add<timeout_component>();
			chunk_entity->add<timer_component>();

			// Chunk
			const auto file_size = request_chunk->get_size();
			const auto file_offset = request_chunk->get_offset();
			const auto chunk = chunk_component::create(file_size, file_offset);
			chunk_entity->add(chunk);

			// Option			
			const auto chunk_size = option->get_chunk_size();
			const auto chunk_buffer = option->get_chunk_buffer();
			const auto chunk_drift = option->get_chunk_drift();

			// Upload						
			const auto upload_queue = chunk_upload_queue_component::create(file_size, file_offset);
			upload_queue->set_chunk_size(chunk_size);
			upload_queue->set_chunk_drift(chunk_drift);
			chunk_entity->add(upload_queue);

			// Download
			const auto download_queue = chunk_download_queue_component::create(file_size, file_offset);
			download_queue->set_chunk_buffer(chunk_buffer);
			chunk_entity->add(download_queue);

			chunk_map->add(request_id, chunk_entity);

			call(function_type::transfer, function_action::add, chunk_entity);
		}

		// Session
		const auto session = entity->get<session_component>();
		if (!session)
		{
			m_socket->write(http_code::internal_server_error);
			return http_code::internal_server_error;
		}

		// Response
		const auto response_id = request_id;
		const auto response_packet = json_packet::create();
		const auto response_message = transfer_response_message::create(response_packet);
		response_message->set_id(response_id);

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
