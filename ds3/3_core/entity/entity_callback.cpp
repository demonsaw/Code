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

#include <cassert>

#include "entity/entity.h"
#include "entity/entity_callback.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_component.h"

namespace eja
{
	// Constructor
	entity_callback::entity_callback(const callback_type type /*= callback_type::none*/) : m_type(type)
	{
		assert(thread_info::main());

		m_callback = function::create([&](const callback_action action, const entity::ptr entity) { on_callback(action, entity); });
	}

	entity_callback::entity_callback(const entity::ptr entity, const callback_type type /*= callback_type::none*/) : entity_type(entity), m_type(type)
	{
		assert(thread_info::main());

		m_callback = function::create([&](const callback_action action, const entity::ptr entity) { on_callback(action, entity); });

		if (has_type())
		{
			const auto callback = m_entity->find<callback_component>();
			callback->add(m_type, m_callback);
		}
	}

	entity_callback::~entity_callback()
	{
		assert(thread_info::main());

		if (has_type())
		{
			if (has_entity())
			{
				const auto callback = m_entity->find<callback_component>();
				callback->remove(m_type, m_callback);
			}
			
			m_callback.reset();

			thread_lock(m_functions);
			m_functions.clear();
		}
	}

	// Callback
	void entity_callback::remove_callback(const callback_action action)
	{
		thread_lock(m_functions);
		m_functions.erase(action); 
	}

	// Set
	void entity_callback::set_entity()
	{		
		if (has_type() && has_entity())
		{
			const auto callback = m_entity->find<callback_component>();
			callback->remove(m_type, m_callback);
		}

		entity_type::set_entity();
	}

	void entity_callback::set_entity(const entity::ptr entity)
	{		
		if (has_type())
		{
			if (has_entity())
			{
				const auto callback = m_entity->find<callback_component>();
				callback->remove(m_type, m_callback);
			}

			const auto callback = entity->find<callback_component>();
			callback->add(m_type, m_callback);
		}

		entity_type::set_entity(entity);
	}
}
