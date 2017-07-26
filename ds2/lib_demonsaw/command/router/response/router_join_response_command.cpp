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
#include <boost/algorithm/string/predicate.hpp>

#include "router_join_response_command.h"
#include "component/spam_component.h"
#include "component/mutex_component.h"
#include "component/session_component.h"
#include "component/spam_component.h"
#include "component/timeout_component.h"
#include "component/client/client_component.h"
#include "component/client/client_option_component.h"
#include "component/group/group_component.h"
#include "json/json_packet.h"
#include "json/value/json_info.h"
#include "message/request/join_request_message.h"
#include "message/response/join_response_message.h"
#include "security/base.h"
#include "system/function/function_action.h"
#include "system/function/function_type.h"

namespace eja
{
	// Utility
	http_status router_join_response_command::execute(const entity::ptr entity, const json_message::ptr message, const json_data::ptr data)
	{
		// Option
		const auto router_option = get_router_option();
		if (!router_option)
		{
			m_socket->write(http_code::internal_server_error);
			return http_code::internal_server_error;
		}

		if (!router_option->get_message_router())
		{
			m_socket->write(http_code::not_implemented);
			return http_code::not_implemented;
		}

		// Request
		const auto request_message = join_request_message::create(message);
		if (request_message->invalid())
		{
			m_socket->write(http_code::bad_request);
			return http_code::bad_request;
		}
		
		const auto request_entity = request_message->get_entity();
		if (request_entity->invalid())
		{
			m_socket->write(http_code::bad_request);
			return http_code::bad_request;
		}

		const auto request_client = request_message->get_client();
		if (request_client->invalid())
		{
			m_socket->write(http_code::bad_request);
			return http_code::bad_request;
		}

		const auto request_group = request_message->get_group();
		/*if (request_group->invalid())
		{
			m_socket->write(http_code::bad_request);
			return http_code::bad_request;
		}*/

		// Quit		
		const auto entity_id = request_entity->get_id();
		remove_client(entity_id);

		// Entity
		const auto entity_map = get_entity_map();
		if (!entity_map)
		{
			m_socket->write(http_code::internal_server_error);
			return http_code::internal_server_error;
		}
		
		// Mutex
		const auto mutex = entity->get<mutex_component>();
		auto_lock_ptr(mutex);

		entity->set_id(entity_id);

		// Spam
		entity->add<spam_component>();
		entity->add<spam_timeout_component>();

		// Client
		const auto client_id = request_client->get_id();		
		const auto client = client_component::create(client_id);
		const auto& client_name = request_client->get_name();
		client->set_name(client_name);
		entity->add(client);

		// Version
		if (request_message->has_version())
			client->set_version(request_message->get_version());		

		// Option
		const auto request_option = request_message->get_option();
		if (request_option->valid())
		{
			const auto client_option = client_option_component::create();			
			client_option->set_browse(request_option->get_browse());
			client_option->set_search(request_option->get_search());
			client_option->set_transfer(request_option->get_transfer());
			client_option->set_chat(request_option->get_chat());
			client_option->set_message(request_option->get_message());
			entity->add(client_option);
		}

		// TODO: Add a high-level add function for groups/clients
		//
		// Public Groups?
		const auto group_id = request_group->get_id();
		if (!strlen(group_id) && !router_option->get_public_router())
		{
			m_socket->write(http_code::not_implemented);
			return http_code::not_implemented;
		}

		const auto group = group_component::create(group_id);
		entity->add(group);

		entity_map->add(entity_id, entity);

		const auto client_map = get_client_entity_map();
		if (!client_map)
		{
			m_socket->write(http_code::internal_server_error);
			return http_code::internal_server_error;
		}

		client_map->add(client_id, entity);

		call(function_type::client, function_action::add, entity);

		// Group
		const auto group_map = get_group_entity_map();
		if (!group_map)
		{
			m_socket->write(http_code::internal_server_error);
			return http_code::internal_server_error;
		}

		entity::ptr group_entity;
		function_action group_action = function_action::refresh;

		{
			// Mutex
			const auto mutex = get_mutex();
			auto_lock_ptr(mutex);

			group_entity = group_map->get(group_id);

			if (!group_entity)
			{
				group_entity = entity::create(group_id);
				group_entity->add<entity_map_component>();
				group_entity->add(group);

				group_map->add(group_id, group_entity);
				group_action = function_action::add;
			}

			const auto group_entity_map = group_entity->get<entity_map_component>();
			if (group_entity_map)
			{
				// FIX: Prevent trolling
				if ((strlen(client_name) > 0) && !boost::equals(client_name, default_client::name))
				{
					for (const auto& e : group_entity_map->get_list())
					{
						const auto c = e->get<client_component>();
						if (c)
						{
							// Change nick?
							if (boost::equals(c->get_name(), client_name))
							{
								const auto str = boost::str(boost::format("%s (%s)") % client_name % hex::random(2));
								client->set_name(str);
								break;
							}
						}
					}
				}

				group_entity_map->add(entity_id, entity);
			}
		}

		call(function_type::group, group_action, group_entity);

		// Session
		const auto session = entity->get<session_component>();
		if (!session)
		{
			m_socket->write(http_code::internal_server_error);
			return http_code::internal_server_error;
		}

		// Response
		const auto response_packet = json_packet::create();
		const auto response_message = join_response_message::create(response_packet);

		// Info
		const auto response_info = response_message->get_info();		
		response_info->set(m_entity);

		// HACK: This really shouldn't be here
		//
		// Join
		response_info->set_name(client->get_name());

		if (router_option->has_message())
			response_info->set_message(router_option->get_message());

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
