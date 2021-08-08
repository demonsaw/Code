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

#ifndef _EJA_HTTP_SERVICE_H_
#define _EJA_HTTP_SERVICE_H_

#include <memory>
#include <string>
#include <boost/asio.hpp>

#include "object/object.h"
#include "system/type.h"

namespace eja
{
	class http_service : public object, public std::enable_shared_from_this<http_service>
	{
	protected:
		io_service_ptr m_service;

	protected:
		http_service(http_service&& obj);
		http_service(const http_service& obj) : object(obj), m_service(obj.m_service) { }
		explicit http_service(const io_service_ptr& service) : m_service(service) { }

		// Operator
		http_service& operator=(http_service&& obj);
		http_service& operator=(const http_service& obj);

		// Interface
		template <typename T>
		std::shared_ptr<T> shared_from_base() { return std::static_pointer_cast<T>(shared_from_this()); }

	public:
		// Resolve
		template <typename callback>
		void resolve(const std::string& address, const u16 port, callback call) const;
		boost::asio::ip::tcp::endpoint resolve(const std::string& address, const u16 port) const;

		// Get
		const io_service_ptr get_service() const { return m_service; }
	};

	// Resolve
	template <typename callback>
	void http_service::resolve(const std::string& address, const u16 port, callback call) const
	{
		// Copy
		const auto service = get_service();
		const auto resolver = resolver_ptr(new boost::asio::ip::tcp::resolver(*service));
		boost::asio::ip::tcp::resolver::query query(address, std::to_string(port));

		resolver->async_resolve(query, [service, resolver, call](const boost::system::error_code& error, boost::asio::ip::tcp::resolver::iterator it)
		{
			boost::asio::ip::tcp::endpoint endpoint;
			if (!error && (it != boost::asio::ip::tcp::resolver::iterator()))
				endpoint = it->endpoint();

			call(error, endpoint);
		});
	}
}

#endif
