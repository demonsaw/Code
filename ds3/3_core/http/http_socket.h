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

#include <chrono>
#include <memory>
#include <string>
#include <boost/asio.hpp>

#include "http/http.h"
#include "http/http_service.h"
#include "system/type.h"

namespace eja
{
	class http_message;

	class http_socket final : public http_service
	{
		make_param_factory(http_socket);

	private:
		size_t m_timeout = 0;
		socket_ptr m_socket;
		timer_ptr m_timer;

	private:
		// Interface
		ptr shared_from_this() { return shared_from_base<http_socket>(); }

		// Read
		template <typename message, typename callback> 
		void read(const io_service_ptr service, const std::shared_ptr<message> msg, const string_ptr body, const size_t bytes_offset, const size_t bytes_remaining, callback call);

		// Callback
		void timeout(const boost::system::error_code& error);

	public:
		http_socket(const http_socket& socket);
		http_socket(const io_service_ptr& service, const size_t timeout);
		http_socket(const io_service_ptr& service);
		virtual ~http_socket() override { close(); }

		// Operator
		http_socket& operator=(const http_socket& socket);
		const boost::asio::ip::tcp::socket& operator*() const { return *get_socket(); }
		boost::asio::ip::tcp::socket& operator*() { return *get_socket(); }

		// Open
		void open(const std::string& address, const u16 port);

		template <typename callback>
		void open(const std::string& address, const u16 port, callback call);

		// Close
		void close();

		// Read
		size_t read(const std::shared_ptr<http_message>& request) { return read(*request); }
		size_t read(http_message& msg);

		template <typename message, typename callback> 
		void read(callback call);

		// Write
		size_t write(const http_message& msg);

		template <typename message, typename callback>
		void write(const std::shared_ptr<message> msg, callback call);
		
		template <typename T>
		size_t write(const std::shared_ptr<T> msg) { return write(*msg); }

		// Utility		
		bool valid() const { return m_socket->is_open(); }
		bool invalid() const { return !valid(); }

		// Set
		void set_timeout(const size_t timeout) { m_timeout = timeout; }
		void set_keep_alive(const bool keep_alive);

		// Get
		size_t get_timeout() const { return m_timeout; }
		const socket_ptr get_socket() const { return m_socket; }
		const timer_ptr get_timer() const { return m_timer; }		

		// Static
		static ptr create(const io_service_ptr& service) { return ptr(new http_socket(service)); }
	};

	// Open
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

	// Read
	template <typename message, typename callback>
	void http_socket::read(callback call)
	{
		const auto self = shared_from_this();
		const auto service = get_service();
		const auto socket = get_socket();

		// Timer
		const auto timer = get_timer();
		timer->expires_from_now(std::chrono::milliseconds(m_timeout));
		timer->async_wait([self, timer](const boost::system::error_code& error) { self->timeout(error); });

		// Read		
		const auto msg = message::create(); 
		streambuf_ptr header_buffer = streambuf_ptr(new boost::asio::streambuf);

		boost::asio::async_read_until(*socket, *header_buffer, http::eom, [self, service, socket, timer, msg, header_buffer, call](const boost::system::error_code& error, const size_t bytes_transferred)
		{
			// Timer
			timer->cancel();

			if (!error)
			{
				// Header
				const auto header = boost::asio::buffer_cast<const char*>(header_buffer->data());
				msg->set(header, bytes_transferred);

				// Body (none)
				if (!msg->has_length())
				{
					call(error, msg);
					return;
				}

				// Body (partial)
				const auto length = msg->get_length();
				const auto overlap = header_buffer->size() - bytes_transferred;
				if (length <= overlap)
				{
					auto body = const_cast<char*>(header + bytes_transferred);
					body[length] = '\0';
					msg->set_body(body, length);

					call(error, msg);
					return;
				}

				const auto body = string_ptr(new std::string);
				body->resize(length);
				memcpy(reinterpret_cast<void*>(const_cast<char*>(body->data())), header + bytes_transferred, overlap);

				// Body (remaining)
				const auto bytes_remaining = length - overlap;
				self->read(service, msg, body, overlap, bytes_remaining, call);
			}
			else
			{
				// Error
				call(error, msg);
			}
		});
	}

	template <typename message, typename callback>
	void http_socket::read(const io_service_ptr service, const std::shared_ptr<message> msg, const string_ptr body, const size_t bytes_offset, const size_t bytes_remaining, callback call)
	{
		// TODO: Add http throttle code
		//

		const auto self = shared_from_this();
		const auto socket = get_socket();

		// Timer
		const auto timer = get_timer();
		timer->expires_from_now(std::chrono::milliseconds(m_timeout));
		timer->async_wait([self, timer](const boost::system::error_code& error) { self->timeout(error); });

		// Body (remaining)		
		const auto bytes_body = bytes_offset + bytes_remaining; 
		const auto body_buffer = boost::asio::buffer(&(*body)[bytes_offset], bytes_remaining);
		
		boost::asio::async_read(*socket, body_buffer, boost::asio::transfer_exactly(bytes_remaining), [service, socket, timer, msg, body, bytes_body, call](const boost::system::error_code& error, const size_t bytes_transferred)
		{
			timer->cancel();

			if (!error)
				msg->set_body(std::move(*body));
			
			call(error, msg);
		});
	}

	// Write
	template <typename message, typename callback>
	void http_socket::write(const std::shared_ptr<message> msg, callback call)
	{
		// TODO: Add http throttle code
		//

		const auto self = shared_from_this();
		const auto service = get_service();
		const auto socket = get_socket();

		// Timer
		const auto timer = get_timer();
		timer->expires_from_now(std::chrono::milliseconds(m_timeout));
		timer->async_wait([self, timer](const boost::system::error_code& error) { self->timeout(error); });

		// Write
		const auto data = msg->str_ptr();
		const auto data_buffer = boost::asio::buffer(*data);

		return boost::asio::async_write(*socket, data_buffer, boost::asio::transfer_all(), [service, socket, timer, msg, data, call](const boost::system::error_code& error, const size_t bytes_transferred)
		{
			// Timer
			timer->cancel();

			call(error, msg);
		});
	}

}

#endif
