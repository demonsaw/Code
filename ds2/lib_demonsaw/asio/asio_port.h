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

#ifndef _EJA_ASIO_PORT_
#define _EJA_ASIO_PORT_

#include <iostream>
#include <limits>
#include <memory>
#include <string>

#include "asio_resolver.h"
#include "system/type.h"

namespace eja
{
	class asio_port final : public type
	{
	private:
		u16 m_port = 0;

	public:
		using ptr = std::shared_ptr<asio_port>;

	public:
		asio_port() { }
		asio_port(const asio_port& port) : m_port(port.m_port) { }		
		asio_port(const std::string& port) : m_port(asio_resolver::get_port(port)) { }
		asio_port(const char* port) : m_port(asio_resolver::get_port(port)) { }
		asio_port(const u16 port) : m_port(port) { }
		virtual ~asio_port() override { }

		// Operator		
		asio_port& operator=(const asio_port& port);		
		asio_port& operator=(const std::string& port);
		asio_port& operator=(const char* port);
		asio_port& operator=(const u16 port);

		const u16& operator*() const { return get_port(); }
		u16& operator*() { return get_port(); }

		friend std::ostream& operator<<(std::ostream& os, const asio_port& port) { return os << port.get_port(); }

		// Utility
		virtual bool valid() const override { return m_port > 0 && m_port <= std::numeric_limits<u16>::max(); }

		// Mutator		
		void set_port(const std::string& port) { m_port = asio_resolver::get_port(port); }
		void set_port(const char* port) { m_port = asio_resolver::get_port(port); }
		void set_port(const u16 port) { m_port = port; }

		// Accessor
		const u16& get_port() const { return m_port; }
		u16& get_port() { return m_port; }

		// Static
		static bool valid(const std::string& port);
		static bool invalid(const std::string& port) { return !valid(port); }

		static bool valid(const char* port);
		static bool invalid(const char* port) { return !valid(port); }

		static bool valid(u16 port) { return port > 0 && port <= std::numeric_limits<u16>::max();; }
		static bool invalid(u16 port) { return !valid(port); }

		// Static
		static ptr create() { return std::make_shared<asio_port>(); }
		static ptr create(const asio_port& port) { return std::make_shared<asio_port>(port); }
		static ptr create(const std::string& port) { return std::make_shared<asio_port>(port); }
		static ptr create(const char* port) { return std::make_shared<asio_port>(port); }
		static ptr create(const u16 port) { return std::make_shared<asio_port>(port); }
	};
}

#endif
