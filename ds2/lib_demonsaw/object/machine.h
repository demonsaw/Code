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

#ifndef _EJA_MACHINE_
#define _EJA_MACHINE_

#include <deque>
#include <iostream>
#include <memory>
#include <thread>

#include "object.h"
#include "status.h"
#include "timeout.h"
#include "system/mutex/mutex_list.h"
#include "system/mutex/mutex_map.h"
#include "system/mutex/mutex_queue.h"
#include "system/mutex/mutex_vector.h"
#include "system/type.h"
#include "utility/default_value.h"

namespace eja
{
	// State
	enum class machine_state : size_t
	{
		none,
		idle, 
		start, resume, restart,
		run, sleep,
		handshake, join, tunnel, group, browse, search, transfer, download, upload, chat, info, ping, quit,
		cancel, stop
	};

	extern std::ostream& operator<<(std::ostream& os, const machine_state& state);
	extern std::string str(const machine_state& state);

	// Object
	class machine : public object, public mutex
	{
	private:		
		std::deque<machine_state> m_states;
		machine_state m_state = machine_state::idle;
		status m_status = status::none;

		bool m_ignore = false;
		size_t m_sleep_delay = default_timeout::none;
		eja::timeout m_sleep_timeout;

	public:
		using ptr = std::shared_ptr<machine>;

	private:
		virtual void sleep(const size_t ms) { sleep(m_state, ms); }
		virtual void sleep(const machine_state state, const size_t ms);

	protected:
		machine() { }
		machine(const eja::machine& machine) : object(machine), m_states(machine.m_states), m_state(machine.m_state), m_status(machine.m_status), m_ignore(machine.m_ignore), m_sleep_delay(machine.m_sleep_delay), m_sleep_timeout(machine.m_sleep_timeout) { }
		machine(const std::string& id) : object(id) { }
		machine(const char* id) : object(id) { }
		virtual ~machine() override { }

		// Interface		
		virtual void main();		

		// Callback
		virtual bool on_none() { return true; }
		virtual bool on_idle();
		virtual bool on_sleep();
		virtual bool on_run() { return true; }

		virtual bool on_start() { return true; }		
		virtual bool on_stop();
		virtual bool on_resume(); 
		virtual bool on_restart();
		virtual bool on_cancel();
		
		virtual bool on_handshake() { return true; }
		virtual bool on_join() { return true; }
		virtual bool on_tunnel() { return true; }		
		virtual bool on_group() { return true; }
		virtual bool on_browse() { return true; }
		virtual bool on_search() { return true; }
		virtual bool on_transfer() { return true; }
		virtual bool on_download() { return true; }
		virtual bool on_upload() { return true; }
		virtual bool on_chat() { return true; }
		virtual bool on_info() { return true; }
		virtual bool on_ping() { return true; }
		virtual bool on_quit() { return true; }

		// Utility		
		virtual bool timeout() const { return false; }
		void ignore(const bool ignore) { m_ignore = ignore; }
		void next_state();

	public:
		// Interface
		virtual void clear() override;		
				
		virtual void start();		
		virtual void stop();
		virtual void resume(); 
		virtual void restart();
		virtual void cancel();
		virtual void kill();

		// State (current)
		bool active() const { return m_state != machine_state::none; }
		bool inactive() const { return !active(); }
		
		bool starting() const { return m_state == machine_state::start; }
		bool stopping() const { return m_state == machine_state::stop; }
		bool resuming() const { return m_state == machine_state::resume; }
		bool restarting() const { return m_state == machine_state::restart; }		

		bool running() const { return (m_state >= machine_state::start) && (m_state <= machine_state::stop); }		
		bool quitting() const { return m_state == machine_state::quit; }
		bool sleeping() const { return m_state == machine_state::sleep; }
		bool idle() const { return m_state == machine_state::idle; }
		
		bool started() const { return (m_state >= machine_state::start) && (m_state <= machine_state::quit); }		
		bool stopped() const { return (m_state == machine_state::idle) || (m_state > machine_state::quit); }

		// State (future)
		bool startable() const { return stopped(); }
		bool stoppable() const { return started(); }
		bool resumable() const { return stopped(); }
		bool restartable() const { return stopped(); }
		bool finishable() const { return stopped(); }
		bool removeable() const { return inactive() || stopped() || cancelled(); }

		// Status
		bool none() const { return m_status == status::none; }
		bool success() const { return m_status == status::success; }
		bool warning() const { return m_status == status::warning; }
		bool error() const { return m_status == status::error; }
		bool info() const { return m_status == status::info; }
		bool pending() const { return m_status == status::pending; }
		bool cancelled() const { return m_status == status::cancelled; }
		bool unknown() const { return m_status == status::unknown; }

		// Utility
		bool valid() const { return none() || success() || info() || pending(); }
		bool invalid() const { return warning() || error() || cancelled() || unknown(); }

		// Mutator
		bool set_state(const machine_state state);
		bool set_state(const machine_state state, const size_t ms);
		virtual void set_status(const status status);

		// Accessor
		const machine_state& get_state() const { return m_state; }
		const status& get_status() const { return m_status; }
	};

	// Container
	derived_type(machine_list, mutex_list<machine>);
	derived_type(machine_map, mutex_map<std::string, machine>);
	derived_type(machine_queue, mutex_queue<machine>);
	derived_type(machine_vector, mutex_vector<machine>);
}

#endif
