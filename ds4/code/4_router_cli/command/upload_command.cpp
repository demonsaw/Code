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

#include "command/upload_command.h"

#include "component/router/router_component.h"
#include "component/transfer/chunk_component.h"
#include "component/transfer/transfer_component.h"

#include "data/request/upload_request_data.h"
#include "data/response/upload_response_data.h"
#include "entity/entity.h"
#include "factory/router_factory.h"

namespace eja
{
	// Interface
	upload_response_data::ptr upload_command::execute(const entity::ptr& entity, const router_request_message::ptr& request_message, const upload_request_data::ptr& request_data) const
	{
		// Validate
		if (unlikely(!validate(request_message, request_data)))
			return nullptr;

		// Transfer
		entity::ptr transfer_entity;
		const auto transfer_map = m_entity->get<transfer_map_component>();
		{
			thread_lock(transfer_map);
			const auto it = transfer_map->find(request_data->get_id());
			if (it != transfer_map->end())
			{
				transfer_entity = it->second;
			}
			else
			{
				transfer_entity = router_factory::create_transfer();

				// Chunk
				const auto chunk_list = transfer_entity->get<chunk_list_component>();
				chunk_list->push_back(0);

				// Transfer
				const auto transfer = transfer_entity->get<transfer_component>();
				transfer->set_id(request_data->get_id());
				transfer->set_running(true);

				const auto pair = std::make_pair(request_data->get_id(), transfer_entity);
				transfer_map->insert(pair);
			}
		}

		const auto transfer = transfer_entity->get<transfer_component>();

		if (transfer->is_running())
		{
			const auto chunk_list = transfer_entity->get<chunk_list_component>();

			thread_lock(chunk_list);

			// Input
			if (request_data->has_data())
			{
				const auto chunk = request_data->get_data();
				const auto sequence = chunk->get_sequence();

				auto it = std::find(chunk_list->rbegin(), chunk_list->rend(), sequence);

				if (it != chunk_list->rend())
				{
					chunk_list->erase(std::next(it).base());

					const auto pair = std::make_pair(sequence, chunk);
					const auto chunk_map = transfer_entity->get<chunk_map_component>();
					chunk_map->insert(pair);

					// Buffer
					const auto router = m_entity->get<router_component>();
					if (chunk_map->size() >= router->get_buffer_size())
						return nullptr;
				}
			}

			// Output
			if (!chunk_list->empty())
			{
				const auto sequence = chunk_list->front();
				chunk_list->pop_front();
				chunk_list->push_back(sequence);

				// Response
				const auto response_data = upload_response_data::create();
				response_data->set_sequence(sequence);

				return response_data;
			}
		}
		else
		{
			// Stop
			return upload_response_data::create();
		}

		return nullptr;		
	}

	// Utility
	bool upload_command::validate(const router_request_message::ptr& request_message, const upload_request_data::ptr& request_data) const
	{
		return request_data->has_id();
	}
}
