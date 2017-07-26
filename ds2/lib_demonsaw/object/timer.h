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

#ifndef _EJA_TIMER_
#define _EJA_TIMER_

#include <chrono>
#include <memory>

#include "object.h"
#include "system/mutex/mutex_list.h"
#include "system/mutex/mutex_map.h"
#include "system/mutex/mutex_queue.h"
#include "system/mutex/mutex_vector.h"
#include "system/type.h"

namespace eja
{
	// Status
	class timer : public object
	{
	protected:
		std::chrono::system_clock::time_point m_start;
		std::chrono::system_clock::time_point m_stop;

	public:
		using ptr = std::shared_ptr<timer>;

	private:
		// Utility
		template <typename T> size_t elapsed() const;

	public:
		timer() { }
		timer(const eja::timer& timer) : object(timer), m_start(timer.m_start), m_stop(timer.m_stop) { }
		timer(const std::string& id) : object(id) { }
		timer(const char* id) : object(id) { }
		virtual ~timer() override { }

		// Interface
		virtual void start();		
		virtual void stop();
		virtual void restart();
		virtual void resume();

		// Utility		
		virtual bool valid() const { return m_start.time_since_epoch() != std::chrono::steady_clock::duration::zero(); }
		bool invalid() const { return !valid(); }	

		size_t elapsed() const { return milliseconds(); }
		bool running() const { return m_stop == std::chrono::system_clock::time_point(); }
		bool stopped() const { return !running(); }

		// Accessor
		size_t hours() const { return elapsed<std::chrono::hours>(); }
		size_t minutes() const { return elapsed<std::chrono::minutes>(); }
		size_t seconds() const { return elapsed<std::chrono::seconds>(); }
		size_t milliseconds() const { return elapsed<std::chrono::milliseconds>(); }
		size_t microseconds() const { return elapsed<std::chrono::microseconds>(); }
		size_t nanoseconds() const { return elapsed<std::chrono::nanoseconds>(); }

		// Static
		static ptr create() { return std::make_shared<timer>(); }
		static ptr create(const eja::timer& timer) { return std::make_shared<eja::timer>(timer); }
		static ptr create(const timer::ptr timer) { return std::make_shared<eja::timer>(*timer); }
		static ptr create(const std::string& id) { return std::make_shared<timer>(id); }
		static ptr create(const char* id) { return std::make_shared<timer>(id); }
	};

	// Utility
	template <typename T>
	size_t timer::elapsed() const
	{
		if (valid())
		{
			const auto now = (m_stop.time_since_epoch() == std::chrono::steady_clock::duration::zero()) ? std::chrono::system_clock::now() : m_stop;
			const auto duration = now - m_start;
			return static_cast<size_t>(std::chrono::duration_cast<T>(duration).count());
		}

		return 0;
	}

	// Container
	derived_type(timer_list, mutex_list<timer>);
	derived_type(timer_map, mutex_map<std::string, timer>);
	derived_type(timer_queue, mutex_queue<timer>);
	derived_type(timer_vector, mutex_vector<timer>);
}

#endif
