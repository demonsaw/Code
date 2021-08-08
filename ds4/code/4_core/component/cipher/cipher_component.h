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

#ifndef _EJA_CIPHER_COMPONENT_H_
#define _EJA_CIPHER_COMPONENT_H_

#include <string>
#include <cryptopp/secblock.h>

#include "component/component.h"
#include "system/type.h"

namespace eja
{
	class cipher;

	class cipher_component : public component
	{
		make_factory(cipher_component);

	protected:
		std::shared_ptr<cipher> m_cipher;

	public:
		cipher_component() { }
		cipher_component(const cipher_component& comp) : component(comp), m_cipher(comp.m_cipher) { }

		// Operator
		cipher_component& operator=(const cipher_component& comp);

		// Interface
		virtual void clear() override;		

		// Utility
		virtual bool valid() const override { return component::valid() && has_cipher(); }

		// Encrypt
		std::string encrypt(const void* input, const size_t input_size) const;
		std::string encrypt(const CryptoPP::SecByteBlock& input) const { return encrypt(input.data(), input.size()); }
		std::string encrypt(const std::string& input) const { return encrypt(input.c_str(), input.size()); }
		std::string encrypt(const char* input) const { return encrypt(input, strlen(input)); }

		// Decrypt
		std::string decrypt(const void* input, const size_t input_size) const;
		std::string decrypt(const CryptoPP::SecByteBlock& input) const { return decrypt(input.data(), input.size()); }
		std::string decrypt(const std::string& input) const { return decrypt(input.c_str(), input.size()); }
		std::string decrypt(const char* input) const { return decrypt(input, strlen(input)); }

		// Has
		bool has_cipher() const { return m_cipher != nullptr; }

		// Set
		void set_cipher(const std::shared_ptr<cipher>& c) { m_cipher = c; }
		template <typename T> void set_cipher() { m_cipher = T::create(); }

		void set_iv();
		void set_iv(const void* iv, const size_t iv_size);
		void set_iv(const CryptoPP::SecByteBlock& iv) { set_iv(iv.data(), iv.size()); }
		void set_iv(const std::string& iv) { set_iv(iv.c_str(), iv.size()); }
		void set_iv(const char* iv) { set_iv(iv, strlen(iv)); }

		void set_key();
		void set_key(const void* key, const size_t key_size);
		void set_key(const CryptoPP::SecByteBlock& key) { set_key(key.data(), key.size()); }
		void set_key(const std::string& key) { set_key(key.c_str(), key.size()); }
		void set_key(const char* key) { set_key(key, strlen(key)); }

		// Get
		std::shared_ptr<cipher> get_cipher() const { return m_cipher; }
	};
}

#endif
