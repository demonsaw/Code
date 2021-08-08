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

#ifndef _EJA_PRIME_H_
#define _EJA_PRIME_H_

#undef MAINTAIN_BACKWARDS_COMPATIBILITY_562

#include <cryptopp/dh.h>
#include <cryptopp/integer.h>

namespace eja
{
	class prime
	{
	private:
		CryptoPP::DH m_dh;

	public:
		prime() { }
		prime(const size_t pbits) { init(pbits); }

		// Interface
		void clear();

		// Utility
		void init(const size_t pbits = get_size());

		// Get		
		const CryptoPP::Integer& get_p() const { return m_dh.GetGroupParameters().GetModulus(); }
		const CryptoPP::Integer& get_q() const { return m_dh.GetGroupParameters().GetSubgroupOrder(); }
		const CryptoPP::Integer& get_g() const { return m_dh.GetGroupParameters().GetGenerator(); }

		// Static
		static size_t get_min_size() { return 128; }
		static size_t get_max_size() { return (4 << 10); }
		static size_t get_size() { return get_min_size(); }
	};
}

#endif
