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

#ifndef _EJA_CLIENT_RESPONSE_MESSAGE_H_
#define _EJA_CLIENT_RESPONSE_MESSAGE_H_

#include <list>
#include <memory>
#include <string>

#include "data/client_data.h"
#include "message/message_type.h"
#include "message/response/response_message.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	class entity;

	// Version
	make_version(client_response_message, 1);

	// Message
	class client_response_message final : public response_message
	{
		make_factory(client_response_message);

	private:
		client_data_list m_clients;

	public:
		client_response_message() : response_message(message_type::client_response) { }
		client_response_message(const client_response_message& msg) : response_message(msg), m_clients(msg.m_clients) { }

		// Operator
		client_response_message& operator=(const client_response_message& msg);

		// Utility
		void add(const client_data::ptr data) { m_clients.push_back(data); }

		// Has
		bool has_clients() const { return !m_clients.empty(); }

		// Set
		void set_clients(const client_data_list& clients) { m_clients = clients; }

		// Get
		const client_data_list& get_clients() const { return m_clients; }

		// Static
		template <typename T>
		static ptr create(const std::shared_ptr<entity> entity, const T& data) { return ptr(new client_response_message(entity, data)); }

		template <typename T>
		static ptr create(const std::shared_ptr<entity> entity, const std::shared_ptr<T> data) { return ptr(new client_response_message(entity, *data)); }

		MSGPACK_DEFINE(MSGPACK_BASE(response_message), m_clients);
	};
}

#endif
