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

#ifndef _EJA_SHA_H_
#define _EJA_SHA_H_

#undef MAINTAIN_BACKWARDS_COMPATIBILITY_562

#include <cryptopp/hmac.h>
#include <cryptopp/sha.h>
#include <cryptopp/sha3.h>

#include "security/hash/hash.h"
#include "security/hash/hmac.h"
#include "security/hash/pbkdf.h"

namespace eja
{
	// Hash
	using sha1 = hash_impl<CryptoPP::SHA1, hash_type::sha1>;
	using sha224 = hash_impl<CryptoPP::SHA224, hash_type::sha224>;
	using sha256 = hash_impl<CryptoPP::SHA256, hash_type::sha256>;
	using sha384 = hash_impl<CryptoPP::SHA384, hash_type::sha384>;
	using sha512 = hash_impl<CryptoPP::SHA512, hash_type::sha512>;

	using sha3_224 = hash_impl<CryptoPP::SHA3_224, hash_type::sha3_224>;
	using sha3_256 = hash_impl<CryptoPP::SHA3_256, hash_type::sha3_256>;
	using sha3_384 = hash_impl<CryptoPP::SHA3_384, hash_type::sha3_384>;
	using sha3_512 = hash_impl<CryptoPP::SHA3_512, hash_type::sha3_512>;

	// HMAC
	using hmac_sha1 = hmac_impl<CryptoPP::HMAC<CryptoPP::SHA1>, hmac_type::sha1>;
	using hmac_sha224 = hmac_impl<CryptoPP::HMAC<CryptoPP::SHA224>, hmac_type::sha224>;
	using hmac_sha256 = hmac_impl<CryptoPP::HMAC<CryptoPP::SHA256>, hmac_type::sha256>;
	using hmac_sha384 = hmac_impl<CryptoPP::HMAC<CryptoPP::SHA384>, hmac_type::sha384>;
	using hmac_sha512 = hmac_impl<CryptoPP::HMAC<CryptoPP::SHA512>, hmac_type::sha512>;

	// PBKDF
	using pbkdf_sha1 = pbkdf_impl<CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA1>, CryptoPP::SHA1, pbkdf_type::sha1>;
	using pbkdf_sha224 = pbkdf_impl<CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA224>, CryptoPP::SHA224, pbkdf_type::sha224>;
	using pbkdf_sha256 = pbkdf_impl<CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA256>, CryptoPP::SHA256, pbkdf_type::sha256>;
	using pbkdf_sha384 = pbkdf_impl<CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA384>, CryptoPP::SHA384, pbkdf_type::sha384>;
	using pbkdf_sha512 = pbkdf_impl<CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA512>, CryptoPP::SHA512, pbkdf_type::sha512>;
}

#endif
