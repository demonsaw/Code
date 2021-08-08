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

#include "command/acceptor_command.h"

#include "component/room_component.h"
#include "component/group/group_component.h"

#include "entity/entity.h"

namespace eja
{
	// Get
	entity::ptr acceptor_command::get_group(const entity::ptr& entity) const
	{
		const auto group = entity->get<group_component>();
		return get_group(group->get_id());
	}

	entity::ptr acceptor_command::get_group(const std::string& id) const
	{
		entity::ptr e;
		{
			const auto group_map = m_entity->get<group_map_component>();

			thread_lock(group_map);
			const auto it = group_map->find(id);
			if (it != group_map->end())
				e = it->second;
		}

		return e;
	}

	entity::ptr acceptor_command::get_room(const entity::ptr& entity, const std::string& id) const
	{
		const auto room_map = entity->get<room_map_component>();
		{
			thread_lock(room_map);
			const auto it = room_map->find(id);

			return (it != room_map->end()) ? it->second : nullptr;
		}
	}
}
