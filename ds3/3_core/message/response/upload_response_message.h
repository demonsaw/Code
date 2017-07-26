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

#ifndef _EJA_UPLOAD_RESPONSE_MESSAGE_H_
#define _EJA_UPLOAD_RESPONSE_MESSAGE_H_

#include "message/message_type.h"
#include "message/response/response_message.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	class entity;

	// Version
	make_version(upload_response_message, 1);

	// Message
	class upload_response_message final : public response_message
	{
		make_factory(upload_response_message);

	private:
		u32 m_index = static_cast<u32>(~0);

	public:
		upload_response_message() : response_message(message_type::upload_response) { }
		upload_response_message(const upload_response_message& msg) : response_message(msg), m_index(msg.m_index) { }

		// Operator
		upload_response_message& operator=(const upload_response_message& msg);

		// Has
		bool has_index() const { return m_index != static_cast<u32>(~0); }

		// Set
		void set_index(const u32 index) { m_index = index; }

		// Get
		u32 get_index() const { return m_index; }

		MSGPACK_DEFINE(MSGPACK_BASE(response_message), m_index);
	};

	// Operator
	inline upload_response_message& upload_response_message::operator=(const upload_response_message& msg)
	{
		if (this != &msg)
		{
			response_message::operator=(msg);

			m_index = msg.m_index;
		}

		return *this;
	}
}

#endif
