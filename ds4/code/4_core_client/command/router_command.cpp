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
#include "command/handshake_command.h"
#include "command/join_command.h"
#include "command/ping_command.h"
#include "command/room_command.h"
#include "command/router_command.h"
#include "command/info_command.h"
#include "command/upload_command.h"

#include "component/session/session_component.h"

#include "data/response/download_response_data.h"
#include "data/response/handshake_response_data.h"
#include "data/response/info_response_data.h"
#include "data/response/join_response_data.h"
#include "data/response/ping_response_data.h"
#include "data/response/room_response_data.h"
#include "data/response/upload_response_data.h"

namespace eja
{	
	// Interface
	void router_command::execute(const router_request_message::ptr& request_message) const
	{
		// Validate
		if (unlikely(!validate(request_message)))
			return;

		// Unpack		
		const auto unpacker = unpack(request_message);
		if (unlikely(unpacker.empty()))
			return;

		// Loop
		//assert(unpacker->size() == 1);

		for (const auto& data : unpacker)
		{
			switch (data->get_type())
			{				
				default:
				{
					assert(false);
				}
			}
		}
	}

	void router_command::execute(const router_response_message::ptr& response_message) const
	{
		// Validate
		if (unlikely(!validate(response_message)))
			return;

		// Unpack		
		const auto unpacker = unpack(response_message);
		if (unlikely(unpacker.empty()))
			return;

		// Loop
		//assert(unpacker->size() == 1);

		for (const auto& data : unpacker)
		{
			switch (data->get_type())
			{
				case data_type::handshake_response:
				{
					// Response
					const auto response_data = std::static_pointer_cast<handshake_response_data>(data);
					
					// Command
					handshake_command cmd(m_entity);
					cmd.execute(response_message, response_data);

					break;
				}				
				case data_type::join_response:
				{					
					// Response					
					const auto response_data = std::static_pointer_cast<join_response_data>(data);
					
					// Command
					join_command cmd(m_entity);
					cmd.execute(response_message, response_data);

					break;
				}
				case data_type::info_response:
				{
					// Data
					const auto response_data = std::static_pointer_cast<info_response_data>(data);

					// Command
					info_command cmd(m_entity);
					cmd.execute(response_message, response_data);

					break;
				}
				case data_type::ping_response:
				{
					// Response					
					const auto response_data = std::static_pointer_cast<ping_response_data>(data);

					// Command
					ping_command cmd(m_entity);
					cmd.execute(response_message, response_data);

					break;
				}
				case data_type::room_response:
				{
					// Response
					const auto response_data = std::static_pointer_cast<room_response_data>(data);

					// Command
					room_command cmd(m_entity);
					cmd.execute(response_message, response_data);

					break;
				}
				default:
				{
					assert(false);
				}
			}
		}
	}

	void router_command::execute(const entity::ptr& entity, const router_response_message::ptr& response_message) const
	{
		// Validate
		if (unlikely(!validate(response_message)))
			return;

		// Unpack		
		const auto unpacker = unpack(response_message);
		if (unlikely(unpacker.empty()))
			return;

		// Loop
		//assert(unpacker->size() == 1);

		for (const auto& data : unpacker)
		{
			switch (data->get_type())
			{
				case data_type::handshake_response:
				{
					// Response
					const auto response_data = std::static_pointer_cast<handshake_response_data>(data);

					// Command
					handshake_command cmd(entity);
					cmd.execute(response_message, response_data);

					break;
				}
				case data_type::download_response:
				{
					// Data
					const auto response_data = std::static_pointer_cast<download_response_data>(data);

					// Command
					download_command cmd(m_entity);
					cmd.execute(entity, response_message, response_data);

					break;
				}				
				case data_type::upload_response:
				{
					// Data
					const auto response_data = std::static_pointer_cast<upload_response_data>(data);

					// Command
					upload_command cmd(m_entity);
					cmd.execute(response_message, response_data);

					break;
				}
				default:
				{
					assert(false);
				}
			}
		}
	}

	// Utility
	bool router_command::validate(const router_request_message::ptr& request_message) const
	{
		return request_message->has_data();
	}

	bool router_command::validate(const router_response_message::ptr& response_message) const
	{
		return response_message->has_data();
	}
}
