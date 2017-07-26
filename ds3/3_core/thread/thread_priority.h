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

#ifndef _EJA_THREAD_PRIORITY_H_
#define _EJA_THREAD_PRIORITY_H_

#include <thread>
#include <boost/thread.hpp>

namespace eja
{
#if _WIN32
	enum class thread_priority
	{
		idle = THREAD_PRIORITY_IDLE,
		lowest = THREAD_PRIORITY_LOWEST,
		below_normal = THREAD_PRIORITY_BELOW_NORMAL,
		normal = THREAD_PRIORITY_NORMAL,
		above_normal = THREAD_PRIORITY_ABOVE_NORMAL,
		highest = THREAD_PRIORITY_HIGHEST,
		time_critical = THREAD_PRIORITY_TIME_CRITICAL
	};
#else
	enum thread_priority
	{
		idle = 1,
		lowest = 15,
		below_normal = 30,
		normal = 50,
		above_normal = 65,
		highest = 85,
		time_critical = 99
	};
	/*
	root@debian:~# chrt -m
	SCHED_OTHER min/max priority	: 0/0
	SCHED_FIFO min/max priority	: 1/99
	SCHED_RR min/max priority	: 1/99
	SCHED_BATCH min/max priority	: 0/0
	SCHED_IDLE min/max priority	: 0/0

	*/
#endif
}

#endif