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

#include "command/browse_command.h"
#include "command/chat_command.h"
#include "command/client_command.h"
#include "command/search_command.h"
#include "command/transfer_command.h"

#include "data/request/browse_request_data.h"
#include "data/request/chat_request_data.h"
#include "data/request/search_request_data.h"
#include "data/request/transfer_request_data.h"

#include "data/response/browse_response_data.h"
#include "data/response/search_response_data.h"
#include "data/response/transfer_response_data.h"

#include "entity/entity.h"
#include "message/message_packer.h"
#include "utility/value.h"

namespace eja
{
	// Interface
	client_response_message::ptr client_command::execute(const client_request_message::ptr& request_message) const
	{
		// Validate
		if (unlikely(!validate(request_message)))
			return nullptr;

		// Unpack		
		const auto unpacker = unpack(request_message);
		if (unlikely(unpacker.empty()))
			return nullptr;

		// Loop		
		//assert(unpacker.size() == 1); 
		data_packer packer;		

		for (const auto& data : unpacker)
		{
			switch (data->get_type())
			{
				case data_type::chat_request:
				{
					// Data
					const auto request_data = std::static_pointer_cast<chat_request_data>(data);

					// Command
					chat_command cmd(m_entity);
					cmd.execute(request_message, request_data);

					break;
				}
				case data_type::browse_request:
				{
					// Data
					const auto request_data = std::static_pointer_cast<browse_request_data>(data);

					// Command
					browse_command cmd(m_entity);
					cmd.set_share_list(m_share_list);
					const auto response_data = cmd.execute(request_message, request_data);
					if (response_data)
						packer.push_back(response_data);

					break;
				}				
				case data_type::search_request:
				{
					// Data
					const auto request_data = std::static_pointer_cast<search_request_data>(data);

					// Command
					search_command cmd(m_entity);
					const auto response_data = cmd.execute(request_message, request_data);
					if (response_data)
						packer.push_back(response_data);

					break;
				}
				case data_type::transfer_request:
				{
					// Data
					const auto request_data = std::static_pointer_cast<transfer_request_data>(data);

					// Command
					transfer_command cmd(m_entity);
					const auto response_data = cmd.execute(request_message, request_data);
					if (response_data)
						packer.push_back(response_data);

					break;
				}
				default:
				{
					assert(false);
				}
			}
		}

		// Pack
		if (!packer.empty())
		{
			// Response
			const auto response_message = client_response_message::create();
			response_message->set_id(request_message->get_id());

			/*const*/ auto data = pack(packer);
			response_message->set_data(std::move(data));

			return response_message;
		}

		return nullptr;
	}

	void client_command::execute(const client_response_message::ptr& response_message) const
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

		for (const auto& packed_data : unpacker)
		{
			switch (packed_data->get_type())
			{
				case data_type::browse_response:
				{
					// Data
					const auto response_data = std::static_pointer_cast<browse_response_data>(packed_data);

					// Command
					browse_command cmd(m_entity);
					cmd.execute(response_message, response_data);

					break;
				}
				case data_type::search_response:
				{
					// Data
					const auto response_data = std::static_pointer_cast<search_response_data>(packed_data);

					// Command
					search_command cmd(m_entity);
					cmd.execute(response_message, response_data);

					break;
				}
				case data_type::transfer_response:
				{
					// Data
					const auto response_data = std::static_pointer_cast<transfer_response_data>(packed_data);

					// Command
					transfer_command cmd(m_entity);
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
	std::string client_command::pack(const data_packer& packer) const
	{
		// Data
		const auto packed_data = packer.pack();

		// Group
		const auto group = m_entity->get<group_component>();
		return group->invalid() ? packed_data : group->encrypt(packed_data);
	}

	bool client_command::validate(const client_request_message::ptr& request_message) const
	{
		// Id
		const auto& id = request_message->get_id();
		if (unlikely(id.size() > io::max_id_size))
			return false;

		// Clients
		if (unlikely(request_message->has_clients()))
			return false;

		// Origin
		const auto& origin = request_message->get_origin();
		if (unlikely(origin.size() > io::max_id_size))
			return false;

		// Room
		const auto& room = request_message->get_room();
		if (unlikely(room.size() > io::max_id_size))
			return false;

		// Data
		return likely(request_message->has_data());
	}

	bool client_command::validate(const client_response_message::ptr& response_message) const
	{
		// Id
		const auto& id = response_message->get_id();
		if (unlikely(id.empty() || (id.size() > io::max_id_size)))
			return false;

		// Clients
		if (unlikely(response_message->has_clients()))
			return false;

		// Origin
		const auto& origin = response_message->get_origin();
		if (unlikely(origin.size() > io::max_id_size))
			return false;

		return true;
	}
}
