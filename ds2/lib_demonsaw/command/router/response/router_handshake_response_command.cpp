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

#include "router_handshake_response_command.h"
#include "component/mutex_component.h"
#include "component/session_component.h"
#include "component/timeout_component.h"
#include "component/router/router_component.h"
#include "json/json_packet.h"
#include "json/value/json_key.h"
#include "message/request/handshake_request_message.h"
#include "message/response/handshake_response_message.h"
#include "object/version.h"
#include "security/base.h"
#include "security/diffie_hellman.h"
#include "security/hex.h"
#include "security/sha.h"
#include "system/function/function_action.h"
#include "system/function/function_type.h"

namespace eja
{
	// Static
	const version router_handshake_response_command::s_min_version(2, 6, 2);

	http_status router_handshake_response_command::execute(const json_message::ptr message, const json_data::ptr data)
	{
		const auto session_map = get_session_entity_map();		
		auto_lock_ptr(session_map);
		
		auto session_id = hex::random();
		while (session_map->has(session_id))
			session_id = hex::random();

		const auto entity = entity::create();
		entity->add<timeout_component>();
		entity->add<mutex_component>();

		const auto session = session_component::create(session_id);
		entity->add(session);

		session_map->add(session_id, entity);

		//call(function_type::session, function_action::add, entity);

		// Command
		return execute(entity, message, data);
	}

	http_status router_handshake_response_command::execute(const entity::ptr entity, const json_message::ptr message, const json_data::ptr data)
	{
		// Request
		const auto request_message = handshake_request_message::create(message);
		if (request_message->invalid())
		{
			m_socket->write(http_code::bad_request);
			return http_code::bad_request;
		}

		const auto request_key = request_message->get_key();
		if (request_key->invalid())
		{
			m_socket->write(http_code::bad_request);
			return http_code::bad_request;
		}

		// Version
		if (!request_message->has_version())
		{
			m_socket->write(http_code::not_implemented);
			return http_code::not_implemented;
		}

		version ver(request_message->get_version());
		if (ver < s_min_version)
		{
			m_socket->write(http_code::not_implemented);
			return http_code::not_implemented;
		}

		// Key
		const auto request_base = request_key->get_base();
		const auto request_prime = request_key->get_prime();
		const auto request_prime_decoded = base64::decode(request_prime);
		diffie_hellman dh(request_prime_decoded, request_base);

		const auto request_public_key = request_key->get_public_key();
		const auto request_public_key_decoded = base64::decode(request_public_key);

		// HACK: If we validate everything goes to fuck and the C# client takes forever to connect
		const bool dh_status = dh.compute(request_public_key_decoded);
		if (!dh_status)
		{
			m_socket->write(http_code::bad_request);
			return http_code::bad_request;
		}

		// Timer
		const auto timeout = entity->get<timeout_component>();
		if (!timeout)
		{
			m_socket->write(http_code::internal_server_error);
			return http_code::internal_server_error;
		}

		timeout->start();

		// Session
		const auto session = entity->get<session_component>();
		if (!session)
		{
			m_socket->write(http_code::internal_server_error);
			return http_code::internal_server_error;
		}

		const auto& dh_prime = dh.get_prime();

		if (dh_prime.BitCount() > default_security::min_prime_size)
		{
			const auto request_size = request_key->get_size();
			const auto& dh_shared_key = dh.get_shared_key();
			const auto key = sha256().compute(dh_shared_key);
			const auto cipher = aes::create(key, request_size >> 3);
			session->set_cipher(cipher);
		}
		else
		{
			const auto& dh_shared_key = dh.get_shared_key();
			const auto cipher = aes::create(dh_shared_key);			
			session->set_cipher(cipher);
		}

		// Security
		const auto security = get_router_security();
		if (!security)
		{
			m_socket->write(http_code::internal_server_error);
			return http_code::internal_server_error;
		}

		// Response
		const auto response_packet = json_packet::create();
		const auto response_message = handshake_response_message::create(response_packet);

		// Version
		const auto router = get_router();
		if (!router)
			return http_code::bad_request;

		response_message->set_version(router->get_version());

		// Key
		const auto response_key = response_message->get_key();
		response_key->set_public_key(base64::encode(dh.get_public_key()));

		const auto json = response_packet->str();
		const auto json_encrypted = security->encrypt(json);
		const auto json_encoded = base64::encode(json_encrypted);
		
		// Http
		http_ok_response response(session->get_id());
		response.set_body(json_encoded);
		m_socket->write(response);

		call(function_type::client, function_action::refresh, entity);

		return response.get_status();
	}
}