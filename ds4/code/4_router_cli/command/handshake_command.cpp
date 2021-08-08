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
#include "component/router/router_component.h"
#include "component/session/session_component.h"

#include "data/request/handshake_request_data.h"
#include "data/response/handshake_response_data.h"
#include "entity/entity.h"
#include "security/security.h"
#include "security/algorithm/diffie_hellman.h"
#include "security/cipher/cipher.h"
#include "security/hash/pbkdf.h"
#include "utility/value.h"

namespace eja
{
	// Interface
	handshake_response_data::ptr handshake_command::execute(const entity::ptr& entity, const router_request_message::ptr& request_message, const handshake_request_data::ptr& request_data) const
	{
		// Validate
		if (unlikely(!validate(request_message, request_data)))
			return nullptr;

		// Client
		const auto session = entity->copy<session_component>();
		std::string session_id;

		while (true)
		{
			session_id = security::get_id();
			const auto pair = std::make_pair(session_id, entity);

			const auto session_map = m_entity->get<session_map_component>();
			{
				thread_lock(session_map);

				const auto it = session_map->find(session_id);

				if (it == session_map->end())
				{
					if (session->has_id())
					{
						const auto it = session_map->find(session->get_id());

						if (it != session_map->end())
						{
							// Add
							const auto& e = it->second;
							const auto pair = std::make_pair(session_id, e);
							session_map->insert(pair);

							// Remove
							session_map->erase(session->get_id());
						}
						else
						{
							// Add
							session_map->insert(pair);
						}
					}
					else
					{
						// Add
						session_map->insert(pair);
					}

					break;
				}
			}
		}

		// Update
		session->set_id(session_id);

		// TODO: Replace with prime cache
		//
		//
		// Key Agreement
		diffie_hellman dh;
		dh.init(request_data->get_prime(), request_data->get_generator());
		dh.compute(request_data->get_public_key());

		// PBKDF
		const auto pbkdf = pbkdf::create(request_data->get_hash());
		pbkdf->set_iterations(request_data->get_iterations());
		pbkdf->set_salt(request_data->get_salt());

		// Key
		const auto key_size = request_data->get_key_size() >> 3;
		const auto key = pbkdf->compute(dh.get_shared_key(), key_size);

		// Cipher
		const auto cipher = cipher::create(request_data->get_cipher());
		cipher->set_key(key);

		// Update the session cipher
		session->set_cipher(cipher);
		entity->set<session_component>(session);

		// Response
		const auto response_data = handshake_response_data::create();
		response_data->set_public_key(security::str(dh.get_public_key()));
		response_data->set_id(session_id);

		return response_data;
	}

	// Utility
	bool handshake_command::validate(const router_request_message::ptr& request_message, const handshake_request_data::ptr& request_data) const
	{
		// Data
		if (request_data->invalid())
			throw std::invalid_argument("Invalid request (handshake)");

		// Prime
		const auto& prime = request_data->get_prime();
		if (prime.size() > io::max_key_size)
			throw std::invalid_argument("Invalid prime (size)");

		// Public
		const auto& public_key = request_data->get_public_key();
		if (public_key.size() > io::max_key_size)
			throw std::invalid_argument("Invalid key (size)");

		// Cipher
		const auto& cipher = request_data->get_cipher();
		if (cipher.size() > io::max_cipher_size)
			throw std::invalid_argument("Invalid cipher (size)");

		// Hash
		const auto& hash = request_data->get_hash();
		if (hash.size() > io::max_hash_size)
			throw std::invalid_argument("Invalid hash (size)");

		// Salt
		const auto& salt = request_data->get_salt();
		if (salt.size() > io::max_salt_size)
			throw std::invalid_argument("Invalid salt (size)");

		return true;
	}
}
