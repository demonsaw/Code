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

#include "command/router_command.h"
#include "command/info_command.h"

#include "component/callback/callback.h"
#include "component/client/client_component.h"

#include "data/response/info_response_data.h"
#include "entity/entity.h"
#include "factory/client_factory.h"
#include "utility/value.h"

namespace eja
{
	// Interface
	void info_command::execute(const router_response_message::ptr& response_message, const info_response_data::ptr& response_data) const
	{
		// Validate
		if (unlikely(!validate(response_message, response_data)))
			return;

		// 1) Find
		entity::ptr client_entity;
		const auto client_map = m_entity->get<client_map_component>();
		{
			thread_lock(client_map);
			const auto it = client_map->find(response_message->get_origin());
			if (it != client_map->end())
				client_entity = it->second;
		}

		// 2) Create
		if (!client_entity)
		{
			const auto& cdata = response_data->get_client();
			if (cdata->invalid())
				return;

			// Client
			client_entity = client_factory::create_client(m_entity);
			const auto c = client_entity->get<client_component>();
			c->set_id(cdata->get_id());

			// Update
			update(client_entity, cdata);

			// Add
			const auto pair = std::make_pair(c->get_id(), client_entity);
			const auto entity_map = m_entity->get<entity_map_component>();
			{
				thread_lock(entity_map);
				entity_map->insert(pair);
			}

			// Callback
			m_entity->async_call(callback::client | callback::add, client_entity);

			if (c->is_share())
				m_entity->async_call(callback::browse | callback::add, client_entity);
		}
	}

	// Utility
	void info_command::update(const entity::ptr& entity, const client_data::ptr& data) const
	{
		// Update
		const auto client = entity->get<client_component>();
		client->set_troll(data->is_troll());
		client->set_verified(data->is_verified());

		if (m_entity != entity)
		{
			client->set_name(data->get_name());
			client->set_color(data->get_color());
			client->set_share(data->is_share());

			// Tags
			if (data->is_troll())
				client->get_name() += suffix::troll;
		}
	}

	bool info_command::validate(const router_response_message::ptr& response_message, const info_response_data::ptr& response_data) const
	{
		return response_data->valid();
	}
}
