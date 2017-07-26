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

#include "http_listener.h"

namespace eja
{
	// Open
	void http_listener::open(const boost::asio::ip::tcp::endpoint& endpoint)
	{
		boost::system::error_code error;
		m_acceptor.open(endpoint.protocol(), error);

		if (error)
			throw boost::system::system_error(error);

		m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true), error);

		if (error)
			throw boost::system::system_error(error);

		m_acceptor.bind(endpoint, error);

		if (error)
			throw boost::system::system_error(error);

		m_acceptor.listen(boost::asio::socket_base::max_connections, error);

		if (error)
			throw boost::system::system_error(error);
	}

	// Close
	void http_listener::close()
	{
		if (valid())
		{
			boost::system::error_code error;
			m_acceptor.close(error);
		}
	}

	// Accept
	http_socket::ptr http_listener::accept()
	{
		http_socket::ptr socket = http_socket::create();
		socket->set_timeout(0);

		accept(*socket);
		
		return socket;
	}

	void http_listener::accept(http_socket& socket)
	{
		boost::system::error_code error;
		m_acceptor.accept(*socket, error);

		if (error)
			throw boost::system::system_error(error);
	}
}
