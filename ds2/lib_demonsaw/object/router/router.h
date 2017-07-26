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

#ifndef _EJA_ROUTER_
#define _EJA_ROUTER_

#include <memory>
#include <string>

#include "object/endpoint.h"
#include "system/mutex/mutex_list.h"
#include "system/mutex/mutex_map.h"
#include "system/mutex/mutex_queue.h"
#include "system/mutex/mutex_vector.h"
#include "system/type.h"

namespace eja
{
	class router : public endpoint
	{
	public:
		using ptr = std::shared_ptr<router>;

	public:
		router() { }
		router(const eja::router& router) : endpoint(router) { }
		router(const std::string& id) : endpoint(id) { }
		router(const char* id) : endpoint(id) { }
		virtual ~router() override { }

		// Static
		static ptr create() { return std::make_shared<router>(); }
		static ptr create(const eja::router& router) { return std::make_shared<eja::router>(router); }
		static ptr create(const router::ptr router) { return std::make_shared<eja::router>(*router); }
		static ptr create(const std::string& id) { return std::make_shared<router>(id); }
		static ptr create(const char* id) { return std::make_shared<router>(id); }
	};

	// Container
	derived_type(router_list, mutex_list<router>);
	derived_type(router_map, mutex_map<std::string, router>);
	derived_type(router_queue, mutex_queue<router>);
	derived_type(router_vector, mutex_vector<router>);
}

#endif
