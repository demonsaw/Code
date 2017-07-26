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

#ifndef _EJA_ASIO_RESOLVER_
#define _EJA_ASIO_RESOLVER_

#include <list>
#include <string>

#include <boost/asio.hpp>
#include <boost/asio/ip/address.hpp>

#include "system/type.h"

namespace eja
{
	class asio_resolver final
	{
	public:
		enum tcp_version { v4 = 1, v6 = 2, all = v4 | v6 };

	private:
		asio_resolver() = delete;
		~asio_resolver() = delete;

	public:
		// Port
		template <typename T> static u16 get_port(const T& port);
		template <typename T, typename Callback> static void get_port(const T& port, Callback callback);
		template <typename T, typename Callback, typename Object> static void get_port(const T& port, Callback callback, Object object);

		// Address		
		template <typename T> static boost::asio::ip::address get_address(const T& address);
		template <typename T, typename Callback> static void get_address(const T& address, Callback callback);
		template <typename T, typename Callback, typename Object> static void get_address(const T& address, Callback callback, Object object);

		static boost::asio::ip::address get_address(const tcp_version version = tcp_version::all);
		static boost::asio::ip::address get_address_v4() { return get_address(tcp_version::v4); }
		static boost::asio::ip::address get_address_v6() { return get_address(tcp_version::v6); }

		static std::list<boost::asio::ip::address> get_addresses(const tcp_version version = tcp_version::all);
		static std::list<boost::asio::ip::address> get_addresses_v4() { return get_addresses(tcp_version::v4); }
		static std::list<boost::asio::ip::address> get_addresses_v6() { return get_addresses(tcp_version::v6); }

		// Endpoint
		template <typename T> static boost::asio::ip::tcp::endpoint get_endpoint(const T& address, const T& port) { return boost::asio::ip::tcp::endpoint(get_address(address), get_port(port)); }
		template <typename T> static boost::asio::ip::tcp::endpoint get_endpoint(const T& address, const u16 port) { return boost::asio::ip::tcp::endpoint(get_address(address), port); }
		
		template <typename T, typename Callback> static void get_endpoint(const T& address, const T& port, Callback callback);
		template <typename T, typename Callback> static void get_endpoint(const T& address, const u16 port, Callback callback);

		template <typename T, typename Callback, typename Object> static void get_endpoint(const T& address, const T& port, Callback callback, Object object);
		template <typename T, typename Callback, typename Object> static void get_endpoint(const T& address, const u16 port, Callback callback, Object object);
	};

	// Port
	template <typename T>
	u16 asio_resolver::get_port(const T& port)
	{
		boost::asio::io_service service;
		boost::asio::ip::tcp::resolver resolver(service);
		boost::asio::ip::tcp::resolver::query query(port);

		boost::system::error_code error;
		const auto& it = resolver.resolve(query, error);

		if (error)
			throw boost::system::system_error(error);

		if (it != boost::asio::ip::tcp::resolver::iterator())
		{
			// Choose the first network interface
			const boost::asio::ip::tcp::endpoint& endpoint = *it;
			return endpoint.port();
		}

		return 0;
	}

	template <typename T, typename Callback>
	void asio_resolver::get_port(const T& port, Callback callback)
	{
		const auto& service = std::make_shared<boost::asio::io_service>();
		boost::asio::ip::tcp::resolver resolver(*service);
		boost::asio::ip::tcp::resolver::query query(port);

		resolver.async_resolve(query, [service, callback] (boost::system::error_code error, boost::asio::ip::tcp::resolver::iterator it)
		{
			if (!error)
			{
				const boost::asio::ip::tcp::endpoint endpoint = *it;
				callback(endpoint.port());
			}
		});

		service->run();
	}

	template <typename T, typename Callback, typename Object>
	void asio_resolver::get_port(const T& port, Callback callback, Object object)
	{
		const auto& service = std::make_shared<boost::asio::io_service>();
		boost::asio::ip::tcp::resolver resolver(*service);
		boost::asio::ip::tcp::resolver::query query(port);

		resolver.async_resolve(query, [service, object, callback] (boost::system::error_code error, boost::asio::ip::tcp::resolver::iterator it)
		{
			if (!error)
			{
				const boost::asio::ip::tcp::endpoint endpoint = *it;
				(static_cast<Object>(object).*callback)(endpoint.port());
			}
		});

		service->run();
	}

