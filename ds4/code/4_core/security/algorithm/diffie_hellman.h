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

#ifndef _EJA_DIFFIE_HELLMAN_H_
#define _EJA_DIFFIE_HELLMAN_H_

#undef MAINTAIN_BACKWARDS_COMPATIBILITY_562

#include <string>
#include <cryptopp/dh.h>
#include <cryptopp/integer.h>
#include <cryptopp/secblock.h>

#include "system/type.h"

namespace eja
{
	class diffie_hellman final
	{
		make_factory(diffie_hellman);

	private:
		CryptoPP::DH m_dh;
		CryptoPP::SecByteBlock m_private_key;
		CryptoPP::SecByteBlock m_public_key;
		CryptoPP::SecByteBlock m_shared_key;

	public:
		diffie_hellman() { }
		diffie_hellman(const CryptoPP::Integer& p, const CryptoPP::Integer& g) { init(p, g); }
		diffie_hellman(const CryptoPP::Integer& p, const size_t g) { init(p, CryptoPP::Integer(static_cast<long>(g))); }
		diffie_hellman(const void* p, const size_t pbits, const size_t g) { init(CryptoPP::Integer(reinterpret_cast<const byte*>(p), pbits), g); }
		diffie_hellman(const std::shared_ptr<std::string>& p, const size_t g) { init(reinterpret_cast<const byte*>(p->c_str()), p->size(), g); }
		diffie_hellman(const std::string& p, const size_t g) { init(reinterpret_cast<const byte*>(p.c_str()), p.size(), g); }
		diffie_hellman(const char* p, const size_t g) { init(reinterpret_cast<const byte*>(p), strlen(p), g); }
		
		diffie_hellman(const CryptoPP::Integer& p, const CryptoPP::Integer& q, const CryptoPP::Integer& g) { init(p, q, g); }
		diffie_hellman(const CryptoPP::Integer& p, const CryptoPP::Integer& q, const size_t g) { init(p, q, CryptoPP::Integer(static_cast<long>(g))); }
		diffie_hellman(const void* p, const size_t pbits, const void* q, const size_t qbits, const size_t g) { init(CryptoPP::Integer(reinterpret_cast<const byte*>(p), pbits), CryptoPP::Integer(reinterpret_cast<const byte*>(q), qbits), g); }
		diffie_hellman(const std::shared_ptr<std::string>& p, const std::shared_ptr<std::string>& q, const size_t g) { init(reinterpret_cast<const byte*>(p->c_str()), p->size(), reinterpret_cast<const byte*>(q->c_str()), q->size(), g); }
		diffie_hellman(const std::string& p, const std::string& q, const size_t g) { init(reinterpret_cast<const byte*>(p.c_str()), p.size(), reinterpret_cast<const byte*>(q.c_str()), q.size(), g); }
		diffie_hellman(const char* p, const char* q, const size_t g) { init(reinterpret_cast<const byte*>(p), strlen(p), reinterpret_cast<const byte*>(q), strlen(q), g); }
		
		// Interface
		void clear();

		// Utility
		size_t size() const { return m_shared_key.size(); }

		// Init
		void init(const size_t pbits = get_size());
		
		void init(const CryptoPP::Integer& p, const CryptoPP::Integer& g);
		void init(const CryptoPP::Integer& p, const size_t g) { init(p, CryptoPP::Integer(static_cast<long>(g))); }
		void init(const void* p, const size_t pbits, const size_t g) { init(CryptoPP::Integer(reinterpret_cast<const byte*>(p), pbits), g); }
		void init(const std::shared_ptr<std::string>& p, const size_t g) { init(reinterpret_cast<const byte*>(p->c_str()), p->size(), g); }
		void init(const std::string& p, const size_t g) { init(reinterpret_cast<const byte*>(p.c_str()), p.size(), g); }
		void init(const char* p, const size_t g) { init(reinterpret_cast<const byte*>(p), strlen(p), g); }

		void init(const CryptoPP::Integer& p, const CryptoPP::Integer& q, const CryptoPP::Integer& g);
		void init(const CryptoPP::Integer& p, const CryptoPP::Integer& q, const size_t g) { init(p, q, CryptoPP::Integer(static_cast<long>(g))); }
		void init(const void* p, const size_t pbits, const void* q, const size_t qbits, const size_t g) { init(CryptoPP::Integer(reinterpret_cast<const byte*>(p), pbits), CryptoPP::Integer(reinterpret_cast<const byte*>(q), qbits), g); }
		void init(const std::shared_ptr<std::string>& p, const std::shared_ptr<std::string>& q, const size_t g) { init(reinterpret_cast<const byte*>(p->c_str()), p->size(), reinterpret_cast<const byte*>(q->c_str()), q->size(), g); }
		void init(const std::string& p, const std::string& q, const size_t g) { init(reinterpret_cast<const byte*>(p.c_str()), p.size(), reinterpret_cast<const byte*>(q.c_str()), q.size(), g); }
		void init(const char* p, const char* q, const size_t g) { init(reinterpret_cast<const byte*>(p), strlen(p), reinterpret_cast<const byte*>(q), strlen(q), g); }

		// Compute		
		void compute(const void* input, const size_t input_size);
		void compute(const CryptoPP::SecByteBlock& input) { return compute(input.data(), input.size()); }
		void compute(const std::shared_ptr<std::string>& input) { return compute(input->c_str(), input->size()); }
		void compute(const std::string& input) { return compute(input.c_str(), input.size()); }
		void compute(const char* input) { return compute(input, strlen(input)); }

		// Get		
		const CryptoPP::Integer& get_p() const { return m_dh.GetGroupParameters().GetModulus(); }
		const CryptoPP::Integer& get_q() const { return m_dh.GetGroupParameters().GetSubgroupOrder(); }
		const CryptoPP::Integer& get_g() const { return m_dh.GetGroupParameters().GetGenerator(); }

		const CryptoPP::SecByteBlock& get_private_key() const { return m_private_key; }
		const CryptoPP::SecByteBlock& get_public_key() const { return m_public_key; }
		const CryptoPP::SecByteBlock& get_shared_key() const { return m_shared_key; }

		// Static
		static const char* get_name() { return "dh"; }
		static size_t get_min_size() { return 128; }
		static size_t get_max_size() { return (4 << 10); }
		static size_t get_size() { return get_min_size(); }
	};
}

#endif
