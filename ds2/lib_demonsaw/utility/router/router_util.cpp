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

#include "router_util.h"
#include "component/function_component.h"
#include "component/mutex_component.h"
#include "component/spam_component.h"
#include "component/group/group_component.h"
#include "component/session_component.h"
#include "component/client/client_component.h"
#include "component/error/error_component.h"
#include "component/router/router_component.h"
#include "component/router/router_idle_component.h"
#include "component/router/router_machine_component.h"
#include "component/router/router_option_component.h"
#include "component/router/router_security_component.h"
#include "component/server/server_component.h"
#include "component/server/server_machine_component.h"
#include "component/server/server_option_component.h"
#include "component/transfer/chunk_component.h"
#include "entity/entity.h"

namespace eja
{
	// Static
	mutex router_util::m_mutex;

	// Utility
	std::shared_ptr<function_map_list_component> router_util::get_function_map()
	{
		const auto function_map_list = function_map_list_component::create();

		// Session
		/*auto function_list = function_list_component::create();
		function_map_list->add(function_type::session, function_list);*/

		// Client
		auto function_list = function_list_component::create();
		function_map_list->add(function_type::client, function_list);

		// Router
		function_list = function_list_component::create();
		function_map_list->add(function_type::router, function_list);

		// Group
		function_list = function_list_component::create();
		function_map_list->add(function_type::group, function_list);

		// Transfer
		function_list = function_list_component::create();
		function_map_list->add(function_type::transfer, function_list);

		// Error
		function_list = function_list_component::create();
		function_map_list->add(function_type::error, function_list);

		return function_map_list;
	}

	entity::ptr router_util::create()
	{
		const auto entity = eja::entity::create();
		entity->add<mutex_component>();

		const auto functions = get_function_map();
		entity->add(functions);

		const auto router = router_component::create();
		router->set_version(default_version::app);
		entity->add(router);

		entity->add<router_option_component>();
		entity->add<router_security_component>();
		entity->add<router_machine_component>();
		entity->add<router_idle_component>();

		// Map
		entity->add<entity_map_component>();
		entity->add<session_entity_map_component>();
		entity->add<client_entity_map_component>();
		entity->add<group_entity_map_component>();
		entity->add<chunk_entity_map_component>();
		entity->add<spam_entity_map_component>();

		// List
		entity->add<server_entity_vector_component>();

		// Error
		entity->add<error_entity_vector_component>();

		return entity;
	}

	entity::ptr router_util::create(const entity::ptr parent)
	{
		const auto entity = create();
		entity->set_entity(parent);

		return entity;
	}

	// Accessor
	entity_vector::ptr router_util::get_active_servers(const entity::ptr owner)
	{
		const auto list = entity_vector::create();
		const auto server_vector = owner->get<server_entity_vector_component>();

		for (const auto& entity : server_vector->copy())
		{
			const auto option = entity->get<server_option_component>();
			if (option->enabled())
			{
				const auto machine = entity->get<server_machine_component>();
				if (machine->running() && machine->success())
					list->add(entity);
			}
		}

		return list;
	}

	entity::ptr router_util::get_active_server(const entity::ptr owner)
	{
		static size_t next = ~0;
		const auto server_vector = get_active_servers(owner);

		if (!server_vector->empty())
		{
			const size_t pos = ++next % server_vector->size();
			return server_vector->get(pos);
		}

		return nullptr;
	}
}