	// Address
	template <typename T>
	boost::asio::ip::address asio_resolver::get_address(const T& address)
	{
		// IP
		boost::system::error_code error;
		const auto& addr = boost::asio::ip::address::from_string(address, error);
		if (!error)
			return addr;

		// DNS
		if (error == boost::asio::error::invalid_argument)
		{
			boost::asio::io_service service;
			boost::asio::ip::tcp::resolver resolver(service);
			boost::asio::ip::tcp::resolver::query query(address, "");
			const auto it = resolver.resolve(query, error);

			if (error)
				throw boost::system::system_error(error);

			if (it != boost::asio::ip::tcp::resolver::iterator())
			{
				// Choose the first network interface
				const boost::asio::ip::tcp::endpoint endpoint = *it;
				return endpoint.address();
			}
		}

		return boost::asio::ip::address();
	}

	template <typename T, typename Callback>
	void asio_resolver::get_address(const T& address, Callback callback)
	{
		// IP
		boost::system::error_code error;
		const auto& addr = boost::asio::ip::address::from_string(address, error);

		if (!error)
		{
			callback(addr);
			return;
		}

		// DNS
		if (error == boost::asio::error::invalid_argument)
		{
			const auto& service = std::make_shared<boost::asio::io_service>();
			boost::asio::ip::tcp::resolver resolver(*service);
			boost::asio::ip::tcp::resolver::query query(address, "");

			resolver.async_resolve(query, [service, callback] (boost::system::error_code error, boost::asio::ip::tcp::resolver::iterator it)
			{
				if (!error)
				{
					const boost::asio::ip::tcp::endpoint endpoint = *it;
					callback(endpoint.address());
				}
			});

			service->run();
		}
	}

	template <typename T, typename Callback, typename Object>
	void asio_resolver::get_address(const T& address, Callback callback, Object object)
	{
		// IP
		boost::system::error_code error;
		const auto& addr = boost::asio::ip::address::from_string(address, error);

		if (!error)
		{
			(static_cast<Object>(object).*callback)(addr);
			return;
		}

		// DNS
		if (error == boost::asio::error::invalid_argument)
		{
			const auto& service = std::make_shared<boost::asio::io_service>();
			boost::asio::ip::tcp::resolver resolver(*service);
			boost::asio::ip::tcp::resolver::query query(address, "");

			resolver.async_resolve(query, [service, object, callback] (boost::system::error_code error, boost::asio::ip::tcp::resolver::iterator it)
			{
				if (!error)
				{
					const boost::asio::ip::tcp::endpoint endpoint = *it;
					(static_cast<Object>(object).*callback)(endpoint.address());
				}
			});

			service->run();
		}
	}

	// Endpoint
	template <typename T, typename Callback>
	void asio_resolver::get_endpoint(const T& address, const T& port, Callback callback)
	{
		get_address(address, [port, callback] (const boost::asio::ip::address& address)
		{
			get_port(port, [address, callback] (const u16 port)
			{
				const boost::asio::ip::tcp::endpoint endpoint(address, port);
				callback(endpoint);
			});
		});
	}

	template <typename T, typename Callback, typename Object>
	void asio_resolver::get_endpoint(const T& address, const T& port, Callback callback, Object object)
	{
		get_address(address, [port, object, callback] (const boost::asio::ip::address& address)
		{
			get_port(port, [address, object, callback] (const u16 port)
			{
				const boost::asio::ip::tcp::endpoint endpoint(address, port);
				(static_cast<Object>(object).*callback)(endpoint);
			});
		});
	}

	template <typename T, typename Callback>
	void asio_resolver::get_endpoint(const T& address, const u16 port, Callback callback)
	{
		get_address(address, [port, callback] (const boost::asio::ip::address& address)
		{
			const boost::asio::ip::tcp::endpoint endpoint(address, port);
			callback(endpoint);
		});
	}

	template <typename T, typename Callback, typename Object>
	void asio_resolver::get_endpoint(const T& address, const u16 port, Callback callback, Object object)
	{
		get_address(address, [port, object, callback] (const boost::asio::ip::address& address)
		{
			const boost::asio::ip::tcp::endpoint endpoint(address, port);
			(static_cast<Object>(object).*callback)(endpoint);
		});
	}
}

#endif
