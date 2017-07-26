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

#ifndef _EJA_TYPE_
#define _EJA_TYPE_

#include <algorithm>
#include <limits>
#include <memory>
#include <sstream>
#include <string>

#include <boost/cstdint.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////
#if _WIN32
#define WIN32_ONLY(...) __VA_ARGS__
#else
#define WIN32_ONLY(...)
#endif

// Defines
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

// Int (signed)
#define int_max std::numeric_limits<int>::max()

using s8 = boost::int_fast8_t;
using int8 = boost::int_fast8_t;
#define s8_max std::numeric_limits<s8>::max()

using s16 = boost::int_fast16_t;
using int16 = boost::int_fast16_t;
#define s16_max std::numeric_limits<s16>::max()

using s32 = boost::int_fast32_t;
using int32 = boost::int_fast32_t;
#define s32_max std::numeric_limits<s32>::max()

using s64 = boost::int_fast64_t;
using int64 = boost::int_fast64_t;
#define s64_max std::numeric_limits<s64>::max()

// Int (unsigned)
#define uint_max std::numeric_limits<int>::max()

using u8 = boost::uint_fast8_t;
using byte = boost::uint_fast8_t;
using uint8 = boost::uint_fast8_t;
#define u8_max std::numeric_limits<u8>::max()

using u16 = boost::uint_fast16_t;
using uint16 = boost::uint_fast16_t;
#define u16_max std::numeric_limits<u16>::max()

using u32 = boost::uint_fast32_t;
using uint32 = boost::uint_fast32_t;
#define u32_max std::numeric_limits<u32>::max()

using u64 = boost::uint_fast64_t;
using uint64 = boost::uint_fast64_t;
#define u64_max std::numeric_limits<u64>::max()

// Long (unsigned)
#define long_max std::numeric_limits<long>::max()
#define ulong_max std::numeric_limits<unsigned long>::max()
using ulong = unsigned long;

// Real
#if SIZE_MAX == 0xffffffff
using real = float;
#define real_max std::numeric_limits<real>::max()
#else
using real = double;
#define real_max std::numeric_limits<real>::max()
#endif

/////////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////////
template <typename T>
T min(const T& lvalue, const T& rvalue) { return (lvalue < rvalue) ? lvalue : rvalue; }

template <typename T>
T max(const T& lvalue, const T& rvalue) { return (lvalue > rvalue) ? lvalue : rvalue; }

template <typename T>
T clamp(const T& lvalue, const T& mvalue, const T& rvalue) { return max(lvalue, min(mvalue, rvalue)); }

namespace eja
{
	// Security	
	/*enum class hash_type : size_t { md5, sha1, sha224, sha256, sha384, sha512, sha3_224, sha3_256, sha3_384, sha3_512, 
		hmac_md5, hmac_sha1, hmac_sha224, hmac_sha256, hmac_sha384, hmac_sha512 };*/
	/*enum class hmac_type : size_t { hmac_md2, hmac_md4, hmac_md5, hmac_sha1, hmac_sha224, hmac_sha256, hmac_sha384, hmac_sha512 };
	enum class pbdkf_type : size_t { pbkdf_md2, pbkdf_md4, pbkdf_md5, pbkdf_sha1, pbkdf_sha224, pbkdf_sha256, pbkdf_sha384, pbkdf_sha512, pbkdf_sha3_224, pbkdf_sha3_256, pbkdf_sha3_384, pbkdf_sha3_512, 
		pbkdf_hmac_md2, pbkdf_hmac_md4, pbkdf_hmac_md5, pbkdf_hmac_sha1, pbkdf_hmac_sha224, pbkdf_hmac_sha256, pbkdf_hmac_sha384, pbkdf_hmac_sha512, pbkdf_hmac_sha3_224, pbkdf_hmac_sha3_256, pbkdf_hmac_sha3_384, pbkdf_hmac_sha3_512
	};*/

	// Function
	template <typename Function>
	struct function_traits : public function_traits < decltype(&Function::operator()) > { };

	template <typename ClassType, typename ReturnType, typename... Args>
	struct function_traits < ReturnType(ClassType::*)(Args...) const >
	{
		typedef ReturnType(*pointer)(Args...);
		typedef std::function<ReturnType(Args...)> function;
	};

