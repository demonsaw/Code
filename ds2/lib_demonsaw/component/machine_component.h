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

#ifndef _EJA_MACHINE_COMPONENT_
#define _EJA_MACHINE_COMPONENT_

#include <exception>

#include "component.h"
#include "function_component.h"
#include "entity/entity.h"
#include "http/http_status.h"
#include "object/machine.h"
#include "system/type.h"
#include "system/function/function.h"
#include "system/function/function_type.h"
#include "utility/default_value.h"

namespace eja
{
	// Component
	class machine_component : public machine, public component
	{
	protected:		
		function_list m_functions;

	public:
		using ptr = std::shared_ptr<machine_component>;

	protected:
		machine_component() { }

		// Utility
		using component::log;
		void log(const http_status& status) { log(status.str()); }

		// Mutator
		bool set_error(const http_status& status);
		bool set_error(const std::exception& ex);		
		bool set_error(const std::string& str);
		bool set_error(const char* psz);
		bool set_error();

	public:		
		virtual ~machine_component() override { }

		// Interface
		virtual void shutdown() override;		

		// Utility		
		virtual void clear() override;
		void add(const function::ptr ptr) { m_functions.add(ptr); }
		void remove(const function::ptr ptr) { m_functions.remove(ptr); }

		// Mutator
		virtual void set_status(const eja::status status) override;

		// Accessor
		function_list& get_functions() { return m_functions; }
	};

	// Container
	derived_type(machine_list_component, list_component<machine>);
	derived_type(machine_map_component, map_component<std::string, machine>);
	derived_type(machine_queue_component, queue_component<machine>);
	derived_type(machine_vector_component, vector_component<machine>);

	derived_type(machine_entity_list_component, entity_list_component);
	derived_type(machine_entity_map_component, entity_map_component);
	derived_type(machine_entity_queue_component, entity_queue_component);
	derived_type(machine_entity_vector_component, entity_vector_component);
}

#endif
