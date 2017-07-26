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
 

#include "component/endpoint_component.h"

namespace eja
{
	// Operator
	endpoint_component& endpoint_component::operator=(const endpoint_component& comp)
	{
		if (this != &comp)
		{
			name_component::operator=(comp);
			
			m_address = comp.m_address;
			m_port = comp.m_port;

			m_address_ext = comp.m_address_ext;
			m_port_ext = comp.m_port_ext;

			m_type = comp.m_type;			
			m_mute = comp.m_mute;
		}

		return *this;
	}

	// Interface
	void endpoint_component::clear()
	{
		name_component::clear();

		m_address.clear();
		m_port = 0;

		m_address_ext.clear();
		m_port_ext = 0;

		m_type = endpoint_type::none;		
		m_mute = false;
	}
}
