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

#ifndef _EJA_TIMER_H_
#define _EJA_TIMER_H_

#include <chrono>

#include "object/object.h"
#include "system/type.h"

namespace eja
{
	class timer : public object
	{
		make_factory(timer);

	protected:
		std::chrono::system_clock::time_point m_start;
		std::chrono::system_clock::time_point m_stop;

	protected:
		// Utility
		template <typename T>
		size_t get_elapsed() const;

	public:
		timer() { }
		timer(const timer& obj) : m_start(obj.m_start), m_stop(obj.m_stop) { }

		// Operator
		timer& operator=(const timer& obj);

		// Interface
		virtual void start();
		virtual void restart();
		virtual void stop() { m_stop = std::chrono::system_clock::now(); }
		virtual void resume() { m_stop = std::chrono::system_clock::time_point(); }

		// Utility
		virtual bool valid() const override { return m_start != std::chrono::system_clock::time_point(); }
		bool elapsed(const size_t ms) const { return get_milliseconds() >= ms; }
		std::string str() const;

		// Get
		size_t get_time() const { return get_milliseconds(); }
		size_t get_hours() const { return get_elapsed<std::chrono::hours>(); }
		size_t get_minutes() const { return get_elapsed<std::chrono::minutes>(); }
		size_t get_seconds() const { return get_elapsed<std::chrono::seconds>(); }
		size_t get_milliseconds() const { return get_elapsed<std::chrono::milliseconds>(); }
		size_t get_microseconds() const { return get_elapsed<std::chrono::microseconds>(); }
		size_t get_nanoseconds() const { return get_elapsed<std::chrono::nanoseconds>(); }
	};

	// Utility
	template <typename T>
	size_t timer::get_elapsed() const
	{
		if (valid())
		{
			const auto now = (m_stop.time_since_epoch() == std::chrono::steady_clock::duration::zero()) ? std::chrono::system_clock::now() : m_stop;
			const auto duration = now - m_start;
			return static_cast<size_t>(std::chrono::duration_cast<T>(duration).count());
		}

		return 0;
	}
}

#endif
