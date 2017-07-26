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

#ifndef _EJA_FUNCTION_
#define _EJA_FUNCTION_

#include <functional>
#include <memory>
#include <string>

#include "function_type.h"
#include "system/mutex/mutex_list.h"
#include "system/mutex/mutex_map.h"
#include "system/mutex/mutex_queue.h"
#include "system/mutex/mutex_vector.h"
#include "system/type.h"

namespace eja
{
	class function
	{
	public:
		using ptr = std::shared_ptr<function>;

	private:
		void* m_function = nullptr;
		const std::type_info* m_signature = nullptr;

	public:
		function() { }
		function(const function&) = delete;
		template <typename T> function(T value) { assign(value); }
		virtual ~function() { delete reinterpret_cast<byte*>(m_function); }

		// Operator
		function& operator=(const function&) = delete;
		template <typename T> function& operator=(T value);
		template <typename ...T> void operator()(T... args) { call(args...); }

		// Utility
		bool empty() const { return !m_function; }
		
		void clear();
		template <typename T> void assign(T value);
		template <typename ...T> void call(T... args);		

		// Static
		static ptr create() { return std::make_shared<function>(); }
		template <typename T> static ptr create(T value) { return std::make_shared<function>(value); }
	};

	// Container
	derived_type(function_list, mutex_list<function>);
	derived_type(function_map, mutex_map<function_type, function>);
	derived_type(function_map_list, mutex_map_list <function_type, function>);
	derived_type(function_queue, mutex_queue<function>);
	derived_type(function_vector, mutex_vector<function>);

	// Operator
	template <typename T> 
	function& function::operator=(T value)
	{
		assign(value);
		
		return *this; 
	}
	
	// Utility
	template <typename ...T>
	void function::call(T... args)
	{
		auto value = static_cast<std::function<void(T...)>*>(m_function);
		assert(typeid(value) == *(m_signature));

		// HACK: To avoid crashing...
		if (typeid(value) == *(m_signature))
			(*value)(args...);
	}

	// Mutator
	template <typename T>
	void function::assign(T value)
	{
		clear();

		auto function = new decltype(to_function(value))(to_function(value));
		m_function = static_cast<void*>(function);
		m_signature = &typeid(function);
	}
}

#endif
