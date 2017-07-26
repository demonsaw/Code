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

#ifndef _EJA_CLIENT_
#define _EJA_CLIENT_

#include <memory>
#include <string>

#include "object/object.h"
#include "system/mutex/mutex_list.h"
#include "system/mutex/mutex_map.h"
#include "system/mutex/mutex_queue.h"
#include "system/mutex/mutex_vector.h"
#include "system/type.h"
#include "utility/default_value.h"

namespace eja
{
	class json_client;

	class client : public object
	{
	protected:
		std::string m_version;
		std::string m_name;
		std::string m_join_name;

	public:
		using ptr = std::shared_ptr<client>;

	public:
		client() : object(default_security::id) { }
		client(const eja::client& client) : object(client), m_version(client.m_version), m_name(client.m_name), m_join_name(client.m_join_name) { }
		client(const std::string& id) : object(id) { }
		client(const char* id) : object(id) { }

		// Interface
		virtual void clear() override;

		// Utility		
		bool has_version() const { return !m_version.empty(); }
		bool has_name() const { return !m_name.empty(); }
		bool has_join_name() const { return !m_join_name.empty(); }

		// Mutator
		void set(const std::shared_ptr<json_client> client);
		void set_version(const std::string& version) { m_version = version; }
		void set_version(const char* version) { m_version = version; }

		void set_name(const std::string& name) { m_name = name; }
		void set_name(const char* name) { m_name = name; }

		void set_join_name() { m_join_name = m_name; }
		void set_join_name(const std::string& join_name) { m_join_name = join_name; }
		void set_join_name(const char* join_name) { m_join_name = join_name; }

		// Accessor
		const std::string& get_version() const { return m_version; }
		const std::string& get_name() const { return m_name; }
		const std::string& get_join_name() const { return m_join_name; }

		// Static
		static ptr create() { return std::make_shared<client>(); }
		static ptr create(const eja::client& client) { return std::make_shared<eja::client>(client); }
		static ptr create(const client::ptr client) { return std::make_shared<eja::client>(*client); }
		static ptr create(const std::string& id) { return std::make_shared<client>(id); }
		static ptr create(const char* id) { return std::make_shared<client>(id); }
	};

	// Container
	derived_type(client_list, mutex_list<client>);
	derived_type(client_map, mutex_map<std::string, client>);
	derived_type(client_queue, mutex_queue<client>);
	derived_type(client_vector, mutex_vector<client>);
}

#endif
