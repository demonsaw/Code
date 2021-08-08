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

#include "http/http_service.h"

namespace eja
{
	// Constructor
	http_service::http_service(http_service&& obj) : object(std::move(obj))
	{
		m_service = obj.m_service;
		
		obj.m_service = nullptr;
	}

	// Operator
	http_service& http_service::operator=(http_service&& obj)
	{
		if (this != &obj)
		{
			object::operator=(std::move(obj));

			m_service = obj.m_service;

			obj.m_service = nullptr;
		}			

		return *this;
	}

	http_service& http_service::operator=(const http_service& obj)
	{
		if (this != &obj)
		{
			object::operator=(obj);

			m_service = obj.m_service;
		}			

		return *this;
	}

	// Get
	boost::asio::ip::tcp::endpoint http_service::resolve(const std::string& address, const u16 port) const
	{
		boost::asio::ip::tcp::resolver resolver(*m_service);
		boost::asio::ip::tcp::resolver::query query(address, std::to_string(port));		
		const auto it = resolver.resolve(query);

		if (it != boost::asio::ip::tcp::resolver::iterator())
			return it->endpoint();

		return boost::asio::ip::tcp::endpoint();
	}
}
