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

#include <cassert>
#include <cryptopp/nbtheory.h>
#include <cryptopp/osrng.h>

#include "security/algorithm/prime.h"

namespace eja
{
	// Init
	void prime::init(const size_t pbits /*= get_default_size()*/)
	{
		assert(pbits >= 128);

		// Safe primes are of the form p = 2q + 1, p and q prime.
		// These parameters do not state a maximum security level based
		// on the prime subgroup order. In essence, we get the maximum
		// security level. There is no free lunch: it means more modular
		// multiplications are performed, which affects performance.

		// For a compare/contrast of meeting a security level, see dh-init.zip.
		// Also see http://www.cryptopp.com/wiki/Diffie-Hellman and
		// http://www.cryptopp.com/wiki/Security_level .
		//
		CryptoPP::AutoSeededX917RNG<CryptoPP::AES> rng;
		m_dh.AccessGroupParameters().Initialize(rng, static_cast<int>(pbits));

		if (!m_dh.GetGroupParameters().ValidateGroup(rng, 3))
			throw std::runtime_error("Failed to validate base and prime");

		// http://groups.google.com/group/sci.crypt/browse_thread/thread/7dc7eeb04a09f0ce
		const auto v = ModularExponentiation(get_g(), get_q(), get_p());
		if (v != CryptoPP::Integer::One())
			throw std::runtime_error("Failed to verify order of the subgroup");
	}
}
