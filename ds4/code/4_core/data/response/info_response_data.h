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

#ifndef _EJA_INFO_RESPONSE_DATA_H_
#define _EJA_INFO_RESPONSE_DATA_H_

#include <string>

#include "data/client_data.h"
#include "data/data.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	class entity;

	make_using_version(info_response_data, 1);

	class info_response_data final : public data
	{
		make_factory(info_response_data);

		enum bits : u8 { none, verified = bit(1), share = bit(2), troll = bit(3)};

	private:
		client_data::ptr m_client;

	public:
		info_response_data() : data(data_type::info_response) { }
		explicit info_response_data(const u8 bits) : data(data_type::info_response, bits) { }

		// Utility
		virtual bool valid() const override { return data::valid() && has_client(); }

		// Has
		bool has_client() const { return m_client != nullptr; }

		// Is
		bool is_share() const { return is_mode(bits::share); }
		bool is_troll() const { return is_mode(bits::troll); }
		bool is_verified() const { return is_mode(bits::verified); }

		// Set
		void set_client() { m_client.reset(); }
		void set_client(const client_data::ptr& client) { m_client = client; }

		void set_share(const bool value) { set_mode(bits::share, value); }
		void set_troll(const bool value) { set_mode(bits::troll, value); }
		void set_verified(const bool value) { set_mode(bits::verified, value); }

		// Get
		const client_data::ptr& get_client() const { return m_client; }

		// Macro
		MSGPACK_DEFINE(MSGPACK_BASE(data), m_client);
	};
}

#endif
