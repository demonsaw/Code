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

#ifndef _EJA_LIST_
#define _EJA_LIST_

#include <list>
#include <memory>
#include <queue>
#include <vector>

#include "mutex.h"
#include "system/type.h"

namespace eja
{
	template <typename T>
	class mutex_list : public mutex
	{
	public:		
		using value_type = std::shared_ptr<T>;
		using list_type = std::list<value_type>;

	protected:
		list_type m_list;
#if _DEBUG
		bool m_copy = false;
#endif
	public:
		mutex_list() { }
		mutex_list(const mutex_list& list);
		mutex_list(const list_type& list);
		mutex_list(const value_type value) { m_list.push_back(value); }
		template <typename U> mutex_list(const U& u);
		virtual ~mutex_list() override { }

		// Operator
		mutex_list& operator=(const mutex_list& list);
		mutex_list& operator=(const list_type& list);

		// Interface
		virtual void clear();

		// Utility
		size_t size() const { return m_list.size(); }
		virtual bool empty() const { return m_list.empty(); }
		virtual bool one() const { return m_list.size() == 1; }
		virtual bool many() const { return m_list.size() > 1; }
		mutex_list copy() const { return mutex_list(*this); }

		template <typename U> void sort(U value);
		void sort();

		// Iterator
		typename list_type::const_iterator begin() const;
		typename list_type::iterator begin();

		typename list_type::const_iterator end() const { return m_list.end(); }
		typename list_type::iterator end() { return m_list.end(); }

		typename list_type::const_reverse_iterator rbegin() const;
		typename list_type::reverse_iterator rbegin();

		typename list_type::const_reverse_iterator rend() const { return m_list.rend(); }
		typename list_type::reverse_iterator rend() { return m_list.rend(); }

		// Container
		virtual void add(const value_type value) { push_back(value); }
		virtual size_t remove(const value_type value);
		virtual size_t remove(const size_t pos);
		template <typename U> size_t remove_if(U value);

		virtual void push(const value_type value) { push_back(value); }
		virtual void push_front(const value_type value);
		virtual void push_back(const value_type value);

		virtual value_type pop() { return pop_back(); }
		virtual value_type pop_front();
		virtual value_type pop_back();

		void set(const size_t pos, const value_type value);
		value_type get(const size_t pos) const;
		size_t get(const value_type value) const;

		value_type find(const value_type value) const;
		template <typename U> value_type find_if(U value) const;
		template <typename U> value_type find_if_not(U value) const;

		bool has(const size_t pos) const { return pos < m_list.size(); }
		bool has(const value_type value) const { return find(value) != nullptr; }
		template <typename U> bool has_if(U value) const { return find_if(value) != nullptr; }
		template <typename U> bool has_if_not(U value) const { return find_if_not(value) != nullptr; }
	};

	// Constructor
	template <typename T>
	mutex_list<T>::mutex_list(const mutex_list& list)
	{
		auto_lock_ref(list);
		m_list = list.m_list;
#if _DEBUG
		m_copy = true;
#endif
	}

	template <typename T>
	mutex_list<T>::mutex_list(const list_type& list)
	{
		m_list = list;
#if _DEBUG
		m_copy = true;
#endif
	}

	template <typename T> template <typename U>
	mutex_list<T>::mutex_list(const U& u)
	{
		auto_lock_ref(u);

		std::transform(u.begin(), u.end(), std::back_inserter(m_list), [](const value_type& value) { return value; });
#if _DEBUG
		m_copy = true;
#endif
	}

	// Operator
	template <typename T>
	mutex_list<T>& mutex_list<T>::operator=(const mutex_list& list)
	{
		auto_lock();
		if (this != &list)
		{
			auto_lock_ref(list);
			m_list = list.m_list;
#if _DEBUG
			m_copy = true;
#endif
		}

		return *this;
	}

	template <typename T>
	mutex_list<T>& mutex_list<T>::operator=(const list_type& list)
	{
		auto_lock();
		if (this != &list)
		{
			m_list = list.m_list;
#if _DEBUG
			m_copy = true;
#endif
		}			

		return *this;
	}

	// Iterator
	template <typename T>
	typename mutex_list<T>::list_type::const_iterator mutex_list<T>::begin() const
	{
#if _DEBUG
		const auto status = locked();
		assert(m_copy || status);
#endif
		return m_list.begin();
	}

