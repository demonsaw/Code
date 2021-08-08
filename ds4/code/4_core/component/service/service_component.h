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
#include <unordered_set>
#include <type_traits>
#include <boost/atomic.hpp>

#include "component/component.h"
#include "component/status/status_component.h"
#include "system/type.h"
#include "thread/thread_safe.h"

namespace eja
{
	class entity;

	make_thread_safe_type(timer_set, std::unordered_set<timer_ptr>);

	class service_component : public component, public std::enable_shared_from_this<service_component>
	{
	private:
		io_service_ptr m_service;
		io_strand_ptr m_strand;
		thread_group_ptr m_thread_group;

		timer_set::ptr m_timers;
		io_work_ptr m_work;

	protected:
		enum class state { none, starting, restarting, stopping, running };

		size_t m_threads = 1;
		boost::atomic<state> m_state;

	protected:
		service_component() : m_state(state::none), m_timers(std::make_shared<timer_set>()) { }
		service_component(const service_component& comp) : m_service(comp.m_service), m_strand(comp.m_strand), m_thread_group(comp.m_thread_group), m_work(comp.m_work), m_threads(comp.m_threads), m_timers(comp.m_timers) { m_state.store(comp.m_state); }
		service_component(const size_t threads) : m_state(state::none), m_threads(threads), m_timers(std::make_shared<timer_set>()) { }

		// Operator
		service_component& operator=(const service_component& comp);

		// Interface
		template <typename T>
		std::shared_ptr<T> shared_from_base() { return std::static_pointer_cast<T>(shared_from_this()); }

		template <typename T>
		const std::shared_ptr<T> shared_from_base() const { return std::const_pointer_cast<T>(std::static_pointer_cast<const T>(shared_from_this())); }

		// Utility
		virtual void close();
		void run();

		// Dispatch
		template <typename T>
		void dispatch(const T& t) { reinterpret_cast<const service_component*>(this)->dispatch(t); }

		template <typename T>
		void dispatch(const T& t) const;

		template <typename T>
		void dispatch_strand(const T& t) { reinterpret_cast<const service_component*>(this)->dispatch_strand(t); }

		template <typename T>
		void dispatch_strand(const T& t) const;

		// Post
		template <typename T>
		void post(const T& t) { reinterpret_cast<const service_component*>(this)->post(t); }

		template <typename T>
		void post(const T& t) const;

		template <typename T>
		void post_strand(const T& t) { reinterpret_cast<const service_component*>(this)->post_strand(t); }

		template <typename T>
		void post_strand(const T& t) const;

		// Wait
		template <typename callback>
		void wait(const size_t ms, callback call);

		// Has
		bool has_service() const { return m_service != nullptr; }
		bool has_state() const { return m_state.load() != state::none; }
		bool has_strand() const { return m_strand != nullptr; }
		bool has_thread_group() const { return m_thread_group != nullptr; }
		bool has_work() const { return m_work != nullptr; }

		// Is
		bool is_none() const { return m_state.load() == state::none; }
		bool is_starting() const { return m_state.load() == state::starting; }
		bool is_restarting() const { return m_state.load() == state::restarting; }
		bool is_stopping() const { return m_state.load() == state::stopping; }
		bool is_running() const { return m_state.load() == state::running; }

		// Set
		void set_status() const { set_status(status::none); }
		virtual void set_status(const std::shared_ptr<entity>& entity, const status value) const;
		void set_status(const status value) const;

		void set_state() { set_state(state::none); }
		void set_state(const state next) { m_state = next; }
		bool set_state(const state next, const state value);

		// Get
		io_strand_ptr get_strand() const { return m_strand; }
		thread_group_ptr get_thread_group() const { return m_thread_group; }
		timer_set::ptr get_timers() const { return m_timers; }
		io_work_ptr get_work() const { return m_work; }

	public:
		// Utility
		virtual void start();
		virtual void restart();
		virtual void stop();

		virtual bool valid() const override { return has_service() && !m_service->stopped(); }

		// Set
		void set_threads(const size_t threads) { m_threads = threads; }

		// Get
		io_service_ptr get_service() const { return m_service; }
		size_t get_threads() const { return m_threads; }
	};

	// Dispatch
	template <typename T>
	void service_component::dispatch(const T& t) const
	{
		if (valid())
		{
			const auto service = get_service();
			m_service->dispatch([service, t]()
			{
				if (!service->stopped())
					t();
			});
		}
	}

	template <typename T>
	void service_component::dispatch_strand(const T& t) const
	{
		if (valid())
		{
			const auto service = get_service();
			m_strand->dispatch([service, t]()
			{
				if (!service->stopped())
					t();
			});
		}
	}

	// Post
	template <typename T>
	void service_component::post(const T& t) const
	{
		if (valid())
		{
			const auto service = get_service();
			m_service->post([service, t]()
			{
				if (!service->stopped())
					t();
			});
		}
	}

	template <typename T>
	void service_component::post_strand(const T& t) const
	{
		if (valid())
		{
			const auto service = get_service();
			m_strand->post([service, t]()
			{
				if (!service->stopped())
					t();
			});
		}
	}

	// Wait
	template <typename callback>
	void service_component::wait(const size_t ms, callback call)
	{
		const auto service = get_service();
		const auto timer = std::make_shared<boost::asio::steady_timer>(*service);
		{
			thread_lock(m_timers);
			m_timers->insert(timer);
		}

		// Timer
		const auto self = shared_from_this();
		const auto timers = get_timers();

		timer->expires_from_now(std::chrono::milliseconds(ms));
		timer->async_wait([self, service, timer, timers, call](const boost::system::error_code& error)
		{
			// Timers
			__lock(timers);
			timers->erase(timer);
			__unlock();

			if (error != boost::asio::error::operation_aborted)
			{
				if (!service->stopped())
					call();
			}
		});
	}
}

#endif
