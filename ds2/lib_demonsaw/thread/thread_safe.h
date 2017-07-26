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

#ifndef _EJA_THREAD_SAFE_H_
#define _EJA_THREAD_SAFE_H_

#include <memory>
#include <mutex>

#include "system/type.h"
#include "thread/thread_proxy.h"

namespace eja
{
	/////////////////////////////////////////////////////////////////////////////
	//
	//
	//
	/////////////////////////////////////////////////////////////////////////////
	#define thread_lock(obj) const auto proxy = obj->get_proxy()

	/////////////////////////////////////////////////////////////////////////////
	//
	//
	//
	/////////////////////////////////////////////////////////////////////////////
	template <typename T>
	class thread_safe : public T
	{
		make_factory(thread_safe);

	public:
		using value_type = thread_safe;
		using proxy_type = thread_proxy<value_type>;

	protected:
		std::mutex m_mutex;

	public:
		thread_safe() { }
		thread_safe(const thread_safe& safe) : T(safe) { }
		thread_safe(const T& t) : T(t) { }

		// Operator
		thread_safe& operator=(const thread_safe& safe)
		{
			if (this != &safe)
				T::operator=(safe);

			return *this;
		}

		thread_safe& operator=(const T& t)
		{
			if (this != &t)
				T::operator=(t);

			return *this;
		}

		const value_type* operator->() const { return this; }
		value_type* operator->() { return this; }

		const value_type& operator*() const { return *this; }
		value_type& operator*() { return *this; }

		// Get
		const proxy_type get_proxy() const { return proxy_type(*this, m_mutex); }
		proxy_type get_proxy() { return proxy_type(*this, m_mutex); }

		// Static
		static ptr create(const T& t) { return ptr(new thread_safe(t)); }
		static ptr create(const std::shared_ptr<T>& t) { return ptr(new thread_safe(*t)); }
	};

	template <typename T, typename U>
	class thread_safe_ex : public T, public U
	{
		make_factory(thread_safe_ex);

	public:
		using value_type = thread_safe_ex;
		using proxy_type = thread_proxy<value_type>;

	protected:
		std::mutex m_mutex;

	public:
		thread_safe_ex() { }
		thread_safe_ex(const thread_safe_ex& safe) : T(safe), U(safe) { }
		thread_safe_ex(const T& t) : T(t) { }
		thread_safe_ex(const U& u) : U(u) { }

		// Operator
		thread_safe_ex& operator=(const thread_safe_ex& safe)
		{
			if (this != &safe)
			{
				T::operator=(safe);
				U::operator=(safe);
			}

			return *this;
		}

		thread_safe_ex& operator=(const T& t)
		{
			if (this != &t)
				T::operator=(t);

			return *this;
		}

		thread_safe_ex& operator=(const U& u)
		{
			if (this != &u)
				U::operator=(u);

			return *this;
		}

		const value_type* operator->() const { return this; }
		value_type* operator->() { return this; }

		const value_type& operator*() const { return *this; }
		value_type& operator*() { return *this; }


		// Interface
		virtual void clear() override
		{
			T::clear();
			U::clear();
		}

		virtual bool empty() const override
		{
			return T::empty() && U::empty();
		}

		// Get
		const proxy_type get_proxy() const { return proxy_type(*this, m_mutex); }
		proxy_type get_proxy() { return proxy_type(*this, m_mutex); }

		// Static
		static ptr create(const T& t) { return ptr(new thread_safe_ex(t)); }
		static ptr create(const std::shared_ptr<T>& t) { return ptr(new thread_safe_ex(*t)); }
		static ptr create(const U& u) { return ptr(new thread_safe_ex(u)); }
		static ptr create(const std::shared_ptr<U>& u) { return ptr(new thread_safe_ex(*u)); }
	};
	
	/////////////////////////////////////////////////////////////////////////////
	//
	//
	//
	/////////////////////////////////////////////////////////////////////////////
	#define make_thread_safe_type(T, ...)													\
	class T final : public thread_safe<__VA_ARGS__>											\
	{																						\
		make_factory(T);																	\
																							\
	public:																					\
		T() { }																				\
		T(const T& t) : thread_safe(t) { }													\
		T(const __VA_ARGS__& u) : thread_safe(u) { }										\
																							\
		T& operator=(const __VA_ARGS__& u)													\
		{																					\
			thread_safe::operator=(u);														\
																							\
			return *this;																	\
		}																					\
																							\
		T& operator=(const T& t)															\
		{																					\
			if (this != &t)																	\
				thread_safe::operator=(t);													\
																							\
			return *this;																	\
		}																					\
																							\
		static ptr create(const __VA_ARGS__& v) { return ptr(new T(v)); }					\
		static ptr create(const std::shared_ptr<__VA_ARGS__>& v) { return ptr(new T(*v)); }	\
	}

	#define make_thread_safe_type_ex(T, U, ...)												\
	class T final : public thread_safe_ex<U, __VA_ARGS__>									\
	{																						\
		make_factory(T);																	\
																							\
	public:																					\
		T() { }																				\
		T(const T& t) : thread_safe_ex(t) { }												\
		T(const U& u) : thread_safe_ex(u) { }												\
		T(const __VA_ARGS__& v) : thread_safe_ex(v) { }										\
																							\
		T& operator=(const T& t)															\
		{																					\
			if (this != &t)																	\
				thread_safe_ex::operator=(t);												\
																							\
			return *this;																	\
		}																					\
																							\
		T& operator=(const U& u)															\
		{																					\
			if (this != &u)																	\
				thread_safe_ex::operator=(u);												\
																							\
			return *this;																	\
		}																					\
																							\
		T& operator=(const __VA_ARGS__& v)													\
		{																					\
			if (this != &v)																	\
				thread_safe_ex::operator=(v);												\
																							\
			return *this;																	\
		}																					\
																							\
		static ptr create(const U& u) { return ptr(new T(u)); }								\
		static ptr create(const std::shared_ptr<U>& u) { return ptr(new T(*u)); }			\
		static ptr create(const __VA_ARGS__& v) { return ptr(new T(v)); }					\
		static ptr create(const std::shared_ptr<__VA_ARGS__>& v) { return ptr(new T(*v)); }	\
	}
}

#endif
