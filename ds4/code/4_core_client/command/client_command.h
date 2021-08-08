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

#ifndef _EJA_CLIENT_COMMAND_H_
#define _EJA_CLIENT_COMMAND_H_

#include <memory>

#include "component/group/group_component.h"

#include "data/data_packer.h"
#include "data/data_unpacker.h"
#include "entity/entity_command.h"
#include "message/request/client_request_message.h"
#include "message/response/client_response_message.h"
#include "system/type.h"

namespace eja
{
	class browse_request_data;
	class browse_response_data;
	class chat_request_data;
	class entity;
	class share_list_component;

	class client_command final : public entity_command
	{
		make_factory(client_command);

	private:
		std::shared_ptr<share_list_component> m_share_list;

	private:
		// Utility
		template <typename T>
		std::string pack(const std::shared_ptr<T>& data) const;
		std::string pack(const data_packer& packer) const;

		template <typename T>
		data_unpacker unpack(const std::shared_ptr<T>& message) const;

		bool validate(const std::shared_ptr<client_request_message>& request_message) const;
		bool validate(const std::shared_ptr<client_response_message>& response_message) const;

	public:
		client_command() { }
		client_command(const std::shared_ptr<entity>& entity) : entity_command(entity) { }

		// Interface
		std::shared_ptr<client_response_message> execute(const std::shared_ptr<client_request_message>& request_message) const;
		void execute(const std::shared_ptr<client_response_message>& response_message) const;

		template <typename T>
		std::shared_ptr<client_response_message> execute(const std::shared_ptr<client_request_message>& request_message, const std::shared_ptr<T>& response_data) const;

		// Set
		void set_share_list(const std::shared_ptr<share_list_component>& share_list) { m_share_list = share_list; }

		// Static
		static ptr create(const std::shared_ptr<entity>& entity) { return std::make_shared<client_command>(entity); }
	};

	// Interface
	template <typename T>
	client_response_message::ptr client_command::execute(const client_request_message::ptr& request_message, const std::shared_ptr<T>& response_data) const
	{
		// Response
		const auto response_message = client_response_message::create();
		response_message->set_id(request_message->get_id());

		// Data
		/*const*/ auto packed_data = pack(response_data);
		response_message->set_data(std::move(packed_data));

		return response_message;
	}

	template <typename T>
	data_unpacker client_command::unpack(const std::shared_ptr<T>& message) const
	{
		assert(message->has_data());

		// Data
		data_unpacker unpacker;
		const auto& unpacked_data = message->get_data();

		// Group
		const auto group = m_entity->get<group_component>();
		if (group->invalid())
		{
			unpacker.unpack(unpacked_data->data(), unpacked_data->size());
		}
		else
		{
			const auto decrypted_data = group->decrypt(unpacked_data);
			unpacker.unpack(decrypted_data.data(), decrypted_data.size());
		}

		return unpacker;
	}

	// Utility
	template <typename T>
	std::string client_command::pack(const std::shared_ptr<T>& data) const
	{
		data_packer packer(data);

		return pack(packer);
	}
}

#endif
