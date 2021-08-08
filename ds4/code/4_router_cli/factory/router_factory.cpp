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

#include "component/error_component.h"
#include "component/mute_component.h"
#include "component/room_component.h"
#include "component/verified_component.h"
#include "component/service/socket_component.h"
#include "component/callback/callback_service_component.h"
#include "component/client/client_component.h"
#include "component/group/group_component.h"
#include "component/group/group_option_component.h"
#include "component/message/message_component.h"
#include "component/router/router_component.h"
#include "component/router/router_acceptor_component.h"
#include "component/session/session_component.h"
#include "component/session/session_option_component.h"
#include "component/time/timeout_component.h"
#include "component/transfer/chunk_component.h"
#include "component/transfer/transfer_component.h"

#include "entity/entity.h"
#include "entity/entity_util.h"
#include "factory/router_factory.h"
#include "security/security.h"
#include "utility/value.h"

namespace eja
{
	// Create
	entity::ptr router_factory::create_router(const entity::ptr& owner /*= nullptr*/)
	{
		// Entity
		const auto entity = entity::create(owner);
		entity->add<router_component>();
		entity->add<status_component>();

		if (!owner)
		{
			entity->add<callback_service_component>();

			entity->add<router_acceptor_component>();
			entity->add<router_list_component>();

			// Set
			entity->add<socket_set_component>();

			// Map
			entity->add<entity_map_component>();
			entity->add<group_map_component>();
			//entity->add<header_map_component>();
			entity->add<room_list_component>();
			entity->add<session_map_component>();
			entity->add<transfer_map_component>();
			entity->add<verified_map_component>();
		}

		return entity;
	}

	entity::ptr router_factory::create_entity(const entity::ptr& owner)
	{
		// Entity
		const auto entity = entity::create(owner);

		// Session
		const auto router = owner->get<router_component>();
		entity->add<session_component>(router->get_password());

		// Components
		entity->add<group_component>();
		entity->add<mute_set_component>();
		entity->add<socket_set_component>();
		entity->add<timeout_component>();

		const auto message_ring = message_ring_component::create();
		message_ring->set_capacity(router::num_ring);
		entity->add(message_ring);

		// Id
		const auto id = entity_util::add<entity_map_component>(owner, entity);
		entity->set_id(id);

		// Client
		const auto client = entity->add<client_component>();
		client->set_id(entity->get_id());

		return entity;
	}

	entity::ptr router_factory::create_group(const entity::ptr& owner)
	{
		// Entity
		const auto entity = entity::create(owner);

		// Components
		entity->add<client_map_component>();
		entity->add<message_map_component>();
		entity->add<name_map_component>();
		entity->add<transfer_map_component>();

		// Rooms
		const auto room_map = entity->add<room_map_component>();
		const auto room_list = owner->get<room_list_component>();

		for (const auto& e : *room_list)
		{
			if (e->enabled())
			{
				// Entity
				const auto room_entity = entity::create(owner);

				// Components
				room_entity->add<client_map_component>();
				const auto room = room_entity->add<room_component>(e);

				const auto pair = std::make_pair(room->get_id(), room_entity);
				room_map->insert(pair);
			}
		}

		return entity;
	}

	entity::ptr router_factory::create_room(const entity::ptr& owner)
	{
		// Entity
		const auto entity = entity::create(owner);

		// Components
		const auto room = entity->add<room_component>();

		return entity;
	}

	entity::ptr router_factory::create_transfer()
	{
		// Entity
		const auto entity = entity::create();

		// Components
		entity->add<chunk_list_component>();
		entity->add<chunk_map_component>();
		entity->add<timeout_component>();
		entity->add<transfer_component>();

		return entity;
	}
}
