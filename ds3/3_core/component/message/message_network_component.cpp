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

#include "component/message/message_network_component.h"
#include "utility/value.h"

namespace eja
{
	// Constructor
	message_network_component::message_network_component()
	{
		m_buffer = network::num_buffer_size;
		m_message = router::message;
		m_transfer = router::transfer;
		m_proxy = router::proxy;
		m_open = router::open;
	}

	// Operator
	message_network_component& message_network_component::operator=(const message_network_component& comp)
	{
		if (this != &comp)
		{
			component::operator=(comp);

			m_motd = comp.m_motd;
			m_redirect = comp.m_redirect;
			m_buffer = comp.m_buffer;

			m_message = comp.m_message;
			m_transfer = comp.m_transfer;
			m_proxy = comp.m_proxy;
			m_open = comp.m_open;
		}

		return *this;
	}

	// Interface
	void message_network_component::clear()
	{
		component::clear();

		m_motd.clear();
		m_redirect.clear();
		m_buffer = 0;

		m_message = false;
		m_transfer = false;
		m_proxy = false;
		m_open = false;
	}
}
