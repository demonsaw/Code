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

#ifndef _EJA_IDLE_COMPONENT_
#define _EJA_IDLE_COMPONENT_

#include <memory>

#include "component.h"
#include "function_component.h"
#include "entity/entity.h"
#include "http/http_status.h"
#include "object/idle.h"
#include "system/type.h"
#include "system/function/function_type.h"
#include "utility/default_value.h"

namespace eja
{
	// Component
	class idle_component : public idle, public component, public std::enable_shared_from_this<idle_component>
	{
	public:
		using ptr = std::shared_ptr<idle_component>;

	protected:
		idle_component() { }
		idle_component(const eja::idle& idle) : eja::idle(idle) { }
		idle_component(const size_t delay) : idle(delay) { }

		// Utility
		using component::log;
		void log(const http_status& status) { log(status.str()); }

	public:
		virtual ~idle_component() override { }

		// Interface
		virtual void start() override;

		// Static
		static ptr create() { return ptr(new idle_component()); }
		static ptr create(const eja::idle& idle) { return ptr(new idle_component(idle)); }
		static ptr create(const size_t delay) { return ptr(new idle_component(delay)); }
	};

	// Container
	derived_type(idle_list_component, list_component<idle>);
	derived_type(idle_map_component, map_component<std::string, idle>);
	derived_type(idle_queue_component, queue_component<idle>);
	derived_type(idle_vector_component, vector_component<idle>);

	derived_type(idle_entity_list_component, entity_list_component);
	derived_type(idle_entity_map_component, entity_map_component);
	derived_type(idle_entity_queue_component, entity_queue_component);
	derived_type(idle_entity_vector_component, entity_vector_component);
}

#endif
