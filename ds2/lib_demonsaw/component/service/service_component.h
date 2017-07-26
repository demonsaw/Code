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

#ifndef _EJA_SERVICE_COMPONENT_H_
#define _EJA_SERVICE_COMPONENT_H_

#include <boost/asio.hpp>

#include "asio/asio.h"
#include "component/component.h"
#include "system/type.h"

namespace eja
{
	class service_component : public component
	{
	protected:
		io_service_ptr m_service;
		io_work_ptr m_work;
		thread_group_ptr m_thread_group;

		size_t m_stack_size = (sizeof(void*) << 10);
		size_t m_threads = 4;

	protected:
		service_component() : m_service(io_service_ptr(new boost::asio::io_service())) { }
		service_component(const service_component& comp) : component(comp), m_service(comp.m_service), m_work(comp.m_work), m_thread_group(comp.m_thread_group), m_stack_size(comp.m_stack_size), m_threads(comp.m_threads) { }
		service_component(const size_t threads) : m_service(io_service_ptr(new boost::asio::io_service())), m_threads(threads) { }
		virtual ~service_component() override;

		// Operator
		service_component& operator=(const service_component& comp);

		// Interface
		static void on_run(const io_service_ptr service);
		
		// Command
		void on_timer(const boost::system::error_code& error);

		// Get
		const thread_group_ptr get_thread_group() const { return m_thread_group; }
		const io_work_ptr get_work() const { return m_work; }

	public:		
		// Interface
		virtual void shutdown() override;		

		// Utility
		virtual void start();
		virtual void restart();
		virtual void stop();

		bool active() const { return m_work != nullptr; }
		bool inactive() const { return !active(); }

		// Dispatch
		template <typename T>
		void dispatch(const T& t) { m_service->dispatch(t); }

		// Post
		template <typename T>
		void post(const T& t) { m_service->post(t); }

		// Set
		void set_stack_size(const size_t stack_size) { m_stack_size = stack_size; }
		void set_threads(const size_t threads) { m_threads = threads; }
		void set_work() { m_work.reset(); }

		// Get
		const io_service_ptr get_service() const { return m_service; }
		size_t get_stack_size() const { return m_stack_size; }
		size_t get_threads() const { return m_threads; }
	};
}

#endif
