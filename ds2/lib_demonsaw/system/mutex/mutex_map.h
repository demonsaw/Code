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

#ifndef _EJA_MAP_
#define _EJA_MAP_

#include <cassert>
#include <map>
#include <memory>

#include "mutex.h"
#include "mutex_list.h"
#include "mutex_queue.h"
#include "mutex_vector.h"
#include "system/type.h"

namespace eja
{
	template <typename K, typename V>
	class mutex_map : public mutex
	{
	public:		
		using key_type = K;
		using value_type = std::shared_ptr<V>;
		using map_type = std::map<key_type, value_type>;
		using map_pair = std::pair<key_type, value_type>;

	protected:
		map_type m_map;
#if _DEBUG
		bool m_copy = false;
#endif
	public:
		mutex_map() { }
		mutex_map(const mutex_map& map);
		mutex_map(const map_type& map);
		virtual ~mutex_map() override { }

		// Operator
		mutex_map& operator=(const mutex_map& map);
		mutex_map& operator=(const map_type& map);
		const value_type operator[](const key_type& key) const { return const_cast<mutex_vector<V>*>(this)->get(key); }
		value_type operator[](const key_type& key);

		// Interface
		virtual void clear();

		// Utility
		size_t size() const { return m_map.size(); }
		virtual bool empty() const { return m_map.empty(); }
		virtual bool one() const { return m_map.size() == 1; }
		virtual bool many() const { return m_map.size() > 1; }
		
		mutex_map copy() const { return mutex_map(*this); }		
		mutex_list<V> get_list() const { return get<mutex_list<V>>(); }
		mutex_queue<V> get_queue() const { return get<mutex_queue<V>>(); }
		mutex_vector<V> get_vector() const { return get<mutex_vector<V>>(); }

		// Iterator
		typename map_type::const_iterator begin() const;
		typename map_type::iterator begin();

		typename map_type::const_iterator end() const { return m_map.end(); }
		typename map_type::iterator end() { return m_map.end(); }

		typename map_type::const_reverse_iterator rbegin() const;
		typename map_type::reverse_iterator rbegin();

		typename map_type::const_reverse_iterator rend() const { return m_map.rend(); }
		typename map_type::reverse_iterator rend() { return m_map.rend(); }

		// Container
		virtual bool add(const key_type& key, const value_type value);
		virtual void replace(const key_type& key, const value_type value);
		virtual size_t remove(const key_type& key);
		
		template <typename T> T get() const;
		virtual value_type get(const key_type& key) const;		
		bool has(const key_type& key) const;
	};

	// Container
	template <typename K, typename V>
	derived_type(mutex_map_list, mutex_map<K, mutex_list<V>>);

	// Constructor
	template <typename K, typename V>
	mutex_map<K, V>::mutex_map(const mutex_map& map)
	{
		auto_lock_ref(map);
		m_map = map.m_map;
#if _DEBUG
		m_copy = true;
#endif
	}

	template <typename K, typename V>
	mutex_map<K, V>::mutex_map(const map_type& map)
	{
		m_map = map;
#if _DEBUG
		m_copy = true;
#endif
	}

	// Operator
	template <typename K, typename V>
	mutex_map<K, V>& mutex_map<K, V>::operator=(const mutex_map& map)
	{
		auto_lock();
		if (this != &map)
		{
			auto_lock_ref(map);
			m_map = map.m_map;
#if _DEBUG
			m_copy = true;
#endif
		}

		return *this;
	}

	template <typename K, typename V>
	mutex_map<K, V>& mutex_map<K, V>::operator=(const map_type& map)
	{
		auto_lock();
		if (this != &map)
		{
			m_map = map.m_map;
#if _DEBUG
			m_copy = true;
#endif
		}			

		return *this;
	}

	template <typename K, typename V>
	typename mutex_map<K, V>::value_type mutex_map<K, V>::operator[](const key_type& key)
	{
		auto_lock();
		return m_map[key];
	}

	// Iterator
	template <typename K, typename V>
	typename mutex_map<K, V>::map_type::const_iterator mutex_map<K, V>::begin() const
	{
#if _DEBUG
		const auto status = locked();
		assert(m_copy || status);
#endif		
		return m_map.begin();
	}

	template <typename K, typename V>
	typename mutex_map<K, V>::map_type::iterator mutex_map<K, V>::begin()
	{
#if _DEBUG
		const auto status = locked();
		assert(m_copy || status);
#endif
		return m_map.begin();
	}

	template <typename K, typename V>
	typename mutex_map<K, V>::map_type::const_reverse_iterator mutex_map<K, V>::rbegin() const
	{
#if _DEBUG
		const auto status = locked();
		assert(m_copy || status);
#endif
		return m_map.rbegin();
	}

	template <typename K, typename V>
	typename mutex_map<K, V>::map_type::reverse_iterator mutex_map<K, V>::rbegin()
	{
#if _DEBUG
		const auto status = locked();
		assert(m_copy || status);
#endif
		return m_map.rbegin();
	}

	// Interface
	template <typename K, typename V>
	void mutex_map<K, V>::clear()
	{
		auto_lock();
		m_map.clear();
	}

	// Utility
	template <typename K, typename V> template <typename T>
	T mutex_map<K, V>::get() const
	{
		auto_lock();

		T container;
		std::transform(m_map.cbegin(), m_map.cend(), std::back_inserter(container), [] (const std::pair<const key_type, value_type>& pair) { return pair.second; });
		return container;
	}

	// Container
	template <typename K, typename V>
	bool mutex_map<K, V>::add(const key_type& key, const value_type value)
	{
		const auto pair = make_pair(key, value);

		auto_lock();
		const auto it = m_map.insert(pair);
		return it.second;
	}

	template <typename K, typename V>
	void mutex_map<K, V>::replace(const key_type& key, const value_type value)
	{
		auto_lock();
		m_map[key] = value;
	}

	template <typename K, typename V>
	size_t mutex_map<K, V>::remove(const key_type& key)
	{
		auto_lock();
		return m_map.erase(key);
	}

	template <typename K, typename V>
	typename mutex_map<K, V>::value_type mutex_map<K, V>::get(const key_type& key) const
	{
		auto_lock();
		const auto it = m_map.find(key);
		return (it != end()) ? it->second : nullptr;
	}

	template <typename K, typename V>
	bool mutex_map<K, V>::has(const key_type& key) const
	{
		auto_lock();
		return m_map.find(key) != m_map.end();
	}
}

#endif
