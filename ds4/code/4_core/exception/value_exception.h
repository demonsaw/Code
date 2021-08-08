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

#ifndef _EJA_VALUE_EXCEPTION_H_
#define _EJA_VALUE_EXCEPTION_H_

#include <exception>

#include "system/type.h"

namespace eja
{
	template <typename T>
	class value_exception : public std::exception
	{
	protected:
		using value_type = T;

		value_type m_value = 0;

	public:
		value_exception() { }
		value_exception(const value_type& value) : m_value(value) { }

		// Utility
		virtual const char* what() const noexcept override { return "Invalid value"; }

		// Has
		bool has_value() const { return m_value != 0; }

		// Set
		void set_value(const value_type& value) { m_value = value; }

		// Get
		const value_type& get_value() const { return m_value; }
	};
}

#endif
