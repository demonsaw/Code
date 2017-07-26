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

#ifndef _EJA_ROUTER_INFO_
#define _EJA_ROUTER_INFO_

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
	class router_info : public object
	{
	protected:		
		size_t m_sessions = 0;
		size_t m_clients = 0;
		size_t m_groups = 0;

	public:
		using ptr = std::shared_ptr<router_info>;

	public:
		router_info() { }
		router_info(const eja::router_info& router_info) : object(router_info), m_sessions(router_info.m_sessions), m_clients(router_info.m_clients), m_groups(router_info.m_groups) { }
		router_info(const std::string& id) : object(id) { }
		router_info(const char* id) : object(id) { }
		virtual ~router_info() override { }

		// Interface
		virtual void clear() override;

		// Mutator		
		void set_sessions(const size_t sessions) { m_sessions = sessions; }
		void set_clients(const size_t clients) { m_clients = clients; }
		void set_groups(const size_t groups) { m_groups = groups; }

		// Accessor		
		size_t get_sessions() const { return m_sessions; }
		size_t get_clients() const { return m_clients; }
		size_t get_groups() const { return m_groups; }

		// Static
		static ptr create() { return std::make_shared<router_info>(); }
		static ptr create(const eja::router_info& router_info) { return std::make_shared<eja::router_info>(router_info); }
		static ptr create(const router_info::ptr router_info) { return std::make_shared<eja::router_info>(*router_info); }
		static ptr create(const std::string& id) { return std::make_shared<router_info>(id); }
		static ptr create(const char* id) { return std::make_shared<router_info>(id); }
	};
}

#endif
