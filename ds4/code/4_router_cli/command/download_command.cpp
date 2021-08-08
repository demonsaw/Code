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

#include "command/download_command.h"

#include "component/router/router_component.h"
#include "component/time/timeout_component.h"
#include "component/transfer/chunk_component.h"
#include "component/transfer/transfer_component.h"

#include "data/request/download_request_data.h"
#include "data/response/download_response_data.h"
#include "entity/entity.h"
#include "factory/router_factory.h"

namespace eja
{
	// Interface
	download_response_data_list::ptr download_command::execute(const entity::ptr& entity, const router_request_message::ptr& request_message, const download_request_data::ptr& request_data) const
	{
		// Validate
		if (unlikely(!validate(request_message, request_data)))
			return nullptr;

		switch (request_data->get_mode())
		{
			case download_request_data::none:
			{
				// Validate
				if (!request_data->has_chunks())
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

						const auto transfer = transfer_entity->get<transfer_component>();
						transfer->set_id(request_data->get_id());
						transfer->set_running(true);

						const auto pair = std::make_pair(request_data->get_id(), transfer_entity);
						transfer_map->insert(pair);
					}
				}

				// Chunks
				auto chunks = request_data->get_chunks();
				const auto chunk_list = transfer_entity->get<chunk_list_component>();
				{
					const auto chunk_map = transfer_entity->get<chunk_map_component>();

					thread_lock(chunk_list);

					// Input
					for (auto it = chunk_list->begin(); it != chunk_list->end(); /*++it*/)
					{
						const auto& chunk = *it;
						if (!chunks.erase(chunk))
							chunk_list->erase(it++);
						else
							++it;
					}

					// Output
					for (auto it = chunk_map->begin(); it != chunk_map->end(); /*++it*/)
					{
						const auto& chunk = it->first;
						if (!chunks.erase(chunk))
							chunk_map->erase(it++);
						else
							++it;
					}

					// Add
					chunk_list->insert(chunk_list->end(), chunks.begin(), chunks.end());

					// Copy
					if (!chunk_map->empty())
					{
						// Timeout
						const auto timeout = transfer_entity->get<timeout_component>();
						timeout->update();

						// Copy
						const auto response_data_list = download_response_data_list::create();
						std::transform(chunk_map->begin(), chunk_map->end(), std::back_inserter(*response_data_list), [](chunk_map_component::value_type& pair) {return pair.second; });
						chunk_map->clear();

						return response_data_list;
					}
				}

				break;
			}
			case download_request_data::remove:
			{
				// Transfer
				entity::ptr transfer_entity;
				const auto transfer_map = m_entity->get<transfer_map_component>();
				{
					thread_lock(transfer_map);
					const auto it = transfer_map->find(request_data->get_id());
					if (it != transfer_map->end())
						transfer_entity = it->second;
				}

				if (transfer_entity)
				{
					// Transfer
					const auto transfer = transfer_entity->get<transfer_component>();
					transfer->set_running(false);

					// Chunk
					const auto chunk_map = transfer_entity->get<chunk_map_component>();
					const auto chunk_list = transfer_entity->get<chunk_list_component>();
					{
						thread_lock(chunk_list);
						chunk_list->clear();
						chunk_map->clear();
					}					
				}

				break;
			}
		}		

		return nullptr;
	}

	// Utility
	bool download_command::validate(const router_request_message::ptr& request_message, const download_request_data::ptr& request_data) const
	{
		return request_data->has_id();
	}
}
