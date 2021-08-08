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

#include <boost/format.hpp>

#include "component/io/file_component.h"
#include "component/router/router_component.h"
#include "component/status/status_component.h"
#include "component/time/timeout_component.h"
#include "component/time/timer_component.h"
#include "component/transfer/transfer_component.h"

#include "entity/entity.h"
#include "object/router.h"
#include "utility/value.h"

namespace eja
{	
	// Static
	//static const size_t s_timeout_ms[] = { 0, 100, 161, 261, 423, 685, 1109, 1794, 2903 };
	static const size_t s_timeout_ms[] = { 0, 100, 100, 200, 300, 500, 800, 1300, 2100, 3400 };	
	static const size_t s_timeout_size = sizeof(s_timeout_ms) / sizeof(size_t);
	static const size_t s_timeout_max = s_timeout_ms[s_timeout_size - 1];

	// Interface
	void transfer_component::update(const u64 size)
	{
		component::update();

		// Status
		const auto owner = get_owner();

		if (is_running())
		{
			// Timeout			
			const auto timeout = owner->get<timeout_component>();
			timeout->update();

			// Timer
			if (!has_progress())
			{
				const auto timer = owner->get<timer_component>();
				timer->restart();
			}

			const auto status = owner->get<status_component>();
			status->set_success();
		}

		// Size
		m_progress += size;
		m_size += size;

		m_retry = 0;		
		m_wait = 0;

		// Done?
		const auto file = owner->get<file_component>();
		if (m_size >= file->get_size())
		{
			stop();

			set_done(true);
		}

		// Should never trigger for download
		assert(m_progress <= file->get_size());
		assert(m_size <= file->get_size());
	}

	void transfer_list_component::clear()
	{
		thread_safe_ex<component, std::deque<entity::ptr>>::clear();

		m_next = 0;
	}

	// Utility
	void transfer_component::start()
	{
		if (!is_running())
		{
			set_running(true);

			m_progress = 0;

			// Status			
			const auto owner = get_owner(); 
			const auto status = owner->get<status_component>();
			status->set_pending();

			// Timeout		
			const auto timeout = owner->get<timeout_component>();
			timeout->start();

			// Timer		
			const auto timer = owner->get<timer_component>();
			timer->start();
		}
	}

	void transfer_component::stop()
	{
		if (is_running())
		{
			set_running(false);

			// Timer
			const auto owner = get_owner();
			const auto timer = owner->get<timer_component>();
			timer->stop();

			// Timeout
			const auto timeout = owner->get<timeout_component>();
			timeout->stop();

			// Transfer
			if (!is_done())
			{
				if (owner->has<transfer_map_component>())
				{
					const auto transfer_map = owner->get<transfer_map_component>();
					{
						thread_lock(transfer_map);
						transfer_map->clear();
					}
				}

				if (owner->has<transfer_list_component>())
				{
					const auto transfer_list = owner->get<transfer_list_component>();
					{
						thread_lock(transfer_list);
						transfer_list->clear();
					}
				}
			}

			//m_retry = 0;
			m_wait = 0;
		}
	}

	void transfer_component::pause()
	{
		if (!is_paused())
		{
			set_paused(true);

			if (is_running())
			{
				set_running(false);

				// Timer
				const auto owner = get_owner();
				const auto timer = owner->get<timer_component>();
				timer->stop();

				// Timeout
				const auto timeout = owner->get<timeout_component>();
				timeout->stop();

				//m_retry = 0;
				m_wait = 0;

				// Status
				const auto status = owner->get<status_component>();
				status->set_pending();
			}
		}		
	}

	void transfer_component::reset()
	{
		m_retry = 0;

		set_cancelled(false);
		set_done(false);
		set_errored(false);
	}

	void transfer_component::resume()
	{
		if (is_paused())
			set_paused(false);
	}

	void transfer_component::cancel()
	{
		if (!is_done())
		{			
			stop();

			m_retry = 0;

			set_cancelled(true);
			set_done(true);
			set_paused(false);
		}		
	}

