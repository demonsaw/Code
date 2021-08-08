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

#ifndef _EJA_ENTITY_NODE_H_
#define _EJA_ENTITY_NODE_H_

#include <deque>
#include <map>
#include <memory>
#include <string>

#include "component/component.h"
#include "entity/entity_type.h"

namespace eja
{
	class entity_node;

	make_thread_safe_component(node_list_component, std::deque<std::shared_ptr<entity_node>>);
	make_thread_safe_component(node_map_component, std::map<std::string, std::shared_ptr<entity_node>>);

	class entity_node final : public entity_type, public std::enable_shared_from_this<entity_node>
	{
	public:
		using ptr = std::shared_ptr<entity_node>;

	private:
		std::weak_ptr<entity_node> m_parent;

	public:
		entity_node() { }
		entity_node(const entity_node::ptr& parent) : m_parent(parent) { }
		entity_node(const entity_node::ptr& parent, const std::shared_ptr<entity>& entity) : entity_type(entity), m_parent(parent) { }
		entity_node(const std::shared_ptr<entity>& entity) : entity_type(entity) { }

		// Has
		bool has_parent() const { return !m_parent.expired(); }

		// Is
		bool is_parent(const entity_node::ptr& parent) const { return get_parent() == parent; }

		// Set
		void set_parent(const entity_node::ptr& parent);
		void set_parent() { m_parent.reset(); }

		// Get
		entity_node::ptr get_parent() const { return m_parent.lock(); }

		// Static
		static ptr create() { return std::make_shared<entity_node>(); }
		static ptr create(const entity_node::ptr& parent) { return std::make_shared<entity_node>(parent); }
		static ptr create(const entity_node::ptr& parent, const std::shared_ptr<entity>& entity) { return std::make_shared<entity_node>(parent, entity); }
		static ptr create(const std::shared_ptr<entity>& entity) { return std::make_shared<entity_node>(entity); }
	};
}

#endif
