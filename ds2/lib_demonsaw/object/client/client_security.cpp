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

#include "client_security.h"

namespace eja
{
	// Interface
	void client_security::clear()
	{
		object::clear();

		m_hash_type = default_security::hash_type;
		m_salt = default_security::salt;

		m_message_prime_size = default_security::message_prime_size;
		m_message_key_size = default_security::message_key_size;

		m_transfer_prime_size = default_security::transfer_prime_size;
		m_transfer_key_size = default_security::transfer_key_size;
	}

	// Accessor
	std::string client_security::get_hash_name() const
	{
		const auto type = static_cast<client_hash_type>(m_hash_type);

		switch (type)
		{
			case client_hash_type::md5:			return client_hash_name::md5;
			case client_hash_type::sha1:		return client_hash_name::sha1;
			case client_hash_type::sha224:		return client_hash_name::sha224;
			case client_hash_type::sha256:		return client_hash_name::sha256;
			case client_hash_type::sha384:		return client_hash_name::sha384;
			case client_hash_type::sha512:		return client_hash_name::sha512;
			case client_hash_type::sha3_224:	return client_hash_name::sha3_224;
			case client_hash_type::sha3_256:	return client_hash_name::sha3_256;
			case client_hash_type::sha3_384:	return client_hash_name::sha3_384;
			case client_hash_type::sha3_512:	return client_hash_name::sha3_512;
			default:							return "";
		}
	}

	hash::ptr client_security::get_hash() const
	{
		const auto type = static_cast<client_hash_type>(m_hash_type);

		switch (type)
		{			
			case client_hash_type::md5:			return md5::create();
			case client_hash_type::sha1:		return sha1::create();
			case client_hash_type::sha224:		return sha224::create();			
			case client_hash_type::sha256:		return sha256::create();
			case client_hash_type::sha384:		return sha384::create();
			case client_hash_type::sha512:		return sha512::create();
			case client_hash_type::sha3_224:	return sha3_224::create();
			case client_hash_type::sha3_256:	return sha3_256::create();
			case client_hash_type::sha3_384:	return sha3_384::create();
			case client_hash_type::sha3_512:	return sha3_512::create();
			default:							return nullptr;
		}
	}
}
