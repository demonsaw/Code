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

#ifndef _EJA_HTTP_TIMER_H_
#define _EJA_HTTP_TIMER_H_

#include <functional>
#include <memory>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>

#include "http/http_service.h"
#include "system/type.h"

namespace eja
{
	class http_timer final : public http_service
	{
		make_param_factory(http_timer);

	public:
		using timer_function = std::function<void(const boost::system::error_code&)>;

	private:
		timer_ptr m_timer;
		timer_function m_function;

	public:
		http_timer(const http_timer& timer) : http_service(timer), m_timer(timer.m_timer) { }
		template <typename T> http_timer(const io_service_ptr& service, T t) : http_service(service), m_timer(timer_ptr(new boost::asio::deadline_timer(*m_service))), m_function(t) { }
		explicit http_timer(const io_service_ptr& service) : http_service(service), m_timer(timer_ptr(new boost::asio::deadline_timer(*m_service))) { }
		~http_timer() { stop(); }

		// Operator
		http_timer& operator=(const http_timer& timer);
		const boost::asio::deadline_timer& operator*() const { return *get_timer(); }
		boost::asio::deadline_timer& operator*() { return *get_timer(); }

		// Interface
		template <typename T> void assign(T t) { m_function = t; }
		void start(const size_t ms);
		void stop();

		// Get
		const timer_ptr get_timer() const { return m_timer; }
		timer_ptr get_timer() { return m_timer; }

		// Static
		template <typename T> static ptr create(const io_service_ptr& service, T t) { return ptr(new http_timer(service, t)); }
		static ptr create(const io_service_ptr& service) { return ptr(new http_timer(service)); }
	};
}

#endif
