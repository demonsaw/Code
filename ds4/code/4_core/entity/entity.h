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
#include <unordered_map>
#include <vector>
#include <boost/atomic/atomic.hpp>
#include <boost/system/error_code.hpp>

#include "component/component.h"
#include "object/function.h"
#include "system/type.h"
#include "thread/thread_safe.h"

namespace eja
{
	class entity;
	class function;
	class http_status;
	enum class http_code;

	make_thread_safe_type(id_set, std::set<std::string>);

	make_thread_safe_type(entity_deque, std::deque<std::shared_ptr<entity>>);
	make_thread_safe_type(entity_list, std::list<std::shared_ptr<entity>>);
	make_thread_safe_type(entity_map, std::map<std::string, std::shared_ptr<entity>>);
	make_thread_safe_type(entity_set, std::set<std::shared_ptr<entity>>);
	make_thread_safe_type(entity_vector, std::vector<std::shared_ptr<entity>>);

	make_thread_safe_component(entity_deque_component, std::deque<std::shared_ptr<entity>>);
	make_thread_safe_component(entity_list_component, std::list<std::shared_ptr<entity>>);
	make_thread_safe_component(entity_map_component, std::map<std::string, std::shared_ptr<entity>>);
	make_thread_safe_component(entity_set_component, std::set<std::shared_ptr<entity>>);
	make_thread_safe_component(entity_vector_component, std::vector<std::shared_ptr<entity>>);

	// Entity
	class entity final : public std::enable_shared_from_this<entity>
	{
	public:
		using key_type = size_t;
		using component_map = std::unordered_map<key_type, component::ptr>;
		using component_map_pair = std::pair<key_type, component::ptr>;
		using ptr = std::shared_ptr<entity>;

	private:
		component_map m_map;
		std::weak_ptr<entity> m_owner;
		std::weak_ptr<entity> m_parent;
		std::shared_ptr<entity> m_data;

		std::string m_id;
		bool m_enabled = true;

	public:
		entity() { }
		entity(const entity& e) : m_map(e.m_map), m_owner(e.m_owner), m_parent(e.m_parent), m_data(e.m_data), m_id(e.m_id), m_enabled(e.m_enabled) { }
		entity(const entity::ptr& owner) : entity() { m_owner = owner; }

		// Operator
		entity& operator=(const entity& e);
		friend std::ostream& operator<<(std::ostream& os, const entity& e);

		// Interface
		using std::enable_shared_from_this<entity>::shared_from_this;
		const ptr shared_from_this() const { return std::const_pointer_cast<entity>(std::enable_shared_from_this<entity>::shared_from_this()); }

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
		template <typename T> bool owner(const std::shared_ptr<T>& comp) const;

		// Component
		void async_call(const key_type key, const std::shared_ptr<entity>& entity) const;
		void async_call(const key_type key) const;

		void call(const key_type key, const std::shared_ptr<entity>& entity) const;
		void call(const key_type key) const;

		void log(const char* psz) const;
		void log(const std::string& str) const { log(str.c_str()); }
		void log(const std::exception& ex) const { log(ex.what()); }
		void log(const boost::system::error_code& error) const { log(error.message()); }
		void log(const http_status& status) const;
		void log(const http_code code) const;

		// Component
		template <typename T> std::shared_ptr<T> add();
		template <typename T, typename ... U> std::shared_ptr<T> add(const U& ... args);

		template <typename T, typename U> std::shared_ptr<U> add();
		template <typename T, typename U, typename ... V> std::shared_ptr<U> add(const V& ... args);

		template <typename T> std::shared_ptr<T> add(const entity::ptr& entity);
		template <typename T> void add(const std::shared_ptr<T>& comp) { add(typeid(T).hash_code(), comp); }
		void add(const key_type key, const component::ptr& comp);

		template <typename T> void remove() { remove(typeid(T).hash_code()); }
		template <typename T> void remove(const std::shared_ptr<T>& comp) { remove(typeid(T).hash_code()); }
		void remove(const key_type key);

