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

#ifndef _EJA_ENTITY_TYPE_H_
#define _EJA_ENTITY_TYPE_H_

#include <memory>

#include "entity/entity.h"
#include "object/object.h"

namespace eja
{
	enum class callback_action;
	enum class callback_type;

	class entity_type : public object
	{
	protected:
		entity::ptr m_entity;

	protected:
		entity_type() { }
		entity_type(const entity::ptr entity) : m_entity(entity) { }

		// Utility
		template <typename ... T>
		void call(const callback_type type, const callback_action action, T ... args) const { m_entity->call(type, action, args...); }
		void call(const callback_type type, const callback_action action) const { m_entity->call(type, action); }

		template <typename T, typename U>
		entity::ptr find_entity(const entity::ptr entity) const;

		template <typename T, typename U>
		bool has_entity(const entity::ptr entity) const { return find_entity<T, U>(entity) != nullptr; }

	public:
		// Utility
		virtual bool valid() const override { return m_entity != nullptr; }

		// Has
		bool has_entity() const { return m_entity != nullptr; }

		// Set
		virtual void set_entity() { m_entity.reset(); }
		virtual void set_entity(const entity::ptr entity) { m_entity = entity; }

		// Get
		const entity::ptr get_entity() const { return m_entity; }
	};

	// Utility
	template <typename T, typename U>
	entity::ptr entity_type::find_entity(const entity::ptr entity) const
	{
		const auto list = m_entity->get<U>();
		const auto it = std::find_if(list->cbegin(), list->cend(), [entity](const entity::ptr e) { return entity->equals<T>(e); });
		return (it != list->end()) ? *it : nullptr;
	}
}

#endif
