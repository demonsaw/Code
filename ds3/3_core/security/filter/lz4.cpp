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

#include "system/type.h"
#include "security/filter/lz4.h"
#include "system/lz4/lz4_lib.h"
#include "system/lz4/lz4hc_lib.h"

namespace eja 
{
	// Encode
	std::string lz4::encode(const void* input, const size_t input_size)
	{
		const auto output_size = LZ4_compressBound(static_cast<int>(input_size));
		if (!output_size)
			throw std::runtime_error("Invalid compression bound");

		std::string output;		
		output.resize(output_size + sizeof(u32));

		// Size
		u32& size = *reinterpret_cast<u32*>(const_cast<char*>(output.data()));
		BOOST_ENDIAN_LITTLE_ONLY(size = boost::endian::endian_reverse(static_cast<u32>(input_size));)
		BOOST_ENDIAN_BIG_ONLY(size = input_size;)
		
		const auto bytes = LZ4_compress_default(reinterpret_cast<const char*>(input), const_cast<char*>(output.data()) + sizeof(u32), static_cast<int>(input_size), output_size);
		if (!bytes)
			throw std::runtime_error("Invalid compression");

		output.resize(bytes + sizeof(u32));

		return output;
	}

	//std::string lz4hc::encode(const void* input, const size_t input_size)
	//{
	//	const auto output_size = LZ4_compressBound(static_cast<int>(input_size));
	//	if (!output_size)
	//		throw std::runtime_error("Invalid compression bound");

	//	std::string output;
	//	output.resize(output_size + sizeof(u32));

	//	// Size
	//	u32& size = *reinterpret_cast<u32*>(const_cast<char*>(output.data()));
	//	BOOST_ENDIAN_LITTLE_ONLY(size = boost::endian::endian_reverse(static_cast<u32>(input_size));)
	//	BOOST_ENDIAN_BIG_ONLY(size = input_size;)

	//	const auto bytes = LZ4_compress_HC(reinterpret_cast<const char*>(input), const_cast<char*>(output.data()) + sizeof(u32), static_cast<int>(input_size), output_size, 0);
	//	if (!bytes)
	//		throw std::runtime_error("Invalid compression");

	//	output.resize(bytes + sizeof(u32));

	//	return output;
	//}

	// Decode
	std::string lz4::decode(const void* input, const size_t input_size)
	{
		// Size
		BOOST_ENDIAN_LITTLE_ONLY(const u32 output_size = boost::endian::endian_reverse(*reinterpret_cast<const u32*>(input));)
		BOOST_ENDIAN_BIG_ONLY(const u32 output_size = *reinterpret_cast<const u32*>(input);)

		std::string output;		
		output.resize(output_size);

		const auto bytes = LZ4_decompress_safe(reinterpret_cast<const char*>(input) + sizeof(u32), const_cast<char*>(output.data()), static_cast<int>(input_size) - sizeof(u32), output_size);
		if (bytes < static_cast<int>(output_size))
			throw std::runtime_error("Invalid decompression");

		return output;
	}
}