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

#ifndef _EJA_ASIO_ADDRESS_
#define _EJA_ASIO_ADDRESS_

#include <memory>
#include <string>
#include <boost/asio.hpp>
#include <boost/asio/ip/address.hpp>

#include "asio_resolver.h"
#include "system/type.h"

namespace eja
{
	class asio_address final : public type
	{
	private:
		boost::asio::ip::address m_address;

	public:
		using ptr = std::shared_ptr<asio_address>;

	public:
		asio_address() { }
		asio_address(const asio_address& address) : m_address(address.m_address) { }
		asio_address(const boost::asio::ip::address& address) : m_address(address) { }
		asio_address(const std::string& address) : m_address(asio_resolver::get_address(address)) { }
		asio_address(const char* address) : m_address(asio_resolver::get_address(address)) { }
		virtual ~asio_address() override { }

		// Operator
		asio_address& operator=(const asio_address& address);
		asio_address& operator=(const boost::asio::ip::address& address);
		asio_address& operator=(const std::string& address);
		asio_address& operator=(const char* address);

		const boost::asio::ip::address& operator*() const { return get_address(); }
		boost::asio::ip::address& operator*() { return get_address(); }

		friend std::ostream& operator<<(std::ostream& os, const asio_address& address) { return os << address.get_address().to_string(); }

		// Utility
		virtual bool valid() const override { return !m_address.is_unspecified(); }

		// Mutator
		void set_address(const boost::asio::ip::address& address) { m_address = address; }
		void set_address(const std::string& address) { m_address = asio_resolver::get_address(address); }
		void set_address(const char* address) { m_address = asio_resolver::get_address(address); }

		// Accessor
		const boost::asio::ip::address& get_address() const { return m_address; }
		boost::asio::ip::address& get_address() { return m_address; }

		// Static
		static bool valid(const std::string& address);
		static bool invalid(const std::string& address) { return !valid(address); }

		static bool valid(const char* address);
		static bool invalid(const char* address) { return !valid(address); }

		static ptr create() { return std::make_shared<asio_address>(); }
		static ptr create(const asio_address& address) { return std::make_shared<asio_address>(address); }
		static ptr create(const boost::asio::ip::address& address) { return std::make_shared<asio_address>(address); }
		static ptr create(const std::string& address) { return std::make_shared<asio_address>(address); }
		static ptr create(const char* address) { return std::make_shared<asio_address>(address); }
	};
}

#endif
