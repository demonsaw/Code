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

#ifndef _EJA_QUEUE_
#define _EJA_QUEUE_

#include <list>
#include <memory>
#include <queue>
#include <vector>

#include "mutex.h"

namespace eja
{
	template <typename T>
	class mutex_queue : public mutex
	{
	public:
		using value_type = std::shared_ptr<T>;
		using queue_type = std::list<value_type>;

	protected:
		queue_type m_queue;
#if _DEBUG
		bool m_copy = false;
#endif
	public:
		mutex_queue() { }
		mutex_queue(const mutex_queue& queue);		
		mutex_queue(const queue_type& queue);
		mutex_queue(const value_type value) { m_queue.push_back(value); }
		template <typename U> mutex_queue(const U& u);
		virtual ~mutex_queue() override { }

		// Operator
		mutex_queue& operator=(const mutex_queue& queue);
		mutex_queue& operator=(const queue_type& queue);

		// Interface
		virtual void clear();

		// Utility
		size_t size() const { return m_queue.size(); }
		virtual bool empty() const { return m_queue.empty(); }
		virtual bool one() const { return m_queue.size() == 1; }
		virtual bool many() const { return m_queue.size() > 1; }
		mutex_queue copy() const { return mutex_queue(*this); }

		// Iterator
		typename queue_type::const_iterator begin() const;
		typename queue_type::iterator begin();

		typename queue_type::const_iterator end() const { return m_queue.end(); }
		typename queue_type::iterator end() { return m_queue.end(); }

		typename queue_type::const_reverse_iterator rbegin() const;
		typename queue_type::reverse_iterator rbegin();

		typename queue_type::const_reverse_iterator rend() const { return m_queue.rend(); }
		typename queue_type::reverse_iterator rend() { return m_queue.rend(); }

		// Container
		virtual void add(const value_type value) { push(value); }
		virtual size_t remove(const value_type value);
		virtual size_t remove(const size_t pos);
		template <typename U> size_t remove_if(U value);

		virtual void push(const value_type value) { push_back(value); }
		virtual void push_front(const value_type value);
		virtual void push_back(const value_type value);

		virtual value_type pop() { return pop_front(); }
		virtual value_type pop_front();
		virtual value_type pop_back();

		virtual value_type peek() const { return peek_front(); }
		virtual value_type peek_front() const;
		virtual value_type peek_back() const;

		void set(const size_t pos, const value_type value);
		value_type get(const size_t pos) const;
		size_t get(const value_type value) const;

		value_type find(const value_type value) const;
		template <typename U> value_type find_if(U value) const;
		template <typename U> value_type find_if_not(U value) const;

		bool has(const size_t pos) const { return pos < m_queue.size(); }
		bool has(const value_type value) const { return find(value) != nullptr; }
		template <typename U> bool has_if(U value) const { return find_if(value) != nullptr; }
		template <typename U> bool has_if_not(U value) const { return find_if_not(value) != nullptr; }
	};

	// Constructor
	template <typename T>
	mutex_queue<T>::mutex_queue(const mutex_queue& queue)
	{
		auto_lock_ref(queue);
		m_queue = queue.m_queue;
#if _DEBUG
		m_copy = true;
#endif
	}

	template <typename T>
	mutex_queue<T>::mutex_queue(const queue_type& queue)
	{
		m_queue = queue;
#if _DEBUG
		m_copy = true;
#endif
	}

	template <typename T> template <typename U>
	mutex_queue<T>::mutex_queue(const U& u)
	{
		auto_lock_ref(u);

		std::transform(u.begin(), u.end(), std::back_inserter(m_queue), [](const value_type& value) { return value; });
#if _DEBUG
		m_copy = true;
#endif
	}

	// Operator
	template <typename T>
	mutex_queue<T>& mutex_queue<T>::operator=(const mutex_queue& queue)
	{
		auto_lock();
		if (this != &queue)
		{
			auto_lock_ref(queue);
			m_queue = queue.m_queue;
#if _DEBUG
			m_copy = true;
#endif
		}

		return *this;
	}

	template <typename T>
	mutex_queue<T>& mutex_queue<T>::operator=(const queue_type& queue)
	{
		auto_lock();
		if (this != &queue)
		{
			m_queue = queue.m_queue;
#if _DEBUG
			m_copy = true;
#endif
		}

		return *this;
	}

	// Iterator
	template <typename T>
	typename mutex_queue<T>::queue_type::const_iterator mutex_queue<T>::begin() const
	{
#if _DEBUG
		const auto status = locked();
		assert(m_copy || status);
#endif
		return m_queue.begin();
	}

