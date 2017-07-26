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

#ifndef _EJA_ENTITY_NODE_
#define _EJA_ENTITY_NODE_

#include <list>

#include "entity/entity.h"

namespace eja
{
	class entity_item final
	{
		using list_type = std::list<entity_item*>;

	private:
		entity::ptr m_entity = nullptr;
		entity_item* m_parent = nullptr;
		list_type m_children;

	public:
		entity_item() { }
		entity_item(const entity::ptr entity) { m_entity = entity; }
		entity_item(const entity::ptr entity, const entity_item* parent);
		~entity_item();

		// Utility
		bool valid() const { return m_entity != nullptr; }
		bool invalid() const { return !valid(); }

		// Children
		size_t size() const { return m_children.size(); }
		bool empty() const { return m_children.empty(); }

		entity_item* get_child(const size_t pos);
		size_t get_index();

		// Mutator
		void set_entity(const entity::ptr entity) { m_entity = entity; }
		void set_parent(const entity_item* parent) { m_parent = const_cast<entity_item*>(parent); }
		void set_children(const list_type& children) { m_children = children; }

		// Accessor
		entity::ptr get_entity() const { return m_entity; }
		entity_item* get_parent() const { return m_parent; }
		const list_type& get_children() const { return m_children; }
	};
}

#endif