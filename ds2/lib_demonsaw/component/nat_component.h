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

#ifndef _EJA_NAT_COMPONENT_
#define _EJA_NAT_COMPONENT_

#include "component.h"
#include "entity/entity.h"
#include "object/nat.h"
#include "system/type.h"

namespace eja
{
	// Component
	derived_value_component(nat_component, u16, nat);

	// Container
	derived_type(nat_list_component, list_component<nat>);
	derived_type(nat_map_component, map_component<std::string, nat>);
	derived_type(nat_queue_component, queue_component<nat>);
	derived_type(nat_vector_component, vector_component<nat>);

	derived_type(nat_entity_list_component, entity_list_component);
	derived_type(nat_entity_map_component, entity_map_component);
	derived_type(nat_entity_queue_component, entity_queue_component);
	derived_type(nat_entity_vector_component, entity_vector_component);
}

#endif
