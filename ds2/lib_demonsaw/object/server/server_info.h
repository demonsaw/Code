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

#ifndef _EJA_SERVER_INFO_
#define _EJA_SERVER_INFO_

#include <memory>
#include <string>

#include "object/option.h"
#include "system/mutex/mutex_list.h"
#include "system/mutex/mutex_map.h"
#include "system/mutex/mutex_queue.h"
#include "system/mutex/mutex_vector.h"
#include "system/type.h"

namespace eja
{
	class server_info : public object
	{
	protected:
		size_t m_sessions = 0;

	public:
		using ptr = std::shared_ptr<server_info>;

	public:
		server_info() { }
		server_info(const eja::server_info& server_info) : object(server_info), m_sessions(server_info.m_sessions) { }
		server_info(const std::string& id) : object(id) { }
		server_info(const char* id) : object(id) { }
		virtual ~server_info() override { }

		// Interface
		virtual void clear() override;

		// Mutator
		void set_sessions(const size_t sessions) { m_sessions = sessions; }

		// Accessor
		size_t get_sessions() const { return m_sessions; }

		// Static
		static ptr create() { return std::make_shared<server_info>(); }
		static ptr create(const eja::server_info& server_info) { return std::make_shared<eja::server_info>(server_info); }
		static ptr create(const server_info::ptr server_info) { return std::make_shared<eja::server_info>(*server_info); }
		static ptr create(const std::string& id) { return std::make_shared<server_info>(id); }
		static ptr create(const char* id) { return std::make_shared<server_info>(id); }
	};
}

#endif
