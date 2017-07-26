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

#ifndef _EJA_ROUTER_COMPONENT_
#define _EJA_ROUTER_COMPONENT_

#include "component/component.h"
#include "entity/entity.h"
#include "object/router/router.h"
#include "system/type.h"

namespace eja
{
	// Type
	enum class router_type { message, transfer, proxy };

	// Component
	derived_id_component(router_component, router);

	// Container
	derived_type(router_list_component, list_component<router>);
	derived_type(router_map_component, map_component<std::string, router>);
	derived_type(router_queue_component, queue_component<router>);
	derived_type(router_vector_component, vector_component<router>);

	derived_type(router_entity_list_component, entity_list_component);
	derived_type(router_entity_map_component, entity_map_component);
	derived_type(router_entity_queue_component, entity_queue_component);
	derived_type(router_entity_vector_component, entity_vector_component);
}

#endif
