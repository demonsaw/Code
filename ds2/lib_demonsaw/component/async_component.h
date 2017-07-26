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

#ifndef _EJA_ASYNC_COMPONENT_
#define _EJA_ASYNC_COMPONENT_

#include <memory>

#include "component.h"
#include "entity/entity.h"
#include "object/async.h"
#include "system/type.h"

namespace eja
{
	// Component
	class async_component : public async, public component, public std::enable_shared_from_this<async_component>
	{
	public:
		using ptr = std::shared_ptr<async_component>;

	protected:
		async_component() { }

	public:
		virtual ~async_component() override { }

		// Interface
		virtual void start();
		virtual void restart();
		virtual void stop() { }
	};

	// Container
	derived_type(async_list_component, list_component<async>);
	derived_type(async_map_component, map_component<std::string, async>);
	derived_type(async_queue_component, queue_component<async>);
	derived_type(async_vector_component, vector_component<async>);

	derived_type(async_entity_list_component, entity_list_component);
	derived_type(async_entity_map_component, entity_map_component);
	derived_type(async_entity_queue_component, entity_queue_component);
	derived_type(async_entity_vector_component, entity_vector_component);
}

#endif
