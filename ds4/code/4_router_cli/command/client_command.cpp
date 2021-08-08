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

#include "command/client_command.h"

#include "component/mute_component.h"
#include "component/client/client_component.h"
#include "component/router/router_acceptor_component.h"

#include "entity/entity.h"
#include "security/filter/exclusive.h"
#include "utility/value.h"

namespace eja
{
	// Interface
	void client_command::execute(const entity::ptr& entity, const client_response_message::ptr& response_message) const
	{
		// Validate
		if (unlikely(!validate(entity, response_message)))
			return;

		// Group
		const auto group_entity = get_group(entity);

		if (likely(group_entity))
		{
			// Message
			const auto message_map = group_entity->get<message_map_component>();
			{
				const auto acceptor = m_entity->get<router_acceptor_component>();

				// Find
				thread_lock(message_map);
				const auto it = message_map->find(response_message->get_id());
				if (it != message_map->end())
				{					
					const auto& e = it->second;
					const auto owner = e->get_owner();

					if (owner)
					{
						// XOR: Don't really need this
						//const auto c = owner->get<client_component>();
						//const auto client = entity->get<client_component>();
						///*const*/ auto idx = exclusive::compute(client->get_id(), c->get_seed());
						//response_message->set_origin(std::move(idx));

						const auto client = entity->get<client_component>();
						response_message->set_origin(client->get_id());

						// Write
						acceptor->async_write(owner, response_message);
					}					
				}
			}
		}
	}

	// Utility
	bool client_command::validate(const entity::ptr& entity, const client_response_message::ptr& response_message) const
	{
		// Id
		const auto& id = response_message->get_id();
		if (unlikely(id.empty() || (id.size() > io::max_id_size)))
			return false;

		// Clients
		if (unlikely(response_message->has_clients()))
			return false;

		// Origin
		if (unlikely(response_message->has_origin()))
			return false;

		// Data
		return likely(response_message->has_data());
	}
}
