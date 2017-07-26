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

#ifndef _EJA_HANDSHAKE_RESPONSE_MESSAGE_H_
#define _EJA_HANDSHAKE_RESPONSE_MESSAGE_H_

#include <memory>
#include <string>

#include "message/message_type.h"
#include "message/response/response_message.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	class entity;

	// Version
	make_version(handshake_response_message, 1);

	// Message
	class handshake_response_message final : public response_message
	{
		make_factory(handshake_response_message);

	private:
		std::string m_version;
		std::string m_public_key;
		std::string m_session;

	public:
		handshake_response_message() : response_message(message_type::handshake_response) { }
		handshake_response_message(const handshake_response_message& msg) : response_message(msg), 
			m_version(msg.m_version), m_public_key(msg.m_public_key), m_session(msg.m_session) { }

		// Operator
		handshake_response_message& operator=(const handshake_response_message& msg);

		// Has
		bool has_version() const { return !m_version.empty(); }
		bool has_public_key() const { return !m_public_key.empty(); }
		bool has_session() const { return !m_session.empty(); }

		// Set
		void set_version(const std::string& version) { m_version = version; }
		void set_public_key(const std::string& key) { m_public_key = key; }
		void set_session(const std::string& session) { m_session = session; }

		// Get
		const std::string& get_version() const { return m_version; }
		const std::string& get_public_key() const { return m_public_key; }
		const std::string& get_session() const { return m_session; }

		// Static
		template <typename T>
		static ptr create(const std::shared_ptr<entity> entity, const T& data) { return ptr(new handshake_response_message(entity, data)); }

		template <typename T>
		static ptr create(const std::shared_ptr<entity> entity, const std::shared_ptr<T> data) { return ptr(new handshake_response_message(entity, *data)); }

		MSGPACK_DEFINE(MSGPACK_BASE(response_message), m_version, m_session, m_public_key);
	};

	// Operator
	inline handshake_response_message& handshake_response_message::operator=(const handshake_response_message& msg)
	{
		if (this != &msg)
		{
			response_message::operator=(msg);

			m_version = msg.m_version;
			m_public_key = msg.m_public_key;
			m_session = msg.m_session;
		}

		return *this;
	}
}

#endif
