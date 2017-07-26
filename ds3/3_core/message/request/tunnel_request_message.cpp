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

#include "message/request/tunnel_request_message.h"
#include "utility/value.h"

namespace eja
{
	// Constructor
	tunnel_request_message::tunnel_request_message() : request_message(message_type::tunnel_request)
	{ 
		m_sockets = static_cast<u8>(network::num_sockets);
	}

	// Operator
	tunnel_request_message& tunnel_request_message::operator=(const tunnel_request_message& msg)
	{
		if (this != &msg)
		{
			request_message::operator=(msg);

			m_address = msg.m_address;
			m_port = msg.m_port;
			m_sockets = msg.m_sockets;
		}

		return *this;
	}
}