		template <typename T> bool has() const { return has(typeid(T).hash_code()); }
		template <typename T> bool has(const std::shared_ptr<T>& comp) const { return get<T>() == comp; }
		bool has(const key_type key) const { return m_map.find(key) != m_map.end(); }

		template <typename T> std::shared_ptr<T> set();
		template <typename T, typename U> std::shared_ptr<U> set();
		template <typename T> std::shared_ptr<T> set(const entity::ptr& entity);
		template <typename T> void set(const std::shared_ptr<T>& comp) { set(typeid(T).hash_code(), comp); }
		void set(const key_type key, const component::ptr& comp);

		template <typename T> std::shared_ptr<T> get() const { return get<T>(typeid(T).hash_code()); }
		template <typename T, typename U> std::shared_ptr<U> get() const { return get<U>(typeid(T).hash_code()); }
		template <typename T> std::shared_ptr<T> get(const key_type key) const;

		template <typename T> std::shared_ptr<T> find() const { return find<T>(typeid(T).hash_code()); }
		template <typename T, typename U> std::shared_ptr<U> find() const { return find<U>(typeid(T).hash_code()); }
		template <typename T> std::shared_ptr<T> find(const key_type key) const;

		template <typename T> std::shared_ptr<T> copy() const;
		template <typename T, typename U> std::shared_ptr<U> copy() const;
		template <typename T> std::shared_ptr<T> copy(const entity::ptr& entity);
		template <typename T> std::shared_ptr<T> copy(const std::shared_ptr<T>& comp);

		template <typename T> bool equals(const entity::ptr& entity) const;
		template <typename T> bool equals(const std::shared_ptr<T>& comp) const;

		// Has
		bool has_id() const { return !m_id.empty(); }
		bool has_owner() const { return !m_owner.expired(); }
		bool has_parent() const { return !m_parent.expired(); }
		bool has_data() const { return m_data != nullptr; }

		// Is
		bool is_id(const std::string& id) const { return m_id == id; }
		bool is_owner(const entity::ptr& entity) const { return get_owner() == entity; }
		bool is_parent(const entity::ptr& entity) const { return get_parent() == entity; }
		bool is_data(const entity::ptr& entity) const { return get_data() == entity; }

		// Set
		void set_id() { m_id.clear(); }
		void set_id(const std::string& id) { m_id = id; }

		void set_enabled(const bool value) { m_enabled = value; }
		void set_disabled(const bool value) { m_enabled = !value; }

		void set_owner(const entity::ptr& owner);
		void set_owner() { m_owner.reset(); }

		void set_parent(const entity::ptr& parent);
		void set_parent() { m_parent.reset(); }

		void set_data(const entity::ptr& data) { m_data = data; }
		void set_data() { m_data.reset(); }

		// Get
		const std::string& get_id() const { return m_id; }
		entity::ptr get_owner() const { return m_owner.lock(); }
		entity::ptr get_parent() const { return m_parent.lock(); }
		entity::ptr get_data() const { return m_data; }

		// Static
		static entity::ptr create() { return std::make_shared<entity>(); }
		static entity::ptr create(const entity::ptr& owner) { return std::make_shared<entity>(owner); }

		template <typename T> static entity::ptr create();
		template <typename T, typename ... U> entity::ptr create(const U& ... args);

		template <typename T> static entity::ptr create(const entity::ptr& owner);
		template <typename T, typename ... U> entity::ptr create(const entity::ptr& owner, const U& ... args);

		template <typename T> static entity::ptr create(const std::shared_ptr<T>& comp);
		template <typename T> static entity::ptr create(const entity::ptr& owner, const std::shared_ptr<T>& comp);
	};

	// Utility
	template <typename T>
	bool entity::owner(const std::shared_ptr<T>& comp) const
	{
		if (comp->get_entity() == shared_from_this())
			return has<T>();

		return false;
	}

	template <typename T>
	std::shared_ptr<T> entity::add()
	{
		const std::shared_ptr<T> c = T::create();
		add(c);

		return c;
	}

	template <typename T>
	std::shared_ptr<T> entity::add(const entity::ptr& entity)
	{
		if (entity)
		{
			const auto c = entity->copy<T>();
			add(c);

			return c;
		}

		return add<T>();
	}

