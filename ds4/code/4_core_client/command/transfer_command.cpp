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

#include <random>
#include <thread>

#include "command/group_command.h"
#include "command/transfer_command.h"

#include "component/callback/callback.h"
#include "component/client/client_component.h"
#include "component/io/file_component.h"
#include "component/message/message_component.h"
#include "component/router/router_component.h"
#include "component/session/session_component.h"
#include "component/time/timeout_component.h"
#include "component/transfer/transfer_component.h"
#include "component/transfer/upload_component.h"
#include "component/transfer/upload_service_component.h"

#include "data/request/transfer_request_data.h"
#include "data/response/transfer_response_data.h"
#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "entity/entity_util.h"
#include "factory/client_factory.h"
#include "security/filter/hex.h"
#include "security/hash/hash.h"
#include "utility/io/file_util.h"

namespace eja
{
	// Interface
	group_request_message::ptr transfer_command::execute(const entity::ptr& entity) const
	{				
		// Message
		const auto transfer = entity->get<transfer_component>();
		if (!transfer->has_id())
		{
			const auto message_entity = entity_factory::create_timeout(m_entity);
			message_entity->set_data(entity);

			const auto message_id = entity_util::add<message_map_component>(m_entity, message_entity);
			transfer->set_id(message_id);
		}
		else
		{
			const auto message_map = m_entity->get<message_map_component>();
			{
				thread_lock(message_map);
				const auto it = message_map->find(transfer->get_id());

				if (it != message_map->end())
				{
					// Timeout
					const auto& message_entity = it->second;
					const auto timeout = message_entity->get<timeout_component>();
					timeout->update();
				}
				else
				{
					const auto message_entity = entity_factory::create_timeout(m_entity);
					message_entity->set_data(entity);

					const auto pair = std::make_pair(transfer->get_id(), message_entity);
					const auto result = message_map->insert(pair);
					assert(result.second);
				}
			}
		}

		// Data
		const auto file = entity->get<file_component>();
		const auto request_data = transfer_request_data::create(file->get_id());
		request_data->set_chunk_size(transfer->get_chunk_size());

		// Command
		group_command cmd(m_entity);
		const auto request_message = cmd.get_request_message(request_data);
		request_message->set_id(transfer->get_id());
		request_message->set_share(true);

		if (entity->has<client_component>())
		{
			const auto client = entity->get<client_component>();
			request_message->set_client(client->get_id());
		}

		return request_message;
	}

