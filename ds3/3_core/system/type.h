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

#ifndef _EJA_TYPE_H_
#define _EJA_TYPE_H_

#define NOMINMAX 1

#include <algorithm>
#include <limits>
#include <memory>
#include <sstream>
#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/cstdint.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include <boost/tokenizer.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/detail/endian.hpp>
#include <boost/endian/conversion.hpp>

//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////
#if _DEBUG
#define DEBUG_ONLY(...) __VA_ARGS__
#else
#define DEBUG_ONLY(...)
#endif

#if _WIN32
#define WIN32_ONLY(...) __VA_ARGS__
#else
#define WIN32_ONLY(...)
#endif

#if _MACX
#define MACX_ONLY(...) __VA_ARGS__
#else
#define MACX_ONLY(...)
#endif

#if BOOST_ENDIAN_BIG_BYTE
#define BOOST_ENDIAN_BIG_ONLY(...) __VA_ARGS__
#define BOOST_ENDIAN_LITTLE_ONLY(...)
#endif

#if BOOST_ENDIAN_LITTLE_BYTE
#define BOOST_ENDIAN_BIG_ONLY(...)
#define BOOST_ENDIAN_LITTLE_ONLY(...) __VA_ARGS__
#endif

#if _WIN32
#define ALIGN(x) __declspec(align(x))
#else
#define ALIGN(x) __attribute__((aligned(x)))
#endif

#if _ANDROID
#define ANDROID_ONLY(...) __VA_ARGS__
#else
#define ANDROID_ONLY(...)
#endif

#if _IOS
#define IOS_ONLY(...) __VA_ARGS__
#else
#define IOS_ONLY(...)
#endif

#define _MOBILE (_ANDROID || _IOS)
#if _MOBILE
#define MOBILE_ONLY(...) __VA_ARGS__
#else
#define MOBILE_ONLY(...)
#endif

//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////
// Int (signed)
#define int_max std::numeric_limits<int>::max()

using s8 = boost::int8_t;
using int8 = boost::int8_t;
#define s8_max std::numeric_limits<s8>::max()

using s16 = boost::int16_t;
using int16 = boost::int16_t;
#define s16_max std::numeric_limits<s16>::max()

using s32 = boost::int32_t;
using int32 = boost::int32_t;
#define s32_max std::numeric_limits<s32>::max()

using s64 = boost::int64_t;
using int64 = boost::int64_t;
#define s64_max std::numeric_limits<s64>::max()

// Int (unsigned)
using u8 = boost::uint8_t;
using byte = boost::uint8_t;
using uint8 = boost::uint8_t;
#define u8_max std::numeric_limits<u8>::max()

using u16 = boost::uint16_t;
using uint16 = boost::uint16_t;
#define u16_max std::numeric_limits<u16>::max()

using u32 = boost::uint32_t;
#define u32_max std::numeric_limits<u32>::max()

using u64 = boost::uint64_t;
using uint64 = boost::uint64_t;
#define u64_max std::numeric_limits<u64>::max()

// Size
#define size_t_max std::numeric_limits<size_t>::max()

// Long (unsigned)
using ulong = unsigned long;

// Real
#if SIZE_MAX == 0xffffffff
using real = float;
#define real_max std::numeric_limits<real>::max()
#else
using real = double;
#define real_max std::numeric_limits<real>::max()
#endif

#if _ANDROID
namespace std
{
	template <typename T>
	inline std::string to_string(T value)
	{
		std::ostringstream os;
		os << value;
		
		return os.str();
	}

	inline int stoi(const std::string& str)
	{
		int value = 0;
		std::istringstream ss(str);
		ss >> value;

		return value;
	}

	inline long long stoll(const std::string& str)
	{
		long long value = 0;
		std::istringstream ss(str);
		ss >> value;

		return value;
	}

	inline double stod(const std::string& str)
	{
		double value = 0;
		std::istringstream ss(str);
		ss >> value;

		return value;
	}
}
#endif

//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////
#define hour_to_ms(value) (value * 60 * 60 * 1000)
#define min_to_ms(value) (value * 60 * 1000)
#define sec_to_ms(value) (value * 1000)

