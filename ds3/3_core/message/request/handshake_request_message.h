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

#ifndef _EJA_HANDSHAKE_REQUEST_MESSAGE_H_
#define _EJA_HANDSHAKE_REQUEST_MESSAGE_H_

#include <memory>
#include <string>

#include "message/message_type.h"
#include "message/request/request_message.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	class entity;

	// Version
	make_version(handshake_request_message, 1);

	// Message
	class handshake_request_message final : public request_message
	{
		make_factory(handshake_request_message);

	private:
		std::string m_version;

		std::string m_prime;
		std::string m_public_key;
		u32 m_generator;

		std::string m_cipher;
		u32 m_key_size;

		std::string m_hash;
		std::string m_salt;
		u32 m_iterations;

	public:
		handshake_request_message() : request_message(message_type::handshake_request), m_iterations(0), m_key_size(0), m_generator(0) { }
		handshake_request_message(const handshake_request_message& msg) : request_message(msg), m_version(msg.m_version), 
			m_prime(msg.m_prime), m_public_key(msg.m_public_key), m_generator(msg.m_generator),
			m_cipher(msg.m_cipher), m_key_size(msg.m_key_size), m_hash(msg.m_hash), m_salt(msg.m_salt), m_iterations(msg.m_iterations) { }
		
		// Operator
		handshake_request_message& operator=(const handshake_request_message& msg);

		// Has
		bool has_version() const { return !m_version.empty(); }

		bool has_prime() const { return !m_prime.empty(); }
		bool has_public_key() const { return !m_public_key.empty(); }
		bool has_generator() const { return m_generator > 0; }

		bool has_cipher() const { return !m_cipher.empty(); }
		bool has_key_size() const { return m_key_size > 0; }

		bool has_hash() const { return !m_hash.empty(); }
		bool has_salt() const { return !m_salt.empty(); }
		bool has_iterations() const { return m_iterations > 0; }

		// Set
		void set_version(const std::string& version) { m_version = version; }

		void set_prime(const std::string& prime) { m_prime = prime; }
		void set_public_key(const std::string& key) { m_public_key = key; }
		void set_generator(const u32& generator) { m_generator = generator; }

		void set_cipher(const std::string& cipher) { m_cipher = cipher; }
		void set_key_size(const u32& size) { m_key_size = size; }

		void set_hash(const std::string& hash) { m_hash = hash; }
		void set_salt(const std::string& salt) { m_salt = salt; }
		void set_iterations(const u32& iterations) { m_iterations = iterations; }

		// Get
		const std::string& get_version() const { return m_version; }

		const std::string& get_prime() const { return m_prime; }
		const std::string& get_public_key() const { return m_public_key; }
		u32 get_generator() const { return m_generator; }

		const std::string& get_cipher() const { return m_cipher; }
		u32 get_key_size() const { return m_key_size; }

		const std::string& get_hash() const { return m_hash; }
		const std::string& get_salt() const { return m_salt; }
		u32 get_iterations() const { return m_iterations; }

		// Static
		template <typename T>
		static ptr create(const std::shared_ptr<entity> entity, const T& data) { return ptr(new handshake_request_message(entity, data)); }

		template <typename T>
		static ptr create(const std::shared_ptr<entity> entity, const std::shared_ptr<T> data) { return ptr(new handshake_request_message(entity, *data)); }

		MSGPACK_DEFINE(MSGPACK_BASE(request_message), m_version, m_prime, m_public_key, m_generator, m_cipher, m_key_size, m_hash, m_salt, m_iterations);
	};

	// Operator
	inline handshake_request_message& handshake_request_message::operator=(const handshake_request_message& msg)
	{
		if (this != &msg)
		{
			request_message::operator=(msg);

			m_version = msg.m_version;

			m_prime = msg.m_prime;
			m_public_key = msg.m_public_key;
			m_generator = msg.m_generator;

			m_cipher = msg.m_cipher;
			m_key_size = msg.m_key_size;

			m_hash = msg.m_hash;
			m_salt = msg.m_salt;
			m_iterations = msg.m_iterations;
		}

		return *this;
	}
}

#endif
