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

#ifndef _EJA_CLIENT_REQUEST_MESSAGE_H_
#define _EJA_CLIENT_REQUEST_MESSAGE_H_

#include <string>

#include "message/message.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	// Version
	make_using_version(client_request_message, 1);

	// Message
	class client_request_message final : public message
	{
		make_factory(client_request_message);

	private:
		std::string m_room;

	public:
		client_request_message() : message(message_type::client_request) { }

		// Has
		bool has_room() const { return !m_room.empty(); }

		// Set
		void set_room(const std::string& room) { m_room = room; }

		// Get
		const std::string& get_room() const { return m_room; }

		// Macro
		MSGPACK_DEFINE(MSGPACK_BASE(message), m_room);
	};
}

#endif
