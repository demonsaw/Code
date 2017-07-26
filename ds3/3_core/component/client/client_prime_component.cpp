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

#include "component/client/client_prime_component.h"
#include "utility/value.h"

namespace eja
{
	// Constructor
	client_prime_component::client_prime_component()
	{
		m_size = prime::size;
		m_buffer_size = prime::buffer;
		m_reuse = prime::reuse;
		m_threshold = prime::threshold;
	}

	// Operator
	client_prime_component& client_prime_component::operator=(const client_prime_component& comp)
	{
		if (this != &comp)
		{
			component::operator=(comp);

			m_size = comp.m_size;
			m_buffer_size = comp.m_buffer_size;
			m_reuse = comp.m_reuse;
			m_threshold = comp.m_threshold;
		}

		return *this;
	}

	// Interface
	void client_prime_component::clear()
	{
		component::clear();

		m_size = 0;
		m_buffer_size = 0;
		m_reuse = 0;
		m_threshold = 0;
	}
}
