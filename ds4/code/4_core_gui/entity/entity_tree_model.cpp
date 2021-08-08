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

#include "entity/entity_tree_model.h"
#include "thread/thread_info.h"

namespace eja
{
	// Has
	bool entity_tree_model::has_entity(const QModelIndex& index) const
	{
		assert(thread_info::main());

		if (index.isValid() && index.internalPointer())
		{
			// NOTE: Very dangerous stuff going on here...
			// We can convert from raw pointer to smart pointer because of the guarantees Qt
			// gives us w.r.t. when QModelIndex objects are deleted
			return index.internalPointer() != nullptr;
		}

		return true;
	}

	bool entity_tree_model::has_parent(const QModelIndex& index) const
	{
		const auto entity = get_entity(index);
		return has_parent(entity);
	}

	// Get
	entity::ptr entity_tree_model::get_entity(const QModelIndex& index, const bool safe /*= false*/) const
	{
		assert(thread_info::main());

		if (index.isValid() && index.internalPointer())
		{
			// NOTE: Very dangerous stuff going on here...
			// We can convert from raw pointer to smart pointer because of the guarantees Qt
			// gives us w.r.t. when QModelIndex objects are deleted
			const auto ptr = static_cast<entity*>(index.internalPointer());
			if (likely(ptr))
				return ptr->shared_from_this();
		}

		return safe ? m_entity : nullptr;
	}

	entity::ptr entity_tree_model::get_parent(const entity::ptr& entity, const bool safe /*= false*/) const
	{
		assert(thread_info::main());

		if (!safe)
			return entity ? entity->get_parent() : nullptr;
		else
			return (entity && entity->has_parent()) ? entity->get_parent() : m_entity;
	}

	entity::ptr entity_tree_model::get_parent(const QModelIndex& index, const bool safe /*= false*/) const
	{
		assert(thread_info::main());

		const auto entity = get_entity(index, safe);
		return get_parent(entity);
	}

	// TODO: tree entity with wrapper nodes
	//
	//
	// Get
	//entity::ptr entity_tree_model::get_entity(const entity::ptr& node) const
	//{
	//	assert(thread_info::main());

	//	return (node && node->has_data()) ? node->get_data() : m_entity;
	//}

	//entity::ptr entity_tree_model::get_entity(const QModelIndex& index) const
	//{
	//	assert(thread_info::main());

	//	const auto node = get_node(index);
	//	return get_entity(node);
	//}

	//entity::ptr entity_tree_model::get_parent_entity(const entity::ptr& node) const
	//{
	//	assert(thread_info::main());

	//	const auto pnode = get_parent_node(node);
	//	return (pnode && pnode->has_data()) ? pnode->get_data() : m_entity;
	//}

	//entity::ptr entity_tree_model::get_parent_entity(const QModelIndex& index) const
	//{
	//	assert(thread_info::main());

	//	const auto pnode = get_parent_node(index);
	//	return (pnode && pnode->has_data()) ? pnode->get_data() : m_entity;
	//}

	//entity::ptr entity_tree_model::get_node(const QModelIndex& index) const
	//{
	//	assert(thread_info::main());

	//	if (index.isValid() && index.internalPointer())
	//	{
	//		// NOTE: Very dangerous stuff going on here...
	//		// We can convert from raw pointer to smart pointer because of the guarantees Qt
	//		// gives us w.r.t. when QModelIndex objects are deleted
	//		const auto ptr = static_cast<entity*>(index.internalPointer());
	//		if (likely(ptr))
	//			return ptr->shared_from_this();
	//	}

	//	return nullptr;
	//}

	//entity::ptr entity_tree_model::get_parent_node(const entity::ptr& node) const
	//{
	//	assert(thread_info::main());

	//	return node->get_owner();
	//}

	//entity::ptr entity_tree_model::get_parent_node(const QModelIndex& index) const
	//{
	//	assert(thread_info::main());

	//	const auto node = get_node(index);
	//	return node ? node->get_owner() : nullptr;
	//}
}
