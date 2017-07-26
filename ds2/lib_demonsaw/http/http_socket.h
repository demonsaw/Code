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

#ifndef _EJA_HTTP_SOCKET_
#define _EJA_HTTP_SOCKET_

#include <memory>
#include <string>
#include <boost/asio.hpp>

#include "asio/asio.h"
#include "asio/asio_address.h"
#include "asio/asio_endpoint.h"
#include "asio/asio_port.h"
#include "asio/asio_service.h"
#include "http/http_code.h"
#include "http/http_request.h"
#include "http/http_response.h"
#include "system/mutex/mutex.h"
#include "system/type.h"
#include "utility/default_value.h"

namespace eja
{
	class http_listener;

	class http_socket final : public type, public mutex, public std::enable_shared_from_this<http_socket>
	{
		friend class http_listener;

	private:
		asio_service m_service;
		boost::asio::ip::tcp::socket m_socket;		
		size_t m_timeout = default_socket::num_timeout;
		bool m_keep_alive = default_socket::keep_alive;

	public:
		using ptr = std::shared_ptr<http_socket>;

	private:
		http_socket() : m_socket(*m_service) { }
		http_socket(io_service_ptr service) : m_service(service), m_socket(*service) { }
		http_socket(const http_socket&) = delete;
		http_socket(const asio_service& service) : m_service(service), m_socket(*const_cast<asio_service&>(service)) { }

		// Operator
		http_socket& operator=(const http_socket&) = delete;		

		// Mutator
		void set_timeout();
		void set_keep_alive();

		// Accessor
		const boost::asio::ip::tcp::socket& get_socket() const { return m_socket; }
		boost::asio::ip::tcp::socket& get_socket() { return m_socket; }

	public:		
		~http_socket() override { close(); }

		// Operator
		const boost::asio::ip::tcp::socket& operator*() const { return get_socket(); }
		boost::asio::ip::tcp::socket& operator*() { return get_socket(); }

		// Utility
		bool has_timeout() const { return m_timeout > 0; }

		// Open
		void open(const boost::asio::ip::tcp::endpoint& endpoint);		
		void open(const boost::asio::ip::address& address, const u16 port) { open(boost::asio::ip::tcp::endpoint(address, port)); }
		void open(const asio_endpoint& endpoint) { open(*endpoint); }
		void open(const asio_address& address, const asio_port& port) { open(boost::asio::ip::tcp::endpoint(address.get_address(), port.get_port())); }
		void open(const std::string& address, const std::string& port) { open(asio_endpoint(address, port)); }
		void open(const std::string& address, const u16 port) { open(asio_endpoint(address, port)); }

		/*template <typename Callback> void open(const boost::asio::ip::tcp::endpoint& endpoint, Callback callback);
		template <typename Callback> void open(const boost::asio::ip::address& address, const u16 port, Callback callback) { open(boost::asio::ip::tcp::endpoint(address, port), callback); }
		template <typename Callback> void open(const asio_endpoint& endpoint, Callback callback) { open(*endpoint, callback); }
		template <typename Callback> void open(const asio_address& address, const asio_port& port, Callback callback) { open(boost::asio::ip::tcp::endpoint(address.get_address(), port.get_port()), callback); }
		template <typename Callback> void open(const std::string& address, const std::string& port, Callback callback);
		template <typename Callback> void open(const std::string& address, const u16 port, Callback callback);

		template <typename Callback, typename Object> void open(const boost::asio::ip::tcp::endpoint& endpoint, Callback callback, Object object);
		template <typename Callback, typename Object> void open(const boost::asio::ip::address& address, const u16 port, Callback callback, Object object) { open(boost::asio::ip::tcp::endpoint(address, port), object, callback); }
		template <typename Callback, typename Object> void open(const asio_endpoint& endpoint, Callback callback, Object object) { open(*endpoint, object, callback); }
		template <typename Callback, typename Object> void open(const asio_address& address, const asio_port& port, Callback callback, Object object) { open(boost::asio::ip::tcp::endpoint(address.get_address(), port.get_port()), object, callback); }
		template <typename Callback, typename Object> void open(const std::string& address, const std::string& port, Callback callback, Object object);
		template <typename Callback, typename Object> void open(const std::string& address, const u16 port, Callback callback, Object object);*/

		// Close
		void close();

