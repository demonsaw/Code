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

#include "security/checksum/xxhash.h"

namespace eja
{
	// Operator
	XXHASH32& XXHASH32::operator=(const XXHASH32& hash)
	{
		if (this != &hash)
			memcpy(&m_state, &hash.m_state, DIGESTSIZE);

		return *this;
	}

	// Utility
	void XXHASH32::CalculateDigest(void* output, const void* input, const size_t input_size)
	{
		Update(input, input_size);
		Final(output);
	}
	
	void XXHASH32::Final(void* output)
	{
		const auto value = XXH32_digest(&m_state);
		memcpy(output, &value, sizeof(value));

		XXH32_reset(&m_state, reinterpret_cast<const XXH_istate32_t&>(m_state).seed);
	}
}
