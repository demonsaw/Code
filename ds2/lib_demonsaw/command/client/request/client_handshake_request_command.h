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

#ifndef _EJA_CLIENT_HANDSHAKE_REQUEST_COMMAND_
#define _EJA_CLIENT_HANDSHAKE_REQUEST_COMMAND_

#include <memory>

#include "command/client/client_command.h"
#include "component/session_component.h"
#include "component/router/router_component.h"
#include "entity/entity.h"
#include "json/json_packet.h"
#include "http/http_socket.h"
#include "http/http_status.h"
#include "message/request/handshake_request_message.h"
#include "message/response/handshake_response_message.h"
#include "security/base.h"
#include "security/diffie_hellman.h"

namespace eja
{
	template <router_type T>
	class client_handshake_request_command final : public client_command
	{
	public:
		using ptr = std::shared_ptr<client_handshake_request_command>;

	private:
		client_handshake_request_command(const entity::ptr entity, const http_socket::ptr socket) : client_command(entity, socket) { }

	public:
		virtual ~client_handshake_request_command() override { }

		// Utility
		virtual http_status execute();

		// Accessor
		size_t get_prime_size() const;
		size_t get_key_size() const;

		// Static
		static ptr create(const entity::ptr entity, const http_socket::ptr socket) { return ptr(new client_handshake_request_command(entity, socket)); }
	};

	// Utility
	template <router_type T>
	http_status client_handshake_request_command<T>::execute()
	{
		// Request
		const auto request_packet = json_packet::create();
		const auto request_message = handshake_request_message::create(request_packet);
		const auto request_key = request_message->get_key();

		// Version
		request_message->set_version(default_version::app);

		// Security
		const auto prime_size = get_prime_size();
		if (!prime_size)
			return http_code::bad_request;

		const auto key_size = get_key_size();
		if (!key_size)
			return http_code::bad_request;

		request_key->set_size(key_size);

		diffie_hellman dh(prime_size);
		const auto& dh_base = dh.get_base();
		request_key->set_base(dh_base);

		const auto& dh_prime = dh.get_prime();
		const auto dh_prime_encoded = base64::encode(dh_prime);
		request_key->set_prime(dh_prime_encoded);

		const auto& dh_public_key = dh.get_public_key();
		const auto key_encoded = base64::encode(dh_public_key);
		request_key->set_public_key(key_encoded);

		// Security
		const auto router_security = get_router_security();
		if (!router_security)
			return http_code::bad_request;

		// Json
		const auto json = request_packet->str();
		const auto json_encrypted = router_security->encrypt(json);
		const auto json_encoded = base64::encode(json_encrypted);

		// Http
		http_request request;
		if (has_session())
		{
			const auto session = get_session();
			if (session)
				request.set_session(session->get_id());
		}

		request.set_host(get_address());
		request.set_body(json_encoded);
		m_socket->write(request);

		http_response response;
		m_socket->read(response);

		// Response
		const auto& status = response.get_status();
		if (status.is_ok())
		{
			// Body
			const auto& body = response.get_body();
			if (body.empty())
				return http_code::not_found;

			const auto body_decoded = base64::decode(body);
			const auto body_decypted = router_security->decrypt(body_decoded);

			const auto response_packet = json_packet::create(body_decypted);
			const auto response_message = handshake_response_message::create(response_packet);

			// Version
			if (response_message->has_version())
			{
				const auto router = get_router();
				if (!router)
					return http_code::bad_request;

				router->set_version(response_message->get_version());
			}

			// Key
			const auto response_key = response_message->get_key();
			const auto response_public_key = response_key->get_public_key();
			const auto response_public_key_decoded = base64::decode(response_public_key);
			const bool status = dh.compute(response_public_key_decoded);
			if (!status)
				return http_code::internal_server_error;

			// Session
			const auto& session_id = response.get_session();
			if (session_id.empty())
				return http_code::internal_server_error;

			const auto session = session_component::create(session_id);
			m_entity->add(session);

			if (prime_size > default_security::min_prime_size)
			{
				const auto& dh_shared_key = dh.get_shared_key();
				const auto key = sha256()(dh_shared_key);
				const auto cipher = aes::create(key, key_size >> 3);
				session->set_cipher(cipher);
			}
			else
			{
				const auto& dh_shared_key = dh.get_shared_key();
				const auto cipher = aes::create(dh_shared_key);
				session->set_cipher(cipher);
			}
		}

		return status;
	}

	// Accessor
	template <router_type T>
	size_t client_handshake_request_command<T>::get_prime_size() const
	{
		if (T == router_type::message)
		{
			const auto client_security = get_client_security();
			if (client_security)
				return client_security->get_message_prime_size();
		}

		if (T == router_type::transfer)
		{
			const auto client_security = get_client_security();
			if (client_security)
				return client_security->get_transfer_prime_size();
		}

		return 0;
	}

	template <router_type T>
	size_t client_handshake_request_command<T>::get_key_size() const
	{
		if (T == router_type::message)
		{
			const auto client_security = get_client_security();
			if (client_security)
				return client_security->get_message_key_size();
		}

		if (T == router_type::transfer)
		{
			const auto client_security = get_client_security();
			if (client_security)
				return client_security->get_transfer_key_size();
		}

		return 0;
	}
}

#endif
