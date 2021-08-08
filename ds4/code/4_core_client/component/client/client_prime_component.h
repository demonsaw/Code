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

#ifndef _EJA_CLIENT_PRIME_COMPONENT_H_
#define _EJA_CLIENT_PRIME_COMPONENT_H_

#include <string>

#include "component/component.h"
#include "system/type.h"

namespace eja
{
	class client_prime_component : public component
	{
		make_factory(client_prime_component);

	private:
		size_t m_size = 0;
		size_t m_buffer_size = 0;
		size_t m_reuse = 0;
		size_t m_threshold = 0;

	public:
		client_prime_component();
		client_prime_component(const client_prime_component& comp) : component(comp),
			m_size(comp.m_size), m_buffer_size(comp.m_buffer_size), m_reuse(comp.m_reuse), m_threshold(comp.m_threshold) { }

		// Operator
		client_prime_component& operator=(const client_prime_component& comp);

		// Interface
		virtual void clear() override;

		// Has		
		bool has_size() const { return m_size > 0; }
		bool has_buffer_size() const { return m_buffer_size > 0; }
		bool has_reuse() const { return m_reuse > 0; }
		bool has_threshold() const { return m_threshold > 0; }

		// Set		
		void set_size(const size_t size) { m_size = size; }
		void set_buffer_size(const size_t buffer_size) { m_buffer_size = buffer_size; }
		void set_reuse(const size_t reuse) { m_reuse = reuse; }
		void set_threshold(const size_t threshold) { m_threshold = threshold; }

		// Get
		const size_t get_size() const { return m_size; }
		const size_t get_buffer_size() const { return m_buffer_size; }
		const size_t get_reuse() const { return m_reuse; }
		const size_t get_threshold() const { return m_threshold; }
	};
}

#endif
