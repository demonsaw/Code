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

#include <stdlib.h>
#include <boost/format.hpp>

#include "command/file_command.h"

#include "component/endpoint_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/client/client_io_component.h"
#include "component/client/client_network_component.h"
#include "component/client/client_service_component.h"
#include "component/io/download_component.h"
#include "component/io/file_component.h"
#include "component/io/transfer_component.h"
#include "component/io/folder_component.h"
#include "component/io/upload_component.h"
#include "component/group/group_component.h"
#include "component/message/message_acceptor_component.h"
#include "component/status/status_component.h"
#include "component/time/timer_component.h"
#include "component/transfer/transfer_service_component.h"

#include "data/data_packer.h"
#include "data/data_unpacker.h"
#include "data/service_data.h"
#include "data/request/file_request_data.h"
#include "data/response/file_response_data.h"
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
	file_request_message::ptr client_file_command::execute(const entity::ptr router, const entity::ptr data, const std::shared_ptr<entity> client /*= nullptr*/)
	{
		assert(data->has_id());

		// Data
		const auto request_data = file_request_data::create();

		// File
		const auto file = data->get<file_component>();
		request_data->set_file(file->get_id());

		// Routers
		const auto service = m_entity->get<client_service_component>();
		const auto transfer_list = service->get_transfer_list();
		for (const auto& e : *transfer_list)
		{
			if (e->enabled())
			{
				const auto endpoint = e->get<endpoint_component>();
				
				const auto rdata = router_data::create();
				rdata->set_address(endpoint->get_address());
				rdata->set_port(endpoint->get_port());

				request_data->add(rdata);
			}
		}

		// Chunk
		const auto transfer = data->get<transfer_component>();
		request_data->set_chunk(transfer->get_chunk());

		// Packer
		data_packer packer;
		packer.push_back(request_data);
		auto packed = packer.pack();

		// Group
		const auto group = m_entity->get<group_component>();
		if (group->valid())
			packed = group->encrypt(packed);

		// Request
		const auto request = file_request_message::create();
		request->set_id(data->get_id());
		request->set_data(packed);

		if (client)
		{
			const auto endpoint = client->get<endpoint_component>();
			request->set_client(endpoint->get_id());
		}
		else if (data->has<endpoint_component>())
		{
			const auto endpoint = data->get<endpoint_component>();
			request->set_client(endpoint->get_id());
		}

		return request;
	}

	void client_file_command::execute(const entity::ptr router, const entity::ptr client, const file_callback_message::ptr callback)
	{
		// N/A
	}

	file_response_message::ptr client_file_command::execute(const entity::ptr router, const file_request_message::ptr request)
	{
		// Verify
		if (unlikely(!request->has_id()))
			return nullptr;

		// Ghosting (redundant check)
		const auto network = m_entity->get<client_network_component>();
		if (!network->has_upload())
			return nullptr;

		// Unpack
		data_unpacker unpacker;
		const auto group = m_entity->get<group_component>();

		if (group->valid())
		{
			const auto packed = group->decrypt(request->get_data());
			unpacker.unpack(packed.data(), packed.size());
		}
		else
		{
			const auto& packed = request->get_data();
			unpacker.unpack(packed.data(), packed.size());
		}

		// Loop
		data_packer packer;
		const auto response = file_response_message::create();
		response->set_id(request->get_id());

		for (const auto& data : unpacker)
		{
			switch (data->get_type())
			{
				case data_type::file_request:
				{
					// Valid?
					const auto request_data = std::static_pointer_cast<file_request_data>(data);
					if (request_data->invalid())
						continue;

					// File
					entity::ptr entity;
					const auto& file_id = request_data->get_file();
					const auto file_map = m_entity->get<file_map_component>();
					{
						thread_lock(file_map);
						const auto it = file_map->find(file_id);
						if (it == file_map->end())
							continue;

						// Entity
						entity = entity_factory::create_client_upload(m_entity);
						entity->add<file_component>(it->second);
						entity->set_id(request->get_id());
					}

					// Chunks
					const size_t chunk_size = request_data->get_chunk();					
					const auto transfer = entity->get<transfer_component>();
					transfer->set_chunk(chunk_size);

					// Shards
					const auto response_data = file_response_data::create();
					const auto network = m_entity->get<client_network_component>();					
					response_data->set_shards(network->get_threads());
					
					// Random shuffle
					const auto service = m_entity->get<client_service_component>();
					const auto transfer_list = service->get_transfer_list()->copy();
					std::random_shuffle(transfer_list->begin(), transfer_list->end());
					
					// Limit the total number of xfr routers
					const auto file = entity->get<file_component>();
					const size_t chunks = (file->get_size() / chunk_size) + ((file->get_size() % chunk_size) ? 1 : 0);
					const auto total = std::min((chunks / network->get_window()) + 1, network::max_routers);					

					// Remove non-matches					
					const auto& rdata_list = request_data->get_routers();
					const auto list = transfer_list_component::create();

					for (const auto& e : *transfer_list)
					{
						const auto status = e->get<status_component>();
						if (status->is_error())
							continue;

						const auto rit = std::find_if(rdata_list.cbegin(), rdata_list.cend(), [e](const router_data::ptr& rdata)
						{
							const auto endpoint = e->get<endpoint_component>();
							return (endpoint->get_address() == rdata->get_address()) && (endpoint->get_port() == rdata->get_port());
						});

						if (rit != rdata_list.cend())
						{
							// Response
							const auto& rdata = *rit;
							response_data->add(rdata);

							// Transfer
							transfer->add_shards(network->get_threads());
							list->push_back(e);

							if (list->size() >= total)
								break;
						}
					}

					// Empty?
					if (!list->empty())
					{		
						// Max
						const auto pair = std::make_pair(request->get_id(), entity);
						const auto upload_map = m_entity->get<upload_map_component>();
						{														
							thread_lock(upload_map);

							if (upload_map->size() >= network->get_uploads())
							{
								const auto file = entity->get<file_component>();
								const auto str = boost::str(boost::format("Unable to upload \"%s\" because you're already uploading %u files") % file->get_name() % network->get_uploads());
								m_entity->log(str);
								continue;
							}
							
							upload_map->insert(pair);
						}

						// Multimap
						const auto upload_multimap = m_entity->get<upload_multimap_component>();
						{							
							thread_lock(upload_multimap);
							for (auto i = 0; i < list->size(); ++i)
								upload_multimap->insert(pair);
						}

						// Callback
						m_entity->call(callback_type::upload, callback_action::add, entity);

						// Status
						const auto status = entity->get<status_component>();
						status->set_status(eja::status::pending);

						// Upload
						for (const auto& e : *list)
						{
							// Wait (for the downloader to contact the transfer routers)
							const auto service = e->get<transfer_service_component>();
							for (auto i = 0; i < network->get_threads(); ++i)
								service->async_upload(entity);
						}

						// Packer
						packer.push_back(response_data);
					}

					break;
				}
			}
		}

		// Packer
		if (!packer.empty())
		{
			// Packer		
			auto packed = packer.pack();

			// Group
			if (group->valid())
				packed = group->encrypt(packed);

			// Response
			response->set_data(packed);

			// Callback
			if (request->has_client())
			{
				const auto& client_id = request->get_client();
				const auto endpoint = m_entity->get<endpoint_component>();
				if (endpoint->get_id() != client_id)
				{
					const auto e = entity_factory::create_statusbar(statusbar::upload);
					m_entity->call(callback_type::status, callback_action::update, e);
				}
			}
		}

		return response;
	}


	void client_file_command::execute(const entity::ptr router, const file_response_message::ptr response)
	{
		// Verify
		if (unlikely(!response->has_id()))
			return;

		// Unpack
		data_unpacker unpacker;
		const auto group = m_entity->get<group_component>();

		if (group->valid())
		{
			const auto packed = group->decrypt(response->get_data());
			unpacker.unpack(packed.data(), packed.size());
		}
		else
		{
			const auto& packed = response->get_data();
			unpacker.unpack(packed.data(), packed.size());
		}

		// Loop
		for (const auto& data : unpacker)
		{
			switch (data->get_type())
			{
				case data_type::file_response:
				{
					const auto response_data = std::static_pointer_cast<file_response_data>(data);

					// Entity
					entity::ptr entity;
					const auto download_map = m_entity->get<download_map_component>();
					{
						thread_lock(download_map);
						const auto it = download_map->find(response->get_id());
						if (it == download_map->end())
							continue;

						entity = it->second;
					}

					// Routers
					const auto& rdata_list = response_data->get_routers();
					if (rdata_list.empty())
						continue;

					// Verify					
					const auto client_service = m_entity->get<client_service_component>();
					const auto client_transfer_list = client_service->get_transfer_list();

					for (const auto& rdata : rdata_list)
					{
						const auto it = std::find_if(client_transfer_list->cbegin(), client_transfer_list->cend(), [rdata](const entity::ptr e)
						{
							const auto endpoint = e->get<endpoint_component>();
							return (endpoint->get_address() == rdata->get_address()) && (endpoint->get_port() == rdata->get_port());
						});

						if (it != client_transfer_list->cend())
						{
							// Transfer router entity
							const auto& transfer_entity = *it;

							// Threads
							const auto transfer = entity->get<transfer_component>();
							transfer->add_shards(response_data->get_shards());							

							// Transfers
							const auto transfer_list = entity->get<transfer_list_component>();
							{
								const auto endpoint = transfer_entity->get<endpoint_component>();

								thread_lock(transfer_list);
								const auto it = std::find_if(transfer_list->cbegin(), transfer_list->cend(), [endpoint](const entity::ptr e)
								{ 
									const auto comp = e->get<endpoint_component>();
									return (endpoint->get_address() == comp->get_address()) && (endpoint->get_port() == comp->get_port());
								});
								
								// Only add transfer routers that aren't already in use
								if (it == transfer_list->cend())
								{
									// Multimap
									const auto pair = std::make_pair(response->get_id(), entity);
									const auto network = m_entity->get<client_network_component>();									
									const auto download_multimap = m_entity->get<download_multimap_component>();
									{
										thread_lock(download_multimap);
										for (auto i = 0; i < network->get_threads(); ++i)
											download_multimap->insert(pair);
									}

									transfer_list->push_back(transfer_entity);
									
									const auto service = transfer_entity->get<transfer_service_component>();
									for (auto i = 0; i < network->get_threads(); ++i)
										service->async_download(entity);
								}
							}
						}
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
	file_callback_message::ptr router_file_command::execute(const entity::ptr client, const file_request_message::ptr request)
	{
		// Verify
		if (unlikely(!request->has_id()))
			return nullptr;

		const auto acceptor = m_entity->get<message_acceptor_component>();

		if (!request->has_client())
		{
			// Group			
			const auto group_map = m_entity->get<group_map_component>();
			{
				const auto group = client->get<group_component>();

				thread_lock(group_map);
				auto range = group_map->equal_range(group->get_id());

				for (auto it = range.first; it != range.second; ++it)
				{
					const auto& e = it->second;

					// Self
					/*if (client == e)
						continue;*/

					// Ghosting
					const auto network = e->get<client_network_component>();
					if (!network->has_upload())
						continue;

					// TODO: XOR
					//
					//
					const auto endpoint = client->get<endpoint_component>();
					request->set_client(endpoint->get_id());

					const auto state = service_data::create(service_action::file, client);
					acceptor->write_request(e, request, state);
				}
			}
		}
		else
		{
			const auto& client_id = request->get_client();
			const auto client_map = m_entity->get<client_map_component>();
			{
				thread_lock(client_map);
				const auto it = client_map->find(client_id);

				if (it != client_map->end())
				{
					const auto& e = it->second;

					/*if (client == e)
						return nullptr;*/

					// Group
					if (unlikely(!e->equals<group_component>(client)))
						return nullptr;

					// Ghosting
					const auto network = e->get<client_network_component>();
					if (!network->has_upload())
						return nullptr;

					// TODO: XOR
					//
					//
					const auto endpoint = client->get<endpoint_component>();
					request->set_client(endpoint->get_id());

					const auto state = service_data::create(service_action::file, client);
					acceptor->write_request(e, request, state);
				}
			}
		}		

		// Callback
		return file_callback_message::create();
	}

	void router_file_command::execute(const entity::ptr client, const entity::ptr data, const file_response_message::ptr response)
	{
		// Verify
		if (unlikely(!response->has_id()))
			return;

		// TODO: XOR
		//
		const auto endpoint = client->get<endpoint_component>();
		response->set_client(endpoint->get_id());
		
		const auto acceptor = m_entity->get<message_acceptor_component>();
		const auto state = service_data::create(service_action::file);
		acceptor->write_request(data, response, state);
	}
}
