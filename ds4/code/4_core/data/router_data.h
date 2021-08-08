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

#include <deque>
#include <memory>
#include <string>
#include <boost/format.hpp>

#include "data/data.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	make_using_version(router_data, 1);

	using router_data_list = std::deque<std::shared_ptr<router_data>>;

	class router_data final : public data
	{
		make_factory(router_data);

	private:
		std::string m_name;
		std::string m_address;
		std::string m_password;
		u16 m_port = 0;

	public:
		router_data() { }

		// Utility
		virtual bool valid() const override { return data::valid() && has_endpoint(); }

		// Has		
		bool has_address() const { return !m_address.empty(); }
		bool has_endpoint() const { return has_address() && has_port(); }
		bool has_name() const { return !m_name.empty(); }
		bool has_password() const { return !m_password.empty(); }
		bool has_port() const { return m_port > 0; }

		// Set		
		void set_address(const std::string& address) { m_address = address; }
		void set_name(const std::string& name) { m_name = name; }
		void set_password(const std::string& password) { m_password = password; }
		void set_port(const u16 port) { m_port = port; }

		// Get
		const std::string& get_address() const { return m_address; }
		std::string get_endpoint() const { return boost::str(boost::format("%s:%u") % m_address % m_port); }
		const std::string& get_name() const { return m_name; }
		const std::string& get_password() const { return m_password; }
		u16 get_port() const { return m_port; }

		MSGPACK_DEFINE(MSGPACK_BASE(data), m_name, m_address, m_port, m_password);
	};
}

#endif
