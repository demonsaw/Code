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

#ifndef _EJA_NONCE_COMPONENT_H_
#define _EJA_NONCE_COMPONENT_H_

#include <boost/atomic/atomic.hpp>

#include "component/component.h"
#include "system/type.h"

namespace eja
{
	class nonce_component final : public component
	{
		make_factory(nonce_component);

	private:
		using value_type = boost::atomic<size_t>;
		value_type m_value;

	public:
		nonce_component() : m_value(0) { }
		nonce_component(const nonce_component& comp) : component(comp), m_value(comp.m_value.load()) { }

		// Operator
		nonce_component& operator=(const nonce_component& comp);

		// Interface
		virtual void clear() override;

		// Set
		void set(const size_t value) { m_value.store(value); }

		// Get		
		size_t get() const { return const_cast<value_type&>(m_value).fetch_add(1); }
	};
}

#endif
