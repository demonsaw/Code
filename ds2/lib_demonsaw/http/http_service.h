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

#include <boost/asio.hpp>

#include "asio/asio.h"
#include "system/type.h"

namespace eja
{
	class http_service
	{
	protected:
		io_service_ptr m_service;

	protected:
		http_service(const http_service& service) : m_service(service.m_service) { }
		explicit http_service(const io_service_ptr& service) : m_service(service) { }
		virtual ~http_service() { }

		// Operator
		http_service& operator=(const http_service& object);

		// Get
		template <typename T>
		boost::asio::ip::tcp::endpoint get_endpoint(const T& address, const u16 port) const;

	public:
		// Get
		const io_service_ptr& get_service() const { return m_service; }
		io_service_ptr& get_service() { return m_service; }
	};

	// Get
	template <typename T>
	boost::asio::ip::tcp::endpoint http_service::get_endpoint(const T& address, const u16 port) const
	{
		boost::asio::ip::tcp::resolver resolver(*m_service);
		boost::asio::ip::tcp::resolver::query query(address, boost::lexical_cast<std::string>(port));
		const auto it = resolver.resolve(query);

		if (it != boost::asio::ip::tcp::resolver::iterator())
			return it->endpoint();

		return boost::asio::ip::tcp::endpoint();
	}
}

#endif