	transfer_response_data::ptr transfer_command::execute(const client_request_message::ptr& request_message, const transfer_request_data::ptr& request_data) const
	{
		// Validate
		if (unlikely(!validate(request_message, request_data)))
			return nullptr;

		// Entity
		entity::ptr client_entity;
		const auto entity_map = m_entity->get<entity_map_component>();
		{
			thread_lock(entity_map);
			const auto it = entity_map->find(request_message->get_origin());
			if (it != entity_map->end())
				client_entity = it->second;
		}

		// Muted?
		if (likely(client_entity))
		{
			const auto client = client_entity->get<client_component>();
			if (client->is_mute())
				return nullptr;
		}

		// 1) Availability
		const auto client = m_entity->get<client_component>();

		entity::ptr upload_entity;
		const auto upload_map = m_entity->get<upload_map_component>();
		{
			thread_lock(upload_map);
			const auto it = upload_map->find(request_message->get_id());

			if (it == upload_map->end())
			{
				if (upload_map->size() >= client->get_uploads())
					return nullptr;
			}
			else
			{
				upload_entity = it->second;
			}
		}

		if (!upload_entity)
		{
			// 2) File
			entity::ptr file_entity;
			const auto file_map = m_entity->get<file_map_component>();
			{
				thread_lock(file_map);
				const auto it = file_map->find(request_data->get_file());
				if (it != file_map->end())
					file_entity = it->second;
			}

			if (file_entity)
			{				
				// 3) Verify
				const auto hash = hash::create(client->get_hash());
				hash->set_salt(client->get_salt());

				const auto file = file_entity->get<file_component>();
				const auto file_id = file_util::get_id(file, hash);
				if (file_id != request_data->get_file())
					return nullptr;

				// 4) Router
				const auto transfer_list = m_entity->get<transfer_list_component>();
				if (!transfer_list->empty())
				{
					// File
					const auto upload_entity = client_factory::create_upload(m_entity, file_entity);
					const auto transfer = upload_entity->get<transfer_component>();
					transfer->set_chunk_size(request_data->get_chunk_size());
					transfer->set_id(request_message->get_id());
					transfer->seed();

					// Router
					std::random_device rnd;
					std::uniform_int_distribution<size_t> dist(0, transfer_list->size() - 1);
					const auto i = dist(rnd);
					
					const auto router_entity = transfer_list->at(i);
					const auto router = router_entity->get<router_component>();
					upload_entity->set<router_component>(router_entity);

					// Session
					if (router->has_password())
					{
						const auto session = upload_entity->get<session_component>();
						session->compute(router->get_password());
					}

					// Callback
					m_entity->async_call(callback::upload | callback::add, upload_entity);

					// Response
					const auto response_data = transfer_response_data::create();
					response_data->set_address(router->get_address());
					response_data->set_port(router->get_port());

					return response_data;
				}
			}
		}
		else
		{
			// Response
			const auto response_data = transfer_response_data::create();

			// Router
			const auto router = upload_entity->get<router_component>();
			response_data->set_address(router->get_address());
			response_data->set_port(router->get_port());

			return response_data;
		}		

		return nullptr;
	}

	void transfer_command::execute(const client_response_message::ptr& response_message, const transfer_response_data::ptr& response_data) const
	{
		// Validate
		if (unlikely(!validate(response_message, response_data)))
			return;

		// Message
		entity::ptr message_entity;
		const auto message_map = m_entity->get<message_map_component>();
		{
			thread_lock(message_map);
			const auto it = message_map->find(response_message->get_id());
			if (it != message_map->end())
				message_entity = it->second;
		}

		if (message_entity)
		{
			// Data
			const auto& transfer_entity = message_entity->get_data();
			if (!transfer_entity)
				return;

			const auto timeout = transfer_entity->get<timeout_component>();
			timeout->update();

			// Router
			entity::ptr router_entity;
			const auto router_id = response_data->get_endpoint();
			const auto transfer_map = m_entity->get<transfer_map_component>();
			{
				thread_lock(transfer_map);
				const auto it = transfer_map->find(router_id);
				if (it != transfer_map->end())
					router_entity = it->second;
			}
			
			if (router_entity)
			{
				// Transfer
				const auto transfer = transfer_entity->get<transfer_component>();
				transfer->seed();

				// Session
				const auto download_entity = client_factory::create_download_router(transfer_entity, router_entity);
				const auto router = router_entity->get<router_component>();

				if (router->has_password())
				{
					const auto session = download_entity->get<session_component>();
					session->compute(router->get_password());
				}

				// No dupes
				const auto tmap = transfer_entity->get<transfer_map_component>();
				{
					thread_lock(tmap);
					const auto it = tmap->find(router_id);
					
					if (it == tmap->end())
					{
						// Insert
						const auto pair = std::make_pair(router_id, download_entity);
						tmap->insert(pair);

						// Add
						const auto transfer_list = transfer_entity->get<transfer_list_component>();
						{
							thread_lock(transfer_list);
							transfer_list->push_back(download_entity);
						}
					}
				}				
			}
		}
	}

	// Utility
	bool transfer_command::validate(const client_request_message::ptr& request_message, const transfer_request_data::ptr& request_data) const
	{
		return request_message->has_id() && request_data->valid();
	}

	bool transfer_command::validate(const client_response_message::ptr& response_message, const transfer_response_data::ptr& response_data) const
	{
		return response_message->has_id() && response_data->valid();
	}
}
