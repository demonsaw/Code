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

#include <regex>

#include "command/browse_command.h"

#include "component/browse_component.h"
#include "component/endpoint_component.h"
#include "component/name_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/client/client_network_component.h"
#include "component/client/client_service_component.h"
#include "component/group/group_component.h"
#include "component/io/file_component.h"
#include "component/io/folder_component.h"
#include "component/io/save_component.h"
#include "component/io/share_component.h"
#include "component/message/message_acceptor_component.h"
#include "component/message/message_service_component.h"
#include "component/time/time_component.h"

#include "data/data_packer.h"
#include "data/data_unpacker.h"
#include "data/service_data.h"
#include "data/request/browse_request_data.h"
#include "data/response/browse_response_data.h"
#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "message/message_packer.h"
#include "message/message_unpacker.h"
#include "message/request/request_message.h"
#include "message/response/response_message.h"
#include "security/security.h"
#include "utility/value.h"
#include "utility/router/router_util.h"

namespace eja
{
	// Client
	browse_request_message::ptr client_browse_command::execute(const entity::ptr router, const entity::ptr data)
	{
		const auto id = security::random(security::s_max);
		data->set_id(id);

		const auto browse_map = m_entity->get<browse_map_component>();
		{
			const auto pair = std::make_pair(id, data);

			thread_lock(browse_map);
			browse_map->insert(pair);
		}

		// Data
		const auto request_data = browse_request_data::create();

		if (data->has<folder_component>())
		{
			const auto folder = data->get<folder_component>();
			request_data->set_folder(folder->get_id());
		}

		// Packer
		data_packer packer;
		packer.push_back(request_data);
		auto packed = packer.pack();

		// Group
		const auto group = m_entity->get<group_component>();
		if (group->valid())
			packed = group->encrypt(packed);

		// Request
		const auto request = browse_request_message::create();
		request->set_id(id);

		const auto endpoint = data->find<endpoint_component>();
		request->set_client(endpoint->get_id());
		request->set_data(packed);

		return request;
	}

	void client_browse_command::execute(const entity::ptr router, const entity::ptr client, const browse_callback_message::ptr callback)
	{
		// N/A
	}

