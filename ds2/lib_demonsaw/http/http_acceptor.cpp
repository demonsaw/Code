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

#include "http/http_acceptor.h"
#include "http/http_socket.h"

namespace eja
{
	// Operator
	http_acceptor& http_acceptor::operator=(const http_acceptor& acceptor)
	{
		if (this != &acceptor)
		{
			http_service::operator=(acceptor);

			m_acceptor = acceptor.m_acceptor;
		}

		return *this;
	}

	// Open
	void http_acceptor::open(const boost::asio::ip::tcp::endpoint& endpoint)
	{
		m_acceptor->open(endpoint.protocol());
		m_acceptor->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
		m_acceptor->bind(endpoint);
		m_acceptor->listen(boost::asio::socket_base::max_connections);
	}

	// Close
	void http_acceptor::close()
	{
		boost::system::error_code error;
		m_acceptor->cancel(error);
		m_acceptor->close(error);
	}
}
