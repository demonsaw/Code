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

#ifndef _EJA_SESSION_SECURITY_COMPONENT_H_
#define _EJA_SESSION_SECURITY_COMPONENT_H_

#include "component/security/security_component.h"
#include "system/type.h"

namespace eja
{
	class session_security_component final : public security_component
	{
		make_factory(session_security_component);

	private:
		std::string m_algorithm;
		size_t m_prime_size;

	public:
		session_security_component();
		session_security_component(const session_security_component& comp) : security_component(comp),
			m_algorithm(comp.m_algorithm),m_prime_size(comp.m_prime_size) { }

		// Operator
		session_security_component& operator=(const session_security_component& comp);

		// Interface
		virtual void clear() override;
		virtual void update() override;

		// Has
		bool has_algorithm() const { return !m_algorithm.empty(); }
		bool has_prime_size() const { return m_prime_size > 0; }

		// Set
		void set_algorithm(const std::string& algorithm) { m_algorithm = algorithm; }
		void set_prime_size(const size_t size) { m_prime_size = size; }

		// Get
		const std::string& get_algorithm() const { return m_algorithm; }
		size_t get_prime_size() const { return m_prime_size; }
	};
}

#endif
