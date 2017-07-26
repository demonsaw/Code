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

#include "asio_port.h"

namespace eja
{
	// Operator
	asio_port& asio_port::operator=(const asio_port& port)
	{
		if (this != &port)
			m_port = port.m_port;

		return *this;
	}

	asio_port& asio_port::operator=(const std::string& port)
	{
		m_port = asio_resolver::get_port(port);

		return *this;
	}

	asio_port& asio_port::operator=(const char* port)
	{
		m_port = asio_resolver::get_port(port);

		return *this;
	}
	
	asio_port& asio_port::operator=(const u16 port)
	{
		m_port = port;

		return *this;
	}

	// Static
	bool asio_port::valid(const std::string& port)
	{
		boost::asio::io_service service;
		boost::asio::ip::tcp::resolver resolver(service);
		boost::asio::ip::tcp::resolver::query query(port);

		boost::system::error_code error;
		auto it = resolver.resolve(query, error);

		return !error && it != boost::asio::ip::tcp::resolver::iterator();
	}

	bool asio_port::valid(const char* port)
	{
		boost::asio::io_service service;
		boost::asio::ip::tcp::resolver resolver(service);
		boost::asio::ip::tcp::resolver::query query(port);

		boost::system::error_code error;
		auto it = resolver.resolve(query, error);

		return !error && it != boost::asio::ip::tcp::resolver::iterator();
	}
}
