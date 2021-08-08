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

#include "security/cipher/aes.h"
#include "security/cipher/cipher.h"
#include "security/cipher/mars.h"
#include "security/cipher/rc6.h"
#include "security/cipher/serpent.h"
#include "security/cipher/twofish.h"

namespace eja
{
	// Interface
	void cipher::clear()
	{ 
		set_iv();
		set_key();
	}
	
	// Set
	void cipher::set_key(const void* key, const size_t key_size)
	{
		assert(key && (key_size == 16) || (key_size == 24) || (key_size == 32));

		m_key.Assign(reinterpret_cast<const byte*>(key), key_size);
	}

	// Static
	cipher::ptr cipher::create(const char* name)
	{
		// AES
		if (boost::equals(name, cipher_name::aes))
			return aes::create();

		// MARS
		if (boost::equals(name, cipher_name::mars))
			return mars::create();

		// RC6
		if (boost::equals(name, cipher_name::rc6))
			return rc6::create();

		// Serpent
		if (boost::equals(name, cipher_name::serpent))
			return serpent::create();

		// Twofish
		if (boost::equals(name, cipher_name::twofish))
			return twofish::create();

		// Default
		return aes::create();
	}
}
