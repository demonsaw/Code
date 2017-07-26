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

#include "component/endpoint_component.h"
#include "component/client/client_service_component.h"
#include "component/io/download_component.h"
#include "component/io/file_component.h"
#include "component/io/transfer_component.h"
#include "component/io/upload_component.h"
#include "component/time/timeout_component.h"
#include "component/time/timer_component.h"
#include "entity/entity.h"
#include "security/security.h"
#include "utility/io/file_util.h"

namespace eja
{
	// Constructor
	transfer_component::transfer_component(const transfer_component& comp) : m_stopped(comp.m_stopped), m_valid(comp.m_valid)
	{
		m_size.store(comp.m_size);
		m_speed_size.store(comp.m_speed_size);
		m_chunk.store(comp.m_chunk);
		m_shards.store(comp.m_shards);
	}

	// Operator
	transfer_component& transfer_component::operator=(const transfer_component& comp)
	{
		if (this != &comp)
		{
			component::operator=(comp);

			m_size.store(comp.m_size);
			m_speed_size.store(comp.m_speed_size);
			m_chunk.store(comp.m_chunk);
			m_shards.store(comp.m_shards);
			
			m_stopped = comp.m_stopped;
			m_valid = comp.m_valid;
		}

		return *this;
	}

	// Interface
	void transfer_component::clear()
	{
		component::clear();

		m_size = 0;
		m_speed_size = 0;
		m_chunk = 0;
		m_shards = 0;
		
		m_stopped = false;
		m_valid = false;
	}

	void transfer_component::resume()
	{		
		const auto owner = get_entity();

		if (owner)
		{			
			// Timer
			const auto timer = owner->get<timer_component>();
			timer->start();

			// Timeout
			const auto timeout = owner->get<timeout_component>();
			timeout->start();

			// List
			const auto transfer_list = owner->get<transfer_list_component>();
			{
				thread_lock(transfer_list);
				transfer_list->clear();
			}
		}
		
		m_shards = 0;
		m_speed_size = 0;

		m_stopped = false;
		m_valid = true;		
	}

	void transfer_component::stop()
	{		
		const auto owner = get_entity();

		if (owner)
		{
			// Timer
			const auto timer = owner->get<timer_component>();
			timer->stop();

			// Timeout
			const auto timeout = owner->get<timeout_component>();
			timeout->stop();

			// Map
			if (empty())
			{
				const auto parent = owner->get_parent();
				const auto download_map = parent->get<download_map_component>();
				{
					thread_lock(download_map);
					download_map->erase(owner->get_id());
				}
			}
		}
		
		m_shards = 0;
		m_stopped = true;
		m_valid = false;
	}

	// Utility
	bool transfer_component::complete() const
	{
		const auto owner = get_entity();
		if (owner)
		{
			const auto file = owner->get<file_component>();
			return m_size >= file->get_size();
		}

		return false;
	}

	// Get
	std::string transfer_component::get_time() const
	{
		const auto owner = get_entity();
		if (owner)
		{
			const auto timer = owner->get<timer_component>();
			auto ms = timer->get_milliseconds();

			if (valid())
			{
				const auto ratio = m_speed_size ? (static_cast<double>(ms) / static_cast<double>(m_speed_size)) : 0;
				const auto file = owner->get<file_component>();
				ms = static_cast<size_t>(ratio * (file->get_size() - m_speed_size));
			}

			if (ms)
			{
				const auto seconds = (ms / 1000) % 60;
				const auto minutes = (ms / (1000 * 60)) % 60;
				const auto hours = (ms / (1000 * 60 * 60)) % 24;

				return boost::str(boost::format("%02d:%02d:%02d") % hours % minutes % (seconds + 1));
			}
		}

		return "00:00:00";
	}

	size_t transfer_component::get_speed() const
	{
		const auto owner = get_entity();
		if (owner)
		{
			const auto timer = owner->get<timer_component>();
			const auto seconds = timer->get_seconds();

			return seconds ? static_cast<size_t>(m_speed_size / seconds) : static_cast<size_t>(m_speed_size);
		}

		return 0;
	}

	double transfer_component::get_ratio() const
	{
		const auto owner = get_entity();
		if (owner)
		{
			const auto file = owner->get<file_component>();
			return file->has_size() ? static_cast<double>(m_size) / static_cast<double>(file->get_size()) : 1.0;
		}

		return 0;
	}
}
