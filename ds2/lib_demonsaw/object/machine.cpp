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

#include <sstream>

#include "machine.h"
#include "utility/default_value.h"

namespace eja
{
	// State
	static const char* machine_state_name[] =
	{
		"none",
		"idle", 
		"start", "restart",
		"run", "sleep",
		"handshake", "join", "tunnel", "group", "browse", "search", "transfer", "download", "upload", "chat", "info", "ping", "quit",
		"cancel", "stop"
	};

	std::ostream& operator<<(std::ostream& os, const machine_state& state)
	{
		return os << machine_state_name[static_cast<size_t>(state)];
	}

	std::string str(const machine_state& state)
	{
		std::ostringstream sout;
		sout << state;
		return sout.str();
	}

	// Interface
	void machine::main()
	{
		try
		{
			do
			{
				// Cancel
				if (timeout())
					cancel();

				const auto& state = get_state();

				switch (state)
				{
					case machine_state::start:
					{
						if (on_start())
							next_state();

						break;
					}					
					case machine_state::stop:
					{
						if (on_stop())
							next_state();

						break;
					}					
					case machine_state::resume:
					{
						if (on_resume())
							next_state();

						break;
					}
					case machine_state::restart:
					{
						if (on_restart())
							next_state();

						break;
					}
					case machine_state::cancel:
					{
						if (on_cancel())
							next_state();

						break;
					}
					case machine_state::run:
					{
						if (on_run())
							next_state();

						break;
					}
					case machine_state::idle:
					{
						if (on_idle())
							next_state();

						break;
					}
					case machine_state::sleep:
					{
						if (on_sleep())
							next_state();

						break;
					}
					case machine_state::handshake:
					{
						if (on_handshake())
							next_state();

						break;
					}
					case machine_state::join:
					{
						if (on_join())
							next_state();

						break;
					}
					case machine_state::tunnel:
					{
						if (on_tunnel())
							next_state();

						break;
					}
					case machine_state::group:
					{
						if (on_group())
							next_state();

						break;
					}
					case machine_state::browse:
					{
						if (on_browse())
							next_state();

						break;
					}
					case machine_state::search:
					{
						if (on_search())
							next_state();

						break;
					}
					case machine_state::transfer:
					{
						if (on_transfer())
							next_state();

						break;
					}
					case machine_state::download:
					{
						if (on_download())
							next_state();

						break;
					}
					case machine_state::upload:
					{
						if (on_upload())
							next_state();

						break;
					}
					case machine_state::chat:
					{
						if (on_chat())
							next_state();

						break;
					}					
					case machine_state::info:
					{
						if (on_info())
							next_state();

						break;
					}
					case machine_state::ping:
					{
						if (on_ping())
							next_state();

						break;
					}
					case machine_state::quit:
					{
						if (on_quit())
							next_state();

						break;
					}
					case machine_state::none:
					{
						break;
					}
					default:
					{
						assert(false);
					}
				}

				// Yield
				std::this_thread::sleep_for(std::chrono::milliseconds(default_timeout::yield));
			} 
			while (active());
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
		catch (...)
		{
			std::cerr << default_error::unknown << std::endl;
		}

		on_none();
	}

	void machine::clear()
	{
		object::clear();

		auto_lock();
		m_states.clear();
		m_state = machine_state::none;
		m_status = status::none;

		m_sleep_delay = default_timeout::none;
		m_sleep_timeout.stop();
	}

	void machine::start()
	{
		auto_lock();
		m_states.clear();
		m_state = machine_state::start;
		
		m_sleep_delay = default_timeout::none;
		m_sleep_timeout.start();
	}

	void machine::stop()
	{
		auto_lock();
		m_states.clear();
		m_state = machine_state::stop;

		m_sleep_delay = default_timeout::none;
		m_sleep_timeout.stop();
	}	

	void machine::resume()
	{
		auto_lock();
		m_states.clear();
		m_state = machine_state::resume;

		m_sleep_delay = default_timeout::none;
		m_sleep_timeout.restart();
	}

	void machine::restart()
	{
		auto_lock();
		m_states.clear();
		m_state = machine_state::restart;

		m_sleep_delay = default_timeout::none;
		m_sleep_timeout.restart();
	}

	void machine::cancel()
	{
		auto_lock();
		m_states.clear();
		m_state = machine_state::cancel;

		m_sleep_delay = default_timeout::none;
		m_sleep_timeout.stop();
	}

	void machine::kill()
	{
		auto_lock();
		m_states.clear();
		m_state = machine_state::none;

		m_sleep_delay = default_timeout::none;
		m_sleep_timeout.stop();
	}

	void machine::sleep(const machine_state state, const size_t ms)
	{
		auto_lock();
		m_states.push_front(state);
		m_states.push_front(machine_state::sleep);

		m_sleep_delay = ms;
		m_sleep_timeout.restart();
	}

	// Callback
	bool machine::on_stop()
	{
		set_state(machine_state::idle);

		return true;
	}
	
	bool machine::on_resume()
	{
		set_state(machine_state::start);

		return true;
	}

	bool machine::on_restart()
	{
		set_state(machine_state::start);

		return true;
	}

	bool machine::on_cancel()
	{
		set_state(machine_state::stop);

		return true;
	}

	bool machine::on_idle()
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(default_timeout::idle));

		return false;
	}

	bool machine::on_sleep()
	{ 
		assert(m_sleep_delay);
		std::this_thread::sleep_for(std::chrono::milliseconds(default_timeout::sleep));

		return m_sleep_timeout.expired(m_sleep_delay);
	}

	// Utility
	void machine::next_state()
	{
		auto_lock();
		if (!m_states.empty())
		{
			m_state = m_states.front();
			m_states.pop_front();
		}
	}

	// Mutator
	bool machine::set_state(const machine_state state)
	{
		if (!m_ignore)
		{
			auto_lock();
			m_states.push_back(state);			
		}

		return true;
	}

	bool machine::set_state(const machine_state state, const size_t ms)
	{
		if (!m_ignore)
		{
			if (state != machine_state::sleep)
				sleep(state, ms);
			else
				sleep(ms);
		}

		return true;
	}

	void machine::set_status(const status status)
	{ 
		if (!m_ignore)
			m_status = status;
	}
}
