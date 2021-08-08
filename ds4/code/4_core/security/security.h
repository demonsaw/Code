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

#ifndef _EJA_SECURITY_H_
#define _EJA_SECURITY_H_

#undef MAINTAIN_BACKWARDS_COMPATIBILITY_562

#include <cassert>
#include <string>
#include <cryptopp/aes.h>
#include <cryptopp/integer.h>
#include <cryptopp/osrng.h>
#include <cryptopp/secblock.h>

#include "system/type.h"

namespace eja
{
	class security final
	{
	private:
		static const size_t s_min = 16;	// 128 bits
		static const size_t s_max = 64;	// 512 bits
		static const size_t s_num = 32;	// 256 bits
		static const size_t s_mod = 4;	//  32 bits

	private:
		security() = delete;
		security(const security&) = delete;
		~security() = delete;

		// Operator
		security& operator=(const security&) = delete;

	public:
		// Utility
		template <typename T>
		static T random();

		template <typename T>
		static std::string random(const size_t output_size);
		static std::string random(const size_t output_size) { return random<CryptoPP::AES>(output_size); }

		static std::string str(const void* input, const size_t input_size);
		static std::string str(const CryptoPP::SecByteBlock& input) { return str(input.data(), input.size()); }
		static std::string str(const CryptoPP::Integer& input);

		// Get
		static std::string get_id() { return get_id(s_num); }
		static std::string get_id(const size_t output_size) { return random(output_size); }
		static std::string get_id(const size_t min_size, const size_t max_size, const size_t mod_size = 1);

		static std::string get_min_id() { return get_id(s_min); }
		static std::string get_max_id() { return get_id(s_max); }
		static std::string get_random_id() { return get_id(s_min, s_max, s_mod); }

		static size_t get_min_size() { return s_min; }
		static size_t get_max_size() { return s_max; }
		static size_t get_size() { return s_num; }
	};

	// Utility
	template <typename T>
	T security::random()
	{
		T output;
		CryptoPP::AutoSeededRandomPool rng;
		rng.GenerateBlock(reinterpret_cast<byte*>(&output), sizeof(T));

		return output;
	}

	template <typename T>
	std::string security::random(const size_t output_size)
	{
		assert(output_size);

		CryptoPP::AutoSeededX917RNG<T> rng;
		CryptoPP::SecByteBlock block(output_size);
		rng.GenerateBlock(block, block.size());

		return str(block);
	}
}

#endif


