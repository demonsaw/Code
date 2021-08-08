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

#include "component/component_callback.h"
#include "component/callback/callback_service_component.h"
#include "entity/entity.h"

namespace eja
{
	// Interface	
	void component_callback::clear()
	{
		remove_callback();

		component::clear();
	}

	// Add
	void component_callback::add_callback(const size_t key, const function::ptr& func)
	{
		const auto owner = get_owner();
		const auto callback = owner->find<callback_service_component>();
		callback->add(key, func);

		const auto pair = std::make_pair(key, func);

		thread_lock(m_map);
		m_map.insert(pair);
	}

	// Remove
	void component_callback::remove_callback()
	{
		const auto owner = get_owner();
		const auto callback = owner->find<callback_service_component>();

		thread_lock(m_map);
		for (const auto& pair : m_map)
			callback->remove(pair.first, pair.second);

		m_map->clear();
	}

	void component_callback::remove_callback(const size_t key)
	{
		const auto owner = get_owner();
		const auto callback = owner->find<callback_service_component>();
		callback->remove(key);

		thread_lock(m_map);
		m_map->erase(key);
	}

	void component_callback::remove_callback(const size_t key, const function::ptr& func)
	{
		const auto owner = get_owner();
		const auto callback = owner->find<callback_service_component>();
		callback->remove(key, func);

		thread_lock(m_map);
		const auto range = m_map.equal_range(key);
		//assert(range.first != range.second);

		for (auto it = range.first; it != range.second; )
		{
			if (func == it->second)
				m_map->erase(it++);
			else
				++it;
		}
	}

	void component_callback::remove_callback(const function::ptr& func)
	{
		const auto owner = get_owner();
		const auto callback = owner->find<callback_service_component>();
		callback->remove(func);

		// O(n)
		thread_lock(m_map);
		for (auto it = m_map.begin(); it != m_map->end(); )
		{
			if (func == it->second)
				m_map->erase(it++);
			else
				++it;
		}
	}
}
