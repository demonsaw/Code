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

#ifndef _EJA_ROOM_REQUEST_DATA_H_
#define _EJA_ROOM_REQUEST_DATA_H_

#include <memory>
#include <string>

#include "data/id_data.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	// Version
	make_using_version(room_request_data, 1);

	// Data
	class room_request_data final : public id_data
	{
		make_factory(room_request_data);

		enum bits : u8 { none, add = bit(1), browse = bit(1), join = bit(2), quit = bit(3), remove = bit(4) };

	public:
		room_request_data() : id_data(data_type::room_request) { }
		explicit room_request_data(const std::string& id) : id_data(data_type::room_request, id) { }
		explicit room_request_data(const u8 bits) : id_data(data_type::room_request, bits) { }

		// Is
		bool is_add() const { return is_mode(bits::add); }
		bool is_browse() const { return is_mode(bits::browse); }
		bool is_join() const { return is_mode(bits::join); }
		bool is_quit() const { return is_mode(bits::quit); }
		bool is_remove() const { return is_mode(bits::remove); }

		// Set
		void set_add(const bool value) { set_mode(bits::add, value); }
		void set_browse(const bool value) { set_mode(bits::browse, value); }
		void set_join(const bool value) { set_mode(bits::join, value); }
		void set_quit(const bool value) { set_mode(bits::quit, value); }
		void set_remove(const bool value) { set_mode(bits::remove, value); }

		// Static
		static ptr create(const std::string& id) { return std::make_shared<room_request_data>(id); }

		MSGPACK_DEFINE(MSGPACK_BASE(id_data)/*, m_name*/);
	};
}

#endif
