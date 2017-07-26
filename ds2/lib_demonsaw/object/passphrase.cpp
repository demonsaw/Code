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

#include "passphrase.h"
#include "security/aes.h"
#include "security/pbkdf.h"

namespace eja
{
	// Interface
	void passphrase::clear()
	{
		cipher::clear();

		m_salt = default_passphrase;
		set_passphrase();		
	}

	//  Mutator
	void passphrase::set_passphrase(const char* passphrase)
	{
		// HACK: The default passphrase is also the salt - gross!
		//
		if (!passphrase || !strlen(passphrase))
		{
			passphrase = default_passphrase;
			m_passphrase.clear();
		}
		else
			m_passphrase = passphrase;
		
		pbkdf1_sha1 password(m_salt, 4);
		std::string key = password.compute(passphrase, aes::get_default_size());
		m_cipher->set_key(key);

		// V2.0 (allow choice of alg)
		//auto cipher = aes::create(key);
		//set_cipher(cipher);
	}
}
