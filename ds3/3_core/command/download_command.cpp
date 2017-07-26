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

#include "command/download_command.h"

#include "component/endpoint_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/client/client_io_component.h"
#include "component/client/client_network_component.h"
#include "component/client/client_service_component.h"
#include "component/io/download_component.h"
#include "component/io/file_component.h"
#include "component/io/transfer_component.h"
#include "component/io/upload_component.h"
#include "component/group/group_component.h"
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
	void client_download_command::stop(const entity::ptr entity, const eja::status value, const bool stop /*= true*/)
	{
		bool done = true;
		const auto transfer = entity->get<transfer_component>();
		const auto download_multimap = m_entity->get<download_multimap_component>();
		{
			thread_lock(download_multimap);
			const auto range = download_multimap->equal_range(entity->get_id());
			auto it = range.first;

			if (it != download_multimap->end())
			{
				download_multimap->erase(it++);

				if (it != range.second)
				{
					// Transfer
					if (transfer->valid())
						transfer->sub_shards(1);
				}
				else
				{
					const auto download_map = m_entity->get<download_map_component>();
					{
						thread_lock(download_map);
						download_map->erase(entity->get_id());
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
					const auto e = entity_factory::create_statusbar(statusbar::download);
					m_entity->call(callback_type::status, callback_action::remove, e);
				}
			}
		}
	}

	download_request_message::ptr client_download_command::execute(const entity::ptr router, const entity::ptr entity)
	{
		// Verify
		if (unlikely(!entity->has_id()))
			return nullptr;

		// Index
		const auto service = m_entity->get<client_service_component>();
		const auto transfer_list = service->get_transfer_list();
		if (transfer_list->empty())
			return nullptr;

		const auto transfer_index = find_index(*transfer_list, router);
		if (transfer_index == npos)
			return nullptr;

		// Request
		const auto request = download_request_message::create();
		request->set_id(entity->get_id());

		// Chunks		
		const auto transfer = entity->get<transfer_component>();
		if (transfer->valid())
		{
			// Timeout
			const auto timeout = entity->get<timeout_component>();
			const auto network = m_entity->get<client_network_component>();

			if (!timeout->expired(network->get_timeout()))
			{
				// Chunks
				std::vector<u32> chunks;
				const auto download_array = entity->get<download_index_array_component>();
				{
					thread_lock(download_array);

					if (!download_array->empty())
					{
						// 1) SPEED OPTIMIZATION: Send as large a window as possible to minimize uploader waits
						// But... this wastes network bandwidth :/
						const auto window = std::min(network->get_window(), download_array->size());

						// 2) BANDIWDTH OPTIMIZATION: Only send a window of chunks
						// But.. this makes transfer much slower (50% the speed) :/
						//const auto window = clamp(network::min_window, download_array->size() / transfer_list->size(), network->get_window());
						chunks.reserve(window);

						auto chunk_index = (transfer_index * (download_array->size() / transfer_list->size()));
						size_t chunk_end = chunk_index + window;

						if (chunk_end < download_array->size())
						{
							chunks.insert(chunks.end(), download_array->cbegin() + chunk_index, download_array->cbegin() + chunk_end);
						}
						else
						{
							chunks.insert(chunks.end(), download_array->cbegin() + chunk_index, download_array->cend());
							
							chunk_end -= download_array->size();
							if (chunk_end <= chunk_index)
								chunks.insert(chunks.end(), download_array->cbegin(), download_array->cbegin() + chunk_end);
						}
						
						// DEBUG
						assert(!chunks.empty());
					}
				}

				request->set_chunks(chunks);
			}
			else
			{
				// Status	
				const auto status = entity->get<status_component>();
				status->set_status(eja::status::cancelled);
			}
		}
		else
		{
			const auto value = transfer->complete() ? eja::status::success : eja::status::cancelled;
			stop(entity, value, true);
		}

		return request;
	}

	void client_download_command::execute(const entity::ptr router, const download_response_message::ptr response)
	{
		// Verify
		if (unlikely(!response->has_id()))
			return;

		// Entity
		entity::ptr entity;
		const auto download_map = m_entity->get<download_map_component>();
		{
			thread_lock(download_map);
			const auto it = download_map->find(response->get_id());
			if (it == download_map->cend())
				return;

			entity = it->second;
		}

		// Status	
		const auto status = entity->get<status_component>();		
		const auto transfer = entity->get<transfer_component>();

		if (!response->has_status())
		{
			const auto value = transfer->complete() ? eja::status::success : eja::status::cancelled;
			stop(entity, value, true);

			return;
		}

		// Data
		if (!response->has_data())
		{
			// Status
			if (!transfer->empty())
			{
				const auto timeout = entity->get<timeout_component>();
				status->set_status(!timeout->expired(milliseconds::progress) ? eja::status::success : eja::status::pending);
			}
			else
			{
				status->set_status(eja::status::pending);
			}

			const auto service = router->get<transfer_service_component>();
			service->wait(milliseconds::download, [service, entity]() { service->async_download(entity); });

			return;
		}

		// Unpack
		const auto unpacker = get_data_unpacker(response);

		for (const auto& data : *unpacker)
		{
			switch (data->get_type())
			{
				case data_type::transfer:
				{					
					const auto response_data = std::static_pointer_cast<transfer_data>(data);

					// Chunks
					auto index = response_data->get_index();
					const auto download_array = entity->get<download_index_array_component>();
					{
						thread_lock(download_array);
						const auto it = std::find(download_array->begin(), download_array->end(), index);
						if (it == download_array->cend())
						{
							const auto service = router->get<transfer_service_component>();
							service->async_download(entity);
							continue;
						}

						download_array->erase(it);
					}

					// File
					const u64 offset = index * transfer->get_chunk();
					const auto timeout = entity->get<timeout_component>();
					const auto network = m_entity->get<client_network_component>();

					const auto file = entity->get<file_component>();
					const auto& file_data = response_data->get_data();
					auto bytes = file->write(file_data.data(), file_data.size(), offset);

					// Try up to 10 times, then abort
					// TODO: Perhaps queue up and retry later (inefficient to re-download bytes)
					//
					for (auto i = 0; !bytes && (i < 10); ++i)
					{
						// Status
						status->set_status(eja::status::warning);

						// Log
						const auto parent = router->get_parent();
						const auto str = boost::str(boost::format("Unable to write to file: %s") % file->get_name());						
						parent->log(str);

						// Sleep
						std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds::file));

						// Retry
						bytes = file->write(file_data.data(), file_data.size(), offset);
					}

					if (bytes)
					{
						// Status
						status->set_status(eja::status::success);

						// Transfer
						transfer->add_size(static_cast<u32>(file_data.size()));

						// Timeout
						timeout->update();

						// Next
						const auto service = router->get<transfer_service_component>();
						service->async_download(entity);
					}
					else
					{
						thread_lock(download_array);
						download_array->push_front(index);
					}

					break;
				}
				default:
				{
					assert(false);
				}
			}
		}
	}

	// Router
	download_response_message::ptr router_download_command::execute(const entity::ptr client, const download_request_message::ptr request)
	{
		// Verify
		if (unlikely(!request->has_id()))
			return nullptr;

		// Response
		const auto response = download_response_message::create();
		response->set_id(request->get_id());

		// Entity
		entity::ptr entity;
		const auto download_map = m_entity->get<download_map_component>();
		{
			thread_lock(download_map);
			const auto it = download_map->find(request->get_id());
			if (it == download_map->cend())
			{
				entity = entity_factory::create_transfer_download(m_entity);
				entity->set_id(request->get_id());

				const auto pair = std::make_pair(request->get_id(), entity);
				download_map->insert(pair);

				// File
				const auto file = entity->get<file_component>();
				file->set_id(request->get_id());

				// Callback
				m_entity->call(callback_type::download, callback_action::add, entity);
			}
			else
			{
				entity = it->second;
			}
		}

		// Done/Timeout
		const auto timeout = entity->get<timeout_component>();

		if (!request->has_chunks() || timeout->expired(milliseconds::transfer))
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

		// Chunks
		const auto& chunks = request->get_chunks();
		std::list<u32> chunk_list(chunks.begin(), chunks.end());

		// Uploaded?
		{
			const auto upload_list = entity->get<upload_data_list_component>();
			thread_lock(upload_list);

			for (auto it = upload_list->begin(); it != upload_list->end(); )
			{
				const auto& item = *it;
				const auto index = item->index;

				const auto it2 = std::find(chunk_list.begin(), chunk_list.end(), index);
				if (it2 != chunk_list.end())
				{
					chunk_list.erase(it2);
					++it;
				}					
				else
				{
					upload_list->sub_buffer(item->data.size());
					it = upload_list->erase(it);
				}					
			}

			if (!upload_list->empty())
			{
				// Data
				auto& item = upload_list->front();
				upload_list->sub_buffer(item->data.size());

				response->set_data(std::move(item->data));
				upload_list->pop_front();

				// Timer				
				timeout->update();
			}
		}

		// Downloaded?		
		{
			const auto download_list = entity->get<download_index_list_component>();			
			thread_lock(download_list);

			for (auto it = download_list->begin(); it != download_list->end(); )
			{				
				const auto index = *it;

				const auto it2 = std::find(chunk_list.begin(), chunk_list.end(), index);
				if (it2 != chunk_list.end())
				{
					chunk_list.erase(it2);
					++it;
				}
				else
				{
					it = download_list->erase(it);
				}					
			}

			// New chunks
			std::copy(chunk_list.begin(), chunk_list.end(), std::back_inserter(*download_list));
		}		

		// Status
		response->set_success();

		return response;
	}
}
