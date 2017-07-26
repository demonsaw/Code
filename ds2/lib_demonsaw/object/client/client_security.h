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

#ifndef _EJA_CLIENT_SECURITY_
#define _EJA_CLIENT_SECURITY_

#include "object/object.h"
#include "security/hash.h"
#include "security/hmac.h"
#include "system/mutex/mutex_list.h"
#include "system/mutex/mutex_map.h"
#include "system/mutex/mutex_queue.h"
#include "system/mutex/mutex_vector.h"
#include "utility/default_value.h"

namespace eja
{
	// Type
	enum class client_hash_type { md5, sha1, sha224, sha256, sha384, sha512, sha3_224, sha3_256, sha3_384, sha3_512 };

	// Name
	namespace client_hash_name
	{
		static const char* md5 = "MD5";
		static const char* sha1 = "SHA1";
		static const char* sha224 = "SHA224";
		static const char* sha256 = "SHA256";
		static const char* sha384 = "SHA384";
		static const char* sha512 = "SHA512";
		static const char* sha3_224 = "SHA3 224";
		static const char* sha3_256 = "SHA3 256";
		static const char* sha3_384 = "SHA3 384";
		static const char* sha3_512 = "SHA3 512";
	}

	// Object
	class client_security : public object
	{
	protected:
		size_t m_hash_type = default_security::hash_type;
		std::string m_salt = default_security::salt;

		size_t m_message_prime_size = default_security::message_prime_size;
		size_t m_message_key_size = default_security::message_key_size;

		size_t m_transfer_prime_size = default_security::transfer_prime_size;
		size_t m_transfer_key_size = default_security::transfer_key_size;

	public:
		using ptr = std::shared_ptr<client_security>;

	public:
		client_security() { }
		client_security(const client_security& client_security) : object(client_security), m_hash_type(client_security.m_hash_type), m_salt(client_security.m_salt), m_message_prime_size(client_security.m_message_prime_size), m_message_key_size(client_security.m_message_key_size), m_transfer_prime_size(client_security.m_transfer_prime_size), m_transfer_key_size(client_security.m_transfer_key_size) { }
		virtual ~client_security() override { }

		// Interface
		virtual void clear() override;

		// Utility
		bool has_hash() const { return (m_hash_type >= static_cast<size_t>(client_hash_type::sha256)) && (m_hash_type <= static_cast<size_t>(client_hash_type::sha3_512)); }
		bool has_salt() const { return !m_salt.empty(); }

		// Mutator
		void set_hash_type(const size_t hash) { m_hash_type = hash; }
		void set_salt(const std::string& salt) { m_salt = salt; }
		void set_salt(const char* salt) { m_salt = salt; }
		void set_salt() { m_salt.clear(); }

		void set_message_prime_size(const size_t prime_size) { m_message_prime_size = prime_size; }
		void set_message_key_size(const size_t key_size) { m_message_key_size = key_size; }

		void set_transfer_prime_size(const size_t prime_size) { m_transfer_prime_size = prime_size; }
		void set_transfer_key_size(const size_t key_size) { m_transfer_key_size = key_size; }

		// Accessor		
		hash::ptr get_hash() const;		
		std::string get_hash_name() const; 		
		size_t get_hash_type() const { return m_hash_type; }
		const std::string& get_salt() const { return m_salt; }

		size_t get_message_prime_size() const { return m_message_prime_size; }
		size_t get_message_key_size() const { return m_message_key_size; }

		size_t get_transfer_prime_size() const { return m_transfer_prime_size; }
		size_t get_transfer_key_size() const { return m_transfer_key_size; }

		// Static
		static ptr create() { return std::make_shared<client_security>(); }
		static ptr create(const client_security& client_security) { return std::make_shared<eja::client_security>(client_security); }
		static ptr create(const client_security::ptr client_security) { return std::make_shared<eja::client_security>(*client_security); }
	};

	//void client_security::set_path(const char* path)
	//{
	//	assert(path);

	//	clear();

	//	std::string data;
	//	eja::file file(path);
	//	file.read(data);

	//	// v2.0
	//	// TODO: Allow choice of password algorithm
	//	//
	//	//pbkdf2_hmac_sha pbkdf;
	//	//const size_t key_size = aes::get_default_size();
	//	//std::string key = pbkdf.compute(&input[0], input.size(), key_size);

	//	// v1.0		
	//	byte key[default_security::seed_size] = { 0 };
	//	memcpy(key, default_security::seed, default_security::seed_size);

	//	const size_t key_size = aes::get_default_size();
	//	assert(key_size == default_security::seed_size);

	//	for (size_t i = 0; i < (data.size() - (data.size() % key_size)); i += key_size)
	//	{
	//		for (size_t j = 0; j < key_size; ++j)
	//			key[j] ^= data[i + j];
	//	}

	//	m_cipher->set_key(key, default_security::seed_size);

	//	m_path = path;
	//	m_id = m_cipher->encrypt(default_security::signature);
	//	m_id = base64::encode(m_id);
	//}
}

#endif
