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

#ifndef _EJA_MD5_H_
#define _EJA_MD5_H_

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#undef MAINTAIN_BACKWARDS_COMPATIBILITY_562

#include <cryptopp/algparam.h>
#include <cryptopp/hmac.h>
#include <cryptopp/md5.h>

#include "security/hash/hash.h"
#include "security/hash/hmac.h"
#include "security/hash/pbkdf.h"

namespace eja
{
	// Hash
	using md5 = hash_impl<CryptoPP::Weak::MD5, hash_type::md5>;

	// HMAC
	using hmac_md5 = hmac_impl<CryptoPP::HMAC<CryptoPP::Weak::MD5>, hmac_type::md5>;

	// PBKDF
	using pbkdf_md5 = pbkdf_impl<CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::Weak::MD5>, CryptoPP::Weak::MD5, pbkdf_type::md5>;
}

#endif
