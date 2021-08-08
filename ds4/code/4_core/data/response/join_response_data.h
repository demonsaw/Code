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

#ifndef _EJA_JOIN_RESPONSE_DATA_H_
#define _EJA_JOIN_RESPONSE_DATA_H_

#include <string>

#include "data/client_data.h"
#include "data/id_data.h"
#include "data/room_data.h"
#include "data/router_data.h"

#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	make_using_version(join_response_data, 1);

	class join_response_data final : public id_data
	{
		make_factory(join_response_data);

	private:
		std::string m_motd;
		client_data_list m_clients;
		router_data_list m_routers;
		room_data_list m_rooms;

	public:
		join_response_data() : id_data(data_type::join_response) { }

		// Utility
		virtual bool valid() const override { return id_data::valid() && has_clients() && has_routers(); }

		void add(const client_data::ptr client) { m_clients.push_back(client); }
		void add(const room_data::ptr room) { m_rooms.push_back(room); }
		void add(const router_data::ptr router) { m_routers.push_back(router); }

		// Has
		bool has_motd() const { return !m_motd.empty(); }
		bool has_clients() const { return !m_clients.empty(); }
		bool has_rooms() const { return !m_rooms.empty(); }
		bool has_routers() const { return !m_routers.empty(); }

		// Set
		void set_motd(const std::string& motd) { m_motd = motd; }
		void set_clients(const client_data_list& clients) { m_clients = clients; }
		void set_rooms(const room_data_list& rooms) { m_rooms = rooms; }
		void set_routers(const router_data_list& routers) { m_routers = routers; }

		// Get
		const std::string& get_motd() const { return m_motd; }
		const client_data_list& get_clients() const { return m_clients; }
		const room_data_list& get_rooms() const { return m_rooms; }
		const router_data_list& get_routers() const { return m_routers; }

		size_t get_client_size() const { return m_clients.size(); }
		size_t get_room_size() const { return m_rooms.size(); }
		size_t get_router_size() const { return m_routers.size(); }

		MSGPACK_DEFINE(MSGPACK_BASE(id_data), m_motd, m_clients, m_routers, m_rooms);
	};
}

#endif
