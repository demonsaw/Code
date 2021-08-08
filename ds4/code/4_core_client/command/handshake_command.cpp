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

#include "command/handshake_command.h"
#include "command/router_command.h"

#include "component/session/session_component.h"
#include "component/session/session_option_component.h"

#include "data/request/handshake_request_data.h"
#include "data/response/handshake_response_data.h"
#include "entity/entity.h"
#include "security/security.h"
#include "security/cipher/cipher.h"
#include "utility/value.h"

namespace eja
{
	// Interface
	router_request_message::ptr handshake_command::execute() const
	{
		const auto option = m_entity->get<session_option_component>();
		return execute(option->get_prime_size());
	}

	router_request_message::ptr handshake_command::execute(const size_t prime_size) const
	{
		// TODO: Replace with prime cache
		//
		//
		// Key Agreement
		const auto session = m_entity->get<session_component>();
		auto& dh = session->get_algorithm();
		dh.init(prime_size);

		// Prime
		const auto request_data = handshake_request_data::create();
		request_data->set_prime(security::str(dh.get_p()));
		request_data->set_generator(dh.get_g().ConvertToLong());
		request_data->set_public_key(security::str(dh.get_public_key()));

		// Cipher
		const auto option = m_entity->get<session_option_component>();
		request_data->set_cipher(option->get_cipher());
		request_data->set_key_size(static_cast<u32>(option->get_key_size()));

		// Hash
		request_data->set_iterations(static_cast<u32>(option->get_iterations()));
		request_data->set_hash(option->get_hash());
		request_data->set_salt(option->get_salt());

		// Command
		router_command cmd(m_entity);
		return cmd.get_request_message(request_data);
	}

	void handshake_command::execute(const router_response_message::ptr& response_message, const handshake_response_data::ptr& response_data) const
	{
		// Validate
		if (unlikely(!validate(response_message, response_data)))
			return;

		// Id
		const auto session = m_entity->get<session_component>();
		session->set_id(response_data->get_id());

		// TODO: Replace with prime cache
		//
		//
		// Diffie Hellman
		auto& dh = session->get_algorithm();
		dh.compute(response_data->get_public_key());

		// Cipher
		const auto option = m_entity->get<session_option_component>();
		const auto cipher = option->create_cipher(dh.get_shared_key());
		session->set_cipher(cipher);

		// Clear
		dh.clear();
	}

	// Utility
	bool handshake_command::validate(const router_response_message::ptr& response_message, const handshake_response_data::ptr& response_data) const
	{
		return response_data->valid();
	}
}
