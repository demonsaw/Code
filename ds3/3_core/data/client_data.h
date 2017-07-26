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

#ifndef _EJA_CLIENT_DATA_H_
#define _EJA_CLIENT_DATA_H_

#include <list>
#include <memory>
#include <string>

#include "data/id_data.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	// Version
	make_version(client_data, 1);

	// Container
	using client_data_list = std::list<std::shared_ptr<client_data>>;

	// Data
	class client_data final : public id_data
	{
		make_factory(client_data);

	private:
		std::string m_name; 
		u32 m_color = 0;
		bool m_share = false;

	public:
		client_data() { }
		client_data(const client_data& dat) : id_data(dat), m_name(dat.m_name), m_color(dat.m_color), m_share(dat.m_share) { }

		// Operator
		client_data& operator=(const client_data& dat);

		// Has
		bool has_name() const { return !m_name.empty(); }
		bool has_color() const { return m_color > 0; }
		bool has_share() const { return m_share; }

		// Set
		void set_name(const std::string& name) { m_name = name; }
		void set_color(const u32 color) { m_color = color; }
		void set_share(const bool share) { m_share = share; }

		// Get
		const std::string& get_name() const { return m_name; }
		u32 get_color() const { return m_color; }

		MSGPACK_DEFINE(m_id, m_name, m_color, m_share);
	};

	// Operator
	inline client_data& client_data::operator=(const client_data& dat)
	{
		if (this != &dat)
		{
			id_data::operator=(dat);

			m_name = dat.m_name;
			m_color = dat.m_color;
			m_share = dat.m_share;
		}

		return *this;
	}
}

#endif
