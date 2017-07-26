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

#include <stdlib.h>

#include "security/security.h"

namespace eja
{
	std::string security::str(const void* input, const size_t input_size)
	{
		std::string output;
		CryptoPP::StringSink sink(output);
		sink.Put(reinterpret_cast<const byte*>(input), input_size);

		return output;
	}

	std::string security::str(const CryptoPP::Integer& input)
	{
		const auto input_size = input.MinEncodedSize();
		CryptoPP::SecByteBlock block(input_size);
		input.Encode(block.BytePtr(), input_size);

		return str(block);

		// NOTE: CryptoPP::Integer has a weird ostream format (uses prefixes to specify numeric base)
		//
		/*std::ostringstream oss;
		oss << std::hex << input;
		return oss.str();*/
	}

	// Get
	std::string security::get_id(const size_t min_size, const size_t max_size, const size_t mod_size /*= 1*/)
	{
		assert(min_size && max_size && mod_size && (min_size <= max_size) && !(min_size % mod_size) && !(max_size % mod_size));

		const auto values = (max_size - min_size) / mod_size;
		const auto output_size = min_size + ((rand() % values) * mod_size);

		CryptoPP::AutoSeededX917RNG<CryptoPP::AES> rng;
		CryptoPP::SecByteBlock block(output_size);
		rng.GenerateBlock(block, block.size());

		return str(block);
	}
}
