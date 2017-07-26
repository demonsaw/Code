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

#ifndef _EJA_GROUP_SECURITY_
#define _EJA_GROUP_SECURITY_

#include <memory>
#include <string>

#include "object/object.h"
#include "security/block_cipher.h"
#include "security/hash.h"
#include "security/hmac.h"
#include "security/pbkdf.h"
#include "system/mutex/mutex_list.h"
#include "system/mutex/mutex_map.h"
#include "system/mutex/mutex_queue.h"
#include "system/mutex/mutex_vector.h"
#include "utility/default_value.h"

namespace eja
{
	// Type
	enum class group_cipher_type { aes, mars, rc6, serpent, twofish };
	enum class group_hash_type { none, sha256, sha384, sha512, sha3_256, sha3_384, sha3_512, pbkdf_sha256, pbkdf_sha384, pbkdf_sha512 };

	// Name
	namespace group_cipher_name
	{
		static const char* aes = "AES";
		static const char* mars = "MARS";
		static const char* rc6 = "RC6";
		static const char* serpent = "Serpent";
		static const char* twofish = "Twofish";
	}

	namespace group_hash_name
	{
		static const char* none = "XOR";
		static const char* sha256 = "SHA256";
		static const char* sha384 = "SHA384";
		static const char* sha512 = "SHA512";
		static const char* sha3_256 = "SHA3 256";
		static const char* sha3_384 = "SHA3 384";
		static const char* sha3_512 = "SHA3 512";
		static const char* pbkdf_sha256 = "PBKDF SHA256";
		static const char* pbkdf_sha384 = "PBKDF SHA384";
		static const char* pbkdf_sha512 = "PBKDF SHA512";		
	}

	// Object
	class group_security : public object
	{
	protected:
		bool m_modified = false;
		double m_entropy = default_security::entropy;

		size_t m_cipher_type = default_security::cipher_type;
		size_t m_key_size = default_security::key_size;

		size_t m_hash_type = default_group::hash_type;
		size_t m_iterations = default_security::iterations;
		std::string m_salt = default_security::salt;

	public:
		using ptr = std::shared_ptr<group_security>;

	public:
		group_security() { }
		group_security(const eja::group_security& group_security) : object(group_security), m_modified(group_security.m_modified), m_entropy(group_security.m_entropy), m_cipher_type(group_security.m_cipher_type), m_key_size(group_security.m_key_size), m_hash_type(group_security.m_hash_type), m_iterations(group_security.m_iterations), m_salt(group_security.m_salt) { }
		group_security(const std::string& id) : object(id) { }
		group_security(const char* id) : object(id) { }
		virtual ~group_security() override { }

		// Interface
		virtual void clear() override;

		// Utility
		bool has_salt() const { return !m_salt.empty(); }
		bool has_xor() const { return m_hash_type == static_cast<size_t>(group_hash_type::none); }
		bool has_hash() const { return (m_hash_type >= static_cast<size_t>(group_hash_type::sha256)) && (m_hash_type <= static_cast<size_t>(group_hash_type::sha3_512)); }
		bool has_pbkdf() const { return (m_hash_type >= static_cast<size_t>(group_hash_type::pbkdf_sha256)) && (m_hash_type <= static_cast<size_t>(group_hash_type::pbkdf_sha512)); }

		// Mutator
		void set_modified(const bool modified) { m_modified = modified; }
		void set_entropy(const double entropy) { m_entropy = entropy; }

		void set_cipher_type(const size_t cipher_type) { m_cipher_type = cipher_type; }
		void set_key_size(const size_t key_size) { m_key_size = key_size; }

		void set_hash_type(const size_t hash) { m_hash_type = hash; }
		void set_iterations(const size_t iterations) { m_iterations = iterations; }
		void set_salt(const std::string& salt) { m_salt = salt; }
		void set_salt(const char* salt) { m_salt = salt; }

		// Accessor
		bool modified() const { return m_modified; }
		double get_entropy() const { return m_entropy; }

		std::string get_cipher_name() const;
		size_t get_cipher_type() const { return m_cipher_type; }		
		size_t get_key_size() const { return m_key_size; }

		std::string get_hash_name() const;
		size_t get_hash_type() const { return m_hash_type; }		
		size_t get_iterations() const { return m_iterations; }
		const std::string& get_salt() const { return m_salt; }

		block_cipher::ptr get_cipher() const;
		hash::ptr get_hash() const;
		pbkdf::ptr get_pbkdf() const;

		// Static
		static ptr create() { return std::make_shared<group_security>(); }
		static ptr create(const eja::group_security& group_security) { return std::make_shared<eja::group_security>(group_security); }
		static ptr create(const group_security::ptr group_security) { return std::make_shared<eja::group_security>(*group_security); }
		static ptr create(const std::string& id) { return std::make_shared<group_security>(id); }
		static ptr create(const char* id) { return std::make_shared<group_security>(id); }
	};

	// Container
	derived_type(group_security_list, mutex_list<group_security>);
	derived_type(group_security_map, mutex_map<std::string, group_security>);
	derived_type(group_security_queue, mutex_queue<group_security>);
	derived_type(group_security_vector, mutex_vector<group_security>);
}

#endif
