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

#ifndef _EJA_SOCKET_COMPONENT_
#define _EJA_SOCKET_COMPONENT_

#include "component.h"
#include "entity/entity.h"
#include "http/http_socket.h"
#include "object/tunnel.h"
#include "system/type.h"

namespace eja
{
	// Component
	derived_value_component(tunnel_component, http_socket::ptr, tunnel);

	// Container
	derived_type(tunnel_list_component, list_component<tunnel>);
	derived_type(tunnel_map_component, map_component<std::string, tunnel>);
	derived_type(tunnel_queue_component, queue_component<tunnel>);
	derived_type(tunnel_vector_component, vector_component<tunnel>);

	derived_type(tunnel_entity_list_component, entity_list_component);
	derived_type(tunnel_entity_map_component, entity_map_component);
	derived_type(tunnel_entity_queue_component, entity_queue_component);
	derived_type(tunnel_entity_vector_component, entity_vector_component);
}

#endif
