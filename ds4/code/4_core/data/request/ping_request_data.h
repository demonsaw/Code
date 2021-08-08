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

#ifndef _EJA_PING_REQUEST_DATA_H_
#define _EJA_PING_REQUEST_DATA_H_

#include "data/data.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	// Version
	make_using_version(ping_request_data, 1);

	// Data
	class ping_request_data final : public data
	{
		make_factory(ping_request_data);

		enum bits : u8 { none, client = bit(1), room = bit(2), router = bit(3) };

	public:
		ping_request_data() : data(data_type::ping_request) { }
		explicit ping_request_data(const u8 bits) : data(data_type::ping_request, bits) { }

		// Utility
		virtual bool valid() const override { return data::valid() && (is_client() || is_room()); }

		// Is
		bool is_client() const { return is_mode(bits::client); }
		bool is_room() const { return is_mode(bits::room); }
		bool is_router() const { return is_mode(bits::router); }

		// Set
		void set_client(const bool value) { set_mode(bits::client, value); }
		void set_room(const bool value) { set_mode(bits::room, value); }
		void set_router(const bool value) { set_mode(bits::router, value); }

		MSGPACK_DEFINE(MSGPACK_BASE(data));
	};
}

#endif
