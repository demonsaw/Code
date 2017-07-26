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

#ifndef _EJA_TUNNEL_REQUEST_MESSAGE_H_
#define _EJA_TUNNEL_REQUEST_MESSAGE_H_

#include <memory>
#include <string>

#include "message/message_type.h"
#include "message/request/request_message.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	class entity;

	// Version
	make_version(tunnel_request_message, 1);

	// Message
	class tunnel_request_message final : public request_message
	{
		make_factory(tunnel_request_message);

	private:
		std::string m_address;
		u16 m_port = 0;
		u8 m_sockets = 0;

	public:
		tunnel_request_message();
		tunnel_request_message(const tunnel_request_message& msg) : request_message(msg), m_address(msg.m_address), m_port(msg.m_port), m_sockets(msg.m_sockets) { }

		// Operator
		tunnel_request_message& operator=(const tunnel_request_message& msg);

		// Has		
		bool has_endpoint() const { return has_address() && has_port(); }
		bool has_address() const { return !m_address.empty(); }
		bool has_port() const { return m_port > 0; }
		bool has_sockets() const { return m_sockets > 0; }

		// Set
		void set_address(const std::string& address) { m_address = address; }
		void set_port(const u16 port) { m_port = port; }
		void set_sockets(const u8 sockets) { m_sockets = sockets; }

		// Get
		const std::string& get_address() const { return m_address; }
		u16 get_port() const { return m_port; }
		u8 get_sockets() const { return m_sockets; }

		// Static
		template <typename T>
		static ptr create(const std::shared_ptr<entity> entity, const T& data) { return ptr(new tunnel_request_message(entity, data)); }

		template <typename T>
		static ptr create(const std::shared_ptr<entity> entity, const std::shared_ptr<T> data) { return ptr(new tunnel_request_message(entity, *data)); }

		MSGPACK_DEFINE(MSGPACK_BASE(request_message), m_address, m_port, m_sockets);
	};
}

#endif
