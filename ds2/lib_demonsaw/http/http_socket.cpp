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

#include <errno.h>
#include <exception>
#include <streambuf>
#include <boost/asio/buffer.hpp>
#include <boost/format.hpp>
#include <boost/shared_array.hpp>

#include "http_socket.h"

namespace eja
{
	// Open
	void http_socket::open(const boost::asio::ip::tcp::endpoint& endpoint)
	{
		close();

		boost::system::error_code error;
		m_socket.connect(endpoint, error);

		if (!error)
		{
			set_timeout();
			set_keep_alive();
		}
		else
		{
			close();

			throw boost::system::system_error(error);
		}
	}

	// Close
	void http_socket::close()
	{
		if (valid())
		{
			boost::system::error_code error;

			auto_lock();
			m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
			m_socket.close(error);
		}
	}

	// Read
	size_t http_socket::read(http_message& message)
	{
		// Header
		boost::system::error_code error;
		boost::asio::streambuf header_buffer;
		const auto header_size = boost::asio::read_until(m_socket, header_buffer, http::eom, error);
		if (!header_size)
			return 0;

		if (error)
		{
			if (boost::asio::error::eof == error)
				return 0;
			
			throw boost::system::system_error(error);
		}			
		
		const char* header = boost::asio::buffer_cast<const char*>(header_buffer.data());
		const std::string header_data(header, header_size);
		message.parse(header_data);

		if (!message.has_length())
			return header_size;

		// Body (overlap)
		const auto overlap = header_buffer.size() - header_size;
		if (message.get_length() < overlap)
		{
			char* body = const_cast<char*>(header + header_size);
			body[message.get_length()] = '\0';
			message.set_body(body);

			return header_buffer.size();
		}

		const auto remaining = message.get_length() - overlap;
		if (!remaining)
		{
			char* body = const_cast<char*>(header + header_size);
			body[message.get_length()] = '\0';
			message.set_body(body);

			return header_buffer.size();
		}
		
		boost::shared_array<char> data(new char[message.get_length() + 1]);
		memcpy(data.get(), header + header_size, overlap);

		// Body (remaining)
		auto data_buffer = boost::asio::buffer(&data[overlap], remaining);
		const auto data_size = boost::asio::read(m_socket, data_buffer, boost::asio::transfer_exactly(remaining), error);
		if (!data_size)
			return 0;

		if (error)
		{
			if (boost::asio::error::eof == error)
				return 0;

			throw boost::system::system_error(error);
		}

		data[message.get_length()] = '\0';
		message.set_body(data.get(), message.get_length());

		return header_size + data_size;
	}

	// Write
	size_t http_socket::write(const http_message& message)
	{
		boost::system::error_code error;
		const std::string data = message.str();
		auto buffer = boost::asio::buffer(data);
		const auto bytes = boost::asio::write(m_socket, buffer, boost::asio::transfer_all(), error);

		if (error)
		{
			if (boost::asio::error::eof == error)
				return 0;

			throw boost::system::system_error(error);
		}

		return bytes;
	}

	size_t http_socket::write(const http_code& code)
	{
		http_response response(code);
		return write(response);
	}

	// Mutator
	void http_socket::set_timeout()
	{
		if (valid())
		{
			boost::system::error_code error;
			m_socket.set_option(boost::asio::ip::tcp::no_delay(true), error);
			if (error)
				return;

#if _WIN32
			// Timeout
			int32_t tdata = static_cast<int32_t>(m_timeout);
			setsockopt(m_socket.native_handle(), SOL_SOCKET, SO_RCVTIMEO, (const char*) &tdata, sizeof(int32_t));
			setsockopt(m_socket.native_handle(), SOL_SOCKET, SO_SNDTIMEO, (const char*) &tdata, sizeof(int32_t));

			// Linger
			linger ldata = { TRUE, 0 };
			setsockopt(m_socket.native_handle(), SOL_SOCKET, SO_LINGER, (const char*) &ldata, sizeof(linger));
#else
			// Timeout
			timeval tdata = { static_cast<int32_t>(m_timeout) / 1000, 0 };
			setsockopt(m_socket.native_handle(), SOL_SOCKET, SO_RCVTIMEO, &tdata, sizeof(timeval));
			setsockopt(m_socket.native_handle(), SOL_SOCKET, SO_SNDTIMEO, &tdata, sizeof(timeval));

			// Linger
			linger ldata = { 1, 0 };
			setsockopt(m_socket.native_handle(), SOL_SOCKET, SO_LINGER, &ldata, sizeof(linger));
#endif
		}
	}

	void http_socket::set_timeout(const size_t timeout)
	{ 
		m_timeout = timeout;
		set_timeout();
	}

	void http_socket::set_keep_alive()
	{
		if (valid())
		{
			boost::system::error_code error;
			m_socket.set_option(boost::asio::socket_base::keep_alive(m_keep_alive), error);
		}			
	}

	void http_socket::set_keep_alive(const bool keep_alive)
	{
		m_keep_alive = keep_alive;
		set_keep_alive();
	}
}
