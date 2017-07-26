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

#ifndef _EJA_URL_H_
#define _EJA_URL_H_

#undef MAINTAIN_BACKWARDS_COMPATIBILITY_562

#include <string>
#include <cryptopp/integer.h>
#include <cryptopp/secblock.h>

#include "security/security.h"

namespace eja
{
	class url final
	{
	public:
		static const size_t default_size = 16;

	private:
		url() = delete;
		url(const url&) = delete;
		~url() = delete;

		// Operator
		url& operator=(const url&) = delete;

	public:
		// Encode
		static std::string encode(const void* input, const size_t input_size);
		static std::string encode(const CryptoPP::Integer& input) { return encode(security::str(input)); }
		static std::string encode(const CryptoPP::SecByteBlock& input) { return encode(input.data(), input.size()); }
		static std::string encode(const std::string& input) { return encode(input.c_str(), input.size()); }
		static std::string encode(const char* input) { return encode(input, strlen(input)); }

		// Decode		
		static std::string decode(const void* input, const size_t input_size);
		static std::string decode(const CryptoPP::Integer& input) { return decode(security::str(input)); }
		static std::string decode(const CryptoPP::SecByteBlock& input) { return decode(input.data(), input.size()); }
		static std::string decode(const std::string& input) { return decode(input.c_str(), input.size()); }
		static std::string decode(const char* input) { return decode(input, strlen(input)); }
	};
}

#endif
