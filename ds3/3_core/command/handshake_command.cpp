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
#include "component/version_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/session/session_component.h"
#include "component/session/session_security_component.h"
#include "entity/entity.h"
#include "message/request/handshake_request_message.h"
#include "message/response/handshake_response_message.h"
#include "security/security.h"
#include "security/algorithm/diffie_hellman.h"
#include "security/cipher/cipher.h"
#include "security/hash/pbkdf.h"

namespace eja
{
	// Client
	handshake_request_message::ptr client_handshake_command::execute(const entity::ptr router, const bool config /*= true*/)
	{
		// TODO: Replace with prime cache
		//
		//
		// Key Agreement
		const auto session = router->get<session_component>();
		auto& dh = session->get_algorithm();

		const auto session_security = m_entity->get<session_security_component>();
		dh.init(config ? session_security->get_prime_size() : dh.get_min_size());

		// Request
		const auto request = handshake_request_message::create();

		// Version
		const auto version = m_entity->get<version_component>();
		request->set_version(version->str());

		// Prime
		request->set_prime(security::str(dh.get_p()));
		request->set_generator(dh.get_g().ConvertToLong());
		request->set_public_key(security::str(dh.get_public_key()));

		// Cipher
		request->set_cipher(session_security->get_cipher());
		request->set_key_size(static_cast<u32>(session_security->get_key_size()));

		// Hash
		request->set_hash(session_security->get_hash());
		request->set_salt(session_security->get_salt());
		request->set_iterations(static_cast<u32>(session_security->get_iterations()));

		return request;
	}

	void client_handshake_command::execute(const entity::ptr router, const handshake_response_message::ptr response)
	{
		// Session
		const auto session = router->get<session_component>();
		session->set_id(response->get_session());

		// Version
		const auto version = router->get<version_component>();
		version->set(response->get_version());

		// TODO: Replace with prime cache
		//
		//
		// Diffie Hellman
		auto& dh = session->get_algorithm();
		dh.compute(response->get_public_key());

		// Cipher
		const auto session_security = m_entity->get<session_security_component>();
		const auto cipher = session_security->create_cipher(dh.get_shared_key());
		session->set_cipher(cipher);

		// Clear
		dh.clear();
	}

	// Router
	handshake_response_message::ptr router_handshake_command::execute(const entity::ptr client, const handshake_request_message::ptr request)
	{
		// Session
		const auto session = client->get<session_component>();

		while (true)
		{
			const auto session_id = security::get_id();
			const auto session_map = m_entity->get<session_map_component>();
			const auto pair = std::make_pair(session_id, client);

			thread_lock(session_map);
			auto it = session_map->find(session_id);

			if (it == session_map->end())
			{
				if (session->has_id())
				{
					auto it = session_map->find(session->get_id());
					
					if (it != session_map->end())
					{						
						session->set_id(session_id);

						const auto& e = it->second;
						const auto pair = std::make_pair(session_id, e);
						session_map->insert(pair);
					}
					else
					{
						session_map->erase(session->get_id());
						m_entity->call(callback_type::session, callback_action::remove, client);

						session->set_id(session_id);
						session_map->insert(pair);

						m_entity->call(callback_type::session, callback_action::add, client);
					}
				}
				else
				{
					session->set_id(session_id);
					session_map->insert(pair);

					m_entity->call(callback_type::session, callback_action::add, client);
				}

				break;
			}
		}

		// Version
		auto version = client->get<version_component>();
		version->set(request->get_version());		

		// TODO: Replace with prime cache
		//
		//
		// Key Agreement
		diffie_hellman dh;
		dh.init(request->get_prime(), request->get_generator());
		dh.compute(request->get_public_key());

		// PBKDF
		const auto pbkdf = pbkdf::create(request->get_hash());
		pbkdf->set_iterations(request->get_iterations());
		pbkdf->set_salt(request->get_salt());

		// Key
		const auto key_size = request->get_key_size() >> 3;
		const auto key = pbkdf->compute(dh.get_shared_key(), key_size);
		//const auto hex = hex::encode(key);

		// Cipher
		const auto cipher = cipher::create(request->get_cipher());
		cipher->set_key(key);

		session->set_cipher(cipher);

		// Response
		const auto response = handshake_response_message::create();

		// Version
		version = m_entity->get<version_component>();
		response->set_version(version->str());

		response->set_public_key(security::str(dh.get_public_key()));
		response->set_session(session->get_id());

		return response;
	}
}
