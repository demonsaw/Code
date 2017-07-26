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

#ifndef _EJA_HEADER_MESSAGE_H_
#define _EJA_HEADER_MESSAGE_H_

#include "message/message_type.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{	
	// Version
	make_version(header_message, 1);

	// Message
	class header_message final : public message_type_list
	{
		make_factory(header_message);

	public:
		header_message() { }
		header_message(const header_message& msg) : message_type_list(msg) { }

		// Operator
		header_message& operator=(const header_message& msg);

	public:
		MSGPACK_DEFINE(MSGPACK_BASE(message_type_list));
	};

	// Operator
	inline header_message& header_message::operator=(const header_message& msg)
	{
		if (this != &msg)
			message_type_list::operator=(msg);

		return *this;
	}
}

#endif
