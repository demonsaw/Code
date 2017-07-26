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

#include <iostream>

#include "server_handshake_request_command.h"
#include "message/request/handshake_request_message.h"
#include "message/response/handshake_response_message.h"
#include "component/session_component.h"
#include "json/json_packet.h"
#include "security/base.h"
#include "security/diffie_hellman.h"

namespace eja
{
	http_status server_handshake_request_command::execute(const entity::ptr entity)
	{
		// Request
		const auto request_packet = json_packet::create();
		const auto request_message = handshake_request_message::create(request_packet);		
		const auto request_key = request_message->get_key();

		// Version
		request_message->set_version(default_version::app);

		// Key
		diffie_hellman dh;
		const auto& dh_base = dh.get_base();
		request_key->set_base(dh_base);
		
		const auto& dh_prime = dh.get_prime();
		const auto dh_prime_encoded = base64::encode(dh_prime);
		request_key->set_prime(dh_prime_encoded);

		const auto& dh_key = dh.get_public_key();
		const auto dh_key_encoded = base64::encode(dh_key);
		request_key->set_public_key(dh_key_encoded);

		// Security
		const auto security = get_server_security();
		if (!security)
			return http_code::bad_request;

		// Json
		const auto json = request_packet->str();
		const auto json_encrypted = security->encrypt(json);
		const auto json_encoded = base64::encode(json_encrypted);

		// Server
		const auto server = get_server();
		if (!server)
			return http_code::bad_request;

		// Http
		http_request request;
		request.set_host(server->get_address());
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
			const auto body_decypted = security->decrypt(body_decoded);

			const auto response_packet = json_packet::create(body_decypted);
			const auto response_message = handshake_response_message::create(response_packet);

			// Version
			if (response_message->has_version())
				server->set_version(response_message->get_version());

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
			entity->add(session);
			
			// Cipher
			const auto cipher = aes::create(dh.get_shared_key());
			session->set_cipher(cipher);			
		}

		return status;
	}
}
