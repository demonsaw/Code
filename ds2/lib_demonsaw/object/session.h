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

#ifndef _EJA_SESSION_
#define _EJA_SESSION_

#include <memory>
#include <string>

#include "cipher.h"
#include "system/mutex/mutex_list.h"
#include "system/mutex/mutex_map.h"
#include "system/mutex/mutex_queue.h"
#include "system/mutex/mutex_vector.h"
#include "system/type.h"
#include "utility/default_value.h"

namespace eja
{
	class session : public cipher
	{
	public:
		using ptr = std::shared_ptr<session>;

	public:
		session() { }
		session(const eja::session& session) : cipher(session) { }
		session(const std::string& id) : cipher(id) { }
		session(const char* id) : cipher(id) { }
		virtual ~session() override { }

		// Static
		static ptr create() { return std::make_shared<session>(); }
		static ptr create(const eja::session& session) { return std::make_shared<eja::session>(session); }
		static ptr create(const session::ptr session) { return std::make_shared<eja::session>(*session); }
		static ptr create(const std::string& id) { return std::make_shared<session>(id); }
		static ptr create(const char* id) { return std::make_shared<session>(id); }
	};

	// Container
	derived_type(session_list, mutex_list<session>);
	derived_type(session_map, mutex_map<std::string, session>);
	derived_type(session_queue, mutex_queue<session>);
	derived_type(session_vector, mutex_vector<session>);
}

#endif
