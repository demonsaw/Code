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

#ifndef _EJA_CHAT_REQUEST_MESSAGE_H_
#define _EJA_CHAT_REQUEST_MESSAGE_H_

#include "message/message_type.h"
#include "message/request/request_message.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	class entity;

	// Version
	make_version(chat_request_message, 1);

	// Message
	class chat_request_message final : public request_message
	{
		make_factory(chat_request_message);

	private:
		std::string m_name;
		u32 m_color = 0;

	public:
		chat_request_message() : request_message(message_type::chat_request) { }
		chat_request_message(const chat_request_message& msg) : request_message(msg), m_name(msg.m_name), m_color(msg.m_color) { }

		// Operator
		chat_request_message& operator=(const chat_request_message& msg);

		// Utility
		virtual bool valid() const { return has_client(); }

		// Has
		bool has_name() const { return !m_name.empty(); }
		bool has_color() const { return m_color > 0; }

		// Set
		void set_name(const std::string& name) { m_name = name; }		
		void set_color(const u8 red, const u8 green, const u8 blue, const u8 alpha = 255) { m_color = (alpha << 24) + (red << 16) + (green << 8) + (blue << 0); }
		void set_color(const u32 color) { m_color = color; }

		// Get
		const std::string& get_name() const { return m_name; }
		u32 get_color() const { return m_color; }

		MSGPACK_DEFINE(MSGPACK_BASE(request_message), m_name, m_color);
	};

	// Operator
	inline chat_request_message& chat_request_message::operator=(const chat_request_message& msg)
	{
		if (this != &msg)
		{
			request_message::operator=(msg);

			m_client = msg.m_client;
			m_name = msg.m_name;
		}

		return *this;
	}
}

#endif
