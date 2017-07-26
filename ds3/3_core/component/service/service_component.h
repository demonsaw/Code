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

#include <chrono>
#include <memory>
#include <type_traits>

#include "component/component.h"
#include "component/socket_component.h"
#include "component/tunnel_component.h"
#include "component/status/status_component.h"
#include "entity/entity.h"
#include "http/http_socket.h"
#include "system/type.h"
#include "thread/thread_priority.h"

#define ASIO_DEBUG _DEBUG

#if ASIO_DEBUG
#define ASIO_DEBUG_ONLY(...) __VA_ARGS__
#else
#define ASIO_DEBUG_ONLY(...)
#endif

namespace eja
{
	class service_component : public component, public std::enable_shared_from_this<service_component>
	{
	private:
		io_service_ptr m_service;
		thread_group_ptr m_thread_group;
		io_work_ptr m_work;

	protected:
		thread_priority m_priority = thread_priority::normal;
		size_t m_stack_size = (sizeof(void*) << 10);
		size_t m_threads = 4;

	protected:
		service_component();
		service_component(const service_component& comp) : component(comp), 
			m_service(comp.m_service), m_work(comp.m_work), m_thread_group(comp.m_thread_group), 
			m_priority(comp.m_priority), m_stack_size(comp.m_stack_size), m_threads(comp.m_threads) { }
		service_component(const size_t threads);

		// Operator
		service_component& operator=(const service_component& comp);

		// Interface
		template <typename T>
		std::shared_ptr<T> shared_from_base() { return std::static_pointer_cast<T>(shared_from_this()); }

		// Utility
		virtual void close() { }
		
		template <typename T, typename U>
		std::shared_ptr<T> copy_if(const U& u) const { return copy_if(get_entity(), u); }

		template <typename T, typename U>
		std::shared_ptr<T> copy_if(const std::shared_ptr<entity> entity, const U& u) const;

		template <typename T>
		std::shared_ptr<T> copy_if_enabled() const { return copy_if_enabled<T>(get_entity()); }

		template <typename T>
		std::shared_ptr<T> copy_if_enabled(const std::shared_ptr<entity> entity) const { return copy_if<T>(entity, [](const entity::ptr e) {return e->enabled(); }); }

		// Callback
		void run();

		// Socket
		bool has_socket(const std::shared_ptr<entity> entity, const std::shared_ptr<http_socket> socket);
		void add_socket(const std::shared_ptr<entity> entity, const std::shared_ptr<http_socket> socket);
		void remove_socket(const std::shared_ptr<entity> entity, const std::shared_ptr<http_socket> socket);
		std::shared_ptr<http_socket> get_socket(const std::shared_ptr<entity> entity);

		// Tunnel
		bool has_tunnel(const std::shared_ptr<entity> entity, const std::shared_ptr<http_socket> socket);
		void add_tunnel(const std::shared_ptr<entity> entity, const std::shared_ptr<http_socket> socket);
		void remove_tunnel(const std::shared_ptr<entity> entity, const std::shared_ptr<http_socket> socket);
		std::shared_ptr<http_socket> get_tunnel(const std::shared_ptr<entity> entity);

		// Set
		void set_priority(const thread_priority priority) { m_priority = priority; }
		virtual void set_status(const entity::ptr entity, const status value);
		virtual void set_status(const status value);		
		virtual void set_status() { }
		// Get
		thread_priority get_priority() const { return m_priority; }
		thread_group_ptr get_thread_group() const { return m_thread_group; }
		io_work_ptr get_work() const { return m_work; }
		
	public:		
		// Utility
		virtual void start();
		virtual void restart();
		virtual void stop();
		
		virtual bool valid() const override { return m_service && !m_service->stopped(); }
		
		template <typename callback>
		void wait(const size_t ms, callback call);

		// Dispatch
		template <typename T> 
		void dispatch(const T& t);

		// Post
		template <typename T> 
		void post(const T& t);

		// Set		
		void set_stack_size(const size_t stack_size) { m_stack_size = stack_size; }
		void set_threads(const size_t threads) { m_threads = threads; }

		// Get
		io_service_ptr get_service() const { return m_service; }
		size_t get_stack_size() const { return m_stack_size; }
		size_t get_threads() const { return m_threads; }
	};

	// Dispatch
	template <typename T>
	void service_component::dispatch(const T& t)
	{
		if (valid())
		{
			const auto owner = get_entity();
			if (owner->enabled())
				m_service->dispatch(t);
		}			
	}

	// Post
	template <typename T>
	void service_component::post(const T& t)
	{ 
		if (valid())
		{
			const auto owner = get_entity();
			if (owner->enabled())
				m_service->post(t);
		}
	}

	// Utility
	template <typename callback>
	void service_component::wait(const size_t ms, callback call)
	{
		if (valid())
		{
			const auto service = get_service();
			const auto timer = timer_ptr(new boost::asio::steady_timer(*service));
			timer->expires_from_now(std::chrono::milliseconds(ms));

			const auto self = shared_from_this();

			timer->async_wait([self, timer, call](const boost::system::error_code& error)
			{
				if (error != boost::asio::error::operation_aborted)
				{
					if (self->valid())
						call();
				}
			});
		}		
	}

	template <typename T, typename U>
	std::shared_ptr<T> service_component::copy_if(const std::shared_ptr<entity> entity, const U& u) const
	{		
		const auto comp = entity->get<T>();
		const auto copy = T::create();

		std::copy_if(comp->begin(), comp->end(), std::back_inserter(*copy), u);

		return copy;
	}
}

#endif
