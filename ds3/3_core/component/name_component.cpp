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

#include "component/name_component.h"
#include "utility/value.h"

namespace eja
{
	// Operator
	name_component& name_component::operator=(const name_component& comp)
	{
		if (this != &comp)
		{
			id_component::operator=(comp);
			
			m_name = comp.m_name;
			m_color = comp.m_color;			
		}

		return *this;
	}

	// Interface
	void name_component::clear()
	{
		id_component::clear();
		
		m_name.clear();
		m_color = 0;		
	}

	// Get
	u32 name_component::get_color()
	{
		if (has_color())
			return m_color;

		auto hash = 0;
		for (const auto& ch : m_name)
			hash += ch;

		return s_colors[hash % m_num_colors];
	}
}
