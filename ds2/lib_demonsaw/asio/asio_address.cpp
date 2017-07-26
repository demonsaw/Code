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

#include "asio_address.h"

namespace eja
{
	// Operator
	asio_address& asio_address::operator=(const asio_address& address)
	{
		if (this != &address)
			m_address = address.m_address;

		return *this;
	}

	asio_address& asio_address::operator=(const boost::asio::ip::address& address)
	{
		m_address = address;

		return *this;
	}

	asio_address& asio_address::operator=(const std::string& address)
	{
		m_address = asio_resolver::get_address(address);

		return *this;
	}

	asio_address& asio_address::operator=(const char* address)
	{
		m_address = asio_resolver::get_address(address);

		return *this;
	}

	// Static
	bool asio_address::valid(const std::string& address)
	{
		// IP
		boost::system::error_code error;
		boost::asio::ip::address::from_string(address, error);

		// DNS
		if (error == boost::asio::error::invalid_argument)
		{
			boost::asio::io_service service;
			boost::asio::ip::tcp::resolver resolver(service);
			boost::asio::ip::tcp::resolver::query query(address, "");
			resolver.resolve(query, error);
		}

		return !error;
	}

	bool asio_address::valid(const char* address)
	{
		// IP
		boost::system::error_code error;
		boost::asio::ip::address::from_string(address, error);

		// DNS
		if (error == boost::asio::error::invalid_argument)
		{
			boost::asio::io_service service;
			boost::asio::ip::tcp::resolver resolver(service);
			boost::asio::ip::tcp::resolver::query query(address, "");
			resolver.resolve(query, error);
		}

		return !error;
	}
}
