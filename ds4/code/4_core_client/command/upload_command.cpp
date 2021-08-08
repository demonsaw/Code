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
#include "command/upload_command.h"

#include "component/callback/callback.h"
#include "component/group/group_component.h"
#include "component/io/file_component.h"
#include "component/status/status_component.h"
#include "component/time/timeout_component.h"
#include "component/transfer/chunk_component.h"
#include "component/transfer/transfer_component.h"

#include "data/request/upload_request_data.h"
#include "data/response/upload_response_data.h"
#include "entity/entity.h"
#include "utility/value.h"
#include "utility/io/file_util.h"

namespace eja
{
	// Interface
	void upload_command::execute() const 
	{
		const auto owner = m_entity->get_owner();		

		try
		{
			const auto file = m_entity->get<file_component>();
			const auto chunk = m_entity->get<chunk_component>();
			const auto transfer = m_entity->get<transfer_component>();
			const auto bytes = file_util::read(file, chunk->get_data(), transfer->get_chunk_size());

			if (bytes)
			{
				chunk->set_sequence(0);

				// Callback
				owner->async_call(callback::upload | callback::update, m_entity);
			}
		}
		catch (const std::exception& ex)
		{
			const auto status = m_entity->get<status_component>();
			status->set_warning();

			owner->log(ex);
		}
	}

	router_request_message::ptr upload_command::execute(const entity::ptr& entity) const
	{
		// Data
		const auto request_data = upload_request_data::create();
		const auto transfer = entity->get<transfer_component>();
		request_data->set_id(transfer->get_id());

		// Chunk
		const auto chunk = entity->get<chunk_component>();
		if (chunk->valid())
		{
			// Response
			const auto response_data = download_response_data::create();			
			chunk_data::ptr cdata;

			// Group
			const auto owner = m_entity->get_owner();
			const auto group = owner->get<group_component>();
			
			if (group->invalid())
			{
				cdata = chunk->get_chunk();	
			}
			else
			{
				cdata = chunk_data::create();
				cdata->set_sequence(chunk->get_sequence());

				/*const*/ auto encrypted = group->encrypt(chunk->get_data());
				cdata->set_data(std::move(encrypted));				
			}

			response_data->set_chunk(cdata);

			// Request
			request_data->set_data(response_data);
		}

		// Command
		router_command cmd(m_entity);
		return cmd.get_request_message(request_data);
	}

	void upload_command::execute(const router_response_message::ptr& response_message, const upload_response_data::ptr& response_data) const
	{
		// Validate
		if (unlikely(!validate(response_message, response_data)))
			return;

		if (response_data->has_sequence())
		{
			const auto chunk = m_entity->get<chunk_component>();
			chunk->set_sequence(response_data->get_sequence());

			u64 bytes = 0;
			const auto transfer = m_entity->get<transfer_component>();
			const u64 offset = response_data->get_sequence() * transfer->get_chunk_size();
			
			const auto owner = m_entity->get_owner();		
			const auto file = m_entity->get<file_component>();
			const auto timeout = m_entity->get<timeout_component>();

			do
			{
				try
				{
					bytes = file_util::read(file, chunk->get_data(), transfer->get_chunk_size(), offset);
					if (!bytes)
						std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds::file));
				}
				catch (const std::exception& ex)
				{
					const auto status = m_entity->get<status_component>();
					status->set_warning();

					owner->log(ex);

					std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds::file));
				}
			} 
			while (!bytes && transfer->is_running() && !timeout->expired(download::timeout));

			// Callback						
			owner->async_call(callback::upload | callback::update, m_entity);
		}
		else
		{
			// Invalid sequence means stop
			const auto transfer = m_entity->get<transfer_component>();
			transfer->finish();
		}			
	}

	// Utility
	bool upload_command::validate(const router_response_message::ptr& response_message, const upload_response_data::ptr& response_data) const
	{
		return true;
	}
}
