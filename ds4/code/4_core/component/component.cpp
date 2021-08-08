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

#include "component/component.h"
#include "component/callback/callback_service_component.h"

#include "entity/entity.h"
#include "http/http_status.h"

namespace eja
{
	// Operator
	component& component::operator=(component&& comp)
	{
		if (this != &comp)
		{
			object::operator=(std::move(comp));
			mode<size_t>::operator=(std::move(comp));

			m_id = std::move(comp.m_id);
			m_owner = std::move(comp.m_owner);
		}

		return *this;
	}

	component& component::operator=(const component& comp)
	{
		if (this != &comp)
		{
			object::operator=(comp);
			mode<size_t>::operator=(comp);

			m_id = comp.m_id; 
			m_owner = comp.m_owner;
		}			

		return *this;
	}

	// Interface
	void component::clear()
	{
		object::clear();

		m_id.clear();
	}

	// Utility
	void component::async_call(const key_type key) const
	{
		const auto owner = get_owner();
		if (likely(owner))
			owner->async_call(key);
	}

	void component::async_call(const key_type key, const entity::ptr& entity) const
	{
		const auto owner = get_owner();
		if (likely(owner))
			owner->async_call(key, entity);
	}

	void component::call(const key_type key) const
	{
		const auto owner = get_owner();
		if (likely(owner))
			owner->call(key);
	}

	void component::call(const key_type key, const entity::ptr& entity) const
	{
		const auto owner = get_owner();
		if (likely(owner))
			owner->call(key, entity);
	}

	void component::log(const char* psz) const
	{
		const auto owner = get_owner();
		if (likely(owner))
			owner->log(psz);
	}

	void component::log(const http_status& status) const
	{
		log(status.get_description()); 
	}

	void component::log(const http_code code) const
	{
		log(std::to_string(static_cast<size_t>(code)));
	}

	// Has
	bool component::has_owner() const
	{ 
		return !m_owner.expired(); 
	}

	// Set
	void component::set_owner()
	{
		m_owner.reset();
	}

	// Get
	entity::ptr component::get_owner() const
	{
		return m_owner.lock();
	}
}
