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

#ifndef _EJA_ENTITY_H_
#define _EJA_ENTITY_H_

#include <deque>
#include <exception>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <boost/atomic/atomic.hpp>
#include <boost/system/error_code.hpp>

#include "component/component.h"
#include "component/callback/callback_component.h"
#include "system/type.h"
#include "thread/thread_safe.h"

namespace eja
{
	class entity;
	class http_status;

	enum class callback_action;
	enum class callback_type;

	make_thread_safe_type(entity_deque, std::deque<std::shared_ptr<entity>>);
	make_thread_safe_type(entity_list, std::list<std::shared_ptr<entity>>);
	make_thread_safe_type(entity_map, std::map<std::string, std::shared_ptr<entity>>);
	make_thread_safe_type(entity_set, std::set<std::shared_ptr<entity>>);
	make_thread_safe_type(entity_vector, std::deque<std::shared_ptr<entity>>);

	make_thread_safe_component(entity_deque_component, std::deque<std::shared_ptr<entity>>);
	make_thread_safe_component(entity_list_component, std::list<std::shared_ptr<entity>>);
	make_thread_safe_component(entity_map_component, std::map<std::string, std::shared_ptr<entity>>);
	make_thread_safe_component(entity_set_component, std::set<std::shared_ptr<entity>>);
	make_thread_safe_component(entity_vector_component, std::deque<std::shared_ptr<entity>>);

	// Entity
	enum class entity_state { none, permanent, temporary };

	class entity final : public std::enable_shared_from_this<entity>
	{
	public:
		// Using
		using key_type = size_t;
		using value_type = std::shared_ptr<component>;
		using map_type = std::map<key_type, value_type>;
		using pair_type = std::pair<key_type, value_type>;
		using ptr = std::shared_ptr<entity>;

	private:
		map_type m_map;
		std::string m_id;
		std::weak_ptr<entity> m_parent;
		std::shared_ptr<entity> m_data;
		entity_state m_state = entity_state::none;
		bool m_enabled = true;		

	private:
		entity() { }
		entity(const entity& e) : m_map(e.m_map), m_id(e.m_id), m_parent(e.m_parent), m_data(e.m_data), m_state(e.m_state), m_enabled(e.m_enabled) { }
		entity(const entity::ptr parent) : entity() { m_parent = parent; }

		// Operator
		entity& operator=(const entity& e);

	public:
		// Operator
		friend std::ostream& operator<<(std::ostream& os, const entity& e);

		// Interface
		void init();
		void update();
		void shutdown();
		void clear();

		// Utility
		bool valid() const;
		bool invalid() const { return !valid(); }
		bool empty() const { return m_map.empty(); }

		void enable() { set_enabled(true); }
		void disable() { set_enabled(false); }

		bool enabled() const { return m_enabled; }
		bool disabled() const { return !m_enabled; }

		size_t size() const { return m_map.size(); }		
		template <typename T> bool owner(const std::shared_ptr<T> comp) const;

		// Component
		template <typename ... T>
		void call(const callback_type type, const callback_action action, T ... args) const;
		void call(const callback_type type, const callback_action action) const;

		void log(const char* psz) const;
		void log(const std::string& str) const { log(str.c_str()); }
		void log(const std::exception& ex) const { log(ex.what()); }
		void log(const boost::system::error_code& error) const { log(error.message()); }
		void log(const http_status& status) const;

		// Iterator
		map_type::const_iterator begin() const { return m_map.begin(); }
		map_type::iterator begin() { return m_map.begin(); }
				 
		map_type::const_iterator end() const { return m_map.end(); }
		map_type::iterator end() { return m_map.end(); }
				 
		map_type::const_reverse_iterator rbegin() const { return m_map.rbegin(); }
		map_type::reverse_iterator rbegin() { return m_map.rbegin(); }
				 
		map_type::const_reverse_iterator rend() const { return m_map.rend(); }
		map_type::reverse_iterator rend() { return m_map.rend(); }

		// Component
		template <typename T> std::shared_ptr<T> add();
		template <typename T, typename U> std::shared_ptr<T> add();
		template <typename T> void add(const value_type comp) { add(typeid(T).hash_code(), comp); }
		template <typename T> void add(const std::shared_ptr<T> comp) { add(typeid(T).hash_code(), comp); }
		template <typename T> void add(const entity::ptr entity) { add(entity->copy<T>()); }
		void add(const key_type& key, const value_type comp);

		template <typename T> void remove() { remove(typeid(T).hash_code()); }
		template <typename T> void remove(const value_type comp) { remove(typeid(T).hash_code()); }
		template <typename T> void remove(const std::shared_ptr<T> comp) { remove(typeid(T).hash_code()); }
		void remove(const key_type& key);

		template <typename T> bool has() const { return has(typeid(T).hash_code()); }
		template <typename T> bool has(const std::shared_ptr<T> comp) const { return get<T>() == comp; }
		bool has(const key_type& key) const { return m_map.find(key) != m_map.end(); }

		template <typename T> std::shared_ptr<T> set();
		template <typename T, typename U> std::shared_ptr<T> set();
		template <typename T> void set(const value_type comp) { set(typeid(T).hash_code(), comp); }
		template <typename T> void set(const std::shared_ptr<T> comp) { set(typeid(T).hash_code(), comp); }
		template <typename T> void set(const entity::ptr entity) { set(entity->copy<T>()); }
		void set(const key_type& key, const value_type comp);

