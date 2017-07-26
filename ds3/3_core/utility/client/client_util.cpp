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

#include "component/endpoint_component.h"
#include "component/client/client_service_component.h"
#include "entity/entity.h"
#include "utility/client/client_util.h"

namespace eja
{
	// Client
	entity::ptr client_util::get_client(const entity::ptr owner, const std::string& id)
	{
		const auto service = owner->get<client_service_component>();
		const auto message_list = service->get_message_list();
		{
			thread_lock(message_list);
			for (const auto& e : *message_list)
			{
				const auto client_map = e->get<client_map_component>();
				{
					thread_lock(client_map);
					const auto it = client_map->find(id);
					if (it != client_map->end())
						return it->second;
				}
			}
		}

		return nullptr;
	}

	// Router
	entity::ptr client_util::get_router(const entity::ptr owner, const std::string& id)
	{
		const auto service = owner->get<client_service_component>();
		const auto message_list = service->get_message_list();
		{
			thread_lock(message_list);
			for (const auto& e : *message_list)
			{
				const auto client_map = e->get<client_map_component>();
				{
					thread_lock(client_map);
					const auto it = client_map->find(id);
					if (it != client_map->end())
						return e;
				}
			}
		}

		return nullptr;
	}

	entity::ptr client_util::get_router(const entity::ptr owner, const entity::ptr entity)
	{
		const auto endpoint = entity->find<endpoint_component>();
		return get_router(owner, endpoint->get_id());
	}
}
