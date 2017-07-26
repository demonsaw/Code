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
		boost::atomic<size_t> m_bytes;
		boost::atomic<size_t> m_max_bytes;

	protected:
		http_throttle() : m_bytes(0), m_max_bytes(size_t_max) { };
		http_throttle(const http_throttle& throttle) = delete;

	public:
		// Operator
		http_throttle& operator=(const http_throttle& throttle) = delete;
		http_throttle& operator+=(const size_t bytes);
		http_throttle& operator-=(const size_t bytes);

		// Interface
		virtual void clear() override;

		// Utility		
		virtual bool valid() const override { return m_bytes.load() <= m_max_bytes.load(); }
		bool empty() const { return m_bytes.load() == 0; }

		// Set
		void set_bytes() { m_bytes.store(0); }
		void set_bytes(const size_t bytes) { m_bytes.store(bytes); }

		void set_max_bytes() { m_max_bytes.store(size_t_max); }
		void set_max_bytes(const size_t bytes) { m_max_bytes.store(bytes); }

		// Get
		static http_throttle& get();
		size_t get_bytes() const { return m_bytes.load(); }
		size_t get_max_bytes() const { return m_max_bytes.load(); }
	};

	// Read
	class http_read_throttle final : public http_throttle
	{
	public:
		// Get
		static http_read_throttle& get();
	};

	// Write
	class http_write_throttle final : public http_throttle
	{
	public:
		// Get
		static http_write_throttle& get();
	};
}

#endif
