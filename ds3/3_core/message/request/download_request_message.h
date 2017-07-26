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

#ifndef _EJA_DOWNLOAD_REQUEST_MESSAGE_H_
#define _EJA_DOWNLOAD_REQUEST_MESSAGE_H_

#include <vector>

#include "message/message_type.h"
#include "message/request/request_message.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	class entity;

	// Version
	make_version(download_request_message, 1);

	// Message
	class download_request_message final : public request_message
	{
		make_factory(download_request_message);

	private:
		std::vector<u32> m_chunks;

	public:
		download_request_message() : request_message(message_type::download_request) { }
		download_request_message(const download_request_message& msg) : request_message(msg), m_chunks(msg.m_chunks) { }

		// Operator
		download_request_message& operator=(const download_request_message& msg);

		void add(const u32 chunk) { m_chunks.push_back(chunk); }

		// Has
		bool has_chunks() const { return !m_chunks.empty(); }

		// Set
		void set_chunks(const std::vector<u32>& chunks) { m_chunks = chunks; }

		// Get
		const std::vector<u32>& get_chunks() const { return m_chunks; }

		MSGPACK_DEFINE(MSGPACK_BASE(request_message), m_chunks);
	};

	// Operator
	inline download_request_message& download_request_message::operator=(const download_request_message& msg)
	{
		if (this != &msg)
		{
			request_message::operator=(msg);

			m_chunks = msg.m_chunks;
		}

		return *this;
	}
}

#endif
