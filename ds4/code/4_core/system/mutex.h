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

#ifndef _EJA_MUTEX_H_
#define _EJA_MUTEX_H_

#include <memory>
#include <mutex>

#include "system/type.h"

namespace eja
{
#if _DEBUG
	#define auto_lock() const auto_mutex lock(*this)
	#define auto_lock_ex(name) const auto_mutex lock_ ## name(*this)

	#define auto_lock_ptr(value) const auto_mutex lock_ ## value(*value)
	#define auto_lock_ptr_ex(value, name) const auto_mutex lock_ ## name(*value)

	#define auto_lock_ref(value) const auto_mutex lock_ ## value(value)
	#define auto_lock_ref_ex(value, name) const auto_mutex lock_ ## name(value)
#else
	#define auto_lock() const auto lock = this->get_lock();
	#define auto_lock_ex(name) const auto lock_ ## name = this->get_lock();

	#define auto_lock_ptr(value) const auto lock_ ## value = value->get_lock();
	#define auto_lock_ptr_ex(value, name) const auto lock_ ## name = value->get_lock();

	#define auto_lock_ref(value) const auto lock_ ## value = value.get_lock();
	#define auto_lock_ref_ex(value, name) const auto lock_ ## name = value.get_lock();
#endif

	class mutex
	{
		// Using
		using mutex_type = std::mutex;
		using unique_type = std::unique_lock<mutex_type>;

	protected:
		mutex_type m_mutex;
		DEBUG_ONLY(unique_type m_unique;)

	public:
		mutex() DEBUG_ONLY(: m_unique(m_mutex, std::defer_lock)) { }
		mutex(const mutex& mutex) = delete;
		virtual ~mutex() { }

		// Operator
		mutex& operator=(const mutex& mutex) = delete;

#if _DEBUG
		// Interface
		void lock() const { const_cast<unique_type&>(m_unique).lock(); }
		void lock() { m_unique.lock(); }
		
		void unlock() const { const_cast<unique_type&>(m_unique).unlock(); }
		void unlock() { m_unique.unlock(); }

		// Utility
		bool locked() const { return m_unique.owns_lock(); }
		bool unlocked() const { return !locked(); }
#endif
		unique_type get_lock() const { return unique_type(const_cast<mutex_type&>(m_mutex)); }
		unique_type get_try_lock() const { return unique_type(const_cast<mutex_type&>(m_mutex), std::try_to_lock); }
		unique_type get_defer_lock() const { return unique_type(const_cast<mutex_type&>(m_mutex), std::defer_lock); }
		unique_type get_adopt_lock() const { return unique_type(const_cast<mutex_type&>(m_mutex), std::adopt_lock); }
	};

#if _DEBUG
	// Debug-only class to make sure that we lock all containers before iteration
	class auto_mutex final
	{
	private:
		mutex& m_mutex;

	public:
		auto_mutex(const eja::mutex& mutex) : m_mutex(const_cast<eja::mutex&>(mutex)) { m_mutex.lock(); }
		~auto_mutex() { m_mutex.unlock(); }
	};
#endif
}

#endif
