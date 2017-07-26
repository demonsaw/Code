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

#ifndef _EJA_SERVER_
#define _EJA_SERVER_

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
	class server : public endpoint
	{
	public:
		using ptr = std::shared_ptr<server>;

	public:
		server() { }
		server(const eja::server& server) : endpoint(server) { }
		server(const std::string& id) : endpoint(id) { }
		server(const char* id) : endpoint(id) { }
		virtual ~server() override { }		

		// Static
		static ptr create() { return std::make_shared<server>(); }
		static ptr create(const eja::server& server) { return std::make_shared<eja::server>(server); }
		static ptr create(const server::ptr server) { return std::make_shared<eja::server>(*server); }
		static ptr create(const std::string& id) { return std::make_shared<server>(id); }
		static ptr create(const char* id) { return std::make_shared<server>(id); }
	};

	// Container
	derived_type(server_list, mutex_list<server>);
	derived_type(server_map, mutex_map<std::string, server>);
	derived_type(server_queue, mutex_queue<server>);
	derived_type(server_vector, mutex_vector<server>);
}

#endif
