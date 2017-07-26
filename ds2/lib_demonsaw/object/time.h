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

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)	// 'localtime': This function or variable may be unsafe.
#endif

#ifndef _EJA_TIME_
#define _EJA_TIME_

#include <chrono>
#include <iostream>
#include <memory>
#include <ctime>

#include "object.h"
#include "system/mutex/mutex_list.h"
#include "system/mutex/mutex_map.h"
#include "system/mutex/mutex_queue.h"
#include "system/mutex/mutex_vector.h"
#include "system/type.h"

namespace eja
{
	// Status
	class time : public object
	{
	protected:
		time_t m_time = 0;

	public:
		using ptr = std::shared_ptr<time>;

	public:
		time() { }
		time(const eja::time& time) : object(time), m_time(time.m_time) { }
		time(const std::string& id) : object(id) { }
		time(const char* id) : object(id) { }
		virtual ~time() override { }

		// Interface
		void start() { m_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()); }
		void restart() { start(); }
		void stop() { m_time = 0; }

		// Utility
		size_t elapsed() const { return static_cast<size_t>(m_time); }
		virtual bool valid() const override { return m_time != 0; }

		// Accessor
		const time_t& data() const { return m_time; }
		size_t hours() const { return localtime(&m_time)->tm_hour; }
		size_t minutes() const { return localtime(&m_time)->tm_min; }
		size_t seconds() const { return localtime(&m_time)->tm_sec; }

		// Static
		static ptr create() { return std::make_shared<time>(); }
		static ptr create(const eja::time& time) { return std::make_shared<eja::time>(time); }
		static ptr create(const time::ptr time) { return std::make_shared<eja::time>(*time); }
		static ptr create(const std::string& id) { return std::make_shared<time>(id); }
		static ptr create(const char* id) { return std::make_shared<time>(id); }
	};

	// Container
	derived_type(time_list, mutex_list<time>);
	derived_type(time_map, mutex_map<std::string, time>);
	derived_type(time_queue, mutex_queue<time>);
	derived_type(time_vector, mutex_vector<time>);
}

#endif
