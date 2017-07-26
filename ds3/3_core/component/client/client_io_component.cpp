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

#include "component/client/client_io_component.h"
#include "utility/value.h"

namespace eja
{
	// Constructor
	client_io_component::client_io_component()
	{
		m_remove = io::remove;
		m_hash = io::hash;
	}

	// Operator
	client_io_component& client_io_component::operator=(const client_io_component& comp)
	{
		if (this != &comp)
		{
			component::operator=(comp);

			m_path = comp.m_path;			
			m_hash = comp.m_hash;
			m_salt = comp.m_salt;
			m_remove = comp.m_remove;
		}

		return *this;
	}

	// Interface
	void client_io_component::clear()
	{
		component::clear();

		m_path.clear();		
		m_hash.clear();
		m_salt.clear();
		m_remove = false;
	}
}
