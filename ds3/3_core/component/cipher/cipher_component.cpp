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

#include "component/cipher/cipher_component.h"
#include "security/cipher/cipher.h"

namespace eja
{
	// Operator
	cipher_component& cipher_component::operator=(const cipher_component& comp)
	{
		if (this != &comp)
		{
			id_component::operator=(comp);

			m_cipher = comp.m_cipher;
		}

		return *this;
	}

	// Interface
	void cipher_component::clear()
	{
		id_component::clear();

		m_cipher.reset();
	}

	// Utility
	std::string cipher_component::encrypt(const void* input, const size_t input_size) const
	{
		assert(m_cipher);

		return m_cipher->encrypt(input, input_size);
	}

	std::string cipher_component::decrypt(const void* input, const size_t input_size) const
	{
		assert(m_cipher);

		return m_cipher->decrypt(input, input_size);
	}

	// Set
	void cipher_component::set_iv()
	{
		assert(m_cipher);

		m_cipher->set_iv();
	}

	void cipher_component::set_iv(const void* iv, const size_t iv_size)
	{
		assert(m_cipher);

		m_cipher->set_iv(iv, iv_size);
	}

	void cipher_component::set_key()
	{
		assert(m_cipher);

		m_cipher->set_key();
	}

	void cipher_component::set_key(const void* key, const size_t key_size)
	{
		assert(m_cipher);

		m_cipher->set_key(key, key_size);
	}
}
