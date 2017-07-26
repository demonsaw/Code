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

#include "group_security.h"

namespace eja
{
	// Interface
	void group_security::clear()
	{
		object::clear();

		m_entropy = default_security::entropy;

		m_cipher_type = default_security::cipher_type;
		m_key_size = default_security::key_size;

		m_hash_type = default_group::hash_type;
		m_iterations = default_security::iterations;
		m_salt = default_security::salt;
	}

	// Accessor
	std::string group_security::get_cipher_name() const
	{
		const auto cipher = static_cast<group_cipher_type>(m_cipher_type);

		switch (cipher)
		{
			case group_cipher_type::aes:		return group_cipher_name::aes;
			case group_cipher_type::mars:		return group_cipher_name::mars;
			case group_cipher_type::rc6:		return group_cipher_name::rc6;
			case group_cipher_type::serpent:	return group_cipher_name::serpent;
			case group_cipher_type::twofish:	return group_cipher_name::twofish;
			default:							return "";
		}
	}

	std::string group_security::get_hash_name() const
	{
		const auto hash = static_cast<group_hash_type>(m_hash_type);

		switch (hash)
		{
			case group_hash_type::none:			return group_hash_name::none;
			case group_hash_type::sha256:		return group_hash_name::sha256;
			case group_hash_type::sha384:		return group_hash_name::sha384;
			case group_hash_type::sha512:		return group_hash_name::sha512;
			case group_hash_type::sha3_256:		return group_hash_name::sha3_256;
			case group_hash_type::sha3_384:		return group_hash_name::sha3_384;
			case group_hash_type::sha3_512:		return group_hash_name::sha3_512;
			case group_hash_type::pbkdf_sha256:	return group_hash_name::pbkdf_sha256;
			case group_hash_type::pbkdf_sha384:	return group_hash_name::pbkdf_sha384;
			case group_hash_type::pbkdf_sha512:	return group_hash_name::pbkdf_sha512;
			default:							return "";
		}
	}

	block_cipher::ptr group_security::get_cipher() const
	{
		const auto type = static_cast<group_cipher_type>(m_cipher_type);

		switch (type)
		{
			case group_cipher_type::aes:		return aes::create();
			case group_cipher_type::mars:		return mars::create();
			case group_cipher_type::rc6:		return rc6::create();
			case group_cipher_type::serpent:	return serpent::create();
			case group_cipher_type::twofish:	return twofish::create();
			default:							return nullptr;
		}
	}

	hash::ptr group_security::get_hash() const
	{
		const auto type = static_cast<group_hash_type>(m_hash_type);

		switch (type)
		{			
			case group_hash_type::sha256:		return sha256::create();
			case group_hash_type::sha384:		return sha384::create();
			case group_hash_type::sha512:		return sha512::create();
			case group_hash_type::sha3_256:		return sha3_256::create();
			case group_hash_type::sha3_384:		return sha3_384::create();
			case group_hash_type::sha3_512:		return sha3_512::create();
			default:							return nullptr;
		}
	}

	pbkdf::ptr group_security::get_pbkdf() const
	{
		const auto type = static_cast<group_hash_type>(m_hash_type);

		switch (type)
		{
			case group_hash_type::pbkdf_sha256:	return pbkdf2_hmac_sha256::create();
			case group_hash_type::pbkdf_sha384:	return pbkdf2_hmac_sha384::create();
			case group_hash_type::pbkdf_sha512:	return pbkdf2_hmac_sha512::create();
			default:							return nullptr;
		}
	}
}
