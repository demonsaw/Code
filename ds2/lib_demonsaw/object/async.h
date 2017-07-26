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

#ifndef _EJA_ASYNC_
#define _EJA_ASYNC_

#include <iostream>
#include <memory>

#include "object.h"
#include "system/mutex/mutex_list.h"
#include "system/mutex/mutex_map.h"
#include "system/mutex/mutex_queue.h"
#include "system/mutex/mutex_vector.h"
#include "system/type.h"

namespace eja
{
	class async : public object
	{
	public:
		using ptr = std::shared_ptr<async>;

	protected:
		async() { }
		async(const eja::async& async) : object(async) { }
		async(const std::string& id) : object(id) { }
		async(const char* id) : object(id) { }
		virtual ~async() override { }

		// Interface
		virtual void main() = 0;

	public:
		// Interface
		virtual void start() = 0;
	};

	// Container
	derived_type(async_list, mutex_list<async>);
	derived_type(async_map, mutex_map<std::string, async>);
	derived_type(async_queue, mutex_queue<async>);
	derived_type(async_vector, mutex_vector<async>);
}

#endif
