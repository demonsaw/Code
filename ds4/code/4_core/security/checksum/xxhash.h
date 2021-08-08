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

#ifndef _EJA_XXHASH_H_
#define _EJA_XXHASH_H_

#include "security/checksum/checksum.h"
#include "system/lz4/xxhash_lib.h"
#include "system/type.h"

namespace eja
{
	class XXHASH32;
	using xxhash32 = checksum_impl<XXHASH32, checksum_type::xxhash32>;	
	using xxhash = xxhash32;

	class XXHASH32 final
	{
	private:
		XXH32_state_t m_state;

	public:
		static const size_t DIGESTSIZE = sizeof(u32);

	public:
		XXHASH32(const u32 seed = 0) { XXH32_reset(&m_state, seed); }
		XXHASH32(const XXHASH32& hash) { memcpy(&m_state, &hash.m_state, DIGESTSIZE); }

		// Operator
		XXHASH32& operator=(const XXHASH32& hash);

		// Utility
		void CalculateDigest(void* output, const void* input, const size_t input_size);
		void Update(const void* input, const size_t input_size) { XXH32_update(&m_state, input, input_size); }
		void Final(void* output);

		// Get
		const u32& get_seed() const { return reinterpret_cast<const XXH_istate32_t&>(m_state).seed; }
	};
}

#endif
