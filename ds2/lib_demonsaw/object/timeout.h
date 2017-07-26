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

#ifndef _EJA_TIMEOUT_
#define _EJA_TIMEOUT_

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
	// Timeout
	class timeout : public object
	{
	protected:
		std::chrono::system_clock::time_point m_start;

	public:
		using ptr = std::shared_ptr<timeout>;

	private:
		// Utility
		template <typename T> size_t elapsed() const;

	public:
		timeout() { }
		timeout(const eja::timeout& timeout) : object(timeout), m_start(timeout.m_start) { }
		timeout(const std::string& id) : object(id) { }
		timeout(const char* id) : object(id) { }
		virtual ~timeout() override { }

		// Interface
		virtual void start();
		virtual void restart();
		virtual void stop();

		// Utility		
		virtual bool valid() const { return m_start.time_since_epoch() != std::chrono::steady_clock::duration::zero(); }
		bool invalid() const { return !valid(); }

		size_t elapsed() const { return milliseconds(); }
		bool expired(const size_t ms) const { return elapsed() >= ms; }

		// Accessor
		size_t hours() const { return elapsed<std::chrono::hours>(); }
		size_t minutes() const { return elapsed<std::chrono::minutes>(); }
		size_t seconds() const { return elapsed<std::chrono::seconds>(); }
		size_t milliseconds() const { return elapsed<std::chrono::milliseconds>(); }
		size_t microseconds() const { return elapsed<std::chrono::microseconds>(); }
		size_t nanoseconds() const { return elapsed<std::chrono::nanoseconds>(); }

		// Static
		static ptr create() { return std::make_shared<timeout>(); }
		static ptr create(const eja::timeout& timeout) { return std::make_shared<eja::timeout>(timeout); }
		static ptr create(const timeout::ptr timeout) { return std::make_shared<eja::timeout>(*timeout); }
		static ptr create(const std::string& id) { return std::make_shared<timeout>(id); }
		static ptr create(const char* id) { return std::make_shared<timeout>(id); }
	};

	// Utility
	template <typename T>
	size_t timeout::elapsed() const
	{
		if (valid())
		{
			const auto duration = std::chrono::system_clock::now() - m_start;
			return static_cast<size_t>(std::chrono::duration_cast<T>(duration).count());
		}

		return 0;
	}

	// Container
	derived_type(timeout_list, mutex_list<timeout>);
	derived_type(timeout_map, mutex_map<std::string, timeout>);
	derived_type(timeout_queue, mutex_queue<timeout>);
	derived_type(timeout_vector, mutex_vector<timeout>);
}

#endif
