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

#ifndef _EJA_JOIN_RESPONSE_MESSAGE_H_
#define _EJA_JOIN_RESPONSE_MESSAGE_H_

#include <string>

#include "data/client_data.h"
#include "data/router_data.h"
#include "message/message_type.h"
#include "message/response/response_message.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	class entity;

	// Version
	make_version(join_response_message, 1);

	// Message
	class join_response_message final : public response_message
	{
		make_factory(join_response_message);

	private:
		std::string m_motd;
		client_data_list m_clients;
		router_data_list m_routers;

	public:
		join_response_message() : response_message(message_type::join_response) { }
		join_response_message(const join_response_message& msg) : response_message(msg), 
			m_motd(msg.m_motd), m_clients(msg.m_clients), m_routers(msg.m_routers) { }

		// Operator
		join_response_message& operator=(const join_response_message& msg);

		// Utility
		void add(const client_data::ptr data) { m_clients.push_back(data); }
		void add(const router_data::ptr data) { m_routers.push_back(data); }

		// Has
		bool has_motd() const { return !m_motd.empty(); }
		bool has_clients() const { return !m_clients.empty(); }
		bool has_routers() const { return !m_routers.empty(); }

		// Set
		void set_motd(const std::string& motd) { m_motd = motd; }
		void set_clients(const client_data_list& clients) { m_clients = clients; }
		void set_routers(const router_data_list& routers) { m_routers = routers; }

		// Get
		const std::string& get_motd() const { return m_motd; }
		const client_data_list& get_clients() const { return m_clients; }
		const router_data_list& get_routers() const { return m_routers; }

		// Static
		template <typename T>
		static ptr create(const std::shared_ptr<entity> entity, const T& data) { return ptr(new join_response_message(entity, data)); }

		template <typename T>
		static ptr create(const std::shared_ptr<entity> entity, const std::shared_ptr<T> data) { return ptr(new join_response_message(entity, *data)); }

		MSGPACK_DEFINE(MSGPACK_BASE(response_message), m_motd, m_clients, m_routers);
	};

	// Operator
	inline join_response_message& join_response_message::operator=(const join_response_message& msg)
	{
		if (this != &msg)
		{
			response_message::operator=(msg);

			m_motd = msg.m_motd;
			m_clients = msg.m_clients;
			m_routers = msg.m_routers;
		}

		return *this;
	}
}

#endif
