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

#ifndef _EJA_JOIN_REQUEST_DATA_H_
#define _EJA_JOIN_REQUEST_DATA_H_

#include <string>

#include "data/data.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	make_using_version(join_request_data, 1);

	class join_request_data final : public data
	{
		make_factory(join_request_data);

		enum bits : u8 { none, verified = bit(1), share = bit(2), troll = bit(3) };

	private:
		std::string m_name;
		std::string m_group;
		std::string m_password;

		u32 m_color = 0;
		u32 m_empty = 0;

	public:
		join_request_data() : data(data_type::join_request) { }
		explicit join_request_data(const u8 bits) : data(data_type::join_request, bits) { }

		// Utility
		virtual bool valid() const override { return data::valid() && has_name(); }

		// Has
		bool has_color() const { return m_color > 0; }
		bool has_group() const { return !m_group.empty(); }
		bool has_name() const { return !m_name.empty(); }
		bool has_password() const { return !m_password.empty(); }

		// Is
		bool is_share() const { return is_mode(bits::share); }
		bool is_troll() const { return is_mode(bits::troll); }
		bool is_verified() const { return is_mode(bits::verified); }

		// Set
		void set_name(const std::string& name) { m_name = name; }
		void set_group(const std::string& group) { m_group = group; }
		void set_password(const std::string& password) { m_password = password; }

		void set_color(const u8 red, const u8 green, const u8 blue, const u8 alpha = 255) { m_color = (alpha << 24) + (red << 16) + (green << 8) + (blue << 0); }
		void set_color(const u32 color) { m_color = color; }

		void set_share(const bool value) { set_mode(bits::share, value); }
		void set_troll(const bool value) { set_mode(bits::troll, value); }
		void set_verified(const bool value) { set_mode(bits::verified, value); }

		// Get
		u32 get_color() const { return m_color; }
		const std::string& get_name() const { return m_name; }
		const std::string& get_group() const { return m_group; }
		const std::string& get_password() const { return m_password; }

		// Macro
		MSGPACK_DEFINE(MSGPACK_BASE(data), m_name, m_group, m_password, m_color, m_empty, m_empty, m_empty, m_empty);
	};
}

#endif
