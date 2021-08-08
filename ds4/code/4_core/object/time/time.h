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

#ifndef _EJA_TIME_H_
#define _EJA_TIME_H_

#include <chrono>
#include <ctime>
#include <string>

#include "object/object.h"
#include "system/type.h"

namespace eja
{
	class time : public object
	{
		make_factory(time);

	protected:
		time_t m_time;

	public:
		time() : m_time(0) { }
		time(const time& obj) : m_time(obj.m_time) { }

		// Operator
		time& operator=(const time& obj);

		// Interface
		virtual void clear() override;

		void start() { m_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()); }
		void restart() { start(); }
		void stop() { m_time = 0; }

		// Utility		
		virtual bool valid() const override { return m_time != 0; }
		std::string str() const;

		// Get
		const time_t& get_time() const { return m_time; }
		time_t& get_time() { return m_time; }

		size_t get_hours() const { return localtime(&m_time)->tm_hour; }
		size_t get_minutes() const { return localtime(&m_time)->tm_min; }
		size_t get_seconds() const { return localtime(&m_time)->tm_sec; }
	};
}

#endif
