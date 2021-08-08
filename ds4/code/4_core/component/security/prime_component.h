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

#ifndef _EJA_PRIME_COMPONENT_H_
#define _EJA_PRIME_COMPONENT_H_

#include <deque>
#include <cryptopp/integer.h>

#include "component/component.h"
#include "security/algorithm/prime.h"
#include "system/type.h"
#include "thread/thread_safe.h"

namespace eja
{
	class entity;

	make_thread_safe_component(prime_list_component, std::deque<std::shared_ptr<entity>>);

	class prime_component final : public component
	{
		make_factory(prime_component);

	private:
		CryptoPP::Integer m_p;
		CryptoPP::Integer m_q;
		CryptoPP::Integer m_g;

	public:
		prime_component() { }
		prime_component(const prime_component& comp) : component(comp), m_p(comp.m_p), m_q(comp.m_q), m_g(comp.m_g) { }
		prime_component(const prime& p) : m_p(p.get_p()), m_q(p.get_q()), m_g(p.get_g()) { }

		// Operator
		prime_component& operator=(const prime_component& comp);
		prime_component& operator=(const prime& p);

		// Interface
		virtual void clear() override;

		// Utility
		virtual bool empty() const { return !(has_p() || has_q() || has_g()); }

		// Has
		bool has_p() const { return m_p.NotZero(); }
		bool has_q() const { return m_q.NotZero(); }
		bool has_g() const { return m_g.NotZero(); }

		// Set
		void set_p(const CryptoPP::Integer& p) { m_p = p; }
		void set_q(const CryptoPP::Integer& q) { m_q = q; }
		void set_g(const CryptoPP::Integer& g) { m_g = g; }

		// Get
		const CryptoPP::Integer& get_p() const { return m_p; }
		const CryptoPP::Integer& get_q() const { return m_q; }
		const CryptoPP::Integer& get_g() const { return m_g; }

		// Static
		static ptr create(const prime& p) { return std::make_shared<prime_component>(p); }
	};
}

#endif
