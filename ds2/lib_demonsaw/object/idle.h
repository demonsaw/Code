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

#ifndef _EJA_IDLE_
#define _EJA_IDLE_

#include <deque>
#include <iostream>
#include <memory>
#include <thread>

#include "object.h"
#include "timeout.h"
#include "system/mutex/mutex_list.h"
#include "system/mutex/mutex_map.h"
#include "system/mutex/mutex_queue.h"
#include "system/mutex/mutex_vector.h"
#include "system/type.h"
#include "system/function/function.h"
#include "utility/default_value.h"

namespace eja
{
	// State
	enum class idle_state : size_t
	{
		none,
		start, restart,
		run, sleep,
		stop
	};

	extern std::ostream& operator<<(std::ostream& os, const idle_state& state);
	extern std::string str(const idle_state& state);

	// Object
	class idle : public object, public mutex
	{
	private:		
		std::deque<idle_state> m_states;
		idle_state m_state = idle_state::none;

		size_t m_sleep_delay = default_timeout::none;
		timeout m_sleep_timeout;

	public:
		using ptr = std::shared_ptr<idle>;

	protected:
		idle() { }
		idle(const eja::idle& idle) : object(idle), m_states(idle.m_states), m_state(idle.m_state), m_sleep_delay(idle.m_sleep_delay), m_sleep_timeout(idle.m_sleep_timeout) { }
		idle(const std::string& id) : object(id) { }
		idle(const char* id) : object(id) { }
		idle(const size_t delay) : m_sleep_delay(delay) { }
		virtual ~idle() override { }
		
		// Operator
		idle& operator=(const idle&) = delete;

		// Interface
		virtual void main();

		// Callback
		virtual bool on_start() { return set_state(idle_state::sleep); }
		virtual bool on_restart() { return set_state(idle_state::start); }
		virtual bool on_stop() { return set_state(idle_state::none); }		
		virtual bool on_run() { return set_state(idle_state::sleep); }
		virtual bool on_sleep();

		// Utility
		void next_state();

		// Mutator
		bool set_state(const idle_state state);		

	public:
		// Interface
		virtual void clear() override;		
		
		virtual void start();
		virtual void restart();
		virtual void stop();

		// State		
		bool active() const { return m_state != idle_state::none; }
		bool inactive() const { return !active(); }

		bool starting() const { return m_state == idle_state::start; }
		bool restarting() const { return m_state == idle_state::restart; }
		bool stopping() const { return m_state == idle_state::stop; }

		bool running() const { return m_state == idle_state::run; }
		bool sleeping() const { return m_state == idle_state::sleep; }

		// Mutator
		void set_delay(const size_t delay) { m_sleep_delay = delay; }

		// Accessor
		const idle_state& get_state() const { return m_state; }
		const size_t& get_delay() const { return m_sleep_delay; }
	};

	// Container
	derived_type(idle_list, mutex_list<idle>);
	derived_type(idle_map, mutex_map<std::string, idle>);
	derived_type(idle_queue, mutex_queue<idle>);
	derived_type(idle_vector, mutex_vector<idle>);
}

#endif