	void transfer_component::error()
	{
		if (!is_done())
		{
			stop();

			m_retry = 0;

			set_done(true);
			set_errored(true);
			set_paused(false);
		}
	}

	void transfer_component::finish()
	{
		if (!is_done())
		{
			stop();

			m_retry = 0;

			set_done(true);
			set_paused(false);
		}
	}

	// Has
	bool transfer_component::has_chunks() const
	{
		const auto owner = get_owner();
		if (owner->has<chunk_list_component>())
		{
			const auto chunk_list = owner->get<chunk_list_component>();
			if (!chunk_list->empty())
				return true;

			const auto chunk_set = owner->get<chunk_set_component>();
			if (!chunk_set->empty())
				return true;
		}

		return false;
	}

	// Is	
	bool transfer_component::is_done() const
	{
		if (is_mode(bits::done))
			return true;

		const auto owner = get_owner();
		const auto file = owner->get<file_component>();
		assert(m_size <= file->get_size());

		return m_size == file->get_size();
	}

	bool transfer_component::is_download() const
	{
		const auto owner = get_owner();
		return owner->has<transfer_list_component>();
	}

	// Get
	size_t transfer_component::get_chunks() const
	{
		const auto owner = get_owner();
		if (owner->has<chunk_list_component>())
		{
			const auto chunk_list = owner->get<chunk_list_component>();
			const auto chunk_set = owner->get<chunk_set_component>();
			return chunk_list->size() + chunk_set->size();
		}

		return 0;
	}

	u64 transfer_component::get_remaining() const
	{
		const auto owner = get_owner();
		const auto file = owner->get<file_component>();
		assert(m_size <= file->get_size());

		return file->get_size() - m_size;
	}

	std::string transfer_component::get_time() const
	{			
		size_t ms = 0;

		if (is_running())
		{
			const auto owner = get_owner();
			const auto timer = owner->get<timer_component>();

			if (has_progress())
			{
				const auto ratio = static_cast<double>(timer->get_milliseconds()) / static_cast<double>(m_progress);
				const auto file = owner->get<file_component>();
				assert(m_progress <= file->get_size());

				ms = static_cast<size_t>(ratio * (file->get_size() - m_size));
			}
			else
			{
				ms = timer->get_milliseconds();
			}
		}
		else if (is_done())
		{
			const auto owner = get_owner();
			const auto timer = owner->get<timer_component>();
			ms = timer->get_milliseconds();
		}

		if (ms)
		{
			const auto seconds = (ms / 1000) % 60;
			const auto minutes = (ms / (1000 * 60)) % 60;
			const auto hours = (ms / (1000 * 60 * 60)) % 24;

			return boost::str(boost::format("%02d:%02d:%02d") % hours % minutes % (seconds + 1));
		}

		return "00:00:00";
	}

	size_t transfer_component::get_speed() const
	{
		const auto owner = get_owner();
		const auto timer = owner->get<timer_component>();
		const auto seconds = timer->get_seconds();

		return seconds ? static_cast<size_t>(m_progress / seconds) : static_cast<size_t>(m_progress);
	}

	double transfer_component::get_ratio() const
	{
		const auto owner = get_owner();
		const auto file = owner->get<file_component>();
		//assert(m_size <= file->get_size());

		if (file->has_size() && (m_size < file->get_size()))
			return static_cast<double>(m_size) / static_cast<double>(file->get_size());

		return 1.0;
	}	

	size_t transfer_component::get_wait() const 
	{
		return (m_wait < s_timeout_size) ? s_timeout_ms[m_wait] : s_timeout_max;
	}

	entity::ptr transfer_list_component::get_entity()
	{
		if (!empty())
		{
			m_next = clamp(static_cast<size_t>(0), m_next, size() - 1);
			const auto entity = at(m_next++);
			return entity;
		}

		return nullptr;
	}
}
