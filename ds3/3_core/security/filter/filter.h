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

#ifndef _EJA_FILTER_H_
#define _EJA_FILTER_H_

#undef MAINTAIN_BACKWARDS_COMPATIBILITY_562

#include <string>
#include <cryptopp/secblock.h>

#include "security/security.h"

namespace eja
{
	template <typename E, typename D>
	class filter final
	{
	private:
		filter() = delete;
		filter(const filter&) = delete;
		~filter() = delete;

		// Operator
		filter& operator=(const filter&) = delete;

	public:
		// Utility
		static std::string random(const size_t output_size = 16) { return encode(security::random(output_size)); }

		// Encode
		template <typename T>
		static std::string encode(const T& t) { return encode(&t, sizeof(T)); }
		static std::string encode(const void* input, const size_t input_size);
		static std::string encode(const CryptoPP::SecByteBlock& input) { return encode(input.data(), input.size()); }
		static std::string encode(const std::string& input) { return encode(input.c_str(), input.size()); }
		static std::string encode(const char* input) { return encode(input, strlen(input)); }

		// Decode		
		template <typename T>
		static std::string decode(const T& t) { return decode(&t, sizeof(T)); }
		static std::string decode(const void* input, const size_t input_size);
		static std::string decode(const CryptoPP::SecByteBlock& input) { return decode(input.data(), input.size()); }
		static std::string decode(const std::string& input) { return decode(input.c_str(), input.size()); }
		static std::string decode(const char* input) { return decode(input, strlen(input)); }
	};

	// Encode
	template <typename E, typename D>
	std::string filter<E, D>::encode(const void* input, const size_t input_size)
	{
		E encoder(NULL, false);
		std::string output;
		encoder.Attach(new CryptoPP::StringSink(output));
		encoder.Put(reinterpret_cast<const byte*>(input), input_size);
		encoder.MessageEnd();
		return output;
	}

	// Decode
	template <typename E, typename D>
	std::string filter<E, D>::decode(const void* input, const size_t input_size)
	{
		D decoder;
		std::string output;
		decoder.Attach(new CryptoPP::StringSink(output));
		decoder.Put(reinterpret_cast<const byte*>(input), input_size);
		decoder.MessageEnd();
		return output;
	}
}

#endif