	template <typename T>
	typename mutex_queue<T>::queue_type::iterator mutex_queue<T>::begin()
	{
#if _DEBUG
		const auto status = locked();
		assert(m_copy || status);
#endif
		return m_queue.begin();
	}

	template <typename T>
	typename mutex_queue<T>::queue_type::const_reverse_iterator mutex_queue<T>::rbegin() const
	{
#if _DEBUG
		const auto status = locked();
		assert(m_copy || status);
#endif
		return m_queue.rbegin();
	}

	template <typename T>
	typename mutex_queue<T>::queue_type::reverse_iterator mutex_queue<T>::rbegin()
	{
#if _DEBUG
		const auto status = locked();
		assert(m_copy || status);
#endif
		return m_queue.rbegin();
	}

	// Interface
	template <typename T>
	void mutex_queue<T>::clear()
	{
		auto_lock();
		m_queue.clear();
	}

	// Container
	template <typename T>
	size_t mutex_queue<T>::remove(const value_type value)
	{
		auto_lock();
		const size_t prev = size();
		m_queue.remove(value);
		return prev - size();
	}

	template <typename T>
	size_t mutex_queue<T>::remove(const size_t pos)
	{
		auto_lock();
		
		if (pos < m_queue.size())
		{
			auto it = m_queue.begin();
			std::advance(it, pos);

			if (it != m_queue.end())
			{
				m_queue.erase(it);
				return 1;
			}
		}

		return 0;
	}

	template <typename T> template <typename U>
	size_t mutex_queue<T>::remove_if(U value)
	{
		auto_lock();
		const size_t prev = size();
		m_queue.remove_if(value);
		return prev - size();
	}

	template <typename T>
	void mutex_queue<T>::push_front(const value_type value)
	{
		auto_lock();
		m_queue.push_front(value);
	}

	template <typename T>
	void mutex_queue<T>::push_back(const value_type value)
	{
		auto_lock();
		m_queue.push_back(value);
	}
	
	template <typename T>
	typename mutex_queue<T>::value_type mutex_queue<T>::pop_front()
	{
		auto_lock();
		const auto item = !m_queue.empty() ? m_queue.front() : nullptr;
		if (!m_queue.empty())
			m_queue.pop_front();

		return item;
	}

	template <typename T>
	typename mutex_queue<T>::value_type mutex_queue<T>::pop_back()
	{
		auto_lock();
		const auto item = !m_queue.empty() ? m_queue.back() : nullptr;
		if (item)
			m_queue.pop_back();

		return item;
	}

	template <typename T>
	typename mutex_queue<T>::value_type mutex_queue<T>::peek_front() const
	{
		auto_lock();
		return !m_queue.empty() ? m_queue.front() : nullptr;
	}

	template <typename T>
	typename mutex_queue<T>::value_type mutex_queue<T>::peek_back() const
	{
		auto_lock();
		return !m_queue.empty() ? m_queue.back() : nullptr;
	}

	template <typename T>
	typename mutex_queue<T>::value_type mutex_queue<T>::get(const size_t pos) const
	{
		auto_lock();

		if (pos < m_queue.size())
		{
			auto it = m_queue.begin();
			std::advance(it, pos);
			return *it;
		}

		return nullptr;
	}

	template <typename T>
	void mutex_queue<T>::set(const size_t pos, const value_type value)
	{
		auto_lock();
		if (pos >= m_queue.size())
			return;

		auto it = m_queue.begin();
		std::advance(it, pos);
		*it = value;
	}

	template <typename T>
	size_t mutex_queue<T>::get(const value_type value) const
	{
		auto_lock();
		const auto it = std::find(m_queue.begin(), m_queue.end(), value);
		return (it != m_queue.end()) ? std::distance(m_queue.begin(), it) : type::npos;
	}

	template <typename T>
	typename mutex_queue<T>::value_type mutex_queue<T>::find(const value_type value) const
	{
		auto_lock();
		const auto it = std::find(m_queue.begin(), m_queue.end(), value);
		return (it != m_queue.end()) ? *it : nullptr;
	}

	template <typename T> template <typename U>
	typename mutex_queue<T>::value_type mutex_queue<T>::find_if(U value) const
	{
		auto_lock();
		const auto it = std::find_if(m_queue.begin(), m_queue.end(), value);
		return (it != m_queue.end()) ? *it : nullptr;
	}

	template <typename T> template <typename U>
	typename mutex_queue<T>::value_type mutex_queue<T>::find_if_not(U value) const
	{
		auto_lock();
		const auto it = std::find_if_not(m_queue.begin(), m_queue.end(), value);
		return (it != m_queue.end()) ? *it : nullptr;
	}
}

#endif
