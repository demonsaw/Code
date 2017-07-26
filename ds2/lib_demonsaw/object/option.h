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

#ifndef _EJA_OPTIONS_
#define _EJA_OPTIONS_

#include <memory>

#include "object.h"
#include "system/mutex/mutex_list.h"
#include "system/mutex/mutex_map.h"
#include "system/mutex/mutex_queue.h"
#include "system/mutex/mutex_vector.h"
#include "system/type.h"
#include "utility/default_value.h"

namespace eja
{
	class option : public object
	{
	protected:
		bool m_enabled = default_value::enabled;
		size_t m_socket_timeout = default_socket::num_timeout;
		size_t m_max_errors = default_error::num_errors;

	public:
		using ptr = std::shared_ptr<option>;

	public:
		option() { }
		option(const eja::option& option) : eja::object(option), m_enabled(option.m_enabled), m_socket_timeout(option.m_socket_timeout), m_max_errors(option.m_max_errors) { }
		option(const std::string& id) : object(id) { }
		option(const char* id) : object(id) { }
		virtual ~option() override { }

		// Interface
		virtual void clear() override;

		// Utility
		bool enabled() const { return m_enabled; }
		bool disabled() const { return !m_enabled; }

		// Mutator
		void set_enabled(const bool value = true) { m_enabled = value; }
		void set_disabled(const bool value = true) { m_enabled = !value; }

		void set_socket_timeout(const size_t socket_timeout) { m_socket_timeout = socket_timeout; }
		void set_max_errors(const size_t max_errors) { m_max_errors = max_errors; }

		// Accessor
		size_t get_socket_timeout() const { return m_socket_timeout; }
		size_t get_max_errors() const { return m_max_errors; }

		// Static
		static ptr create() { return std::make_shared<option>(); }
		static ptr create(const eja::option& option) { return std::make_shared<eja::option>(option); }
		static ptr create(const option::ptr option) { return std::make_shared<eja::option>(*option); }
		static ptr create(const std::string& id) { return std::make_shared<option>(id); }
		static ptr create(const char* id) { return std::make_shared<option>(id); }
	};

	// Container
	derived_type(options_list, mutex_list<option>);
	derived_type(options_map, mutex_map<std::string, option>);
	derived_type(options_queue, mutex_queue<option>);
	derived_type(options_vector, mutex_vector<option>);
}

#endif
