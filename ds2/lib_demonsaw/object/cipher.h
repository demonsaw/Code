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

#ifndef _EJA_KEY_
#define _EJA_KEY_

#include <memory>
#include <string>
#include <cryptopp/secblock.h>

#include "object.h"
#include "security/block_cipher.h"
#include "system/mutex/mutex.h"
#include "system/mutex/mutex_list.h"
#include "system/mutex/mutex_map.h"
#include "system/mutex/mutex_queue.h"
#include "system/mutex/mutex_vector.h"
#include "system/type.h"

namespace eja
{
	class cipher : public object
	{
	private:
		using default_cipher = aes;

	protected:
		block_cipher::ptr m_cipher;
		mutex m_mutex;

	public:
		using ptr = std::shared_ptr<cipher>;

	protected:
		cipher() : object() { set_cipher<default_cipher>(); }
		cipher(const eja::cipher& cipher) : object(cipher), m_cipher(cipher.m_cipher) { }
		cipher(const size_t size) : object(size) { set_cipher<default_cipher>(); }
		cipher(const std::string& id) : object(id) { set_cipher<default_cipher>(); }
		cipher(const char* id) : object(id) { set_cipher<default_cipher>(); }

		virtual ~cipher() override { }

	public:
		// Interface
		virtual void clear() override;

		// Encrypt
		std::string encrypt(const byte* input, const size_t input_size) const { auto_lock_ref(m_mutex); return m_cipher->encrypt(input, input_size); }
		std::string encrypt(const CryptoPP::SecByteBlock& input) const { auto_lock_ref(m_mutex); return m_cipher->encrypt(input); }
		std::string encrypt(const std::string& input) const { auto_lock_ref(m_mutex); return m_cipher->encrypt(input); }
		std::string encrypt(const char* input) const { auto_lock_ref(m_mutex); return m_cipher->encrypt(input); }

		// Decrypt
		std::string decrypt(const byte* input, const size_t input_size) const { auto_lock_ref(m_mutex); return m_cipher->decrypt(input, input_size); }
		std::string decrypt(const CryptoPP::SecByteBlock& input) const { auto_lock_ref(m_mutex); return m_cipher->decrypt(input); }
		std::string decrypt(const std::string& input) const { auto_lock_ref(m_mutex); return m_cipher->decrypt(input); }
		std::string decrypt(const char* input) const { auto_lock_ref(m_mutex); return m_cipher->decrypt(input); }

		// Mutator
		void set_key(const byte* key, const size_t key_size) { auto_lock_ref(m_mutex); m_cipher->set_key(key, key_size); }
		void set_key(const CryptoPP::SecByteBlock& key) { auto_lock_ref(m_mutex); m_cipher->set_key(key.data(), key.size()); }
		void set_key(const std::string& key, const size_t key_size) { auto_lock_ref(m_mutex); m_cipher->set_key(reinterpret_cast<const byte*>(key.c_str()), key_size); }
		void set_key(const std::string& key) { auto_lock_ref(m_mutex); m_cipher->set_key(reinterpret_cast<const byte*>(key.c_str()), key.size()); }
		void set_key(const char* key, const size_t key_size) { auto_lock_ref(m_mutex); m_cipher->set_key(reinterpret_cast<const byte*>(key), key_size); }
		void set_key(const char* key) { auto_lock_ref(m_mutex); m_cipher->set_key(reinterpret_cast<const byte*>(key), strlen(key)); }

		// Utility
		virtual bool valid() const override { auto_lock_ref(m_mutex); return object::valid() && has_cipher(); }
		bool has_cipher() const { auto_lock_ref(m_mutex); return !m_cipher->empty(); }

		// Mutator
		template <typename T> void set_cipher() { auto_lock_ref(m_mutex); m_cipher = T::create(); }
		void set_cipher(const block_cipher::ptr cipher) { auto_lock_ref(m_mutex); m_cipher = cipher; }
		void set_cipher() { auto_lock_ref(m_mutex); m_cipher = default_cipher::create(); }

		// HACK: Don't allow out-of-mutex updates!
		//
		// Accessor
		//block_cipher::ptr get_cipher() const { return m_cipher; }
		std::string get_key() const;
	};

	// Container
	derived_type(cipher_list, mutex_list<cipher>);
	derived_type(cipher_map, mutex_map<std::string, cipher>);
	derived_type(cipher_queue, mutex_queue<cipher>);
	derived_type(cipher_vector, mutex_vector<cipher>);
}

#endif
