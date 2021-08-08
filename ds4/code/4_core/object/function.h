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

#ifndef _EJA_FUNCTION_H_
#define _EJA_FUNCTION_H_

#include <functional>
#include <list>
#include <map>

#include "object/object.h"
#include "system/type.h"
#include "thread/thread_safe.h"

namespace eja
{
	// Macro
	template <typename T>
	struct function_traits : public function_traits < decltype(&T::operator()) > { };

	template <typename ClassType, typename ReturnType, typename ... Args>
	struct function_traits < ReturnType(ClassType::*)(Args...) const >
	{
		typedef ReturnType(*pointer)(Args...);
		typedef std::function<ReturnType(Args...)> function;
	};

	template <typename T>
	typename function_traits<T>::pointer to_function_pointer(const T& t)
	{
		return static_cast<typename function_traits<T>::pointer>(t);
	}

	template <typename T>
	typename function_traits<T>::function to_function(const T& t)
	{
		return static_cast<typename function_traits<T>::function>(t);
	}

	// Container
	class function;

	make_thread_safe_type(function_list, std::list<std::shared_ptr<function>>);	
	make_thread_safe_type(function_map, std::multimap<size_t, std::shared_ptr<function>>);

	// Class
	class function final : public object
	{
		make_factory(function);

	private:
		void* m_function = nullptr;
		const std::type_info* m_signature = nullptr;

	public:
		function() { }
		function(const function& obj);
		template <typename T> function(const T& t) { assign(t); }
		virtual ~function() override { delete[] static_cast<byte*>(m_function); }

		// Operator
		function& operator=(const function& obj);
		template <typename T> function& operator=(const T& t);
		template <typename ... T> void operator()(T ... args) { call(args...); }

		// Utility
		virtual void clear() override;
		virtual bool valid() const override { return (m_function != nullptr) && (m_signature != nullptr); }

		template <typename T> void assign(const T& t);
		template <typename ... T> void call(const T& ... args) const;

		// Has
		bool has_function() const { return !m_function; }
		bool has_signature() const { return !m_signature; }

		// Get
		const void* get_function() const { return m_function; }
		const std::type_info* get_signature() const { return m_signature; }

		// Static
		template <typename T> static ptr create(const T& t) { return std::make_shared<function>(t); }
	};

	// Operator
	template <typename T>
	function& function::operator=(const T& t)
	{
		assign(t);

		return *this;
	}

	// Utility
	template <typename ... T>
	void function::call(const T& ... args) const
	{
		// NOTE: Skip signatures that don't match (function overloading)
		auto callback = static_cast<std::function<void(T...)>*>(m_function);
		if (typeid(callback) == *(m_signature))
			(*callback)(args...);
	}

	template <typename T>
	void function::assign(const T& t)
	{
		clear();

		auto callback = new decltype(to_function(t))(to_function(t));
		m_function = static_cast<void*>(callback);
		m_signature = &typeid(callback);
	}
}

#endif