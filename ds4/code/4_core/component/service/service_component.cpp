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
#include <boost/thread.hpp>

#include "component/service/service_component.h"

#include "entity/entity.h"
#include "thread/thread_util.h"
#include "utility/value.h"

namespace eja
{
	// Operator
	service_component& service_component::operator=(const service_component& comp)
	{
		if (this != &comp)
		{
			component::operator=(comp);

			m_service = comp.m_service;
			m_strand = comp.m_strand;
			m_thread_group = comp.m_thread_group;
			m_timers = comp.m_timers;
			m_work = comp.m_work;

			m_threads = comp.m_threads;
			m_state.store(comp.m_state);
		}

		return *this;
	}

	// Utility
	void service_component::start()
	{
		assert(m_threads && invalid());

		if (invalid())
		{
			m_service = std::make_shared<boost::asio::io_service>();
			m_strand = std::make_shared<boost::asio::io_service::strand>(*m_service);
			m_thread_group = std::make_shared<boost::thread_group>();
			m_work = std::make_shared<boost::asio::io_service::work>(*m_service);

			// Threads
			const auto self = shared_from_this();
			for (size_t i = 0; i < m_threads; ++i)
				m_thread_group->create_thread([self]() { self->run(); });
		}
	}

	void service_component::restart()
	{
		if (valid())
		{
			service_component::stop();

			service_component::start();
		}
	}

	void service_component::stop()
	{
		if (valid())
		{
			// 1) Stop new work requests
			m_work.reset();

			// 2) Close asio objects (must happen BEFORE io_service stop)
			close();

			// 3) Stop
			m_service->stop();
			m_strand.reset();

			// 4) Wait on the remaining threads
			if (m_thread_group)
			{
				try
				{
					m_thread_group->interrupt_all();
					m_thread_group->join_all();
					m_thread_group.reset();
				}
				catch (const std::exception& ex)
				{
					log(ex);

					assert(false);
				}
			}

			// 5) Clear UI
			clear();
		}
	}

	void service_component::close()
	{
		// NOTE: We must cancel all outstanding timers to prevent leaking threads
		boost::system::error_code error;

		thread_lock(m_timers);
		for (const auto& timer : *m_timers)
			timer->cancel(error);

		m_timers->clear();
	}

	void service_component::run()
	{
		try
		{
			WIN32_ONLY(srand(static_cast<unsigned int>(time(0)));)

			const auto service = get_service();
			service->run();
		}
		catch (const std::exception& ex)
		{
			log(ex);
		}
	}

	// Set
	void service_component::set_status(const status value) const
	{
		const auto owner = get_owner();
		set_status(owner, value);
	}

	void service_component::set_status(const entity::ptr& entity, const status value) const
	{
		const auto status = entity->get<status_component>();
		status->set_status(value);
	}

	bool service_component::set_state(const state next, const state value)
	{
		state expected = value;
		return m_state.compare_exchange_strong(expected, next);
	}
}