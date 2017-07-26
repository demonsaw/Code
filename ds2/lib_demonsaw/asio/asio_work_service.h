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

#ifndef _EJA_ASIO_WORK_SERVICE_
#define _EJA_ASIO_WORK_SERVICE_

#include <memory>
#include <boost/asio.hpp>

#include "asio.h"
#include "asio_service.h"

namespace eja
{
	class asio_work_service final : public asio_service
	{
	private:
		io_work_ptr m_work;

	public:
		using ptr = std::shared_ptr<asio_work_service>;

	public:
		asio_work_service() : asio_service(), m_work(std::make_shared<boost::asio::io_service::work>(get_service())) { }
		asio_work_service(const asio_work_service& service) : asio_service(service), m_work(service.m_work) { }
		virtual ~asio_work_service() override { }

		// Operator
		asio_work_service& operator=(const asio_work_service& service);

		// Interface
		virtual size_t run() const override { m_service->run(); }

		// Accessor
		const boost::asio::io_service::work& get_work() const { return *m_work; }
		boost::asio::io_service::work& get_work() { return *m_work; }

		// Static
		static ptr create() { return std::make_shared<asio_work_service>(); }
	};
}

#endif
