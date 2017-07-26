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

#ifndef _EJA_OBJECT_H_
#define _EJA_OBJECT_H_

#include <ostream>
#include <memory>
#include <string>
#include <boost/lexical_cast.hpp>

#include "system/type.h"

namespace eja
{
	// Object
	class object
	{
	protected:
		object() { }
		object(object&&) { }
		object(const object&) { }
		virtual ~object() { }

		// Operator
		object& operator=(object&&) { return *this; }
		object& operator=(const object&) { return *this; }

	public:		
		// Interface
		virtual void clear() { }

		// Utility
		virtual bool valid() const { return true; }
		bool invalid() const { return !valid(); }
	};

	// Macro
	#define make_object(T, ...) make_type_ex(T, object, __VA_ARGS__)
	#define make_thread_safe_object(T, ...) make_thread_safe_type_ex(T, object, __VA_ARGS__)
}

#endif
