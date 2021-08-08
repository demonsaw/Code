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

#include "component/session/session_component.h"
#include "security/cipher/aes.h"
#include "security/hash/sha.h"
#include "utility/value.h"

namespace eja
{
	// Constructor
	session_component::session_component()
	{
		compute();
	}

	session_component::session_component(const std::string& password)
	{
		set_password(password);

		compute();
	}

	// Interface
	void session_component::clear()
	{
		// NOTE: Do NOT reset the smart ptr (set_cipher call below is thread-safe)
		//
		//cipher_component::clear();
		m_id.clear();

		// Clears private, public & shared keys (not p, q, or g)
		m_algorithm.clear();
		m_name.clear();

		compute();
	}

	// Utility
	void session_component::compute()
	{
		// Hash
		const auto pbkdf = pbkdf_sha256::create();
		const auto key = pbkdf->compute(m_password, static_cast<size_t>(cipher_bytes::large));

		// Cipher
		const auto cipher = aes::create();
		cipher->set_key(key);
		cipher_component::set_cipher(cipher);
	}

	void session_component::compute(const std::string& password)
	{
		set_password(password);

		compute();
	}
}
