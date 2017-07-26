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

#include <boost/format.hpp>

#include "command/upload_command.h"

#include "component/endpoint_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/client/client_io_component.h"
#include "component/client/client_network_component.h"
#include "component/io/download_component.h"
#include "component/io/file_component.h"
#include "component/io/transfer_component.h"
#include "component/io/upload_component.h"
#include "component/group/group_component.h"
#include "component/message/message_acceptor_component.h"
#include "component/message/message_network_component.h"
#include "component/status/status_component.h"
#include "component/time/timeout_component.h"
#include "component/time/timer_component.h"
#include "component/transfer/transfer_service_component.h"

#include "data/data_packer.h"
#include "data/data_unpacker.h"
#include "data/transfer_data.h"

#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "message/message_packer.h"
#include "message/message_unpacker.h"
#include "message/request/request_message.h"
#include "message/response/response_message.h"
#include "security/security.h"
#include "utility/value.h"

namespace eja
{
	// Client
	void client_upload_command::stop(const entity::ptr entity, const eja::status value, const bool stop /*= true*/)
	{
		bool done = true;
		const auto transfer = entity->get<transfer_component>();
		const auto upload_multimap = m_entity->get<upload_multimap_component>();
		{
			thread_lock(upload_multimap);
			const auto range = upload_multimap->equal_range(entity->get_id());
			auto it = range.first;

			if (it != upload_multimap->end())
			{
				upload_multimap->erase(it++);

				if (it != range.second)
				{
					// Transfer
					if (transfer->valid())
						transfer->sub_shards(1);
				}
				else
				{
					const auto upload_map = m_entity->get<upload_map_component>();
					{
						thread_lock(upload_map);
						upload_map->erase(entity->get_id());
					}

					// Transfer
					if (stop)
						transfer->stop();
					else if (transfer->valid())
						transfer->sub_shards(1);

					// Status
					const auto status = entity->get<status_component>();
					status->set_status(value);

					// Callback
					const auto e = entity_factory::create_statusbar(statusbar::upload);
					m_entity->call(callback_type::status, callback_action::remove, e);
				}
			}
		}
	}

	upload_request_message::ptr client_upload_command::execute(const entity::ptr router, const entity::ptr entity)
	{
		// Verify
		if (unlikely(!entity->has_id()))
			return nullptr;

		// Invalid
		const auto transfer = entity->get<transfer_component>();
		if (transfer->invalid())
		{
			stop(entity, eja::status::cancelled, false);
			return nullptr;
		}

		// Request		
		const auto request = upload_request_message::create();
		request->set_id(entity->get_id());

		// Upload
		size_t index = npos;
		const auto upload_index_map = entity->get<upload_index_map_component>();
		{
			thread_lock(upload_index_map);
			const auto range = upload_index_map->equal_range(router);

			if (range.first != range.second)
			{
				const auto it = range.first;
				index = it->second;

				upload_index_map->erase(it);
			}
		}

		// File
		if (index != npos)
		{
			const auto file = entity->get<file_component>();			
			const u64 offset = index * transfer->get_chunk();
			/*const*/ auto data = file->read(transfer->get_chunk(), offset);

			// Data
			if (!data.empty())
			{
				// Transfer
				std::pair<upload_index_set_component::iterator, bool> pair;
				const auto upload_set = entity->get<upload_index_set_component>();
				{
					thread_lock(upload_set);
					pair = upload_set->insert(index);					
				}

				if (pair.second)
					transfer->add_size(data.size());

				// Request				
				const auto request_data = transfer_data::create();
				request_data->set_index(index); 
				request_data->set_size(data.size());
				request_data->set_data(std::move(data));				

				/*const*/ auto packed = pack_data(request_data);
				request->set_data(std::move(packed));
				request->set_index(index);

				// Timeout
				const auto timeout = entity->get<timeout_component>();
				timeout->update();
			}
			else
			{
				// Status
				const auto status = entity->get<status_component>();
				status->set_status(eja::status::warning);

				// Log
				const auto parent = entity->get_parent();
				const auto str = boost::str(boost::format("Unable to read from file: %s") % file->get_name());
				parent->log(str);

				// TODO: Should be abort here?
				//
				//
			}
		}
		else
		{
			// Done?
			if (transfer->valid() && transfer->complete())
			{
				stop(entity, eja::status::success, true);
				return nullptr;
			}
		}

		return request;
	}

