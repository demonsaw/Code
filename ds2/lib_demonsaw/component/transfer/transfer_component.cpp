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

#include "transfer_component.h"
#include "component/timer_component.h"
#include "component/io/file_component.h"
#include "utility/io/file_util.h"

namespace eja
{
	// Utility
	bool transfer_component::done() const
	{
		if (!m_done)
		{
			const auto owner = get_entity();
			const auto file = owner->get<file_component>();

			// v2.0
			////////////////////////////////////////////////////////
			//assert(m_size <= file->get_size());
			////////////////////////////////////////////////////////

			return file && m_size >= file->get_size();
		}

		return true;
	}

	// Accessor
	std::string transfer_component::get_elapsed() const
	{
		const auto owner = get_entity();
		const auto timer = owner->get<timer_component>();
		return timer ? timer->str() : "";
	}

	std::string transfer_component::get_estimate() const
	{
		if (m_size)
		{
			const auto owner = get_entity();
			const auto timer = owner->get<timer_component>();
			const auto file = owner->get<file_component>();

			if (timer && file)
			{
				const auto ratio = static_cast<double>(timer->milliseconds()) / static_cast<double>(m_size);
				const auto remaining = static_cast<size_t>(ratio * (file->get_size() - m_size));

				const auto hour = ms_to_hour(remaining);
				const auto min = ms_to_min(remaining);
				const auto sec = ms_to_sec(remaining) + 1;
				return boost::str(boost::format("%02d:%02d:%02d") % hour % min % sec);
			}
		}

		return "00:00:00";
	}

	size_t transfer_component::get_speed() const
	{
		const auto owner = get_entity();
		const auto timer = owner->get<timer_component>();
		if (timer)
		{
			const auto size = m_size - m_offset;
			const auto seconds = timer->seconds();			
			return seconds ? static_cast<size_t>(size / seconds) : static_cast<size_t>(size);
		}
		
		return 0;
	}

	double transfer_component::get_ratio() const
	{
		const auto owner = get_entity();
		const auto file = owner->get<file_component>();

		if (file && !file->empty())
		{
			const auto value = (static_cast<double>(m_size) / static_cast<double>(file->get_size()));

			// v1.0
			////////////////////////////////////////////////////////
			return (value <= 1.0) ? value : 1.0; 
			////////////////////////////////////////////////////////

			// v2.0
			////////////////////////////////////////////////////////
			//assert(value <= 100.0);
			//return value;
			////////////////////////////////////////////////////////
		}

		return 0.0;
	}
}
