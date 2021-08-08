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
#include "command/group_command.h"

#include "component/chat_component.h"
#include "component/room_component.h"
#include "component/callback/callback.h"
#include "component/client/client_component.h"
#include "component/io/file_component.h"
#include "component/io/folder_component.h"
#include "component/io/share_component.h"
#include "component/message/message_component.h"
#include "component/router/router_service_component.h"

#include "data/request/chat_request_data.h"
#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "entity/entity_util.h"
#include "factory/client_factory.h"
#include "thread/thread_info.h"

namespace eja
{	
	// Interface
	group_request_message::ptr chat_command::execute(const std::string& text) const
	{
		// Data
		const auto request_data = chat_request_data::create(text);

		// Command
		group_command cmd(m_entity);
		const auto request_message = cmd.get_request_message(request_data);

		return request_message;
	}

	group_request_message::ptr chat_command::execute(const entity::ptr& entity, const std::string& text) const
	{
		// Data
		const auto request_data = chat_request_data::create(text);
		request_data->set_pm(true);
		
		// Command
		group_command cmd(m_entity);
		const auto request_message = cmd.get_request_message(request_data);
		
		const auto client = entity->get<client_component>();
		request_message->set_client(client->get_id());

		return request_message;
	}

	void chat_command::execute(const client_request_message::ptr& request_message, const chat_request_data::ptr& request_data) const
	{
		// Validate
		if (unlikely(!validate(request_message, request_data)))
			return;

		// Entity
		entity::ptr client_entity;
		const auto entity_map = m_entity->get<entity_map_component>();
		{
			thread_lock(entity_map);
			const auto it = entity_map->find(request_message->get_origin());
			if (it != entity_map->end())
				client_entity = it->second;
		}

		// Create new client
		if (likely(client_entity))
		{
			// Mute
			const auto client = client_entity->get<client_component>();
			if (client->is_mute())
				return;

			// Chat
			const auto e = client_factory::create_chat(client_entity, std::move(request_data->get_text()));

			if (!request_data->is_pm())
			{
				// Callback
				if (!request_message->has_room())
				{
					m_entity->async_call(callback::chat | callback::add, e);
				}
				else
				{
					const auto room = e->add<room_component>();
					room->set_id(request_message->get_room());

					m_entity->async_call(callback::room | callback::add, e);
				}
			}
			else
			{
				// Callback
				m_entity->async_call(callback::pm | callback::add, e);
			}
		}
	}

	// Utility
	bool chat_command::validate(const client_request_message::ptr& request_message, const chat_request_data::ptr& request_data) const
	{
		return request_data->valid();
	}
}
