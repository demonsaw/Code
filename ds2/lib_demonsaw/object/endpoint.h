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

#ifndef _EJA_ENDPOINT_
#define _EJA_ENDPOINT_

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
	class endpoint : public object
	{
	protected:
		std::string m_version;
		std::string m_name;
		std::string m_address = default_network::empty;
		u16 m_port = default_network::port;

	public:
		using ptr = std::shared_ptr<endpoint>;	

	public:
		endpoint() : object(default_security::id) { }
		endpoint(const eja::endpoint& endpoint) : object(endpoint), m_version(endpoint.m_version), m_name(endpoint.m_name), m_address(endpoint.m_address), m_port(endpoint.m_port) { }
		endpoint(const std::string& id) : object(id) { }
		endpoint(const char* id) : object(id) { }
		virtual ~endpoint() override { }

		// Interface
		virtual void clear() override;

		// Utility		
		virtual bool valid() const override { return object::valid() && has_address() && asio_port::valid(m_port); }		
		
		bool has_version() const { return !m_version.empty(); }
		bool has_name() const { return !m_name.empty(); }
		bool has_address() const { return !m_address.empty(); }
		bool has_port() const { return m_port > 0; }

		// Mutator
		void set_version(const std::string& version) { m_version = version; }
		void set_version(const char* version) { m_version = version; }

		void set_name(const std::string& name) { m_name = name; }
		void set_name(const char* name) { m_name = name; }

		void set_address(const std::string& address) { m_address = address; }
		void set_address(const char* address) { m_address = address; }

		void set_port(const std::string& port) { m_port = asio_resolver::get_port(port); }
		void set_port(const char* port) { m_port = asio_resolver::get_port(port); }
		void set_port(const u16 port) { m_port = port; }

		// Accessor
		const std::string& get_version() const { return m_version; }
		const std::string& get_name() const { return m_name; }
		const std::string& get_address() const { return m_address; }
		const u16& get_port() const { return m_port; }

		// Static
		static ptr create() { return std::make_shared<endpoint>(); }
		static ptr create(const eja::endpoint& endpoint) { return std::make_shared<eja::endpoint>(endpoint); }
		static ptr create(const endpoint::ptr endpoint) { return std::make_shared<eja::endpoint>(*endpoint); }
		static ptr create(const std::string& id) { return std::make_shared<endpoint>(id); }
		static ptr create(const char* id) { return std::make_shared<endpoint>(id); }
	};

	// Container
	derived_type(endpoint_list, mutex_list<endpoint>);
	derived_type(endpoint_map, mutex_map<std::string, endpoint>);
	derived_type(endpoint_queue, mutex_queue<endpoint>);
	derived_type(endpoint_vector, mutex_vector<endpoint>);
}

#endif