	browse_response_message::ptr client_browse_command::execute(const entity::ptr router, const browse_request_message::ptr request)
	{		
		// Verify
		if (unlikely(!request->has_id()))
			return nullptr;

		// Ghosting (redundant check)
		const auto network = m_entity->get<client_network_component>();
		if (!network->has_browse())
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
		const auto response = browse_response_message::create();
		response->set_id(request->get_id());

		for (const auto& data : unpacker)
		{
			switch (data->get_type())
			{
				case data_type::browse_request:
				{
					const auto request_data = std::static_pointer_cast<browse_request_data>(data);
					const auto response_data = browse_response_data::create();

					if (request_data->has_folder())
					{
						const auto& folder_id = request_data->get_folder();
						const auto folder_map = m_entity->get<folder_map_component>();
						{
							thread_lock(folder_map);
							const auto it = folder_map->find(folder_id);
							
							if (it != folder_map->end())
							{
								const auto& entity = it->second;
								const auto folder = entity->get<folder_component>();

								// Folders
								const auto folder_list = folder->get_folders();
								{
									for (const auto& e : *folder_list)
									{
										const auto folder = e->get<folder_component>();

										const auto fdata = folder_data::create();
										fdata->set_id(folder->get_id());
										fdata->set_name(folder->get_name());
										fdata->set_size(folder->get_size());

										response_data->add(fdata);
									}
								}

								// Files
								const auto file_list = folder->get_files();
								{
									for (const auto& e : *file_list)
									{
										const auto file = e->get<file_component>();

										const auto fdata = file_data::create();
										fdata->set_id(file->get_id());
										fdata->set_name(file->get_name());
										fdata->set_size(file->get_size());

										response_data->add(fdata);
									}
								}
							}
						}						
					}
					else
					{
						const auto service = m_entity->get<client_service_component>();
						const auto share_list = service->get_share_list();

						for (const auto& e : *share_list)
						{
							if (e->has<folder_component>())
							{
								const auto folder = e->get<folder_component>();
								
								const auto fdata = folder_data::create();
								fdata->set_id(folder->get_id());
								fdata->set_name(folder->get_name());
								fdata->set_size(folder->get_size());
								
								response_data->add(fdata);
							}
							else
							{
								const auto file = e->get<file_component>();
								
								const auto fdata = file_data::create();
								fdata->set_id(file->get_id());
								fdata->set_name(file->get_name());
								fdata->set_size(file->get_size());

								response_data->add(fdata);
							}
						}
					}

					// Empty?
					if (response_data->has_files() || response_data->has_folders())
						packer.push_back(response_data);

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
		}

		// Callback
		const auto e = entity_factory::create_statusbar(statusbar::browse);
		m_entity->call(callback_type::status, callback_action::update, e);

		return response;
	}


	void client_browse_command::execute(const entity::ptr router, const browse_response_message::ptr response)
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
				case data_type::browse_response:
				{					
					const auto response_data = std::static_pointer_cast<browse_response_data>(data);

					entity::ptr parent;
					const auto browse_map = m_entity->get<browse_map_component>();
					{
						thread_lock(browse_map);
						const auto it = browse_map->find(response->get_id());
						if (it == browse_map->end())
							continue;

						parent = it->second;
					}
					
					// Save
					std::string root;
					const auto save_map = m_entity->get<save_map_component>();
					{
						thread_lock(save_map);
						const auto it = save_map->find(response->get_id());
						if (it != save_map->end())
						{
							root = it->second;
							save_map->erase(it);
						}
					}

					if (root.empty())
					{
						// Files
						const auto& files = response_data->get_files();
						
						for (const auto& fdata : files)
						{
							assert(parent);
							const auto e = entity_factory::create_client_browse(parent);

							const auto file = file_component::create();
							file->set_id(fdata->get_id());
							file->set_path(fdata->get_name());
							file->set_size(fdata->get_size());
							e->add(file);

							// Callback
							m_entity->call(callback_type::browse, callback_action::add, e);
						}

						// Folders
						const auto& folders = response_data->get_folders();

						for (const auto& fdata : folders)
						{
							assert(parent);
							const auto e = entity_factory::create_client_browse(parent);

							const auto folder = folder_component::create();
							folder->set_id(fdata->get_id());
							folder->set_path(fdata->get_name());
							folder->set_size(fdata->get_size());
							e->add(folder);

							// Callback
							m_entity->call(callback_type::browse, callback_action::add, e);
						}
					}
					else
					{
						const auto client_service = m_entity->get<client_service_component>();
						const auto endpoint = parent->find<endpoint_component>();
						const auto client = endpoint->get_entity();

						// Files
						const auto& files = response_data->get_files();

						for (const auto& fdata : files)
						{
							assert(parent);
							const auto e = entity_factory::create_client_browse(parent);

							const auto file = file_component::create();
							file->set_id(fdata->get_id());
							file->set_path(fdata->get_name());
							file->set_size(fdata->get_size());
							e->add(file);

							// Callback
							m_entity->call(callback_type::browse, callback_action::add, e);

							// Download
							client_service->async_file(e, root, client);
						}

						// Folders
						const auto message_service = router->get<message_service_component>();
						const auto& folders = response_data->get_folders();

						for (const auto& fdata : folders)
						{
							assert(parent);
							const auto e = entity_factory::create_client_browse(parent);

							const auto folder = folder_component::create();
							folder->set_id(fdata->get_id());
							folder->set_path(fdata->get_name());
							folder->set_size(fdata->get_size());
							e->add(folder);

							// Callback
							m_entity->call(callback_type::browse, callback_action::add, e);

							// Download
							message_service->async_folder(e, root);
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
	browse_callback_message::ptr router_browse_command::execute(const entity::ptr client, const browse_request_message::ptr request)
	{	
		// Verify
		if (unlikely(!request->has_id()))
			return nullptr;

		// Exist
		const auto& client_id = request->get_client();
		const auto entity = router_util::get_client(m_entity, client_id);
		if (!entity)
			return nullptr;

		// Group
		if (unlikely(!entity->equals<group_component>(client)))
			return nullptr;

		// Ghosting
		const auto network = entity->get<client_network_component>();
		if (!network->has_browse())
			return nullptr;

		// TODO: Update the client id (xor back to normal)
		//
		//
		
		const auto acceptor = m_entity->get<message_acceptor_component>();
		const auto state = service_data::create(service_action::browse, client);
		acceptor->write_request(entity, request, state);

		// Callback
		return browse_callback_message::create();
	}

	void router_browse_command::execute(const entity::ptr client, const entity::ptr data, const browse_response_message::ptr response)
	{
		// Verify
		if (unlikely(!response->has_id()))
			return;

		// TODO: XOR
		//
		const auto endpoint = client->get<endpoint_component>();
		response->set_client(endpoint->get_id());

		const auto acceptor = m_entity->get<message_acceptor_component>();
		const auto state = service_data::create(service_action::browse);
		acceptor->write_request(data, response, state);
	}
}