#define ms_to_sec(value) ((value / 1000) % 60)
#define ms_to_min(value) ((value / (1000 * 60)) % 60)
#define ms_to_hour(value) ((value / (1000 * 60 * 60)) % 24)

#define mb_to_kb(value) (value << 10)
#define kb_to_b(value) (value << 10)
#define mb_to_b(value) (value << 20)

#define kb_to_mb(value) (value >> 10)
#define b_to_kb(value) (value >> 10)
#define b_to_mb(value) (value >> 20)

namespace eja
{
	/////////////////////////////////////////////////////////////////////////////
	//
	//
	//
	/////////////////////////////////////////////////////////////////////////////
	using io_service_ptr = std::shared_ptr<boost::asio::io_service>;
	using io_strand_ptr = std::shared_ptr<boost::asio::io_service::strand>;
	using io_work_ptr = std::shared_ptr<boost::asio::io_service::work>;	

	using acceptor_ptr = std::shared_ptr<boost::asio::ip::tcp::acceptor>;
	using address_ptr = std::shared_ptr<boost::asio::ip::address>;	
	using endpoint_ptr = std::shared_ptr<boost::asio::ip::tcp::endpoint>;
	using mutable_buffer_ptr = std::shared_ptr<boost::asio::mutable_buffer>;
	using socket_ptr = std::shared_ptr<boost::asio::ip::tcp::socket>;
	using resolver_ptr = std::shared_ptr<boost::asio::ip::tcp::resolver>;
	using streambuf_ptr = std::shared_ptr<boost::asio::streambuf>;
	using timer_ptr = std::shared_ptr<boost::asio::steady_timer>;

	using string_ptr = std::shared_ptr<std::string>;
	using string_tokenizer = boost::tokenizer<boost::char_separator<char>>;
	using thread_group_ptr = std::shared_ptr<boost::thread_group>;

	enum ip_version : size_t { v4 = 1, v6 = 2, all = v4 | v6 };
	static const size_t npos = -1;

	/////////////////////////////////////////////////////////////////////////////
	//
	//
	//
	/////////////////////////////////////////////////////////////////////////////
	template <typename T>
	constexpr T min(const T& lvalue, const T& rvalue) { return (lvalue < rvalue) ? lvalue : rvalue; }

	template <typename T>
	constexpr T max(const T& lvalue, const T& rvalue) { return (lvalue > rvalue) ? lvalue : rvalue; }

	template <typename T>
	constexpr T clamp(const T& lvalue, const T& mvalue, const T& rvalue) { return max(lvalue, min(mvalue, rvalue)); }

#if _DEBUG
	template <typename T>
	constexpr bool likely(const T& t)
	{
		assert(t);

		return t ? true : false;
	}

	template <typename T>
	constexpr bool unlikely(const T& t)
	{
		assert(!t);

		return t ? true : false;
}
#else
	template <typename T>
	constexpr bool likely(const T& t)
	{
		return true;
	}

	template <typename T>
	constexpr bool unlikely(const T& t)
	{
		return false;
	}
#endif

	template<typename T, typename U>
	size_t find_index(const T& list, const U& value)
	{
		size_t index = 0;
		for (auto it = list.cbegin(); it != list.cend(); ++it, ++index)
			if (*it == value)
				return index;

		return npos;
	}

	template <typename T>
	size_t get_unique_keys(const T& t)
	{
		size_t size = 0;
		for (auto it = t.begin(); it != t.end(); it = t.upper_bound(it->first))
			++size;

		return size;
	}

	/////////////////////////////////////////////////////////////////////////////
	//
	//
	//
	/////////////////////////////////////////////////////////////////////////////
	template<typename T, typename ... U>
	struct assign_wrapper;

	template<typename T>
	struct assign_wrapper<T>
	{
		static void assign(T* ptr, const T& t) { }
	};

	template<typename T, typename U, typename ... V>
	struct assign_wrapper<T, U, V...>
	{
		static void assign(T* ptr, const T& t)
		{
			ptr->U::operator=(t);

			return assign_wrapper<T, V...>::assign(ptr, t);
		}
	}; 
	
