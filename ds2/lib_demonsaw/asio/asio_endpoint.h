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

#ifndef _EJA_ASIO_ENDPOINT_
#define _EJA_ASIO_ENDPOINT_

#include <limits>
#include <string>
#include <boost/asio.hpp>

#include "asio.h"
#include "asio_address.h"
#include "asio_port.h"
#include "asio_resolver.h"
#include "network/tcp.h"
#include "system/type.h"

namespace eja
{
	class asio_endpoint final : public type
	{
	private:
		boost::asio::ip::tcp::endpoint m_endpoint;

	public:
		using ptr = std::shared_ptr<asio_endpoint>;

	public:
		asio_endpoint() { }
		asio_endpoint(const asio_endpoint& endpoint) : m_endpoint(endpoint.m_endpoint) { }
		asio_endpoint(const boost::asio::ip::tcp::endpoint& endpoint) : m_endpoint(endpoint) { }

		asio_endpoint(const boost::asio::ip::address& address, const u16 port) : m_endpoint(address, port) { }
		asio_endpoint(const asio_address& address, const asio_port& port) : m_endpoint(address.get_address(), port.get_port()) { }

		asio_endpoint(const std::string& address, const std::string& port) : m_endpoint(asio_resolver::get_address(address), asio_resolver::get_port(port)) { }
		asio_endpoint(const char* address, const char* port) : m_endpoint(asio_resolver::get_address(address), asio_resolver::get_port(port)) { }

		asio_endpoint(const std::string& address, const u16 port) : m_endpoint(asio_resolver::get_address(address), port) { }
		asio_endpoint(const char* address, const u16 port) : m_endpoint(asio_resolver::get_address(address), port) { }

		virtual ~asio_endpoint() override { }

		// Operator
		asio_endpoint& operator=(const asio_endpoint& endpoint);
		asio_endpoint& operator=(const boost::asio::ip::tcp::endpoint& endpoint);
		const boost::asio::ip::tcp::endpoint& operator*() const { return get_endpoint(); }
		boost::asio::ip::tcp::endpoint& operator*() { return get_endpoint(); }

		friend std::ostream& operator<<(std::ostream& os, const asio_endpoint& endpoint) { return os << endpoint.get_address().to_string() << ':' << endpoint.get_port(); }

		// Utility
		virtual bool valid() const override { return !get_address().is_unspecified() && (get_port() > 0 && get_port() <= std::numeric_limits<u16>::max()); }

		// Mutator
		void set_endpoint(const boost::asio::ip::tcp::endpoint& endpoint) { m_endpoint = endpoint; }

		void set_address(const asio_address& address) { m_endpoint.address(address.get_address()); }
		void set_address(const std::string& address) { m_endpoint.address(asio_resolver::get_address(address)); }
		void set_address(const char* address) { m_endpoint.address(asio_resolver::get_address(address)); }

		void set_port(const asio_port& port) { m_endpoint.port(port.get_port()); }		
		void set_port(const std::string& port)  { m_endpoint.port(asio_resolver::get_port(port)); }
		void set_port(const char* port)  { m_endpoint.port(asio_resolver::get_port(port)); }
		void set_port(const u16 port) { m_endpoint.port(port); }

		// Accessor
		const boost::asio::ip::tcp::endpoint& get_endpoint() const { return m_endpoint; }
		boost::asio::ip::tcp::endpoint& get_endpoint() { return m_endpoint; }

		boost::asio::ip::tcp::endpoint::protocol_type get_protocol() const { return m_endpoint.protocol(); }
		boost::asio::ip::address get_address() const { return m_endpoint.address(); }
		u16 get_port() const { return m_endpoint.port(); }

		// Static
		static ptr create() { return std::make_shared<asio_endpoint>(); }
		static ptr create(const asio_endpoint& endpoint) { return std::make_shared<asio_endpoint>(endpoint); }
		static ptr create(const boost::asio::ip::tcp::endpoint& endpoint) { return std::make_shared<asio_endpoint>(endpoint); }

		static ptr create(const boost::asio::ip::address& address, const u16 port) { return std::make_shared<asio_endpoint>(address, port); }
		static ptr create(const asio_address& address, const asio_port& port) { return std::make_shared<asio_endpoint>(address, port); }
		
		static ptr create(const std::string& address, const std::string& port) { return std::make_shared<asio_endpoint>(address, port); }
		static ptr create(const char* address, const char* port) { return std::make_shared<asio_endpoint>(address, port); }

		static ptr create(const std::string& address, const u16 port) { return std::make_shared<asio_endpoint>(address, port); }
		static ptr create(const char* address, const u16 port) { return std::make_shared<asio_endpoint>(address, port); }
	};
}

#endif
