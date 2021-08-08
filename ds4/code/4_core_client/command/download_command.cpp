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

#include <thread>

#include "command/router_command.h"
#include "command/download_command.h"

#include "component/callback/callback.h"
#include "component/callback/callback_service_component.h"
#include "component/client/client_component.h"
#include "component/group/group_component.h"
#include "component/io/file_component.h"
#include "component/status/status_component.h"
#include "component/router/router_component.h"
#include "component/time/timeout_component.h"
#include "component/transfer/chunk_component.h"
#include "component/transfer/transfer_component.h"

#include "data/chunk_data.h"
#include "data/response/download_response_data.h"
#include "data/request/quit_request_data.h"
#include "entity/entity.h"
#include "factory/client_factory.h"
#include "http/http_throttle.h"
#include "utility/value.h"
#include "utility/io/file_util.h"

namespace eja
{
	// Interface
	router_request_message::ptr download_command::execute(const entity::ptr& entity, const size_t mode /*= download_request_data::none*/) const
	{
		const auto transfer = m_entity->get<transfer_component>();

		switch (mode)
		{
			case download_request_data::none:
			{
				// Data
				const auto router_chunk_list = entity->get<chunk_list_component>();
				auto free_size = transfer->get_buffer_size() - router_chunk_list->size();

				if (free_size)
				{
					const auto chunk_list = m_entity->get<chunk_list_component>();
					const auto chunk_set = m_entity->get<chunk_set_component>();

					// 1) Remove
					router_chunk_list->erase(std::remove_if(router_chunk_list->begin(), router_chunk_list->end(), [&](const u32 sequence)
					{
						return (chunk_set->find(sequence) == chunk_set->end());
					}), router_chunk_list->end());

					// 2) Copy (unassigned)
					if (!chunk_list->empty())
					{
						auto end = chunk_list->begin();
						const auto i = std::min(free_size, chunk_list->size());
						std::advance(end, i);

						// Assigned
						for (auto it = chunk_list->begin(); it != end; ++it)
							chunk_set->insert(*it);

						// Router
						router_chunk_list->splice(router_chunk_list->end(), *chunk_list, chunk_list->begin(), end);
						free_size -= i;

						assert(free_size < transfer->get_buffer_size());
					}

					// 3) Copy (assigned)
					if (free_size && (router_chunk_list->size() < chunk_set->size()))
					{
						std::vector<u32> items;
						free_size = std::min((chunk_set->size() - router_chunk_list->size()), free_size);
						items.reserve(free_size);

						size_t i = 0;
						for (auto it = chunk_set->rbegin(); it != chunk_set->rend(); ++it)
						{
							const auto& sequence = *it;
							if (std::find(router_chunk_list->rbegin(), router_chunk_list->rend(), sequence) == router_chunk_list->rend())
							{
								items.push_back(sequence);
								if (++i >= free_size)
									break;
							}
						}

						assert(!items.empty());

						// Router
						std::random_shuffle(items.begin(), items.end());
						std::copy(items.begin(), items.end(), std::back_inserter(*router_chunk_list));
					}

					// DEBUG
					/*for (const auto& sequence : *router_chunk_list)
					std::cout << sequence << ",";
					std::cout << std::endl;*/
				}
				/*else
				{
				std::cout << "NO FREE SIZE" << std::endl;
				}*/

				if (!router_chunk_list->empty())
				{
					// 3) Copy
					const auto request_data = download_request_data::create();
					request_data->set_id(transfer->get_id());

					request_data->set_chunks(*router_chunk_list);
					assert(request_data->has_chunks());

					// 4) Request
					router_command cmd(m_entity);
					return cmd.get_request_message(request_data);
				}

				break;
			}
			case download_request_data::remove:
			{
				// Packer
				data_packer packer;

				// Download
				{
					const auto request_data = download_request_data::create();					
					request_data->set_id(transfer->get_id());
					request_data->set_remove(true);

					packer.push_back(request_data);
				}

				// Quit
				{
					const auto request_data = quit_request_data::create();

					packer.push_back(request_data);
				}				

				// Request
				const auto request_message = router_request_message::create();				
				/*const*/ auto data = packer.pack();
				request_message->set_data(std::move(data));

				return request_message;
			}
			default:
			{
				assert(false);
				break;
			}
		}
		
		return nullptr;
	}

	void download_command::execute(const entity::ptr& entity, const router_response_message::ptr& response_message, const download_response_data::ptr& response_data) const
	{
		// Validate
		if (unlikely(!validate(response_message, response_data)))
			return;
		
		// Chunk
		const auto cdata = response_data->get_chunk();
		const auto sequence = cdata->get_sequence();

		// List	
		const auto chunk_list = entity->get<chunk_list_component>();
		chunk_list->remove(sequence);

		// Set
		const auto chunk_set = m_entity->get<chunk_set_component>();
		if (chunk_set->erase(sequence))
		{
			// Group
			const auto owner = m_entity->get_owner();
			const auto group = owner->get<group_component>();

			if (group->valid())
			{
				/*const*/ auto decrypted = group->decrypt(cdata->get_data());
				cdata->set_data(std::move(decrypted));
			}

			// Throttle
			const auto throttle = http_download_throttle::get();
			if (throttle->enabled())
				throttle->add(cdata->get_size());

			u64 bytes = 0;
			const auto transfer = m_entity->get<transfer_component>();
			const auto offset = static_cast<u64>(sequence) * static_cast<u64>(transfer->get_chunk_size());

			const auto file = m_entity->get<file_component>();
			const auto timeout = m_entity->get<timeout_component>();

			do
			{
				try
				{
					bytes = file_util::write(file, cdata->get_data(), offset);

					if (bytes)
					{
						// Update
						transfer->update(cdata->get_size());
					}
					else
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds::file));
					}
				}
				catch (const std::exception& ex)
				{
					const auto status = m_entity->get<status_component>();
					status->set_warning();

					owner->log(ex);

					std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds::file));
				}
			} while (!bytes && transfer->is_running() && !timeout->expired(download::timeout));

			// Callback
			owner->async_call(callback::download | callback::update, m_entity);
		}
	}

	// Utility
	bool download_command::validate(const router_response_message::ptr& response_message, const download_response_data::ptr& response_data) const
	{
		return response_data->valid();
	}
}
