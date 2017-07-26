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

#ifndef _EJA_SECURITY_COMPONENT_H_
#define _EJA_SECURITY_COMPONENT_H_

#include <cryptopp/secblock.h>

#include "component/component.h"
#include "system/type.h"

namespace eja
{
	class cipher;

	class security_component : public component
	{
		make_factory(security_component);

	protected:
		std::string m_cipher;
		size_t m_key_size;

		std::string m_hash;
		std::string m_salt;
		size_t m_iterations;

	protected:
		security_component() { }
		security_component(const security_component& comp) : component(comp),
			m_cipher(comp.m_cipher), m_key_size(comp.m_key_size),
			m_hash(comp.m_hash), m_salt(comp.m_salt), m_iterations(comp.m_iterations) { }

		// Operator
		security_component& operator=(const security_component& comp);

	public:
		// Interface
		virtual void clear() override;

		// Utility
		std::shared_ptr<cipher> create_cipher(const void* input, const size_t input_size) const;
		std::shared_ptr<cipher> create_cipher(const CryptoPP::SecByteBlock& input) const { return create_cipher(input.data(), input.size()); }
		std::shared_ptr<cipher> create_cipher(const std::string& input) const { return create_cipher(input.c_str(), input.size()); }
		std::shared_ptr<cipher> create_cipher(const char* input) const { return create_cipher(input, strlen(input)); }

		// Has
		bool has_cipher() const { return !m_cipher.empty(); }
		bool has_key_size() const { return m_key_size > 0; }

		bool has_hash() const { return !m_hash.empty(); }
		bool has_salt() const { return !m_salt.empty(); }
		bool has_iterations() const { return m_iterations > 0; }

		// Set
		void set_cipher(const std::string& cipher) { m_cipher = cipher; }
		void set_key_size(const size_t key_size) { m_key_size = key_size; }

		void set_hash(const std::string& hash) { m_hash = hash; }
		void set_salt(const std::string& salt) { m_salt = salt; }
		void set_iterations(const size_t iterations) { m_iterations = iterations; }

		// Get
		const std::string& get_cipher() const { return m_cipher; }
		size_t get_key_size() const { return m_key_size; }

		const std::string& get_hash() const { return m_hash; }
		const std::string& get_salt() const { return m_salt; }
		size_t get_iterations() const { return m_iterations; }
	};
}

#endif
