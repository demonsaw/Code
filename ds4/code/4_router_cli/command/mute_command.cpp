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

#include "command/mute_command.h"

#include "component/mute_component.h"
#include "component/client/client_component.h"

#include "entity/entity.h"
#include "security/filter/exclusive.h"

namespace eja
{
	// Interface
	void mute_command::execute(const entity::ptr& entity, const router_request_message::ptr& request_message, const mute_request_data::ptr& request_data) const
	{
		// Validate
		if (unlikely(!validate(request_message, request_data)))
			return;

		// Client
		const auto client = entity->get<client_component>();

		// Group
		const auto group_entity = get_group(entity);
		if (likely(group_entity))
		{
			const auto mute_set = entity->get<mute_set_component>();
			const auto client_map = group_entity->get<client_map_component>();			

			for (const auto& id : request_message->get_clients())
			{
				// XOR: Don't really need this
				//const auto idx = exclusive::compute(id, client->get_seed());

				entity::ptr e;				
				{
					thread_lock(client_map);
					const auto it = client_map->find(id);
					if (it != client_map->end())
						e = it->second;
				}

				if (e)
				{
					switch (request_data->get_mode())
					{
						case mute_request_data::add:
						{
							{
								thread_lock(mute_set);
								mute_set->insert(e);
							}

							break;
						}
						case mute_request_data::remove:
						{
							{
								thread_lock(mute_set);
								mute_set->erase(e);
							}

							break;
						}
						default:
						{
							assert(false);
						}
					}
				}
			}
		}
	}

	// Utility
	bool mute_command::validate(const router_request_message::ptr& request_message, const mute_request_data::ptr& request_data) const
	{		
		return request_data->valid();
	}
}
