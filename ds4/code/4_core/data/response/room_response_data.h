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

#ifndef _EJA_ROOM_RESPONSE_DATA_H_
#define _EJA_ROOM_RESPONSE_DATA_H_

#include "data/client_data.h"
#include "data/data.h"
#include "data/room_data.h"

#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	make_using_version(room_response_data, 1);

	class room_response_data final : public data
	{
		make_factory(room_response_data);

	private:
		client_data_list m_clients;
		room_data_list m_rooms;

	public:
		room_response_data() : data(data_type::room_response) { }

		// Utility
		virtual bool valid() const override { return data::valid() && has_clients(); }

		// Utility
		void add(const client_data::ptr client) { m_clients.push_back(client); }
		void add(const room_data::ptr room) { m_rooms.push_back(room); }

		// Has
		bool has_clients() const { return !m_clients.empty(); }
		bool has_rooms() const { return !m_rooms.empty(); }

		// Set
		void set_clients(const client_data_list& clients) { m_clients = clients; }
		void set_rooms(const room_data_list& rooms) { m_rooms = rooms; }

		// Get
		const client_data_list& get_clients() const { return m_clients; }
		const room_data_list& get_rooms() const { return m_rooms; }

		size_t get_client_size() const { return m_clients.size(); }
		size_t get_room_size() const { return m_rooms.size(); }

		MSGPACK_DEFINE(MSGPACK_BASE(data), m_clients, m_rooms);
	};
}

#endif
