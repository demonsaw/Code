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

#ifndef _EJA_STATUS_COMPONENT_
#define _EJA_STATUS_COMPONENT_

#include <memory>

#include "component.h"
#include "entity/entity.h"
#include "system/function/function.h"
#include "system/function/function_type.h"
#include "object/object.h"
#include "object/status.h"
#include "system/type.h"

namespace eja
{
	// Component
	class status_component : public object, public component
	{
	protected:
		function_list m_functions;
		eja::status m_status = status::none;		

	public:
		using ptr = std::shared_ptr<status_component>;

	public:
		status_component() { }
		status_component(const eja::status status) : m_status(status) { }
		virtual ~status_component() override { }

		// Interface
		virtual void clear() override;

		// Utility		
		void add(const function::ptr ptr) { m_functions.add(ptr); }
		void remove(const function::ptr ptr) { m_functions.remove(ptr); }

		bool none() const { return m_status == status::none; }	
		bool success() const { return m_status == status::success; }
		bool warning() const { return m_status == status::warning; }
		bool error() const { return m_status == status::error; }
		bool info() const { return m_status == status::info; }
		bool pending() const { return m_status == status::pending; }
		bool cancelled() const { return m_status == status::cancelled; }
		bool unknown() const { return m_status == status::unknown; }

		// Mutator
		virtual void set_status(const eja::status status);

		// Accessor
		const eja::status get_status() const { return m_status; }

		// Static
		static ptr create() { return ptr(new status_component()); }
		static ptr create(const eja::status status) { return ptr(new status_component(status)); }
	};

	// Container
	derived_type(status_list_component, list_component<status>);
	derived_type(status_map_component, map_component<std::string, status>);
	derived_type(status_queue_component, queue_component<status>);
	derived_type(status_vector_component, vector_component<status>);

	derived_type(status_entity_list_component, entity_list_component);
	derived_type(status_entity_map_component, entity_map_component);
	derived_type(status_entity_queue_component, entity_queue_component);
	derived_type(status_entity_vector_component, entity_vector_component);
}

#endif
