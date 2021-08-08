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

#include "component/security/prime_component.h"

namespace eja
{
	// Operator
	prime_component& prime_component::operator=(const prime_component& comp)
	{
		if (this != &comp)
		{
			component::operator=(comp);

			m_p = comp.m_p;
			m_q = comp.m_q;
			m_g = comp.m_g;
		}

		return *this;
	}

	prime_component& prime_component::operator=(const prime& p)
	{
		m_p = p.get_p();
		m_q = p.get_q();
		m_g = p.get_g();

		return *this;
	}

	// Interface
	void prime_component::clear()
	{
		component::clear();

		m_p.Zero();
		m_q.Zero();
		m_g.Zero();
	}
}
