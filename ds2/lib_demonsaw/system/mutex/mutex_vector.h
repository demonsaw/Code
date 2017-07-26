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

#ifndef _EJA_VECTOR_
#define _EJA_VECTOR_

#include <list>
#include <memory>
#include <queue>
#include <vector>

#include "mutex.h"
#include "system/type.h"

namespace eja
{
	template <typename T>
	class mutex_vector : public mutex
	{
	public:
		using value_type = std::shared_ptr<T>;
		using vector_type = std::vector<value_type>;

	protected:
		vector_type m_vector;
#if _DEBUG
		bool m_copy = false;
#endif
	public:
		mutex_vector() { }
		mutex_vector(const mutex_vector& vector);
		mutex_vector(const vector_type& vector);
		mutex_vector(const value_type value) { m_vector.push_back(value); }
		template <typename U> mutex_vector(const U& u);
		virtual ~mutex_vector() override { }

		// Operator
		mutex_vector& operator=(const mutex_vector& vector);
		mutex_vector& operator=(const vector_type& vector);
		const value_type operator[](const size_t pos) const { return const_cast<mutex_vector*>(this)->get(pos); }
		value_type operator[](const size_t pos);

		// Interface
		virtual void clear();

		// Utility
		size_t size() const { return m_vector.size(); }
		virtual bool empty() const { return m_vector.empty(); }
		virtual bool one() const { return m_vector.size() == 1; }
		virtual bool many() const { return m_vector.size() > 1; }
		mutex_vector copy() const { return mutex_vector(*this); }

		template <typename U> void sort(U value);
		void sort();

		// Iterator
		typename vector_type::const_iterator begin() const;
		typename vector_type::iterator begin();

		typename vector_type::const_iterator end() const { return m_vector.end(); }
		typename vector_type::iterator end() { return m_vector.end(); }

		typename vector_type::const_reverse_iterator rbegin() const;
		typename vector_type::reverse_iterator rbegin();

		typename vector_type::const_reverse_iterator rend() const { return m_vector.rend(); }
		typename vector_type::reverse_iterator rend() { return m_vector.rend(); }

		// Container
		virtual void add(const value_type value) { push(value); }
		virtual size_t remove(const value_type value);
		virtual size_t remove(const size_t pos);
		template <typename U> size_t remove_if(U value);

		virtual void push(const value_type value) { push_back(value); }
		virtual void push_back(const value_type value);

		virtual value_type pop() { return pop_back(); }
		virtual value_type pop_back();

		void set(const size_t pos, const value_type value);
		value_type get(const size_t pos) const;
		size_t get(const value_type value) const;

		value_type find(const value_type value) const;
		template <typename U> value_type find_if(U value) const;
		template <typename U> value_type find_if_not(U value) const;

		bool has(const size_t pos) const { return pos < m_vector.size(); }
		bool has(const value_type value) const { return find(value) != nullptr; }
		template <typename U> bool has_if(U value) const { return find_if(value) != nullptr; }
		template <typename U> bool has_if_not(U value) const { return find_if_not(value) != nullptr; }
	};

	// Constructor
	template <typename T>
	mutex_vector<T>::mutex_vector(const mutex_vector& vector)
	{
		auto_lock_ref(vector);
		m_vector = vector.m_vector;
#if _DEBUG
		m_copy = true;
#endif
	}

	template <typename T>
	mutex_vector<T>::mutex_vector(const vector_type& vector)
	{
		m_vector = vector;
#if _DEBUG
		m_copy = true;
#endif
	}

	template <typename T> template <typename U>
	mutex_vector<T>::mutex_vector(const U& u)
	{
		auto_lock_ref(u);

		std::transform(u.begin(), u.end(), std::back_inserter(m_vector), [](const value_type& value) { return value; });
#if _DEBUG
		m_copy = true;
#endif
	}

	// Operator
	template <typename T>
	mutex_vector<T>& mutex_vector<T>::operator=(const mutex_vector& vector)
	{
		auto_lock();
		if (this != &vector)
		{
			auto_lock_ref(vector);
			m_vector = vector.m_vector;
#if _DEBUG
			m_copy = true;
#endif
		}

		return *this;
	}

	template <typename T>
	mutex_vector<T>& mutex_vector<T>::operator=(const vector_type& vector)
	{
		auto_lock();
		if (this != &vector)
		{
			m_vector = vector.m_vector;
#if _DEBUG
			m_copy = true;
#endif
		}

		return *this;
	}

