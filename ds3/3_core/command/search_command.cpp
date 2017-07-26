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

#include "command/search_command.h"

#include "component/endpoint_component.h"
#include "component/name_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/client/client_network_component.h"
#include "component/group/group_component.h"
#include "component/io/file_component.h"
#include "component/io/share_component.h"
#include "component/message/message_acceptor_component.h"
#include "component/search/keyword_component.h"
#include "component/search/search_component.h"
#include "component/time/time_component.h"

#include "data/data_packer.h"
#include "data/data_unpacker.h"
#include "data/service_data.h"
#include "data/request/search_request_data.h"
#include "data/response/search_response_data.h"
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
	search_request_message::ptr client_search_command::execute(const entity::ptr router, const entity::ptr data)
	{
		assert(data->has_id());

		// Data		
		const auto request_data = search_request_data::create();
		const auto keyword = data->get<keyword_component>();
		request_data->set_text(keyword->get_text());

		// Packer
		data_packer packer;
		packer.push_back(request_data);
		auto packed = packer.pack();

		// Group
		const auto group = m_entity->get<group_component>();
		if (group->valid())
			packed = group->encrypt(packed);

		// Request
		const auto request = search_request_message::create();
		request->set_id(data->get_id());
		request->set_data(packed);

		return request;
	}
	
	void client_search_command::execute(const entity::ptr router, const entity::ptr client, const search_callback_message::ptr callback)
	{
		// N/A
	}

	search_response_message::ptr client_search_command::execute(const entity::ptr router, const search_request_message::ptr request)
	{		
		// Verify
		if (unlikely(!request->has_id()))
			return nullptr;

		// Ghosting (redundant check)
		const auto network = m_entity->get<client_network_component>();
		if (!network->has_search())
			return nullptr;

		// Data		
		const auto unpacker = get_data_unpacker(request);

		data_packer packer;
		const auto response = search_response_message::create();
		response->set_id(request->get_id());

		for (const auto& data : *unpacker)
		{
			switch (data->get_type())
			{
				case data_type::search_request:
				{					
					const auto request_data = std::static_pointer_cast<search_request_data>(data);					
					const auto response_data = search_response_data::create();

					std::map<std::string, file_data::ptr> file_map;					
					const auto share_map = m_entity->get<share_map_component>();

					const auto& text = request_data->get_text();
					string_tokenizer tokens(text, boost::char_separator<char>(search::delimiters));

					if (boost::starts_with(text, "\"") && boost::ends_with(text, "\""))
					{
						bool init = true;

						for (const std::string& token : tokens)
						{
							if (token.empty())
								continue;

							thread_lock(share_map);
							auto range = share_map->equal_range(token);
							std::map<std::string, file_data::ptr> map;

							for (auto it = range.first; it != range.second; )
							{
								const auto& e = it->second;

								if (e->has<file_component>())
								{
									const auto file = e->get<file_component>();

									if (init)
									{
										const auto fdata = file_data::create();
										fdata->set_id(file->get_id());
										fdata->set_name(file->get_name());
										fdata->set_size(file->get_size());
										fdata->add_weight();

										const auto pair = std::make_pair(file->get_id(), fdata);
										map.insert(pair);
									}
									else
									{
										const auto it = file_map.find(file->get_id());

										if (it != file_map.end())
										{
											const auto& fdata = it->second;
											const auto pair = std::make_pair(file->get_id(), fdata);
											map.insert(pair);
										}
									}
								}

								++it;
							}

							init = false;
							file_map.clear();
							file_map.insert(map.begin(), map.end());							
							if (map.empty())
								break;
						}
					}
					else
					{
						for (const std::string& token : tokens)
						{
							if (token.empty())
								continue;

							thread_lock(share_map);
							auto range = share_map->equal_range(token);

							for (auto it = range.first; it != range.second; )
							{
								const auto& e = it->second;

								if (e->has<file_component>())
								{
									const auto file = e->get<file_component>();
									auto& fdata = file_map[file->get_id()];

									if (!fdata)
									{
										fdata = file_data::create();
										fdata->set_id(file->get_id());
										fdata->set_name(file->get_name());
										fdata->set_size(file->get_size());
									}

									fdata->add_weight();
								}

								++it;
							}
						}
					}					

					// Response					
					for (const auto& pair : file_map)
					{
						const auto& fdata = pair.second;
						response_data->add(fdata);
					}

					// Empty?
					if (response_data->has_files())
						packer.push_back(response_data);

					break;
				}
				default:
				{
					assert(false);
				}
			}
		}

		// Packer
		if (!packer.empty())
		{
			auto packed = packer.pack();

			// Group
			const auto group = m_entity->get<group_component>();
			if (group->valid())
				packed = group->encrypt(packed);

			// Response
			response->set_data(packed);
		}

		return response;
	}


	void client_search_command::execute(const entity::ptr router, const search_response_message::ptr response)
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
				case data_type::search_response:
				{
					const auto response_data = std::static_pointer_cast<search_response_data>(data);

					// Files
					const auto& files = response_data->get_files();

					for (const auto& fdata : files)
					{
						const auto e = entity::create(m_entity);
						e->set_id(response->get_id());

						const auto search = search_component::create();
						search->set_weight(fdata->get_weight());
						e->add(search);

						const auto file = file_component::create();
						file->set_id(fdata->get_id());
						file->set_path(fdata->get_name());
						file->set_size(fdata->get_size());
						e->add(file);

						// Callback
						m_entity->call(callback_type::search, callback_action::add, e);
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
	search_callback_message::ptr router_search_command::execute(const entity::ptr client, const search_request_message::ptr request)
	{
		// Verify
		if (unlikely(!request->has_id()))
			return nullptr;		

		// Group
		const auto acceptor = m_entity->get<message_acceptor_component>();
		const auto group_map = m_entity->get<group_map_component>();
		{
			const auto group = client->get<group_component>();

			thread_lock(group_map);
			auto range = group_map->equal_range(group->get_id());

			for (auto it = range.first; it != range.second; ++it)
			{
				const auto& e = it->second;

				// Self
				if (client == e)
					continue;

				// Ghosting
				const auto network = e->get<client_network_component>();
				if (!network->has_search())
					continue;
				
				// TODO: XOR
				//
				const auto endpoint = client->get<endpoint_component>();
				request->set_client(endpoint->get_id());

				const auto state = service_data::create(service_action::search, client);
				acceptor->write_request(e, request, state);
			}
		}

		// Callback
		return search_callback_message::create();
	}

	void router_search_command::execute(const entity::ptr client, const entity::ptr data, const search_response_message::ptr response)
	{
		// Verify
		if (unlikely(!response->has_id()))
			return;

		// TODO: XOR
		//
		const auto endpoint = client->get<endpoint_component>();
		response->set_client(endpoint->get_id());

		const auto acceptor = m_entity->get<message_acceptor_component>();
		const auto state = service_data::create(service_action::search);		
		acceptor->write_request(data, response, state);
	}
}
