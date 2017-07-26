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

#ifndef _EJA_ASIO_SERVICE_
#define _EJA_ASIO_SERVICE_

#include <memory>
#include <boost/asio.hpp>

#include "asio.h"
#include "system/type.h"

namespace eja
{
	class asio_service
	{
	protected:
		io_service_ptr m_service;

	public:
		using ptr = std::shared_ptr<asio_service>;

	public:
		asio_service() : m_service(std::make_shared<boost::asio::io_service>()) { }
		asio_service(io_service_ptr service) : m_service(service) { }
		virtual ~asio_service() { }

		// Operator
		const boost::asio::io_service& operator*() const { return *m_service; }
		boost::asio::io_service& operator*() { return *m_service; }

		// Interface
		virtual size_t run() const { return m_service->run(); }
		void stop() const { m_service->stop(); }
		void reset() const { m_service->reset(); }

		// Utility
		bool is_stopped() const { return m_service->stopped(); }

		// Accessor
		const io_service_ptr& get_service() const { return m_service; }
		io_service_ptr& get_service() { return m_service; }

		// Static
		static ptr create() { return std::make_shared<asio_service>(); }
	};
}

#endif