	template <typename T>
	typename mutex_vector<T>::value_type mutex_vector<T>::operator[](const size_t pos)
	{
		auto_lock();

		if (pos < m_vector.size())
			return m_vector[pos];

		return nullptr;
	}

	// Iterator
	template <typename T>
	typename mutex_vector<T>::vector_type::const_iterator mutex_vector<T>::begin() const
	{
#if _DEBUG
		const auto status = locked();
		assert(m_copy || status);
#endif
		return m_vector.begin();
	}

	template <typename T>
	typename mutex_vector<T>::vector_type::iterator mutex_vector<T>::begin()
	{
#if _DEBUG
		const auto status = locked();
		assert(m_copy || status);
#endif
		return m_vector.begin();
	}

	template <typename T>
	typename mutex_vector<T>::vector_type::const_reverse_iterator mutex_vector<T>::rbegin() const
	{
#if _DEBUG
		const auto status = locked();
		assert(m_copy || status);
#endif
		return m_vector.rbegin();
	}

	template <typename T>
	typename mutex_vector<T>::vector_type::reverse_iterator mutex_vector<T>::rbegin()
	{
#if _DEBUG
		const auto status = locked();
		assert(m_copy || status);
#endif
		return m_vector.rbegin();
	}

	// Interface
	template <typename T>
	void mutex_vector<T>::clear()
	{
		auto_lock();
		m_vector.clear();
	}

	// Utility
	template <typename T>
	void mutex_vector<T>::sort()
	{
		auto_lock();
		m_vector.sort();
	}

	template <typename T> template <typename U>
	void mutex_vector<T>::sort(U value)
	{
		auto_lock();
		return m_vector.sort(value);
	}

	// Container
	template <typename T>
	size_t mutex_vector<T>::remove(const value_type value)
	{
		auto_lock();
		const size_t prev = size();
		m_vector.erase(std::remove(m_vector.begin(), m_vector.end(), value), m_vector.end());
		return prev - size();
	}

	template <typename T>
	size_t mutex_vector<T>::remove(const size_t pos)
	{
		auto_lock();

		if (pos < m_vector.size())
		{
			auto it = m_vector.begin();
			std::advance(it, pos);

			if (it != m_vector.end())
			{
				m_vector.erase(it);
				return 1;
			}
		}

		return 0;
	}

	template <typename T> template <typename U>
	size_t mutex_vector<T>::remove_if(U value)
	{
		auto_lock();
		const size_t prev = size();
		m_vector.erase(std::remove_if(m_vector.begin(), m_vector.end(), value), m_vector.end());
		return prev - size();
	}

	template <typename T>
	void mutex_vector<T>::push_back(const value_type value)
	{
		auto_lock();
		m_vector.push_back(value);
	}

	template <typename T>
	typename mutex_vector<T>::value_type mutex_vector<T>::pop_back()
	{
		auto_lock();
		if (!m_vector.empty())
		{
			const auto& item = m_vector.back();
			m_vector.pop_back();
			return item;
		}

		return nullptr;
	}

	template <typename T>
	void mutex_vector<T>::set(const size_t pos, const value_type value)
	{
		auto_lock();
		if (pos >= m_vector.size())
			return;

		m_vector[pos] = value;
	}

	template <typename T>
	typename mutex_vector<T>::value_type mutex_vector<T>::get(const size_t pos) const
	{
		auto_lock();
		return (pos < m_vector.size()) ? m_vector[pos] : nullptr;
	}

	template <typename T>
	size_t mutex_vector<T>::get(const value_type value) const
	{
		auto_lock();
		const auto it = std::find(m_vector.begin(), m_vector.end(), value);
		return (it != m_vector.end()) ? (it - m_vector.begin()) : type::npos;
	}

	template <typename T>
	typename mutex_vector<T>::value_type mutex_vector<T>::find(const value_type value) const
	{
		auto_lock();
		const auto it = std::find(m_vector.begin(), m_vector.end(), value);
		return (it != m_vector.end()) ? *it : nullptr;
	}

	template <typename T> template <typename U>
	typename mutex_vector<T>::value_type mutex_vector<T>::find_if(U value) const
	{
		auto_lock();
		const auto it = std::find_if(m_vector.begin(), m_vector.end(), value);
		return (it != m_vector.end()) ? *it : nullptr;
	}

	template <typename T> template <typename U>
	typename mutex_vector<T>::value_type mutex_vector<T>::find_if_not(U value) const
	{
		auto_lock();
		const auto it = std::find_if_not(m_vector.begin(), m_vector.end(), value);
		return (it != m_vector.end()) ? *it : nullptr;
	}	
}

#endif
