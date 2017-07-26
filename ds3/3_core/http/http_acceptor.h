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

#ifndef _EJA_HTTP_ACCEPTOR_H_
#define _EJA_HTTP_ACCEPTOR_H_

#include <string>
#include <boost/asio.hpp>

#include "http/http_service.h"
#include "http/http_socket.h"
#include "system/type.h"

namespace eja
{
	class http_acceptor final : public http_service
	{
		make_param_factory(http_acceptor);

	private:
		acceptor_ptr m_acceptor;

	private:
		// Interface
		ptr shared_from_this() { return shared_from_base<http_acceptor>(); }

	public:
		http_acceptor(const http_acceptor& acceptor) : http_service(acceptor), m_acceptor(acceptor.m_acceptor) { }		
		http_acceptor(const io_service_ptr& service, const size_t timeout) : http_service(service), m_acceptor(acceptor_ptr(new boost::asio::ip::tcp::acceptor(*m_service))) { }
		http_acceptor(const io_service_ptr& service) : http_service(service), m_acceptor(acceptor_ptr(new boost::asio::ip::tcp::acceptor(*m_service))) { }
		virtual ~http_acceptor() override { close(); }

		// Operator
		http_acceptor& operator=(const http_acceptor& listener);
		const boost::asio::ip::tcp::acceptor& operator*() const { return *get_acceptor(); }
		boost::asio::ip::tcp::acceptor& operator*() { return *get_acceptor(); }

		// Open
		void open(const std::string& address, const u16 port) { open(resolve(address, port)); }
		void open(const boost::asio::ip::tcp::endpoint& endpoint);

		// Close
		void close();

		// Accept
		template <typename callback> 
		void accept(callback call);

		// Utility
		bool valid() const { return m_acceptor->is_open(); }
		bool invalid() const { return !valid(); }

		// Get
		const acceptor_ptr get_acceptor() const { return m_acceptor; }

		boost::asio::ip::tcp::endpoint::protocol_type get_protocol() const { return m_acceptor->local_endpoint().protocol(); }
		std::string get_address() const { return m_acceptor->local_endpoint().address().to_string(); }
		u16 get_port() const { return m_acceptor->local_endpoint().port(); }

		// Static
		static ptr create(const io_service_ptr& service) { return ptr(new http_acceptor(service)); }
	};

	// Accept
	template <typename callback>
	void http_acceptor::accept(callback call)
	{
		const auto acceptor = get_acceptor();
		const auto socket = http_socket::create(m_service);
		const auto self = shared_from_this();

		acceptor->async_accept(**socket, [self, acceptor, socket, call](const boost::system::error_code& error)
		{
			if (!error)
				self->accept(call);

			call(error, socket);
		});
	}
}

#endif
