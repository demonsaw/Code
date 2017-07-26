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

#ifndef _EJA_SESSION_COMPONENT_
#define _EJA_SESSION_COMPONENT_

#include <memory>
#include <string>

#include "component.h"
#include "entity/entity.h"
#include "object/session.h"
#include "system/type.h"

namespace eja
{
	// Component
	derived_id_component(session_component, session);

	// Container
	derived_type(session_list_component, list_component<session>);
	derived_type(session_map_component, map_component<std::string, session>);
	derived_type(session_queue_component, queue_component<session>);
	derived_type(session_vector_component, vector_component<session>);

	derived_type(session_entity_list_component, entity_list_component);
	derived_type(session_entity_map_component, entity_map_component);
	derived_type(session_entity_queue_component, entity_queue_component);
	derived_type(session_entity_vector_component, entity_vector_component);
}

#endif