	void client_upload_command::execute(const entity::ptr router, const upload_response_message::ptr response)
	{
		// Verify
		if (unlikely(!response->has_id()))
			return;

		// Entity
		entity::ptr entity;
		const auto upload_map = m_entity->get<upload_map_component>();
		{
			thread_lock(upload_map);
			const auto it = upload_map->find(response->get_id());
			if (it == upload_map->cend())
				return;

			entity = it->second;
		}

		// Status		
		const auto network = m_entity->get<client_network_component>();

		if (response->has_status())
		{
			// Timeout
			const auto timeout = entity->get<timeout_component>();

			if (!timeout->expired(network->get_timeout()))
			{
				// Index
				if (response->has_index())
				{
					// Status
					const auto status = entity->get<status_component>();
					status->set_status(eja::status::success);

					const auto upload_index_map = entity->get<upload_index_map_component>();
					{
						const auto pair = std::make_pair(router, response->get_index());

						thread_lock(upload_index_map);
						upload_index_map->insert(pair);
					}

					// Request
					const auto service = router->get<transfer_service_component>();
					service->async_upload(entity);
				}
				else
				{
					// Status
					const auto status = entity->get<status_component>();
					status->set_status(!timeout->expired(milliseconds::progress) ? eja::status::success : eja::status::pending);

					// Request
					const auto service = router->get<transfer_service_component>();
					service->wait(milliseconds::upload, [service, entity]() { service->async_upload(entity); });
				}
			}
			else
			{
				stop(entity, eja::status::cancelled, true);
			}
		}
		else
		{
			// Timeout (Give a grace period)
			const auto timer = entity->get<timer_component>();
			const auto timeout = entity->get<timeout_component>();
			const auto transfer = entity->get<transfer_component>();

			if (transfer->complete() || (transfer->progress() && timer->elapsed(milliseconds::sync)) || timeout->expired(milliseconds::sync))
			{
				const auto value = !transfer->empty() ? eja::status::success : eja::status::none;
				stop(entity, value, true);
			}
			else
			{
				// Status
				const auto status = entity->get<status_component>();
				status->set_status(eja::status::pending);

				// Request
				const auto service = router->get<transfer_service_component>();
				const auto ms = !transfer->empty() ? milliseconds::upload : milliseconds::download;
				service->wait(ms, [service, entity]() { service->async_upload(entity); });
			}
		}
	}

	// Router
	upload_response_message::ptr router_upload_command::execute(const entity::ptr client, const upload_request_message::ptr request)
	{
		// Verify
		if (unlikely(!request->has_id()))
			return nullptr;

		// Response
		const auto response = upload_response_message::create();
		response->set_id(request->get_id());

		// Entity
		entity::ptr entity;
		const auto download_map = m_entity->get<download_map_component>();
		{
			thread_lock(download_map);
			const auto it = download_map->find(request->get_id());
			if (it == download_map->cend())
				return response;

			entity = it->second;
		}

		// Timeout
		const auto timeout = entity->get<timeout_component>();

		if (timeout->expired(milliseconds::transfer))
		{
			const auto download_map = m_entity->get<download_map_component>();
			{
				thread_lock(download_map);
				download_map->erase(request->get_id());

				// Callback
				m_entity->call(callback_type::download, callback_action::remove, entity);
			}

			return response;
		}

		// Upload
		const auto upload_list = entity->get<upload_data_list_component>();
		const auto download_list = entity->get<download_index_list_component>();

		if (request->has_index() && request->has_data())
		{
			const auto index = request->get_index();

			// Data
			const auto item = upload_data::create();			
			item->data = std::move(request->get_data());
			item->index = index;

			// Upload
			upload_list->add_buffer(item->data.size());
			{
				thread_lock(upload_list);
				upload_list->push_back(item);
			}

			// Download
			{
				thread_lock(download_list);
				download_list->remove(index);
			}
		}
		
		// Download		
		{
			const auto network = m_entity->get<message_network_component>();

			if (upload_list->get_buffer() < network->get_buffer())
			{				
				thread_lock(download_list);

				if (!download_list->empty())
				{
					const auto index = download_list->front();
					download_list->pop_front();
					response->set_index(index);
				}
			}
		}

		// Status
		response->set_success();

		return response;
	}
}