	template <typename T, typename ... U>
	std::shared_ptr<T> entity::add(const U& ... args)
	{
		const std::shared_ptr<T> c = T::create(args...);
		add(c);

		return c;
	}

	template <typename T, typename U>
	std::shared_ptr<U> entity::add()
	{
		const std::shared_ptr<U> c = U::create();
		add(typeid(T).hash_code(), c);

		return c;
	}

	template <typename T, typename U, typename ... V>
	std::shared_ptr<U> entity::add(const V& ... args)
	{
		const std::shared_ptr<U> c = U::create(args...);
		add(typeid(T).hash_code(), c);

		return c;
	}

	template <typename T>
	std::shared_ptr<T> entity::copy() const
	{
		const auto c = get<T>();
		return c->copy();
	}

	template <typename T, typename U>
	std::shared_ptr<U> entity::copy() const
	{
		const auto c = get<T>();
		return std::static_pointer_cast<U>(c)->copy();
	}

	template <typename T>
	std::shared_ptr<T> entity::copy(const entity::ptr& entity)
	{
		const auto c = entity->get<T>();
		return copy(c);
	}

	template <typename T>
	std::shared_ptr<T> entity::copy(const std::shared_ptr<T>& comp)
	{
		const auto c = comp->copy();
		add(c);

		return c;
	}

	template <typename T>
	std::shared_ptr<T> entity::set()
	{
		const std::shared_ptr<T> c = T::create();
		set(c);

		return c;
	}

	template <typename T, typename U>
	std::shared_ptr<U> entity::set()
	{
		const std::shared_ptr<U> c = U::create();
		set(typeid(T).hash_code(), c);

		return c;
	}

	template <typename T>
	std::shared_ptr<T> entity::set(const entity::ptr& entity)
	{
		if (entity)
		{
			const auto c = entity->copy<T>();
			set(c);

			return c;
		}

		return set<T>();
	}

	template <typename T>
	bool entity::equals(const entity::ptr& entity) const
	{
		const auto c = entity->get<T>();
		return equals(c);
	}

	template <typename T>
	bool entity::equals(const std::shared_ptr<T>& comp) const
	{
		const auto c = get<T>();
		return *c == *comp;
	}

	// Get
	template <typename T>
	std::shared_ptr<T> entity::get(const key_type key) const
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
	std::shared_ptr<T> entity::find(const key_type key) const
	{
#if _DEBUG
		// All components must be present
		bool found = false;
		auto e = shared_from_this();

		while (e)
		{
			found = e->has(key);
			if (found)
				break;

			e = e->get_owner();
		}
#endif
		const auto it = m_map.find(key);
		if (it != m_map.end())
			return std::static_pointer_cast<T>(it->second);

		// Owner
		const auto owner = get_owner();
		return owner ? owner->find<T>(key) : nullptr;
	}

	// Create
	template <typename T>
	typename entity::ptr entity::create()
	{
		const auto e = std::make_shared<entity>();
		e->add<T>();

		return e;
	}

	template <typename T, typename ... U>
	entity::ptr create(const U& ... args)
	{
		const auto e = std::make_shared<entity>();
		e->add<T>(args...);

		return e;
	}

	template <typename T>
	typename entity::ptr entity::create(const entity::ptr& owner)
	{
		const auto e = std::make_shared<entity>(owner);
		e->add<T>();

		return e;
	}

	template <typename T, typename ... U>
	entity::ptr create(const entity::ptr& owner, const U& ... args)
	{
		const auto e = std::make_shared<entity>(owner);
		e->add<T>(args...);

		return e;
	}

	template <typename T>
	typename entity::ptr entity::create(const std::shared_ptr<T>& comp)
	{
		const auto e = std::make_shared<entity>();
		e->add(comp);

		return e;
	}

	template <typename T>
	typename entity::ptr entity::create(const entity::ptr& owner, const std::shared_ptr<T>& comp)
	{
		const auto e = std::make_shared<entity>(owner);
		e->add(comp);

		return e;
	}
}

#endif
