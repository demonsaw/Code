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

#ifndef _EJA_MUTEX_COMPONENT_H_
#define _EJA_MUTEX_COMPONENT_H_

#include <mutex>

#include "component/component.h"
#include "system/type.h"

namespace eja
{
	class mutex_component : public component
	{
		make_factory(mutex_component);

		using mutex_type = std::mutex;
		using unique_type = std::unique_lock<mutex_type>;

	protected:
		mutex_type m_mutex;
		DEBUG_ONLY(unique_type m_unique;)

	public:
		mutex_component() DEBUG_ONLY(: m_unique(m_mutex, std::defer_lock)) { }
		mutex_component(const mutex_component& mutex) { }

		// Operator
		mutex_component& operator=(const mutex_component& mutex) { return *this; }

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
}

#endif
