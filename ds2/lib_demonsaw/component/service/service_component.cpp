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

#include <cassert>

#include "component/service/service_component.h"

namespace eja
{
	// Constructor
	service_component::~service_component()
	{
		m_service->stop();
		m_work.reset();
	}

	// Operator
	service_component& service_component::operator=(const service_component& comp)
	{
		if (this != &comp)
		{
			component::operator=(comp);

			m_service = comp.m_service;
			m_thread_group = comp.m_thread_group;
			m_work = comp.m_work;

			m_stack_size = comp.m_stack_size;
			m_threads = comp.m_threads;
		}

		return *this;
	}

	// Interface
	void service_component::shutdown()
	{
		m_service->stop();
		m_work.reset();
	}

	// Utility
	void service_component::start()
	{		
		assert(m_threads && inactive());

		m_work = io_work_ptr(new boost::asio::io_service::work(*m_service));
		m_thread_group = thread_group_ptr(new boost::thread_group());

		// Threads
		const auto service = get_service();
		for (size_t i = 0; i < m_threads; ++i)
			m_thread_group->create_thread([service]() { on_run(service); });
	}

	void service_component::restart()
	{
		if (m_service->stopped())
			return;

		stop();
		start();
	}

	void service_component::stop()
	{
		if (m_service->stopped())
			return;

		m_work.reset();
		m_service->stop();

		if (m_thread_group)
		{
			m_thread_group->interrupt_all();
			m_thread_group->join_all();
			m_thread_group.reset();
		}

		// HACK: Fixes inability to restart
		m_service = io_service_ptr(new boost::asio::io_service());
	}

	// Static
	void service_component::on_run(const io_service_ptr service)
	{
		while (true)
		{
			try
			{
				service->reset();

				boost::system::error_code code;				
				service->run(code);

				if (code)
				{
					std::cerr << "error_code: " << code << std::endl;

					// TODO: call error
					//
					//
				}

				break;
			}
			catch (std::exception & ex)
			{
				std::cerr << "exception: " << ex.what() << std::endl;

				// TODO: call error
				//
				//
			}
			catch (...)
			{
				std::cerr << "Unknown error" << std::endl;
			}
		}
	}
	void service_component::on_timer(const boost::system::error_code& error)
	{
		try
		{
			if (error == boost::asio::error::operation_aborted)
			{
				//std::cout << "Timer Aborted: " << error.message() << std::endl;
			}
			else
			{
				std::cout << "Timer Expired" << std::endl;
			}
		}
		catch (const std::exception& ex)
		{
			std::cerr << ex.what() << std::endl;
		}
	}
}