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

#ifndef _EJA_NAT_
#define _EJA_NAT_

#include <cassert>
#include <memory>
#include <string>

#include "object.h"
#include "asio/asio_port.h"
#include "asio/asio_resolver.h"
#include "system/mutex/mutex_list.h"
#include "system/mutex/mutex_map.h"
#include "system/mutex/mutex_queue.h"
#include "system/mutex/mutex_vector.h"
#include "system/type.h"
#include "utility/default_value.h"

namespace eja
{
	class nat : public object
	{
	private:
		std::string m_name;
		u16 m_port = default_network::port;

	public:
		using ptr = std::shared_ptr<nat>;

	public:
		nat() { }		
		nat(const eja::nat& nat) : object(nat), m_name(nat.m_name), m_port(nat.m_port) { }		
		nat(const std::string& id) : object(id) { }
		nat(const char* id) : object(id) { }

		nat(const u16 port) : m_port(port) { assert(asio_port::valid(m_port)); }
		virtual ~nat() override { }

		// Interface
		virtual void clear() override;

		// Utility
		virtual bool valid() const override { return object::valid() && asio_port::valid(m_port); }
		bool has_name() const { return !m_name.empty(); }
		bool has_port() const { return m_port > 0; }

		// Mutator
		void set_name(const std::string& name) { m_name = name; }

		void set_port(const std::string& port) { set_port(port.c_str()); }
		void set_port(const char* port) { m_port = asio_resolver::get_port(port); }
		void set_port(const u16 port) { m_port = port; }

		// Accessor
		const std::string& get_name() const { return m_name; }
		const u16& get_port() const { return m_port; }

		// Static
		static ptr create() { return std::make_shared<nat>(); }		
		static ptr create(const eja::nat& nat) { return std::make_shared<eja::nat>(nat); }
		static ptr create(const nat::ptr nat) { return std::make_shared<eja::nat>(*nat); }
		static ptr create(const std::string& id) { return std::make_shared<nat>(id); }
		static ptr create(const char* id) { return std::make_shared<nat>(id); }
		static ptr create(const u16 port) { return std::make_shared<nat>(port); }
	};

	// Container
	derived_type(nat_list, mutex_list<nat>);
	derived_type(nat_map, mutex_map<std::string, nat>);
	derived_type(nat_queue, mutex_queue<nat>);
	derived_type(nat_vector, mutex_vector<nat>);
}

#endif
