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

#include "idle.h"
#include "utility/default_value.h"

namespace eja
{
	// State
	static const char* idle_state_name[] =
	{
		"none",
		"start", "restart",
		"run", "sleep",
		"stop"
	};

	std::ostream& operator<<(std::ostream& os, const idle_state& state)
	{
		return os << idle_state_name[static_cast<size_t>(state)];
	}

	std::string str(const idle_state& state)
	{
		std::ostringstream sout;
		sout << state;
		return sout.str();
	}

	// Interface
	void idle::main()
	{
		try
		{
			do
			{
				const auto& state = get_state();

				switch (state)
				{
					case idle_state::start:
					{
						if (on_start())
							next_state();

						break;
					}
					case idle_state::restart:
					{
						if (on_restart())
							next_state();

						break;
					}
					case idle_state::stop:
					{
						if (on_stop())
							next_state();

						break;
					}
					case idle_state::sleep:
					{
						if (on_sleep())
							next_state();

						break;
					}
					case idle_state::run:
					{
						if (on_run())
							next_state();

						break;
					}
					case idle_state::none:
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
	}

	void idle::clear()
	{
		object::clear();

		auto_lock();
		m_states.clear();		
		m_state = idle_state::none;
	}

	void idle::start()
	{
		auto_lock();
		m_states.clear();
		m_state = idle_state::sleep;
		m_sleep_timeout.start();
	}

	void idle::restart()
	{
		auto_lock();
		m_states.clear();
		m_state = idle_state::restart;
		m_sleep_timeout.restart();
	}

	void idle::stop()
	{
		auto_lock();
		m_states.clear();
		m_state = idle_state::stop;
		m_sleep_timeout.stop();
	}

	// Callback
	bool idle::on_sleep()
	{
		if (m_sleep_timeout.expired(m_sleep_delay))
		{
			m_sleep_timeout.restart();
			return set_state(idle_state::run);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(default_timeout::idle));

		return false;
	}

	// Utility
	void idle::next_state()
	{
		auto_lock();
		if (!m_states.empty())
		{
			m_state = m_states.front();
			m_states.pop_front();
		}
	}

	// Mutator
	bool idle::set_state(const idle_state state)
	{
		auto_lock();
		m_states.push_back(state);
		return true;
	}
}