	template<typename T, typename ... U>
	struct clear_wrapper;

	template<typename T>
	struct clear_wrapper<T>
	{
		static void clear(T*) { }
	};

	template<typename T, typename U, typename ... V>
	struct clear_wrapper<T, U, V...>
	{
		static void clear(T* ptr)
		{
			ptr->U::clear();

			return clear_wrapper<T, V...>::clear(ptr);
		}
	};

	/////////////////////////////////////////////////////////////////////////////
	//
	//
	//
	/////////////////////////////////////////////////////////////////////////////
	#define make_using(T, U)													\
	class U;																	\
	using T = U
		
	#define make_version(T, U)													\
	class T;																	\
	using T ## _ ## U = T

	/////////////////////////////////////////////////////////////////////////////
	//
	//
	//
	/////////////////////////////////////////////////////////////////////////////
	#define make_factory(...)													\
	public:																		\
	using ptr = std::shared_ptr<__VA_ARGS__>;									\
	ptr copy() const { return create(*this); }									\
	static ptr create() { return ptr(new __VA_ARGS__()); }						\
	static ptr create(const __VA_ARGS__& t) { return ptr(new __VA_ARGS__(t)); }	\
	static ptr create(const ptr& p) { return ptr(new __VA_ARGS__(*p)); }

	#define make_nocopy_factory(...)											\
	public:																		\
	using ptr = std::shared_ptr<__VA_ARGS__>;									\
	static ptr create() { return ptr(new __VA_ARGS__()); }						\
	static ptr create(const __VA_ARGS__& t) { return ptr(new __VA_ARGS__(t)); }	\
	static ptr create(const ptr& p) { return ptr(new __VA_ARGS__(*p)); }

	#define make_default_factory(...)											\
	public:																		\
	using ptr = std::shared_ptr<__VA_ARGS__>;									\
	static ptr create() { return ptr(new __VA_ARGS__()); }

	#define make_param_factory(...)												\
	public:																		\
	using ptr = std::shared_ptr<__VA_ARGS__>;									\
	ptr copy() const { return create(*this); }									\
	static ptr create(const __VA_ARGS__& t) { return ptr(new __VA_ARGS__(t)); }	\
	static ptr create(const ptr& p) { return ptr(new __VA_ARGS__(*p)); }

	/////////////////////////////////////////////////////////////////////////////
	//
	//
	//
	/////////////////////////////////////////////////////////////////////////////
	template <typename T>
	class type final : public T
	{
		make_factory(type);

	public:
		type() { }
		type(const type& t) : T(t) { }
		type(const T& t) : T(t) { }

		// Operator
		type& operator=(const type& t)
		{
			if (this != &t)
				T::operator=(t);

			return *this;
		}

		type& operator=(const T& t)
		{
			if (this != &t)
				T::operator=(t);

			return *this;
		}

		// Static
		static ptr create(const T& t) { return ptr(new type(t)); }
		static ptr create(const std::shared_ptr<T> t) { return ptr(new type(*t)); }
	};

	template <typename T, typename U>
	class type_ex final : public T, public U
	{
		make_factory(type_ex);

	public:
		type_ex() { }
		type_ex(const type_ex& t) : T(t), U(t) { }
		type_ex(const T& t) : T(t) { }
		type_ex(const U& u) : U(u) { }

		// Operator
		type_ex& operator=(const type_ex& t)
		{
			if (this != &t)
			{
				T::operator=(t);
				U::operator=(t);
			}

			return *this;
		}

		type_ex& operator=(const T& t)
		{
			if (this != &t)
				T::operator=(t);

			return *this;
		}

		type_ex& operator=(const U& u)
		{
			if (this != &u)
				U::operator=(u);

			return *this;
		}

		// Interface
		virtual void clear() override
		{ 
			T::clear();
			U::clear();
		}

		// Static
		static ptr create(const T& t) { return ptr(new type_ex(t)); }
		static ptr create(const std::shared_ptr<T> t) { return ptr(new type_ex(*t)); }
		static ptr create(const U& u) { return ptr(new type_ex(u)); }
		static ptr create(const std::shared_ptr<U> u) { return ptr(new type_ex(*u)); }
	};

