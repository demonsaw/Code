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

#ifndef _EJA_HTTP_THROTTLE_H_
#define _EJA_HTTP_THROTTLE_H_

#include <boost/atomic/atomic.hpp>

#include "object/object.h"
#include "system/type.h"

namespace eja
{
	class http_throttle : public object
	{
	private:
		boost::atomic<size_t> m_size;
		size_t m_max_size;

	protected:
		http_throttle() : m_size(0), m_max_size(0) { }
		http_throttle(const size_t max_bytes) : m_size(0), m_max_size(max_bytes) { }

	public:
		// Operator
		http_throttle& operator+=(const size_t size) { add(size); }
		http_throttle& operator-=(const size_t size) { remove(size); }

		// Interface
		void update();
		virtual void clear() override;

		// Utility
		virtual bool valid() const override { return m_size.load() < m_max_size; }
		bool empty() const { return m_size.load() == 0; }

		bool enabled() const { return m_max_size > 0; }
		bool disabled() const { return !enabled(); }

		size_t add(const size_t size) { return m_size.fetch_add(size); }
		size_t remove(const size_t size) { return m_size.fetch_sub(size); }

		// Set
		void set_max_size() { m_max_size = 0; }
		void set_max_size(const size_t size) { m_max_size = size; }

		// Get
		size_t get_size() const { return m_size.load(); }
		size_t get_max_size() const { return m_max_size; }
	};

	// Macro
	#define make_http_throttle(T)													\
	class T final : public http_throttle											\
	{																				\
	public:																			\
		using ptr = T*;																\
																					\
	private:																		\
		static ptr s_ptr;															\
																					\
	public:																			\
		T()																			\
		{																			\
			assert(!s_ptr);															\
			s_ptr = this;															\
		}																			\
																					\
		T(const size_t max_bytes) : http_throttle(max_bytes)						\
		{																			\
			assert(!s_ptr);															\
			s_ptr = this;															\
		}																			\
																					\
		static ptr get() { return s_ptr; }											\
	}

	#define make_http_throttle_impl(T)												\
	T::ptr T::s_ptr = nullptr;

	// Macro
	make_http_throttle(http_download_throttle);
	make_http_throttle(http_upload_throttle);
}

#endif