	template <typename Function>
	typename function_traits<Function>::pointer to_function_pointer(Function& lambda)
	{
		return static_cast<typename function_traits<Function>::pointer>(lambda);
	}

	template <typename Function>
	typename function_traits<Function>::function to_function(Function& lambda)
	{
		return static_cast<typename function_traits<Function>::function>(lambda);
	}

	// Unused
	template <typename T>
	void unused(T &&) { }

		/////////////////////////////////////////////////////////////////////////////
	//
	//
	//
	/////////////////////////////////////////////////////////////////////////////
	#define make_factory(...)													\
	public:																		\
	using ptr = std::shared_ptr<__VA_ARGS__>;									\
	static ptr create() { return ptr(new __VA_ARGS__()); }						\
	static ptr create(const __VA_ARGS__& t) { return ptr(new __VA_ARGS__(t)); }	\
	static ptr create(const ptr& p) { return ptr(new __VA_ARGS__(*p)); }

	#define make_default_factory(...)											\
	public:																		\
	using ptr = std::shared_ptr<__VA_ARGS__>;									\
	static ptr create() { return ptr(new __VA_ARGS__()); }						\

	#define make_proxy_factory(...)												\
	public:																		\
	using ptr = std::shared_ptr<__VA_ARGS__>;									\
	static ptr create() { return ptr(new __VA_ARGS__()); }						\
	static ptr create(const __VA_ARGS__& t) { return ptr(new __VA_ARGS__(t)); }	\
	static ptr create(const ptr& p) { return ptr(new __VA_ARGS__(*p)); }

	#define make_param_factory(...)												\
	public:																		\
	using ptr = std::shared_ptr<__VA_ARGS__>;									\
	static ptr create(const __VA_ARGS__& t) { return ptr(new __VA_ARGS__(t)); }	\
	static ptr create(const ptr& p) { return ptr(new __VA_ARGS__(*p)); }

	// Type
	class type
	{
	protected:
		type() { }
		virtual ~type() { }

	public:
		enum { npos = -1 };

	public:
		// Operator
		friend std::ostream& operator<<(std::ostream& os, const type& type) { return os << type.str(); }

		// Interface
		virtual std::string str() const { return boost::lexical_cast<std::string>(this); }

		// Utility
		virtual bool valid() const { return true; }
		bool invalid() const { return !valid(); }
	};

	#define derived_type(derived, ...)														\
	class derived : public __VA_ARGS__														\
	{																						\
	public:																					\
		using ptr = std::shared_ptr<derived>;												\
																							\
	public:																					\
		derived() { }																		\
		virtual ~derived() override { }														\
																							\
		static ptr create() { return std::make_shared<derived>(); }							\
	}

	#define derived_value_type(derived, type, ...)											\
	class derived : public __VA_ARGS__														\
	{																						\
	public:																					\
		using ptr = std::shared_ptr<derived>;												\
																							\
	public:																					\
		derived() { }																		\
		derived(const type& value) : __VA_ARGS__(value) { }									\
		virtual ~derived() override { }														\
																							\
		static ptr create() { return std::make_shared<derived>(); }							\
		static ptr create(const type& value) { return std::make_shared<derived>(value); }	\
	}
	
	#define derived_id_type(derived, ...)													\
	class derived : public __VA_ARGS__														\
	{																						\
	public:																					\
		using ptr = std::shared_ptr<derived>;												\
																							\
	public:																					\
		derived() { }																		\
		derived(const std::string& id) : __VA_ARGS__(id) { }								\
		derived(const char* id) : __VA_ARGS__(id) { }										\
		virtual ~derived() override { }														\
																							\
		static ptr create() { return std::make_shared<derived>(); }							\
		static ptr create(const std::string& id) { return std::make_shared<derived>(id); }	\
		static ptr create(const char* id) { return std::make_shared<derived>(id); }			\
	}
	
	using string_ptr = std::shared_ptr<std::string>;

	template <typename T>
	struct shared
	{
		using ptr = std::shared_ptr<T>;
	};

	// Empty
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

	// Cast
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
		return string_cast<T>(str, 0);
	}

	template <>
	inline bool string_cast<bool>(const std::string& str, const bool& defaultValue)
	{
		std::string value(str);
		std::transform(str.begin(), str.end(), value.begin(), ::tolower);
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
