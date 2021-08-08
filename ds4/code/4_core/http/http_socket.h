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

#ifndef _EJA_HTTP_SOCKET_H_
#define _EJA_HTTP_SOCKET_H_

#include <memory>
#include <mutex>
#include <string>
#include <boost/asio.hpp>

#include "http/http_service.h"
#include "system/type.h"

namespace eja
{
	class http_message;
	class http_request;
	class http_response;

	class http_socket : public http_service
	{
		make_param_factory(http_socket);

	private:
		size_t m_timeout = 0;
		socket_ptr m_socket;
		timer_ptr m_timer;
		std::mutex m_mutex;

	private:
		// Interface
		ptr shared_from_this() { return shared_from_base<http_socket>(); }

		// Callback
		void timeout(const boost::system::error_code& error);

	public:
		http_socket(http_socket&& obj);
		http_socket(const http_socket& obj);
		http_socket(const io_service_ptr& service, const size_t timeout);
		explicit http_socket(const io_service_ptr& service);
		virtual ~http_socket() override { close(); }

		// Operator
		http_socket& operator=(http_socket&& obj);
		http_socket& operator=(const http_socket& obj);

		const boost::asio::ip::tcp::socket& operator*() const { return *get_socket(); }
		boost::asio::ip::tcp::socket& operator*() { return *get_socket(); }

		// Interface
		template <typename callback>
		void open(const std::string& address, const u16 port, callback call);
		void open(const std::string& address, const u16 port);
		void close();

		template <typename T>
		std::shared_ptr<T> read();
		void read(const std::shared_ptr<http_message>& request) { return read(*request); }
		void read(http_message& msg);

		std::shared_ptr<http_request> read_request();
		std::shared_ptr<http_response> read_response();

		template <typename T>
		void write(const std::shared_ptr<T> msg) { return write(*msg); }
		void write(const http_message& msg);

		// Utility
		bool valid() const { return m_socket->is_open(); }
		bool invalid() const { return !valid(); }

		// Has
		bool has_timeout() const { return m_timeout > 0; }

		// Set
		void set_keep_alive(const bool keep_alive);

		void set_timeout() { m_timeout = 0; }
		void set_timeout(const size_t timeout) { m_timeout = timeout; }

		// Get
		std::string get_local_address() const;
		u16 get_local_port() const;

		std::string get_remote_address() const;
		u16 get_remote_port() const;

		socket_ptr get_socket() const { return m_socket; }
		timer_ptr get_timer() const { return m_timer; }
		size_t get_timeout() const { return m_timeout; }

		// Static
		static ptr create(const io_service_ptr& service, const size_t timeout) { return std::make_shared<http_socket>(service, timeout); }
		static ptr create(const io_service_ptr& service) { return std::make_shared<http_socket>(service); }
	};

	// Interface
	template <typename callback>
	void http_socket::open(const std::string& address, const u16 port, callback call)
	{
		const auto self = shared_from_this();
		const auto service = get_service();
		const auto socket = get_socket();

		// Timer
		const auto timer = get_timer();
		timer->expires_from_now(std::chrono::milliseconds(m_timeout));
		timer->async_wait([self, timer](const boost::system::error_code& error) { self->timeout(error); });

		resolve(address, port, [self, service, socket, timer, call](const boost::system::error_code& error, const boost::asio::ip::tcp::endpoint& endpoint)
		{
			if (!error)
			{
				// Connect
				socket->async_connect(endpoint, [service, timer, call](const boost::system::error_code& error)
				{
					// Timer
					timer->cancel();

					call(error);
				});
			}
			else
			{
				// Timer
				timer->cancel();

				call(error);
			}
		});
	}

	template <typename T>
	std::shared_ptr<T> http_socket::read()
	{
		const auto message = T::create();
		read(message);

		return message;
	}
}

#endif
