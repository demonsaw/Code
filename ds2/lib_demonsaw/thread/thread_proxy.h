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

#ifndef _EJA_THREAD_PROXY_H_
#define _EJA_THREAD_PROXY_H_

#include <memory>
#include <mutex>

namespace eja
{
	template <typename T>
	class thread_proxy final
	{
		using value_type = T;
		using mutex_type = std::mutex;

	private:
		value_type& m_value;
		mutex_type& m_mutex;

	public:
		thread_proxy(const value_type& value, const mutex_type& mutex) : m_value(const_cast<value_type&>(value)), m_mutex(const_cast<mutex_type&>(mutex)) { m_mutex.lock(); }
		~thread_proxy() { m_mutex.unlock(); }

		// Operator
		const value_type& operator->() const { return m_value; }
		value_type& operator->() { return m_value; }

		const T& operator*() const { return *m_value; }
		T& operator*() { return *m_value; }
	};
}

#endif