		template <typename T> std::shared_ptr<T> get() const { return get<T>(typeid(T).hash_code()); }
		template <typename T, typename U> std::shared_ptr<T> get() const { return get<T>(typeid(U).hash_code()); }
		template <typename T> std::shared_ptr<T> get(const key_type& key) const;

		template <typename T> std::shared_ptr<T> find() const { return find<T>(typeid(T).hash_code()); }
		template <typename T, typename U> std::shared_ptr<T> find() const { return find<T>(typeid(U).hash_code()); }
		template <typename T> std::shared_ptr<T> find(const key_type& key) const;
		
		template <typename T> std::shared_ptr<T> copy() const;

		template <typename T> bool equals(const entity::ptr entity) const;
		template <typename T> bool equals(const std::shared_ptr<T> comp) const;

		// Has		
		bool has_id() const { return !m_id.empty(); }
		bool has_parent() const { return !m_parent.expired(); }
		bool has_data() const { return m_data != nullptr; }

		// Is
		bool is_id(const std::string& id) const { return m_id == id; }
		bool is_data(const entity::ptr entity) const { return get_data() == entity; }
		bool is_parent(const entity::ptr entity) const { return get_parent() == entity; }

		// Set
		void set_id() { m_id.clear(); }
		void set_id(const std::string& id) { m_id = id; }
		void set_enabled(const bool enabled) { m_enabled = enabled; }		
		void set_state(const entity_state state) { m_state = state; }

		void set_parent(const entity::ptr parent);
		void set_parent() { m_parent.reset(); }

		void set_data(const entity::ptr data) { m_data = data; }
		void set_data() { m_data.reset(); }
		
		// Get
		const std::string& get_id() const { return m_id; }
		entity_state get_state() const { return m_state; }
		entity::ptr get_parent() const { return m_parent.lock(); }
		entity::ptr get_data() const { return m_data; }

		// Static
		static entity::ptr create() { return ptr(new entity()); }
		static entity::ptr create(const entity::ptr parent) { return ptr(new entity(parent)); }

		template <typename T> static entity::ptr create();
		template <typename T> static entity::ptr create(const entity::ptr parent);

		template <typename T> static entity::ptr create(const std::shared_ptr<T> comp);
		template <typename T> static entity::ptr create(const entity::ptr parent, const std::shared_ptr<T> comp);
	};

	// Utility
	template <typename T>
	bool entity::owner(const std::shared_ptr<T> comp) const
	{
		if (comp->get_entity() == shared_from_this())
			return has<T>();
		
		return false;
	}
	
	template <typename T> 
	std::shared_ptr<T> entity::add()
	{ 
		const std::shared_ptr<T> comp = T::create();
		add(comp);

		return comp;
	}

	template <typename T, typename U>
	std::shared_ptr<T> entity::add()
	{
		const std::shared_ptr<T> comp = T::create();
		add<U>(comp);

		return comp;
	}

	template <typename T>
	std::shared_ptr<T> entity::copy() const
	{		
		const auto comp = get<T>();
		return comp->copy();
	}

	template <typename T>
	std::shared_ptr<T> entity::set()
	{
		const std::shared_ptr<T> comp = T::create();
		set(comp);

		return comp;
	}

	template <typename T, typename U>
	std::shared_ptr<T> entity::set()
	{
		const std::shared_ptr<T> comp = T::create();
		set<U>(comp);

		return comp;
	}

	template <typename T> 
	bool entity::equals(const entity::ptr entity) const
	{
		const auto comp = entity->get<T>();
		return equals(comp);
	}

	template <typename T>
	bool entity::equals(const std::shared_ptr<T> comp) const
	{
		const auto c = get<T>();
		return *c == *comp;
	}

	template <typename ... T>
	void entity::call(const callback_type type, const callback_action action, T ... args) const
	{
		const auto callback_map = find<callback_map_component>();

		thread_lock(callback_map);
		auto range = callback_map->equal_range(type);
		//assert(range.first != range.second);		

		for (auto it = range.first; it != range.second; ++it)
		{
			const auto& callback = it->second;
			callback->call(action, args...);
		}
	}

	// Get
	template <typename T>
	std::shared_ptr<T> entity::get(const key_type& key) const
	{
#if _DEBUG
		// All components must be present
		const auto found = has(key);
		assert(found);
#endif
		const auto it = m_map.find(key);
		return std::static_pointer_cast<T>(it->second);
	}

	// Find
	template <typename T>
	std::shared_ptr<T> entity::find(const key_type& key) const
	{
#if _DEBUG
		// All components must be present
		bool found = false;
		auto entity = shared_from_this();		

		while (entity)
		{
			found = entity->has(key);
			if (found)
				break;

			entity = entity->get_parent();
		}
#endif
		const auto it = m_map.find(key);
		if (it != m_map.end())
			return std::static_pointer_cast<T>(it->second);

		// Parent
		const auto parent = get_parent();
		return parent ? parent->find<T>(key) : nullptr;
	}

	// Create
	template <typename T>
	typename entity::ptr entity::create()
	{
		const auto e = ptr(new entity());
		e->add<T>();

		return e;
	}

	template <typename T>
	typename entity::ptr entity::create(const entity::ptr parent)
	{
		const auto e = ptr(new entity(parent));
		e->add<T>();

		return e;
	}

	template <typename T>
	typename entity::ptr entity::create(const std::shared_ptr<T> comp)
	{
		const auto e = ptr(new entity());
		e->add(comp);

		return e;
	}

	template <typename T>
	typename entity::ptr entity::create(const entity::ptr parent, const std::shared_ptr<T> comp)
	{
		const auto e = ptr(new entity(parent));
		e->add(comp);

		return e;
	}
}

#endif
