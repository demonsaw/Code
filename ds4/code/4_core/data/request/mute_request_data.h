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

#ifndef _EJA_MUTE_REQUEST_DATA_H_
#define _EJA_MUTE_REQUEST_DATA_H_

#include <memory>
#include <string>

#include "data/data.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	// Version
	make_using_version(mute_request_data, 1);

	// Data
	class mute_request_data final : public data
	{
		make_factory(mute_request_data);

		enum bits : u8 { none, add = bit(1), remove = bit(2) };

	public:
		mute_request_data() : data(data_type::mute_request) { }
		explicit mute_request_data(const u8 bits) : data(data_type::mute_request, bits) { }

		// Utility
		virtual bool valid() const override { return data::valid() && has_mode(); }

		// Is
		bool is_add() const { return is_mode(bits::add); }
		bool is_remove() const { return is_mode(bits::remove); }

		// Set
		void set_add(const bool value) { set_mode(bits::add, value); }
		void set_remove(const bool value) { set_mode(bits::remove, value); }

		MSGPACK_DEFINE(MSGPACK_BASE(data));
	};
}

#endif
