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

#ifndef _EJA_SALT_H_
#define _EJA_SALT_H_

#undef MAINTAIN_BACKWARDS_COMPATIBILITY_562

#include <string>
#include <cryptopp/secblock.h>

namespace eja
{
	class salt
	{
	protected:
		CryptoPP::SecByteBlock m_salt;

	protected:
		salt() { }
		salt(const salt& s) : m_salt(s.m_salt) { }
		virtual ~salt() { }

		// Operator
		salt& operator=(const salt& s);

	public:
		// Interface
		virtual void clear() { set_salt(); }

		// Has
		bool has_salt() const { return !m_salt.empty(); }

		// Set
		void set_salt() { m_salt.resize(0); }
		void set_salt(const void* salt, const size_t salt_size) { m_salt.Assign(reinterpret_cast<const byte*>(salt), salt_size); }
		void set_salt(const CryptoPP::SecByteBlock& salt) { set_salt(salt.data(), salt.size()); }
		void set_salt(const std::string& salt) { set_salt(salt.c_str(), salt.size()); }
		void set_salt(const char* salt) { set_salt(salt, strlen(salt)); }

		// Get
		const CryptoPP::SecByteBlock& get_salt() const { return m_salt; }
	};

	// Operator
	inline salt& salt::operator=(const salt& s)
	{
		if (this != &s)
			m_salt = s.m_salt;

		return *this;
	}
}

#endif
