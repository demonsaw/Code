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

#ifndef _EJA_CHECKSUM_H_
#define _EJA_CHECKSUM_H_

#undef MAINTAIN_BACKWARDS_COMPATIBILITY_562

#include <memory>
#include <string>
#include <cryptopp/config.h>
#include <cryptopp/osrng.h>
#include <cryptopp/secblock.h>

#include "security/security.h"
#include "system/type.h"

namespace eja
{
	// Type
	enum class checksum_type { none, adler32, crc32, xxhash32 };

	// Name
	namespace checksum_name
	{
		static const char* none = "none";
		static const char* adler32 = "adler32";
		static const char* crc32 = "crc32";
		static const char* xxhash32 = "xxhash32";
	}

	// Base
	class checksum
	{
	public:
		using ptr = std::shared_ptr<checksum>;

	protected: 
		checksum() { }
		virtual ~checksum() { }

	public:
		// Utility
		virtual u32 random() const = 0;
		virtual size_t size() const = 0;

		// Compute
		template <typename T>
		u32 compute(const T& t) { return compute(&t, sizeof(T)); }
		virtual u32 compute(const void* input, const size_t input_size) = 0;
		u32 compute(const CryptoPP::SecByteBlock& input) { return compute(input.data(), input.size()); }

		u32 compute(const std::shared_ptr<std::wstring>& input) { return compute(input->c_str(), input->size() * sizeof(wchar_t)); }
		u32 compute(const std::wstring& input) { return compute(input.c_str(), input.size() * sizeof(wchar_t)); }
		u32 compute(const wchar_t* input) { return compute(input, wcslen(input)); }

		u32 compute(const std::shared_ptr<std::string>& input) { return compute(input->c_str(), input->size()); }
		u32 compute(const std::string& input) { return compute(input.c_str(), input.size()); }
		u32 compute(const char* input) { return compute(input, strlen(input)); }

		// Update
		template <typename T>
		void update(const T& t) { return update(&t, sizeof(T)); }
		virtual void update(const void* input, const size_t input_size) = 0;
		void update(const CryptoPP::SecByteBlock& input) { update(input.data(), input.size()); }

		void update(const std::shared_ptr<std::wstring>& input) { update(input->c_str(), input->size() * sizeof(wchar_t)); }
		void update(const std::wstring& input) { update(input.c_str(), input.size() * sizeof(wchar_t)); }
		void update(const wchar_t* input) { update(input, wcslen(input)); }

		void update(const std::shared_ptr<std::string>& input) { update(input->c_str(), input->size()); }
		void update(const std::string& input) { update(input.c_str(), input.size()); }
		void update(const char* input) { update(input, strlen(input)); }

		// Finish
		virtual u32 finish() = 0;

		// Create
		static ptr create(const std::string& name) { return create(name.c_str()); }
		static ptr create(const char* name);
	};

	// Implementation
	template <typename T, checksum_type U>
	class checksum_impl final : public checksum
	{
		make_factory(checksum_impl);

	private:
		T m_routine;

	public:
		checksum_impl() { }
		checksum_impl(const u32 seed) : m_routine(seed) { }

		u32 operator()() const { return checksum_impl::random(); }

		template <typename V>
		u32 operator()(const V& v) const { return checksum_impl().compute(&v, sizeof(V)); }
		u32 operator()(const void* input, const size_t input_size) const { return checksum_impl().compute(input, input_size); }
		u32 operator()(const CryptoPP::SecByteBlock& input) const { return checksum_impl().compute(input); }

		u32 operator()(const std::shared_ptr<std::wstring>& input) const { return checksum_impl().compute(input); }
		u32 operator()(const std::wstring& input) const { return checksum_impl().compute(input); }
		u32 operator()(const wchar_t* input) const { return checksum_impl().compute(input); }

		u32 operator()(const std::shared_ptr<std::string>& input) const { return checksum_impl().compute(input); }
		u32 operator()(const std::string& input) const { return checksum_impl().compute(input); }
		u32 operator()(const char* input) const { return checksum_impl().compute(input); }

		// Utility
		virtual size_t size() const override { return T::DIGESTSIZE; }
		virtual u32 random() const override;

		// Compute
		using checksum::compute;
		virtual u32 compute(const void* input, const size_t input_size) override;

		// Update
		using checksum::update;
		virtual void update(const void* input, const size_t input_size) override { m_routine.Update(reinterpret_cast<const byte*>(input), input_size); }

		// Finish
		virtual u32 finish() override;

		// Get
		static const char* get_name();
		static checksum_type get_type() { return U; }
	};

	// Utility
	template <typename T, checksum_type U>
	u32 checksum_impl<T, U>::random() const
	{
		assert(T::DIGESTSIZE == sizeof(u32));

		u32 output;
		CryptoPP::AutoSeededX917RNG<CryptoPP::AES> rng;
		rng.GenerateBlock(reinterpret_cast<byte*>(&output), T::DIGESTSIZE);
		return output;
	}

	// Compute	
	template <typename T, checksum_type U>
	u32 checksum_impl<T, U>::compute(const void* input, const size_t input_size)
	{
		assert(T::DIGESTSIZE == sizeof(u32));

		u32 output;
		m_routine.CalculateDigest(reinterpret_cast<byte*>(&output), reinterpret_cast<const byte*>(input), input_size);
		return output;
	}

	// Finish
	template <typename T, checksum_type U>
	u32 checksum_impl<T, U>::finish()
	{
		assert(T::DIGESTSIZE == sizeof(u32));

		u32 output;
		m_routine.Final(reinterpret_cast<byte*>(&output));
		return output;
	}

	// Get
	template <typename T, checksum_type U>
	const char* checksum_impl<T, U>::get_name()
	{
		switch (U)
		{			
			case checksum_type::adler32:	return checksum_name::adler32;
			case checksum_type::crc32:		return checksum_name::crc32;
			case checksum_type::xxhash32:	return checksum_name::xxhash32;
			default:						return checksum_name::none;
		}
	}
}

#endif
