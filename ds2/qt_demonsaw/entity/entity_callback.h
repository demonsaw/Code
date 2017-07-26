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

#ifndef _EJA_ENTITY_CALLBACK_
#define _EJA_ENTITY_CALLBACK_

#include "entity/entity.h"
#include "system/function/function_type.h"

namespace eja
{
	enum class client_statusbar;
	enum class router_statusbar;

	class entity_callback
	{
	protected:
		entity::ptr m_entity = nullptr;

	protected:
		entity_callback() { }
		entity_callback(const entity::ptr entity) : m_entity(entity) { }
		virtual ~entity_callback() { }

		// Callback
		virtual void callback(const function_action action, const entity::ptr entity);
		void call(const function_type type, const function_action action, const entity::ptr entity = nullptr) const { m_entity->call(type, action, entity); }
		void add_callback(const function_type type);

	public:
		// Interface
		virtual void init() { }
		virtual void shutdown() { }
		virtual void update() { }

		virtual void add(const entity::ptr entity) { }
		virtual void remove(const entity::ptr entity) { }
		virtual void refresh(const entity::ptr entity) { }
		virtual void clear(const entity::ptr entity) { }

		// Utility		
		virtual bool valid(const entity::ptr entity) const { return entity != nullptr; }
		bool invalid(const entity::ptr entity) const { return !valid(entity); }

		// Mutator
		virtual void set_entity(const entity::ptr entity) { m_entity = entity; }
		virtual void set_modified(const bool value);

		void set_status(const client_statusbar index, const size_t value);
		void set_status(const router_statusbar index, const size_t value);

		// Accessor
		bool has_entity() const { return m_entity != nullptr; }
		entity::ptr get_entity() const { return m_entity; }
	};
}

#endif
