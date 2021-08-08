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

#ifndef _EJA_ROOM_DATA_H_
#define _EJA_ROOM_DATA_H_

#include <deque>
#include <memory>
#include <string>

#include "data/id_data.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	make_using_version(room_data, 1);

	using room_data_list = std::deque<std::shared_ptr<room_data>>;

	class room_data final : public id_data
	{
		make_factory(room_data);

	private:
		std::string m_name;
		u32 m_color = 0; 
		u32 m_size = 0;

	public:
		room_data() { }

		// Utility
		virtual bool valid() const override { return id_data::valid() && has_name(); }

		// Has
		bool has_color() const { return m_color > 0; }
		bool has_name() const { return !m_name.empty(); }
		bool has_size() const { return m_size > 0; }

		// Set
		void set_color(const u32 color) { m_color = color; }
		void set_name(const std::string& name) { m_name = name; }
		void set_size(const u32 size) { m_size = size; }

		// Get
		u32 get_color() const { return m_color; }
		const std::string& get_name() const { return m_name; }
		u32 get_size() const { return m_size; }

		MSGPACK_DEFINE(MSGPACK_BASE(id_data), m_name, m_color, m_size);
	};
}

#endif
