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

#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "entity/entity.h"
#include "http/http_status.h"

namespace eja
{
	// Operator
	entity& entity::operator=(const eja::entity& e)
	{
		if (this != &e)
		{			
			m_id = e.m_id;
			m_enabled = e.m_enabled;			

			m_map = e.m_map;
			m_parent = e.m_parent;
			m_data = e.m_data;
		}

		return *this;
	}

	// Operator
	std::ostream& operator<<(std::ostream& os, const entity& e)
	{
		for (const auto& pair : e.m_map)
			os << pair.second;

		return os;
	}

	// Interface
	void entity::init()
	{
		for (const auto& pair : m_map)
		{
			const auto& c = pair.second;
			c->init();
		}
	}

	void entity::shutdown()
	{
		for (const auto& pair : m_map)
		{
			const auto& c = pair.second;
			c->shutdown();
		}
	}

	void entity::update()
	{
		for (const auto& pair : m_map)
		{
			const auto& c = pair.second;
			c->update();
		}
	}

	void entity::clear()
	{
		m_id.clear();
		m_map.clear();
		m_parent.reset();
		m_data.reset();
	}

	// Utility
	bool entity::valid() const
	{
		for (const auto& pair : m_map)
		{
			const auto& c = pair.second;
			if (c->invalid())
				return false;
		}

		return true;
	}

	void entity::add(const key_type& key, const value_type comp)
	{
		assert(!comp->has_entity());

		m_map[key] = comp;

		const auto self = shared_from_this();
		comp->set_entity(self);
		comp->init();
	}

	void entity::remove(const key_type& key)
	{
		const auto it = m_map.find(key);
		if (it != m_map.end())
		{
			const auto& comp = it->second;
			comp->shutdown();
			comp->set_entity();

			m_map.erase(key);
		}		
	}

	void entity::set(const key_type& key, const value_type comp)
	{
		assert(!comp->has_entity());

#if _DEBUG
		// All components must be present
		const auto found = has(key);
		assert(found);
#endif
		m_map[key] = comp;

		const auto self = shared_from_this();
		comp->set_entity(self);
		comp->init();
	}

	void entity::call(const callback_type type, const callback_action action) const
	{
		call(type, action, std::shared_ptr<entity>());
	}

	void entity::log(const char* psz) const
	{
		const auto callback = find<callback_component>();
		callback->log(psz);
	}

	void entity::log(const http_status& status) const
	{
		log(status.get_description());
	}

	// Set
	void entity::set_parent(const entity::ptr parent)
	{		
#if _DEBUG
		const auto self = shared_from_this();
		auto entity = parent;

		while (entity)
		{
			assert(self != entity);
			entity = entity->get_parent();
		}
#endif
		m_parent = parent;
	}
}
