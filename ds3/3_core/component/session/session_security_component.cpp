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

#include "component/session/session_security_component.h"
#include "utility/value.h"

namespace eja
{
	// Constructor
	session_security_component::session_security_component()
	{
		m_algorithm = session::algorithm;
		m_prime_size = session::prime_size;

		m_cipher = session::cipher;
		m_key_size = session::key_size;

		m_hash = session::hash;
		m_iterations = session::iterations;
	}

	// Operator
	session_security_component& session_security_component::operator=(const session_security_component& comp)
	{
		if (this != &comp)
		{
			security_component::operator=(comp);

			m_algorithm = comp.m_algorithm;
			m_prime_size = comp.m_prime_size;
		}

		return *this;
	}

	// Interface
	void session_security_component::clear()
	{
		security_component::clear();

		m_algorithm.clear();
		m_prime_size = 0;
	}

	void session_security_component::update()
	{
		security_component::update();

		// TODO: Update session_component
		//
		//
	}
}
