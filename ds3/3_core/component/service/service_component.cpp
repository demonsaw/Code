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
#include <stdlib.h>
#include <boost/thread.hpp>

#include "component/tunnel_component.h"
#include "component/service/service_component.h"
#include "component/status/status_component.h"
#include "thread/thread_util.h"
#include "utility/value.h"

namespace eja
{
	// Constructor
	service_component::service_component()
	{
		m_stack_size = io::stack_size;
		m_threads = network::num_threads;
	}

	service_component::service_component(const size_t threads)
	{
		m_stack_size = io::stack_size;
		m_threads = network::num_threads;
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

			m_priority = comp.m_priority;
			m_stack_size = comp.m_stack_size;
			m_threads = comp.m_threads;
		}

		return *this;
	}

	// Utility
	void service_component::start()
	{
		assert(m_threads && invalid());
		
		m_service = io_service_ptr(new boost::asio::io_service());
		m_work = io_work_ptr(new boost::asio::io_service::work(*m_service));
		m_thread_group = thread_group_ptr(new boost::thread_group());

		// Threads
		boost::thread::attributes attrs;
		attrs.set_stack_size(m_stack_size);
		const auto self = shared_from_this();

		for (size_t i = 0; i < m_threads; ++i)
		{
			auto thread = new boost::thread(attrs, [self]() { self->run(); });

			// TODO: Enable priorities for Linux (more work, range 1 - 99 usually)
			WIN32_ONLY(thread_util::set_priority(thread, m_priority);)

			m_thread_group->add_thread(thread);
		}
	}

	void service_component::restart()
	{
		stop();
		start();
	}

	void service_component::stop()
	{		
		// 1) Stop new work requests
		m_work.reset();

		// 2) Close asio objects (must happen BEFORE io_service stop)
		close();		

		// 3) Stop
		m_service->stop();

		// 4) Wait on the remaining threads
		if (m_thread_group)
		{
			try
			{
				m_thread_group->interrupt_all();
				m_thread_group->join_all();
				m_thread_group.reset();
			}
			catch (const std::exception& /*ex*/) { }
		}

		// 5) Clear UI
		clear();
	}

	// Socket
	bool service_component::has_socket(const entity::ptr entity, const http_socket::ptr socket)
	{
		assert(socket);

		const auto set = entity->get<socket_set_component>();
		{
			thread_lock(set);
			return set->find(socket) != set->end();
		}
	}

	void service_component::add_socket(const entity::ptr entity, const http_socket::ptr socket)
	{
		assert(socket);

		const auto set = entity->get<socket_set_component>();
		{
			thread_lock(set);
			set->insert(socket);
		}
	}

	void service_component::remove_socket(const entity::ptr entity, const http_socket::ptr socket)
	{
		assert(socket);

		const auto set = entity->get<socket_set_component>();
		{
			thread_lock(set);
			set->erase(socket);
		}
	}

	http_socket::ptr service_component::get_socket(const entity::ptr entity)
	{
		const auto set = entity->get<socket_set_component>();
		{
			thread_lock(set);

			if (!set->empty())
			{
				const auto it = set->begin();
				const auto socket = *it;
				set->erase(socket);
				return socket;
			}
		}

		return http_socket::create(get_service());
	}

	// Tunnel
	bool service_component::has_tunnel(const entity::ptr entity, const http_socket::ptr socket)
	{
		assert(socket);

		const auto tunnel_list = entity->get<tunnel_list_component>();
		{
			thread_lock(tunnel_list);
			return std::find(tunnel_list->begin(), tunnel_list->end(), socket) != tunnel_list->end();
		}
	}

	void service_component::add_tunnel(const entity::ptr entity, const http_socket::ptr socket)
	{
		assert(socket);

		const auto tunnel_list = entity->get<tunnel_list_component>();
		{
			thread_lock(tunnel_list);
			tunnel_list->push_back(socket);
		}
	}

	void service_component::remove_tunnel(const entity::ptr entity, const http_socket::ptr socket)
	{
		assert(socket);

		const auto tunnel_list = entity->get<tunnel_list_component>();
		{
			thread_lock(tunnel_list);
			tunnel_list->remove(socket);
		}
	}

	http_socket::ptr service_component::get_tunnel(const entity::ptr entity)
	{
		const auto tunnel_list = entity->get<tunnel_list_component>();
		{
			thread_lock(tunnel_list);

			if (!tunnel_list->empty())
			{
				// Use a stack (LIFO)
				const auto socket = tunnel_list->front();
				tunnel_list->pop_front();
				assert(std::find(tunnel_list->begin(), tunnel_list->end(), socket) == tunnel_list->end());

				return socket;
			}
		}

		return nullptr;
	}

	// Callback
	void service_component::run()
	{
		srand(static_cast<unsigned int>(time(0)));

		try
		{
			const auto service = get_service();			
			service->run();
		}
		catch (const std::exception& ex) 
		{
			log(ex);
		}
	}

	// Set
	void service_component::set_status(const status value)
	{
		const auto owner = get_entity();
		if (owner)
		{
			const auto status = owner->get<status_component>();
			status->set_status(value);

			set_status();
		}
	}

	void service_component::set_status(const entity::ptr entity, const status value)
	{
		const auto status = entity->get<status_component>();
		status->set_status(value);

		set_status();
	}
}