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

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif

#include <chrono>
#include <boost/algorithm/string.hpp>
#include <boost/asio/buffer.hpp>

#include "http/http.h"
#include "http/http_message.h"
#include "http/http_request.h"
#include "http/http_response.h"
#include "http/http_socket.h"

#include "security/checksum/xxhash.h"
#include "utility/value.h"

namespace eja
{
	// Constructor
	http_socket::http_socket(http_socket&& obj) : http_service(std::move(obj))
	{
		m_timeout = obj.m_timeout;
		m_socket = obj.m_socket;
		m_timer = obj.m_timer;

		obj.m_timeout = 0;
		obj.m_socket = nullptr;
		obj.m_timer = nullptr;
	}

	http_socket::http_socket(const http_socket& obj) : http_service(obj)
	{
		m_timeout = obj.m_timeout;
		m_socket = obj.m_socket;
		m_timer = obj.m_timer;
	}

	http_socket::http_socket(const io_service_ptr& service, const size_t timeout) : http_service(service)
	{
		m_timeout = timeout;
		m_socket = socket_ptr(new boost::asio::ip::tcp::socket(*m_service));
		m_timer = timer_ptr(new boost::asio::steady_timer(*m_service));
	}

	http_socket::http_socket(const io_service_ptr& service) : http_service(service)
	{
		m_timeout = milliseconds::socket;
		m_socket = socket_ptr(new boost::asio::ip::tcp::socket(*m_service));
		m_timer = timer_ptr(new boost::asio::steady_timer(*m_service));
	}

	// Operator
	http_socket& http_socket::operator=(http_socket&& obj)
	{
		if (this != &obj)
		{
			http_service::operator=(std::move(obj));

			m_timeout = obj.m_timeout;
			m_socket = obj.m_socket;
			m_timer = obj.m_timer;

			obj.m_timeout = 0;
			obj.m_socket = nullptr;
			obj.m_timer = nullptr;
		}

		return *this;
	}

	http_socket& http_socket::operator=(const http_socket& obj)
	{
		if (this != &obj)
		{
			http_service::operator=(obj);

			m_timeout = obj.m_timeout;
			m_socket = obj.m_socket;
			m_timer = obj.m_timer;
		}

		return *this;
	}

	// Interface
	void http_socket::open(const std::string& address, const u16 port)
	{
		boost::system::error_code error;
		const auto timer = get_timer();
		timer->cancel(error);

		try
		{
			// Resolve
			const auto endpoint = resolve(address, port);

			// Timer
			const auto self = shared_from_this();
			timer->expires_from_now(std::chrono::milliseconds(m_timeout));
			timer->async_wait([self, timer](const boost::system::error_code& error) { self->timeout(error); });

			// Open
			const auto socket = get_socket();
			socket->connect(endpoint);

			timer->cancel(error);
		}
		catch (...)
		{
			timer->cancel(error);

			throw;
		}
	}

	void http_socket::close()
	{
		// Thread-safe
		m_mutex.lock();

		// Timer
		boost::system::error_code error;
		m_timer->cancel(error);

		m_socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
		m_socket->close(error);

		m_mutex.unlock();
	}

	http_request::ptr http_socket::read_request()
	{
		return read<http_request>();
	}

	http_response::ptr http_socket::read_response()
	{
		return read<http_response>();
	}

	void http_socket::read(http_message& msg)
	{
		const auto self = shared_from_this();
		const auto service = get_service();
		const auto socket = get_socket();

		boost::system::error_code error;
		const auto timer = get_timer();
		timer->cancel(error);

		try
		{
			// Timer
			timer->expires_from_now(std::chrono::milliseconds(m_timeout));
			timer->async_wait([self, timer](const boost::system::error_code& error) { self->timeout(error); });

			// Header
			boost::asio::streambuf header_buffer;
			const auto header_size = boost::asio::read_until(*m_socket, header_buffer, http::eom);
			timer->cancel(error);

			const auto header = boost::asio::buffer_cast<const char*>(header_buffer.data());
			msg.set(header, header_size);

			// Body (empty)
			if (!msg.has_length())
				return;

			// Body (overlap)
			const auto length = msg.get_length();
			const auto overlap = header_buffer.size() - header_size;
			if (length <= overlap)
			{
				auto body = const_cast<char*>(header + header_size);
				msg.set_body(body, length);

				return;
			}

			/*const*/ std::string body;
			body.resize(length);
			memcpy(reinterpret_cast<void*>(const_cast<char*>(body.data())), header + header_size, overlap);

			// Timer
			timer->expires_from_now(std::chrono::milliseconds(m_timeout));
			timer->async_wait([self, timer](const boost::system::error_code& error) { self->timeout(error); });

			// Body (remaining)
			const auto bytes_remaining = length - overlap;
			auto body_buffer = boost::asio::buffer(&body[overlap], bytes_remaining);
			const auto body_size = boost::asio::read(*m_socket, body_buffer, boost::asio::transfer_exactly(bytes_remaining));
			timer->cancel(error);

			if (body_size)
				msg.set_body(std::move(body));
		}
		catch (...)
		{
			timer->cancel(error);

			throw;
		}
	}

	void http_socket::write(const http_message& msg)
	{
		const auto self = shared_from_this();
		const auto service = get_service();
		const auto socket = get_socket();

		boost::system::error_code error;
		const auto timer = get_timer();
		timer->cancel(error);

		try
		{
			// Timer
			timer->expires_from_now(std::chrono::milliseconds(m_timeout));
			timer->async_wait([self, timer](const boost::system::error_code& error) { self->timeout(error); });

			// Write
			const auto data = msg.str();
			const auto buffer = boost::asio::buffer(data);
			const auto data_size = boost::asio::write(*m_socket, buffer, boost::asio::transfer_all());
			timer->cancel(error);
		}
		catch (...)
		{
			timer->cancel(error);

			throw;
		}
	}

	// Callback
	void http_socket::timeout(const boost::system::error_code& error)
	{
		if (error != boost::asio::error::operation_aborted)
			close();
	}

	// Set
	void http_socket::set_keep_alive(const bool value)
	{
		boost::system::error_code error;
		m_socket->set_option(boost::asio::socket_base::keep_alive(value), error);
		assert(!error);
	}

	// Get
	std::string http_socket::get_local_address() const
	{
		boost::asio::ip::tcp::endpoint endpoint = m_socket->local_endpoint();
		boost::asio::ip::address address = endpoint.address();
		return address.to_string();
	}

	u16 http_socket::get_local_port() const
	{
		boost::asio::ip::tcp::endpoint endpoint = m_socket->local_endpoint();
		return endpoint.port();
	}

	std::string http_socket::get_remote_address() const
	{
		boost::asio::ip::tcp::endpoint endpoint = m_socket->remote_endpoint();
		boost::asio::ip::address address = endpoint.address();
		return address.to_string();
	}

	u16 http_socket::get_remote_port() const
	{
		boost::asio::ip::tcp::endpoint endpoint = m_socket->remote_endpoint();
		return endpoint.port();
	}
}
