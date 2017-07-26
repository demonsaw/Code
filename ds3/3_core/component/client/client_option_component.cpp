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

#include "component/client/client_option_component.h"
#include "utility/value.h"

namespace eja
{
	// Constructor
	client_option_component::client_option_component()
	{
		m_timestamp = client::timestamp;
		m_beep = client::beep;
		m_flash = client::flash;
		m_vibrate = client::vibrate;
	}

	// Operator
	client_option_component& client_option_component::operator=(const client_option_component& comp)
	{
		if (this != &comp)
		{
			component::operator=(comp);
						
			m_timestamp = comp.m_timestamp;
			m_beep = comp.m_beep;
			m_flash = comp.m_flash;
			m_vibrate = comp.m_vibrate;
		}

		return *this;
	}

	// Interface
	void client_option_component::clear()
	{
		component::clear();
				
		m_timestamp.clear();
		m_beep = false;
		m_flash = false;
		m_vibrate = false;
	}
}
