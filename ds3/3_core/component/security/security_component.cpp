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

#include "component/security/security_component.h"
#include "security/cipher/cipher.h"
#include "security/hash/pbkdf.h"

namespace eja
{
	// Operator
	security_component& security_component::operator=(const security_component& comp)
	{
		if (this != &comp)
		{
			component::operator=(comp);

			m_cipher = comp.m_cipher;
			m_key_size = comp.m_key_size;

			m_hash = comp.m_hash;
			m_salt = comp.m_salt;
			m_iterations = comp.m_iterations;
		}

		return *this;
	}

	// Interface
	void security_component::clear()
	{
		component::clear();

		m_cipher.clear();
		m_key_size = 0;

		m_hash.clear();
		m_salt.clear();
		m_iterations = 0;
	}

	// Utility
	cipher::ptr security_component::create_cipher(const void* input, const size_t input_size) const
	{
		// PBKDF
		const auto pbkdf = pbkdf::create(get_hash());
		pbkdf->set_iterations(get_iterations());
		pbkdf->set_salt(get_salt());

		// Key
		const auto key_size = get_key_size() >> 3;
		const auto key = pbkdf->compute(input, input_size, key_size);
		//const auto hex = hex::encode(key);

		// Cipher
		const auto cipher = cipher::create(get_cipher());
		cipher->set_key(key);

		return cipher;
	}
}
