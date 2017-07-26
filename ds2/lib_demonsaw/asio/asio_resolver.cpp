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

#include "asio_resolver.h"

namespace eja
{
	// Address
	boost::asio::ip::address asio_resolver::get_address(const tcp_version version /*= tcp_version::all*/)
	{		
		boost::asio::io_service service;
		boost::asio::ip::tcp::resolver resolver(service);

		const auto& address = boost::asio::ip::host_name();
		const boost::asio::ip::tcp::resolver::query query(address, "");

		boost::system::error_code error;
		auto it = resolver.resolve(query, error);

		if (!error)
		{
			while (it != boost::asio::ip::tcp::resolver::iterator())
			{
				const auto& protocol = *it++;
				const auto& endpoint = protocol.endpoint();
				const auto& address = endpoint.address();
				if (((version & tcp_version::v4) && address.is_v4()) || ((version & tcp_version::v6) && address.is_v6()))
					return address;
			}
		}

		return (version & tcp_version::v4) ? get_address("127.0.0.1") : get_address("::1");
	}

	std::list<boost::asio::ip::address> asio_resolver::get_addresses(const tcp_version version /*= tcp_version::all*/)
	{		
		boost::asio::io_service service;
		boost::asio::ip::tcp::resolver resolver(service);

		const auto& address = boost::asio::ip::host_name();
		const boost::asio::ip::tcp::resolver::query query(address, "");

		boost::system::error_code error;
		std::list<boost::asio::ip::address> list;		
		auto it = resolver.resolve(query, error);

		if (version & tcp_version::v4)
			list.push_back(get_address("127.0.0.1"));
		else if (version & tcp_version::v6)
			list.push_back(get_address("::1"));

		if (!error)
		{			
			while (it != boost::asio::ip::tcp::resolver::iterator())
			{
				const auto& protocol = *it++;
				const auto& endpoint = protocol.endpoint();
				const auto& address = endpoint.address();
				if (((version & tcp_version::v4) && address.is_v4()) || ((version & tcp_version::v6) && address.is_v6()))
					list.push_back(address);
			}
		}

		return list;
	}
}