	/////////////////////////////////////////////////////////////////////////////
	//
	//
	//
	/////////////////////////////////////////////////////////////////////////////
	#define make_type(T, ...)																\
	class T final : public __VA_ARGS__														\
	{																						\
		make_factory(T);																	\
																							\
	public:																					\
		T() { }																				\
		T(const T& t) : __VA_ARGS__(t) { }													\
		T(const __VA_ARGS__& v) : __VA_ARGS__(v) { }										\
																							\
		T& operator=(const T& t)															\
		{																					\
			if (this != &t)																	\
				__VA_ARGS__::operator=(t);													\
																							\
			return *this;																	\
		}																					\
																							\
		T& operator=(const __VA_ARGS__& v)													\
		{																					\
			if (this != &v)																	\
				__VA_ARGS__::operator=(v);													\
																							\
			return *this;																	\
		}																					\
																							\
		static ptr create(const __VA_ARGS__& v) { return ptr(new T(v)); }					\
		static ptr create(const std::shared_ptr<__VA_ARGS__>& v) { return ptr(new T(*v)); }	\
	}

	#define make_type_ex(T, U, ...)															\
	class T final : public U, public __VA_ARGS__											\
	{																						\
		make_factory(T);																	\
																							\
	public:																					\
		T() { }																				\
		T(const T& t) : U(t), __VA_ARGS__(t) { }											\
		T(const U& u) : U(u) { }															\
		T(const __VA_ARGS__& v) : __VA_ARGS__(v) { }										\
																							\
		T& operator=(const T& t)															\
		{																					\
			if (this != &t)																	\
			{																				\
				U::operator=(t);															\
				__VA_ARGS__::operator=(t);													\
			}																				\
																							\
			return *this;																	\
		}																					\
																							\
		T& operator=(const U& u)															\
		{																					\
			if (this != &u)																	\
				U::operator=(u);															\
																							\
			return *this;																	\
		}																					\
																							\
		T& operator=(const __VA_ARGS__& v)													\
		{																					\
			if (this != &v)																	\
				__VA_ARGS__::operator=(v);													\
																							\
			return *this;																	\
		}																					\
																							\
		virtual void clear() override														\
		{																					\
			U::clear();																		\
			__VA_ARGS__::clear();															\
		}																					\
																							\
		static ptr create(const U& u) { return ptr(new T(u)); }								\
		static ptr create(const std::shared_ptr<U> u) { return ptr(new T(*u)); }			\
		static ptr create(const __VA_ARGS__& v) { return ptr(new T(v)); }					\
		static ptr create(const std::shared_ptr<__VA_ARGS__>& v) { return ptr(new T(*v)); }	\
	}

	/////////////////////////////////////////////////////////////////////////////
	//
	//
	//
	/////////////////////////////////////////////////////////////////////////////
	template <typename T>
	void unused(T &&) { }

	/////////////////////////////////////////////////////////////////////////////
	//
	//
	//
	/////////////////////////////////////////////////////////////////////////////
	template <typename T>
	inline const T& empty_numeric()
	{
		static T value;
		return value;
	}

	inline const std::string& empty_string()
	{
		static std::string value;
		return value;
	}

	/////////////////////////////////////////////////////////////////////////////
	//
	//
	//
	/////////////////////////////////////////////////////////////////////////////
	template <typename T>
	inline T string_cast(const std::string& str, const T& defaultValue)
	{
		T t = defaultValue;

		try
		{
			t = boost::lexical_cast<T>(str);
		}
		catch (...) {}

		return t;
	}

	template <typename T>
	inline T string_cast(const std::string& str)
	{
		return string_cast<T>(str, static_cast<T>(0));
	}

	template <>
	inline bool string_cast<bool>(const std::string& str, const bool& defaultValue)
	{
		std::string value = boost::algorithm::to_lower_copy(str);
		std::istringstream is(value);

		bool b = defaultValue;
		is >> std::boolalpha >> b;

		return b;
	}

	template <>
	inline bool string_cast<bool>(const std::string& str)
	{
		return string_cast<bool>(str, false);
	}
}

#endif