		// Read		
		size_t read(http_message& message);
		size_t read(const http_message::ptr message) { return read(*message); }
		template <typename T, typename Callback> void read(Callback callback);
		template <typename T, typename Callback, typename Object> void read(Callback callback, Object object);

		// Write
		size_t write(const http_message& message);
		size_t write(const http_message::ptr message) { return write(*message); }
		template <typename T, typename Callback> void write(const T& message, Callback callback);
		template <typename T, typename Callback, typename Object> void write(const T& message, Callback callback, Object object);

		size_t write(const http_code& code);
		template <typename T, typename Callback> void write(const http_code& code, Callback callback);
		template <typename T, typename Callback, typename Object> void write(const http_code& code, Callback callback, Object object);

		size_t write(const http_status& status) { return write(status.get_code()); }
		template <typename T, typename Callback> void write(const http_status& status, Callback callback) { write(status.get_code(), callback); }
		template <typename T, typename Callback, typename Object> void write(const http_status& status, Callback callback, Object object) { write(status.get_code(), callback, object); }

		// Utility
		virtual bool valid() const override { return m_socket.is_open(); }
		size_t run() const { return m_service.run(); }
		void reset() const { m_service.reset(); }

		// Mutator
		void set_timeout(const size_t timeout); 
		void set_keep_alive(const bool keep_alive);

		// Accessor		
		size_t get_timeout() const { return m_timeout; }

		const asio_service& get_service() const { return m_service; }
		asio_service& get_service() { return m_service; }

		// Static
		static ptr create() { return ptr(new http_socket()); }
		static ptr create(const asio_service& service) { return ptr(new http_socket(service)); }
	};

	// Open
	/*template <typename Callback>
	void http_socket::open(const boost::asio::ip::tcp::endpoint& endpoint, Callback callback)
	{
		assert(invalid());

		const auto self = shared_from_this();

		m_socket.async_connect(endpoint, [self, callback] (boost::system::error_code error)
		{
			if (!error)
				callback();
		});
	}

	template <typename Callback, typename Object>
	void http_socket::open(const boost::asio::ip::tcp::endpoint& endpoint, Callback callback, Object object)
	{
		assert(invalid());

		const auto self = shared_from_this();

		m_socket.async_connect(endpoint, [self, object, callback] (boost::system::error_code error)
		{
			if (!error)
				(static_cast<Object>(object).*callback)();
		});
	}

	template <typename Callback> 
	void http_socket::open(const std::string& address, const std::string& port, Callback callback)
	{
		assert(invalid());

		const auto self = shared_from_this();

		asio_address::get_address(address, [self, port, callback] (const boost::asio::ip::address& address)
		{
			asio_port::get_port(port, [self, address, callback] (const u16 port)
			{
				asio_endpoint endpoint(address, port);
				self->open(endpoint, callback);
			});
		});
	}

	template <typename Callback, typename Object>
	void http_socket::open(const std::string& address, const std::string& port, Callback callback, Object object)
	{
		assert(invalid());

		const auto self = shared_from_this();

		asio_address::get_address(address, [self, port, object, callback] (const boost::asio::ip::address& address)
		{
			asio_port::get_port(port, [self, address, object, callback] (const u16 port)
			{
				asio_endpoint endpoint(address, port);
				self->open(endpoint, object, callback);
			});
		});
	}

	template <typename Callback>
	void http_socket::open(const std::string& address, const u16 port, Callback callback)
	{
		assert(invalid());

		const auto self = shared_from_this();

		asio_address::get_address(address, [self, port, callback] (const boost::asio::ip::address& address)
		{
			asio_endpoint endpoint(address, port);
			self->open(endpoint, callback);
		});
	}

	template <typename Callback, typename Object>
	void http_socket::open(const std::string& address, const u16 port, Callback callback, Object object)
	{
		assert(invalid());

		const auto self = shared_from_this();

		asio_address::get_address(address, [self, port, object, callback] (const boost::asio::ip::address& address)
		{
			asio_endpoint endpoint(address, port);
			self->open(endpoint, object, callback);
		});
	}*/

