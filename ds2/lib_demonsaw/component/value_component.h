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

#ifndef _EJA_VALUE_COMPONENT_
#define _EJA_VALUE_COMPONENT_

#include <memory>

#include "component.h"

namespace eja
{
	template <typename T>
	class value_component : public component
	{
	public:
		using value_type = std::shared_ptr<T>;
		using ptr = std::shared_ptr<value_component<T>>;

	private:
		value_type m_value;

	protected:
		value_component() { }		
		value_component(const value_type& value) { set_value(value); }

	public:		
		virtual ~value_component() override { }

		// Mutator
		void set_value(const value_type& value) { m_value = value; }

		// Accessor
		value_type get_value() const { return m_value; }

		// Static
		static ptr create() { return ptr(new value_component<T>()); }
		static ptr create(const value_type& value) { return ptr(new value_component<T>)(value); }
	};
}

#endif
