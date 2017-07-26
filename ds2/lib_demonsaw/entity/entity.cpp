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

#include "entity.h"
#include "component/function_component.h"
#include "component/client/client_component.h"
#include "component/error/error_component.h"
#include "component/router/router_component.h"
#include "component/server/server_component.h"
#include "entity/entity_factory.h"

namespace eja
{
	// Interface
	void entity::init()
	{
		auto_lock();
		for (const auto& pair : m_map)
		{
			const auto comp = pair.second;
			comp->init();
		}
	}

	void entity::shutdown()
	{
		auto_lock();
		for (const auto& pair : m_map)
		{
			const auto comp = pair.second;
			comp->shutdown();
		}
	}

	void entity::update()
	{
		auto_lock();
		for (const auto& pair : m_map)
		{
			const auto comp = pair.second;
			comp->update();
		}
	}

	void entity::clear()
	{
		object::clear();

		auto_lock();
		m_map.clear();
		m_entity = nullptr;
	}

	// Utility
	void entity::call(const function_type type, const function_action action, const entity::ptr entity /*= nullptr*/) const
	{
		const auto self = const_cast<eja::entity*>(this);
		const auto function_map_list = self->get<function_map_list_component>();
		const auto function_list = function_map_list->get(type);
		assert(function_list);

		if (!function_list->empty())
		{
			auto_lock_ptr(function_list);
			for (const auto& function : *function_list)
				function->call(action, entity);
		}
	}

	void entity::log(const char* psz /*= default_error::unknown*/)
	{
		const auto self = shared_from_this();
		const auto entity = entity_factory::create_error(psz, self);

		// HACK: We need the source for the old code...
		//
		//

		// Name
		const auto error = entity->get<error_component>();
		if (error)
		{
			std::string name;
			if (self->has<client_component>())
			{
				const auto client = self->get<client_component>();
				if (!client)
					return;

				name = client->get_name();
			}
			else if (self->has<router_component>())
			{
				const auto router = self->get<router_component>();
				if (!router)
					return;
				
				name = router->has_name() ? router->get_name() : router->get_address();
			}
			else if (self->has<server_component>())
			{
				const auto server = self->get<server_component>();
				if (!server)
					return;

				name = server->has_name() ? server->get_name() : server->get_address();
			}

			error->set_name(name);
		}		

		call(function_type::error, function_action::add, entity);
	}

	// Add
	void entity::add(const key_type& key, const value_type item)
	{
		if (!item->has_entity())
		{
			const auto self = shared_from_this();
			item->set_entity(self);
			item->init();
		}			

		auto_lock();
		m_map[key] = item;
	}

	// Remove
	void entity::remove(const key_type& key)
	{
		auto_lock();
		m_map.erase(key);
	}

	// Has
	bool entity::has(const key_type& key, const bool parent /*= true*/)
	{
		{
			auto_lock();
			if (m_map.find(key) != m_map.end())
				return true;
		}

		return (parent && has_entity()) ? m_entity->has(key, parent) : false;
	}

	// Mutator
	void entity::set_entity(const entity::ptr entity)
	{ 
		assert((shared_from_this() != entity) && (shared_from_this() != entity->get_entity()));

		m_entity = entity; 
	}
}