	// Read
	template <typename T, typename Callback>
	void http_socket::read(Callback callback)
	{
		const auto self = shared_from_this();
		auto header_buffer = std::make_shared<boost::asio::streambuf>();

		// Header
		boost::asio::async_read_until(m_socket, *header_buffer, http::eom, [self, header_buffer, callback] (boost::system::error_code error, const size_t bytes_transferred)
		{
			if (!error && bytes_transferred)
			{
				auto message = std::make_shared<T>();
				const char* header = boost::asio::buffer_cast<const char*>(header_buffer->data());
				const std::string header_data(header, bytes_transferred);
				message.parse(header_data);

				if (!message->has_length())
				{
					callback(message);
					return;
				}

				// Body (overlap)				
				const size_t overlap = header_buffer->size() - bytes_transferred;
				const size_t remaining = message->get_length() - overlap;
				if (!remaining)
				{
					char* body = const_cast<char*>(header + bytes_transferred);
					body[message.get_length()] = '\0';
					message->set_body(body);

					callback(message);
					return;
				}
				
				char* body = new char[message->get_length() + 1];
				memcpy(body, header + bytes_transferred, overlap);

				// Body (remaining)
				boost::system::error_code data_error;
				auto data_buffer = boost::asio::buffer(&body[overlap], remaining);
				boost::asio::async_read(self->get_socket(), data_buffer, boost::asio::transfer_exactly(remaining), [self, message, body, callback] (boost::system::error_code error, const size_t bytes_transferred)
				{
					if (!error && bytes_transferred)
					{
						body[message->get_length()] = '\0';
						message->set_body(body);

						callback(message);
					}

					delete[] body;
				});
			}
		});
	}

	template <typename T, typename Callback, typename Object>
	void http_socket::read(Callback callback, Object object)
	{
		const auto self = shared_from_this();
		auto header_buffer = std::make_shared<boost::asio::streambuf>();

		// Header
		boost::asio::async_read_until(m_socket, *header_buffer, http::eom, [self, header_buffer, object, callback] (boost::system::error_code error, const size_t bytes_transferred)
		{
			if (!error && bytes_transferred)
			{
				auto message = std::make_shared<T>();
				const char* header = boost::asio::buffer_cast<const char*>(header_buffer->data());
				const std::string header_data(header, bytes_transferred);
				message.parse(header_data);

				if (!message->has_length())
				{
					(static_cast<Object>(object).*callback)(message);
					return;
				}

				// Body (overlap)				
				const size_t overlap = header_buffer->size() - bytes_transferred;
				const size_t remaining = message->get_length() - overlap;
				if (!remaining)
				{
					char* body = const_cast<char*>(header + bytes_transferred);
					body[message.get_length()] = '\0';
					message->set_body(body);

					(static_cast<Object>(object).*callback)(message);
					return;
				}
				
				char* body = new char[message->get_length() + 1];
				memcpy(body, header + bytes_transferred, overlap);

				// Body (remaining)
				boost::system::error_code data_error;
				auto data_buffer = boost::asio::buffer(&body[overlap], remaining);
				boost::asio::async_read(self->get_socket(), data_buffer, boost::asio::transfer_exactly(remaining), [self, message, body, object, callback] (boost::system::error_code error, const size_t bytes_transferred)
				{
					if (!error && bytes_transferred)
					{
						body[message->get_length()] = '\0';
						message->set_body(body);

						(static_cast<Object>(object).*callback)(message);
					}

					delete[] body;
				});
			}
		});
	}

	// Write
	template <typename T, typename Callback>
	void http_socket::write(const T& http, Callback callback)
	{
		const auto self = shared_from_this();
		const string_ptr data = std::make_shared<std::string>(http.str());
		auto buffer = boost::asio::buffer(*data);

		boost::asio::async_write(m_socket, buffer, boost::asio::transfer_all(), [self, data, callback] (boost::system::error_code error, const size_t bytes_transferred)
		{
			if (!error && bytes_transferred)
				callback(bytes_transferred);
		});
	}

	template <typename Callback> 
	void write(const http_code& code, Callback callback)
	{
		http_response response(code);
		write(code, callback);
	}

	template <typename T, typename Callback, typename Object>
	void http_socket::write(const T& http, Callback callback, Object object)
	{
		const auto self = shared_from_this();
		const string_ptr data = std::make_shared<std::string>(http.str());
		auto buffer = boost::asio::buffer(*data);

		boost::asio::async_write(m_socket, buffer, boost::asio::transfer_all(), [self, data, object, callback] (boost::system::error_code error, const size_t bytes_transferred)
		{
			if (!error && bytes_transferred)
				(static_cast<Object>(object).*callback)(bytes_transferred);
		});
	}

	template <typename Callback, typename Object>
	void write(const http_code& code, Callback callback, Object object)
	{
		http_response response(code);
		write(code, object, callback);
	}
}

#endif
