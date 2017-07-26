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

#ifndef _EJA_SET_
#define _EJA_SET_

#include <cassert>
#include <memory>
#include <set>

#include "mutex.h"
#include "system/type.h"

namespace eja
{
	template <typename T>
	class mutex_set : public mutex
	{
	public:
		using value_type = std::shared_ptr<T>;
		using set_type = std::set<value_type>;
		using set_pair = std::pair<typename set_type::iterator, bool>;

	protected:
		set_type m_set;
#if _DEBUG
		bool m_copy = false;
#endif
	public:
		mutex_set() { }
		mutex_set(const mutex_set& set);
		mutex_set(const set_type& set);
		mutex_set(const value_type value) { m_set.insert(value); }
		template <typename U> mutex_set(const U& u);
		virtual ~mutex_set() override { }

		// Operator
		mutex_set& operator=(const mutex_set& set);
		mutex_set& operator=(const set_type& set);

		// Interface
		virtual void clear();

		// Utility
		size_t size() const { return m_set.size(); }
		virtual bool empty() const { return m_set.empty(); }
		virtual bool one() const { return m_set.size() == 1; }
		virtual bool many() const { return m_set.size() > 1; }

		mutex_set copy() const { return mutex_set(*this); }
		mutex_list<T> get_list() const { return get<mutex_list<T>>(); }
		mutex_queue<T> get_queue() const { return get<mutex_queue<T>>(); }
		mutex_vector<T> get_vector() const { return get<mutex_vector<T>>(); }

		// Iterator
		typename set_type::const_iterator begin() const;
		typename set_type::iterator begin();

		typename set_type::const_iterator end() const { return m_set.end(); }
		typename set_type::iterator end() { return m_set.end(); }

		typename set_type::const_reverse_iterator rbegin() const;
		typename set_type::reverse_iterator rbegin();

		typename set_type::const_reverse_iterator rend() const { return m_set.rend(); }
		typename set_type::reverse_iterator rend() { return m_set.rend(); }

		// Container
		virtual bool add(const value_type value);
		virtual size_t remove(const value_type value);

		template <typename U> U get() const;
		virtual value_type get(const value_type value) const;
		bool has(const value_type value) const;
	};

	// Constructor
	template <typename T>
	mutex_set<T>::mutex_set(const mutex_set& set)
	{
		auto_lock_ref(set);
		m_set = set.m_set;
#if _DEBUG
		m_copy = true;
#endif
	}

	template <typename T>
	mutex_set<T>::mutex_set(const set_type& set)
	{
		m_set = set;
#if _DEBUG
		m_copy = true;
#endif
	}

	template <typename T> template <typename U>
	mutex_set<T>::mutex_set(const U& u)
	{
		auto_lock_ref(u);

		std::transform(u.begin(), u.end(), std::back_inserter(m_set), [](const value_type& value) { return value; });
#if _DEBUG
		m_copy = true;
#endif
	}

	// Operator
	template <typename T>
	mutex_set<T>& mutex_set<T>::operator=(const mutex_set& set)
	{
		auto_lock();
		if (this != &set)
		{
			auto_lock_ref(set);
			m_set = set.m_set;
#if _DEBUG
			m_copy = true;
#endif
		}

		return *this;
	}

	template <typename T>
	mutex_set<T>& mutex_set<T>::operator=(const set_type& set)
	{
		auto_lock();
		if (this != &set)
		{
			m_set = set.m_set;
#if _DEBUG
			m_copy = true;
#endif
		}

		return *this;
	}

	// Iterator
	template <typename T>
	typename mutex_set<T>::set_type::const_iterator mutex_set<T>::begin() const
	{
#if _DEBUG
		const auto status = locked();
		assert(m_copy || status);
#endif		
		return m_set.begin();
	}

	template <typename T>
	typename mutex_set<T>::set_type::iterator mutex_set<T>::begin()
	{
#if _DEBUG
		const auto status = locked();
		assert(m_copy || status);
#endif
		return m_set.begin();
	}

	template <typename T>
	typename mutex_set<T>::set_type::const_reverse_iterator mutex_set<T>::rbegin() const
	{
#if _DEBUG
		const auto status = locked();
		assert(m_copy || status);
#endif
		return m_set.rbegin();
	}

	template <typename T>
	typename mutex_set<T>::set_type::reverse_iterator mutex_set<T>::rbegin()
	{
#if _DEBUG
		const auto status = locked();
		assert(m_copy || status);
#endif
		return m_set.rbegin();
	}

	// Interface
	template <typename T>
	void mutex_set<T>::clear()
	{
		auto_lock();
		m_set.clear();
	}

	// Utility
	template <typename T> template <typename U>
	U mutex_set<T>::get() const
	{
		auto_lock();

		U container;
		std::transform(m_set.cbegin(), m_set.cend(), std::back_inserter(container), [](const value_type& value) { return value; });
		return container;
	}

	// Container
	template <typename T>
	bool mutex_set<T>::add(const value_type value)
	{
		auto_lock();
		const auto pair = m_set.insert(value);
		return pair.second;
	}

	template <typename T>
	size_t mutex_set<T>::remove(const value_type value)
	{
		auto_lock();
		return m_set.erase(value);
	}

	template <typename T>
	typename mutex_set<T>::value_type mutex_set<T>::get(const value_type value) const
	{
		auto_lock();
		const auto it = m_set.find(value);
		return (it != end()) ? *it : nullptr;
	}

	template <typename T>
	bool mutex_set<T>::has(const value_type value) const
	{
		auto_lock();
		return m_set.find(value) != m_set.end();
	}
}

#endif
