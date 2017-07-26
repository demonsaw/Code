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

#include "command/chat_command.h"

#include "component/chat_component.h"
#include "component/endpoint_component.h"
#include "component/name_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/client/client_network_component.h"
#include "component/group/group_component.h"
#include "component/message/message_acceptor_component.h"

#include "data/data_packer.h"
#include "data/data_unpacker.h"
#include "data/request/chat_request_data.h"
#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "message/message_packer.h"
#include "message/message_unpacker.h"
#include "message/request/request_message.h"
#include "message/response/response_message.h"
#include "utility/value.h"
#include "utility/router/router_util.h"

namespace eja
{
	// Client
	chat_request_message::ptr client_chat_command::execute(const entity::ptr router, const entity::ptr data, const entity::ptr client /*= nullptr*/)
	{
		// Data
		const auto request_data = chat_request_data::create();
		const auto chat = data->get<chat_component>();
		request_data->set_text(chat->get_text());
		request_data->set_pm(client != nullptr);

		// Packer
		data_packer packer;
		packer.push_back(request_data);
		auto packed = packer.pack();

		// Group
		const auto group = m_entity->get<group_component>();
		if (group->valid())
			packed = group->encrypt(packed);

		// Request
		const auto request = chat_request_message::create();		
		request->set_data(packed);

		if (client)
		{
			const auto endpoint = client->get<endpoint_component>();
			request->set_client(endpoint->get_id());
		}

		return request;
	}

	chat_response_message::ptr client_chat_command::execute(const entity::ptr router, const chat_request_message::ptr request)
	{
		// TODO: What about empty data? Error condition?
		//
		//

		data_unpacker unpacker;

		// Group
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
		for (const auto& data : unpacker)
		{
			switch (data->get_type())
			{
				case data_type::chat_request:
				{
					const auto request_data = std::static_pointer_cast<chat_request_data>(data);

					// Ghosting (redundant check)
					const auto network = m_entity->get<client_network_component>();
					if ((!network->has_chat() && !request_data->has_pm()) || (!network->has_pm() && request_data->has_pm()))
						continue;

					// New or existing?
					const auto& client_id = request->get_client();
					entity::ptr client = router_util::get_client(router, client_id);					

					if (client)
					{
						// Muted?
						const auto endpoint = client->get<endpoint_component>();
						if (endpoint->muted())
							continue;

						// Update
						endpoint->set_name(request->get_name());
						endpoint->set_color(request->get_color());
					}
					else
					{
						// New
						client = entity_factory::create_client_client(m_entity);

						const auto endpoint = client->get<endpoint_component>();
						endpoint->set_id(client_id);
						endpoint->set_name(request->get_name());
						endpoint->set_color(request->get_color());

						const auto pair = std::make_pair(client_id, client);
						const auto client_map = router->get<client_map_component>();
						{
							thread_lock(client_map);
							client_map->insert(pair);
						}						

						// Callback
						m_entity->call(callback_type::client, callback_action::add, client);
					}

					// Chat
					const auto echat = entity_factory::create_client_chat(client);
					echat->add<endpoint_component>(client);

					const auto chat = echat->get<chat_component>();
					chat->set_text(request_data->get_text());

					if (request_data->has_pm())
						chat->set_client(client_id);
					
					// Callback
					m_entity->call(callback_type::chat, callback_action::add, echat);

					break;
				}
				default:
				{
					assert(false);
				}
			}
		}

		// Callback
		const auto e = entity_factory::create_statusbar(statusbar::chat);
		m_entity->call(callback_type::status, callback_action::update, e);

		return chat_response_message::create();
	}

	void client_chat_command::execute(const entity::ptr router, const chat_response_message::ptr response)
	{
		// N/A
	}

	// Router
	chat_response_message::ptr router_chat_command::execute(const entity::ptr client, const chat_request_message::ptr request)
	{		
		// TODO: Update the client id (xor back to normal)
		//
		//

		// Acceptor
		const auto acceptor = m_entity->get<message_acceptor_component>();

		if (!request->has_client())
		{
			// Group
			const auto group = client->get<group_component>();
			const auto group_map = m_entity->get<group_map_component>();
			{
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
					if (!network->has_chat())
						continue;
					
					// TODO: XOR
					//
					const auto endpoint = client->get<endpoint_component>();
					request->set_client(endpoint->get_id());
					request->set_name(endpoint->get_name());
					request->set_color(endpoint->get_color());

					acceptor->write_request(e, request);
				}
			}
		}
		else
		{
			// PM
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
					if (!network->has_pm())
						return nullptr;

					// TODO: XOR
					//
					const auto endpoint = client->get<endpoint_component>();
					request->set_client(endpoint->get_id());
					request->set_name(endpoint->get_name());
					request->set_color(endpoint->get_color());

					acceptor->write_request(e, request);
				}
			}
		}

		return chat_response_message::create();
	}

	void router_chat_command::execute(const entity::ptr client, const entity::ptr data, const chat_response_message::ptr response)
	{
		// N/A
	}
}
