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

#include "http/http_throttle.h"

namespace eja
{
	// Interface
	void http_throttle::clear()
	{
		object::clear();

		m_bytes.store(0);
		m_max_bytes.store(size_t_max);
	}

	// Operator
	http_throttle& http_throttle::operator+=(const size_t bytes)
	{ 
		m_bytes.fetch_add(bytes); 

		return *this;
	}

	http_throttle& http_throttle::operator-=(const size_t bytes)
	{
		m_bytes.fetch_sub(bytes);

		return *this;
	}

	// Get
	http_throttle& http_throttle::get()
	{
		static http_throttle throttle;
		return throttle;
	}

	http_read_throttle& http_read_throttle::get()
	{
		static http_read_throttle throttle;
		return throttle;
	}

	http_write_throttle& http_write_throttle::get()
	{
		static http_write_throttle throttle;
		return throttle;
	}
}