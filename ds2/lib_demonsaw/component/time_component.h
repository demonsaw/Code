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

#ifndef _EJA_TIME_COMPONENT_
#define _EJA_TIME_COMPONENT_

#include <memory>
#include <string>

#include "component.h"
#include "entity/entity.h"
#include "system/type.h"
#include "object/time.h"

namespace eja
{
	// Component
	class time_component final : public time, public component
	{
	public:
		using ptr = std::shared_ptr<time_component>;

	private:
		time_component() { }

	public:
		virtual ~time_component() override { }

		// Interface
		virtual void init() override;
		virtual void shutdown() override;

		virtual std::string str() const override;

		// Static
		static ptr create() { return ptr(new time_component()); }
	};

	// Container
	derived_type(time_list_component, list_component<time>);
	derived_type(time_map_component, map_component<std::string, time>);
	derived_type(time_queue_component, queue_component<time>);
	derived_type(time_vector_component, vector_component<time>);

	derived_type(time_entity_list_component, entity_list_component);
	derived_type(time_entity_map_component, entity_map_component);
	derived_type(time_entity_queue_component, entity_queue_component);
	derived_type(time_entity_vector_component, entity_vector_component);
}

#endif
