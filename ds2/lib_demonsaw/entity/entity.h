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

#ifndef _EJA_ENTITY_
#define _EJA_ENTITY_

#include <map>
#include <memory>
#include <string>

#include "component/component.h"
#include "component/value_component.h"
#include "controller/list_controller.h"
#include "controller/value_controller.h"
#include "object/object.h"
#include "system/function/function_type.h"
#include "system/function/function_action.h"
#include "system/mutex/mutex.h"
#include "system/mutex/mutex_list.h"
#include "system/mutex/mutex_map.h"
#include "system/mutex/mutex_queue.h"
#include "system/mutex/mutex_set.h"
#include "system/mutex/mutex_value.h"
#include "system/mutex/mutex_vector.h"
#include "system/state.h"
#include "system/type.h"
#include "utility/default_value.h"

namespace eja
{
	// Entity
	class entity final : public object, public state, public mutex, public std::enable_shared_from_this<entity>
	{
	private:
		// Using
		using key_type = size_t;
		using value_type = std::shared_ptr<component>;
		using map_type = std::map<key_type, value_type>;	

	public:
		using ptr = std::shared_ptr<entity>;

	protected:
		map_type m_map;
		entity::ptr m_entity = nullptr;

	private:
		entity() : object(default_security::id) { }
		entity(const entity::ptr entity) : object(default_security::id), m_entity(entity) { }
		entity(const std::string& id) : object(id) { }
		entity(const char* id) : object(id) { }

	public:		
		virtual ~entity() override { shutdown(); }

		// Interface
		virtual void init() override;
		virtual void update() override;
		virtual void shutdown() override;		
		virtual void clear() override;

		// Utility
		size_t size() const { return m_map.size(); }
		bool empty() const { return m_map.empty(); }
		bool has_entity() const { return m_entity != nullptr; }
		template <typename T> bool owner(const std::shared_ptr<T> item);
		
		void call(const function_type type, const function_action action, const entity::ptr entity = nullptr) const;
		void log(const std::string& str) { log(str.c_str()); }
		void log(const char* psz = default_error::unknown);
		void log(const std::exception& e) { log(e.what()); }

		// Add
		template <typename T> std::shared_ptr<T> add();
		template <typename T, typename U> std::shared_ptr<T> add();
		template <typename T> void add(const entity::ptr entity) { add(typeid(T).hash_code(), entity->get<T>()); }
		template <typename T> void add(const value_type item) { add(typeid(T).hash_code(), item); }
		template <typename T> void add(const std::shared_ptr<T> item) { add(typeid(T).hash_code(), item); }
		void add(const key_type& key, const value_type item);

		// Remove
		template <typename T> void remove() { remove(typeid(T).hash_code()); }
		template <typename T> void remove(const value_type item) { remove(typeid(T).hash_code()); }
		template <typename T> void remove(const std::shared_ptr<T> item) { remove(typeid(T).hash_code()); }
		void remove(const key_type& key);

		// Get
		template <typename T> std::shared_ptr<T> get(const bool parent = true) { return get<T>(typeid(T).hash_code(), parent); }
		template <typename T, typename U> std::shared_ptr<T> get(const bool parent = true) { return get<T>(typeid(U).hash_code(), parent); }
		template <typename T> std::shared_ptr<T> get(const key_type& key, const bool parent = true);
		
		// Copy
		template <typename T> std::shared_ptr<T> copy() { return T::create(get<T>()); }

		// Has
		template <typename T> bool has(const bool parent = true) { return has(typeid(T).hash_code(), parent); }
		template <typename T> bool has(const std::shared_ptr<T> item, const bool parent = true) { return get<T>(parent) == item; }
		bool has(const key_type& key, const bool parent = true);

		// Mutator		
		void set_entity(const entity::ptr entity);
		void set_entity() { m_entity = nullptr; }

		// Accessor		
		entity::ptr get_entity() const { return m_entity; }

		// Static
		static entity::ptr create() { return entity::ptr(new eja::entity()); }
		static entity::ptr create(const entity::ptr entity) { return entity::ptr(new eja::entity(entity)); }
		static entity::ptr create(const std::string& id) { return entity::ptr(new eja::entity(id)); }
		static entity::ptr create(const char* id) { return entity::ptr(new eja::entity(id)); }

		template <typename T> static entity::ptr create();
		template <typename T> static entity::ptr create(const entity::ptr entity);

		template <typename T> static entity::ptr create(const std::shared_ptr<T> item);
		template <typename T> static entity::ptr create(const entity::ptr entity, const std::shared_ptr<T> item);
	};

	template <typename T>
	bool entity::owner(const std::shared_ptr<T> item)
	{
		if (item->get_entity() == shared_from_this())
			return has<T>();
		
		return false;
	}

	template <typename T> 
	std::shared_ptr<T> entity::add()
	{ 
		const std::shared_ptr<T> item = T::create();
		add(item);

		return item;
	}

	template <typename T, typename U>
	std::shared_ptr<T> entity::add()
	{
		const std::shared_ptr<T> item = T::create();
		add<U>(item);

		return item;
	}

	template <typename T>
	std::shared_ptr<T> entity::get(const key_type& key, const bool parent /*= true*/)
	{
#if _DEBUG
		const auto found = has(key, parent);
#endif
		assert(has(key));

		{
			auto_lock();
			const auto it = m_map.find(key);
			if (it != m_map.end())
				return std::static_pointer_cast<T>(it->second);
		}

		return (parent && has_entity()) ? m_entity->get<T>(key, parent) : nullptr;
	}

	template <typename T>
	typename entity::ptr entity::create()
	{
		const auto e = entity::ptr(new eja::entity());
		e->add<T>();

		return e;
	}

	template <typename T>
	typename entity::ptr entity::create(const entity::ptr entity)
	{
		const auto e = entity::ptr(new eja::entity(entity));
		e->add<T>();

		return e;
	}

	template <typename T>
	typename entity::ptr entity::create(const std::shared_ptr<T> item)
	{
		const auto e = entity::ptr(new eja::entity());
		e->add(item);

		return e;
	}

	template <typename T>
	typename entity::ptr entity::create(const entity::ptr entity, const std::shared_ptr<T> item)
	{
		const auto e = entity::ptr(new eja::entity(entity));
		e->add(item);

		return e;
	}

	// Controller
	derived_value_type(entity_controller, entity::ptr, value_controller<entity>);
	derived_type(entity_list_controller, list_controller<entity>);	

	// Container
	derived_value_type(entity_value, entity::ptr, mutex_value<entity>);
	derived_type(entity_list, mutex_list<entity>);
	derived_type(entity_map, mutex_map<std::string, entity>);
	derived_type(entity_queue, mutex_queue<entity>);
	derived_type(entity_set, mutex_set<entity>);
	derived_type(entity_vector, mutex_vector<entity>);

	// Component
	derived_value_type(entity_component, entity::ptr, value_component<entity>);
	derived_type(entity_list_component, list_component<entity>);
	derived_type(entity_map_component, map_component<std::string, entity>);
	derived_type(entity_queue_component, queue_component<entity>);
	derived_type(entity_set_component, set_component<entity>);
	derived_type(entity_vector_component, vector_component<entity>);
}

#endif
