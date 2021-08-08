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

#include "command/client_command.h"
#include "command/command_util.h"

#include "component/room_component.h"
#include "component/client/client_component.h"
#include "component/router/router_component.h"

#include "data/client_data.h"
#include "data/room_data.h"
#include "data/router_data.h"
#include "entity/entity.h"
#include "factory/client_factory.h"
#include "utility/value.h"

namespace eja
{
	// Utility
	void command_util::update(const entity::ptr& entity, const client_data::ptr& data)
	{
		// Update
		const auto client = entity->get<client_component>();
		client->set_troll(data->is_troll());
		client->set_verified(data->is_verified());

		if (entity->has_owner())
		{
			client->set_name(data->get_name());
			client->set_color(data->get_color());
			client->set_share(data->is_share());

			// Tags
			if (data->is_troll())
				client->get_name() += suffix::troll;
		}
	}

	void command_util::update(const entity::ptr& entity, const room_data::ptr& data)
	{
		const auto room = entity->get<room_component>();
		room->set_color(data->get_color());
		room->set_name(data->get_name());
		room->set_size(data->get_size());
	}

	void command_util::update(const entity::ptr& entity, const router_data::ptr& data)
	{
		const auto router = entity->get<router_component>();
		router->set_name(data->get_name());
		router->set_address(data->get_address());
		router->set_password(data->get_password());
		router->set_port(data->get_port());
	}

	// Get
	entity::ptr command_util::get_browse_entity(const entity::ptr& entity)
	{
		const auto client = entity->get<client_component>();

		// HACK: We can't have our own entity pointer in the browse_list_component
		// To get around this, copy the needed client values to fake it
		const auto e = client_factory::create_client(entity);
		const auto c = e->get<client_component>();
		c->set_id(client->get_id());

		c->set_name(client->get_name());
		c->set_color(client->get_color());

		c->set_verified(client->is_verified());
		c->set_share(client->is_share());
		c->set_troll(client->is_troll());

		// Tags
		if (client->is_troll())
			c->get_name() += suffix::troll;

		return e;
	}
}
