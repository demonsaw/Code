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

#include "security/hash/hash.h"
#include "security/hash/md.h"
#include "security/hash/sha.h"

namespace eja
{
	// Static
	hash::ptr hash::create(const hash_type type)
	{
		switch (type)
		{
			case hash_type::md5:
			{
				return md5::create();
			}
			case hash_type::sha1:
			{
				return sha1::create();
			}
			case hash_type::sha224:
			{
				return sha224::create();
			}
			case hash_type::sha256:
			{
				return sha256::create();
			}
			case hash_type::sha384:
			{
				return sha384::create();
			}
			case hash_type::sha512:
			{
				return sha512::create();
			}
			case hash_type::sha3_224:
			{
				return sha3_224::create();
			}
			case hash_type::sha3_256:
			{
				return sha3_256::create();
			}
			case hash_type::sha3_384:
			{
				return sha3_384::create();
			}
			case hash_type::sha3_512:
			{
				return sha3_512::create();
			}
			default:
			{
				return sha1::create();
			}
		}
	}

	hash::ptr hash::create(const char* name)
	{
		// MD
		if (boost::equals(name, hash_name::md5))
			return md5::create();

		// SHA-1
		if (boost::equals(name, hash_name::sha1))
			return sha1::create();

		// SHA-2
		if (boost::equals(name, hash_name::sha224))
			return sha224::create();

		if (boost::equals(name, hash_name::sha256))
			return sha256::create();

		if (boost::equals(name, hash_name::sha384))
			return sha384::create();

		if (boost::equals(name, hash_name::sha512))
			return sha512::create();

		// SHA-3
		if (boost::equals(name, hash_name::sha3_224))
			return sha3_224::create();

		if (boost::equals(name, hash_name::sha3_256))
			return sha3_256::create();

		if (boost::equals(name, hash_name::sha3_384))
			return sha3_384::create();

		if (boost::equals(name, hash_name::sha3_512))
			return sha3_512::create();
		
		// Default
		return sha1::create();
	}
}
