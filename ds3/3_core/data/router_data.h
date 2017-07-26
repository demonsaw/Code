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

#ifndef _EJA_ROUTER_DATA_H_
#define _EJA_ROUTER_DATA_H_

#include <list>
#include <memory>

#include "data/id_data.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	// Version
	make_version(router_data, 1);

	// Container
	using router_data_list = std::list<std::shared_ptr<router_data>>;

	// Data
	class router_data final : public id_data
	{
		make_factory(router_data);

	private:
		std::string m_name;
		std::string m_address;
		u16 m_port = 0;

	public:
		router_data() { }
		router_data(const router_data& dat) : id_data(dat), m_name(dat.m_name), m_address(dat.m_address), m_port(dat.m_port) { }

		// Operator
		router_data& operator=(const router_data& dat);

		// Has
		bool has_name() const { return !m_name.empty(); }
		bool has_address() const { return !m_address.empty(); }
		bool has_port() const { return m_port > 0; }

		// Set
		void set_name(const std::string& name) { m_name = name; }
		void set_address(const std::string& address) { m_address = address; }
		void set_port(const u16 port) { m_port = port; }

		// Get
		const std::string& get_name() const { return m_name; }
		const std::string& get_address() const { return m_address; }
		u16 get_port() const { return m_port; }

		MSGPACK_DEFINE(m_id, m_name, m_address, m_port);
	};

	// Operator
	inline router_data& router_data::operator=(const router_data& dat)
	{
		if (this != &dat)
		{
			id_data::operator=(dat);

			m_name = dat.m_name;
			m_address = dat.m_address;
			m_port = dat.m_port;
		}

		return *this;
	}
}

#endif
