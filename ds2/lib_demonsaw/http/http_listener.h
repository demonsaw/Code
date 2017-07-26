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

#ifndef _EJA_HTTP_LISTENER_
#define _EJA_HTTP_LISTENER_

#include <memory>
#include <boost/asio.hpp>

#include "http_socket.h"
#include "asio/asio_endpoint.h"
#include "asio/asio_service.h"
#include "network/tcp.h"
#include "system/type.h"

namespace eja
{
	class http_listener final : public type, public std::enable_shared_from_this<http_listener>
	{
	private:
		asio_service m_service;
		boost::asio::ip::tcp::acceptor m_acceptor;

	public:
		using ptr = std::shared_ptr<http_listener>;

	private:
		http_listener() : m_acceptor(*m_service) { }
		http_listener(const http_listener&) = delete;
		http_listener(const asio_service& service) : m_service(service), m_acceptor(*m_service) { }

		// Operator
		http_listener& operator=(const http_listener&) = delete;

	public:		
		~http_listener() { }

		// Open
		void open(const boost::asio::ip::tcp::endpoint& endpoint);		
		void open(const boost::asio::ip::address& address, const u16 port) { open(boost::asio::ip::tcp::endpoint(address, port)); }
		void open(const asio_endpoint& endpoint) { open(*endpoint); }
		void open(const asio_address& address, const asio_port& port) { open(boost::asio::ip::tcp::endpoint(address.get_address(), port.get_port())); }
		void open(const std::string& address, const std::string& port) { open(asio_endpoint(address, port)); }
		void open(const std::string& address, const u16 port) { open(asio_endpoint(address, port)); }

		// Close
		void close();

		// Accept
		http_socket::ptr accept();
		void accept(http_socket& socket);
		void accept(const http_socket::ptr socket)  { accept(*socket); }
		template <typename Callback> void accept(Callback callback);
		template <typename Callback, typename Object> void accept(Callback callback, Object object);

		// Utility
		virtual bool valid() const override { return m_acceptor.is_open(); }
		size_t run() const { return m_service.run(); }
		void reset() const { m_service.reset(); }

		// Accessor
		const asio_service& get_service() const { return m_service; }
		asio_service& get_service() { return m_service; }

		boost::asio::ip::tcp::endpoint::protocol_type get_protocol() const { return m_acceptor.local_endpoint().protocol(); }
		boost::asio::ip::address get_address() const { return m_acceptor.local_endpoint().address(); }
		u16 get_port() const { return m_acceptor.local_endpoint().port(); }

		// Static
		static ptr create() { return ptr(new http_listener()); }
		static ptr create(const asio_service& service) { return ptr(new http_listener(service)); }
	};

	// Accept
	template <typename Callback>
	void http_listener::accept(Callback callback)
	{
		const auto self = shared_from_this();
		const auto socket = http_socket::create();
		socket->set_timeout(0);

		m_acceptor.async_accept(**socket, [self, socket, callback] (const boost::system::error_code& error)
		{
			callback(!error ? socket : nullptr);
		});
	}

	template <typename Callback, typename Object> 
	void http_listener::accept(Callback callback, Object object)
	{
		const auto self = shared_from_this();
		const auto socket = http_socket::create();
		socket->set_timeout(0);

		m_acceptor.async_accept(**socket, [self, socket, object, callback] (const boost::system::error_code& error)
		{
			(static_cast<Object>(object).*callback)(!error ? socket : nullptr);
		});
	}
}

#endif
