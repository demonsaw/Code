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

#ifndef _EJA_ROUTER_COMMAND_H_
#define _EJA_ROUTER_COMMAND_H_

#include <memory>

#include "component/session/session_component.h"

#include "data/data_packer.h"
#include "data/data_unpacker.h"
#include "entity/entity_type.h"
#include "message/request/router_request_message.h"
#include "message/response/router_response_message.h"
#include "system/type.h"

namespace eja
{
	class entity;
	class http_socket;
	class message_packer;
	enum class router_acceptor_state;
	class session_component;

	class router_command final : public entity_type
	{
		make_factory(router_command);

	private:
		std::shared_ptr<session_component> m_session;
		router_acceptor_state m_state;

	private:
		// Utility
		template <typename T>
		data_unpacker unpack(const std::shared_ptr<T>& t) const;

		bool validate(const std::shared_ptr<entity>& entity, const std::shared_ptr<router_request_message>& request_message) const;

	public:
		router_command();
		router_command(const std::shared_ptr<entity>& entity);

		// Interface
		std::shared_ptr<router_response_message> execute(const std::shared_ptr<entity>& entity, const std::shared_ptr<http_socket>& socket, const std::shared_ptr<router_request_message>& request_message);

		// Has
		bool has_session() const { return m_session != nullptr; }
		bool has_state() const;

		// Get
		template <typename T>
		std::shared_ptr<router_request_message> get_request_message(const T& t) const;

		template <typename T>
		std::shared_ptr<router_response_message> get_response_message(const T& t) const;

		std::shared_ptr<session_component> get_session() const { return m_session; }
		router_acceptor_state get_state() const { return m_state; }

		// Static
		static ptr create(const std::shared_ptr<entity>& entity) { return std::make_shared<router_command>(entity); }
	};

	// Utility
	template <typename T>
	data_unpacker router_command::unpack(const std::shared_ptr<T>& t) const
	{
		assert(t->has_data());

		data_unpacker unpacker;
		const auto& data = t->get_data();
		unpacker.unpack(data->data(), data->size());

		return unpacker;
	}

	// Get
	template <typename T>
	std::shared_ptr<router_request_message> router_command::get_request_message(const T& t) const
	{
		// Data
		data_packer packer(t);
		/*const*/ auto data = packer.pack();

		// Response
		const auto request_message = router_request_message::create();
		request_message->set_data(std::move(data));

		return request_message;
	}

	template <typename T>
	std::shared_ptr<router_response_message> router_command::get_response_message(const T& t) const
	{
		// Data
		data_packer packer(t);
		/*const*/ auto data = packer.pack();

		// Response
		const auto response_message = router_response_message::create();
		response_message->set_data(std::move(data));

		return response_message;
	}
}

#endif