	template <typename T>
	typename mutex_list<T>::list_type::iterator mutex_list<T>::begin()
	{
#if _DEBUG
		const auto status = locked();
		assert(m_copy || status);
#endif		
		return m_list.begin();
	}

	template <typename T>
	typename mutex_list<T>::list_type::const_reverse_iterator mutex_list<T>::rbegin() const
	{
#if _DEBUG
		const auto status = locked();
		assert(m_copy || status);
#endif
		return m_list.rbegin();
	}

	template <typename T>
	typename mutex_list<T>::list_type::reverse_iterator mutex_list<T>::rbegin()
	{
#if _DEBUG
		const auto status = locked();
		assert(m_copy || status);
#endif		
		return m_list.rbegin();
	}

	// Interface
	template <typename T>
	void mutex_list<T>::clear()
	{
		auto_lock();
		m_list.clear();
	}

	// Utility
	template <typename T>
	void mutex_list<T>::sort()
	{
		auto_lock();
		m_list.sort();
	}

	template <typename T> template <typename U>
	void mutex_list<T>::sort(U value)
	{
		auto_lock();
		return m_list.sort(value);
	}

	// Container
	template <typename T>
	size_t mutex_list<T>::remove(const value_type value)
	{
		auto_lock();
		const size_t prev = size();
		m_list.remove(value);
		return prev - size();
	}

	template <typename T>
	size_t mutex_list<T>::remove(const size_t pos)
	{
		auto_lock();
		
		if (pos < m_list.size())
		{
			auto it = m_list.begin();
			std::advance(it, pos);

			if (it != m_list.end())
			{
				m_list.erase(it);
				return 1;
			}
		}	

		return 0;
	}

	template <typename T> template <typename U>
	size_t mutex_list<T>::remove_if(U value)
	{
		auto_lock();
		const size_t prev = size();
		m_list.remove_if(value);
		return prev - size();
	}

	template <typename T>
	void mutex_list<T>::push_front(const value_type value)
	{
		auto_lock();
		m_list.push_front(value);
	}

	template <typename T>
	void mutex_list<T>::push_back(const value_type value)
	{
		auto_lock();
		m_list.push_back(value);
	}

	template <typename T>
	typename mutex_list<T>::value_type mutex_list<T>::pop_front()
	{
		auto_lock();
		if (!m_list.empty())
		{
			const auto& item = m_list.front();
			m_list.pop_front();
			return item;
		}

		return nullptr;
	}

	template <typename T>
	typename mutex_list<T>::value_type mutex_list<T>::pop_back()
	{
		auto_lock();
		if (!m_list.empty())
		{
			const auto& item = m_list.back();
			m_list.pop_back();
			return item;
		}

		return nullptr;
	}

	template <typename T>
	void mutex_list<T>::set(const size_t pos, const value_type value)
	{
		auto_lock();
		if (pos >= m_list.size())
			return;

		auto it = m_list.begin();
		std::advance(it, pos);
		*it = value;
	}

	template <typename T>
	typename mutex_list<T>::value_type mutex_list<T>::get(const size_t pos) const
	{
		auto_lock();
		if (pos >= m_list.size())
			return nullptr;
		
		auto it = m_list.begin();
		std::advance(it, pos);
		return *it;
	}

	template <typename T>
	size_t mutex_list<T>::get(const value_type value) const
	{
		auto_lock();
		const auto it = std::find(m_list.begin(), m_list.end(), value);
		return (it != m_list.end()) ? std::distance(m_list.begin(), it) : type::npos;
	}

	template <typename T>
	typename mutex_list<T>::value_type mutex_list<T>::find(const value_type value) const
	{
		auto_lock();
		const auto it = std::find(m_list.begin(), m_list.end(), value);
		return (it != m_list.end()) ? *it : nullptr;
	}

	template <typename T> template <typename U>
	typename mutex_list<T>::value_type mutex_list<T>::find_if(U value) const
	{
		auto_lock();
		const auto it = std::find_if(m_list.begin(), m_list.end(), value);
		return (it != m_list.end()) ? *it : nullptr;
	}

	template <typename T> template <typename U>
	typename mutex_list<T>::value_type mutex_list<T>::find_if_not(U value) const
	{
		auto_lock();
		const auto it = std::find_if_not(m_list.begin(), m_list.end(), value);
		return (it != m_list.end()) ? *it : nullptr;
	}
}

#endif
