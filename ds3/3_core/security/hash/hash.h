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

#ifndef _EJA_HASH_H_
#define _EJA_HASH_H_

#undef MAINTAIN_BACKWARDS_COMPATIBILITY_562

#include <memory>
#include <string>
#include <cryptopp/osrng.h>
#include <cryptopp/secblock.h>

#include "security/salt.h"
#include "security/security.h"
#include "system/type.h"

namespace eja
{
	// Type
	enum class hash_type { none, md5, sha1, sha224, sha256, sha384, sha512, sha3_224, sha3_256, sha3_384, sha3_512 };

	// Name
	namespace hash_name
	{
		static const char* none = "none";
		static const char* md5 = "md5";
		static const char* sha1 = "sha1";
		static const char* sha224 = "sha224";
		static const char* sha256 = "sha256";
		static const char* sha384 = "sha384";
		static const char* sha512 = "sha512";
		static const char* sha3_224 = "sha3_224";
		static const char* sha3_256 = "sha3_256";
		static const char* sha3_384 = "sha3_384";
		static const char* sha3_512 = "sha3_512";
	}

	// Base
	class hash : public salt
	{
	public:
		using ptr = std::shared_ptr<hash>;

	protected:
		hash() { }
		hash(const hash& h) : salt(h) { }
		virtual ~hash() { }

		// Operator
		hash& operator=(const hash& h);

	public:
		// Interface
		virtual ptr copy() const = 0;

		// Utility
		virtual std::string random() const = 0;
		virtual size_t size() const = 0;

		// Compute
		virtual std::string compute() = 0;
		virtual std::string compute(const void* input, const size_t input_size) = 0;
		std::string compute(const CryptoPP::SecByteBlock& input) { return compute(input.data(), input.size()); }
		std::string compute(const std::string& input) { return compute(input.c_str(), input.size()); }
		std::string compute(const char* input) { return compute(input, strlen(input)); }

		// Update
		virtual void update(const void* input, const size_t input_size) = 0;
		void update(const CryptoPP::SecByteBlock& input) { update(input.data(), input.size()); }
		void update(const std::string& input) { update(input.c_str(), input.size()); }
		void update(const char* input) { update(input, strlen(input)); }

		// Get
		virtual const char* get_name() const = 0;
		virtual hash_type get_type() const = 0;

		virtual size_t get_min_size() const = 0;
		virtual size_t get_max_size() const = 0;
		virtual size_t get_size() const = 0;

		// Create
		static ptr create(const std::string& name) { return create(name.c_str()); }
		static ptr create(const char* name);
	};

	// Implementation
	template <typename T, hash_type U>
	class hash_impl final : public hash
	{
		make_nocopy_factory(hash_impl);

	private:
		T m_routine;

	public:
		hash_impl() { }
		hash_impl(const hash_impl& impl) : hash(impl), m_routine(impl.m_routine) { }

		// Operator
		hash_impl& operator=(const hash_impl& impl);

		// Interface
		virtual hash::ptr copy() const override { return create(*this); }

		// Utility
		virtual std::string random() const override { return security::random(T::DIGESTSIZE); }
		virtual size_t size() const override { return T::DIGESTSIZE; }

		// Compute
		using hash::compute;
		virtual std::string compute() override;
		virtual std::string compute(const void* input, const size_t input_size) override;
		
		// Update
		using hash::update;
		virtual void update(const void* input, const size_t input_size) override;

		// Get
		virtual const char* get_name() const override;
		virtual hash_type get_type() const override { return U; }

		virtual size_t get_min_size() const override { return T::DIGESTSIZE; }
		virtual size_t get_max_size() const override { return T::DIGESTSIZE; }
		virtual size_t get_size() const override { return T::DIGESTSIZE; }
	};

	// Operator
	template <typename T, hash_type U>
	hash_impl<T, U>& hash_impl<T, U>::operator=(const hash_impl& impl)
	{
		if (this != &impl)
		{
			hash::operator=(impl);

			m_routine = impl.m_routine;
		}

		return *this;
	}

	// Compute
	template <typename T, hash_type U>
	std::string hash_impl<T, U>::compute()
	{		
		byte digest[T::DIGESTSIZE];		
		m_routine.Final(digest);

		return security::str(digest, T::DIGESTSIZE);
	}

	template <typename T, hash_type U>
	std::string hash_impl<T, U>::compute(const void* input, const size_t input_size)
	{
		byte digest[T::DIGESTSIZE];
		m_routine.Update(reinterpret_cast<const byte*>(input), input_size);
		m_routine.Update(m_salt.data(), m_salt.size());
		m_routine.Final(digest);

		return security::str(digest, T::DIGESTSIZE);
	}

	// Update
	template <typename T, hash_type U>
	void hash_impl<T, U>::update(const void* input, const size_t input_size)
	{
		m_routine.Update(reinterpret_cast<const byte*>(input), input_size);
		m_routine.Update(m_salt.data(), m_salt.size());
	}

	// Get
	template <typename T, hash_type U>
	const char* hash_impl<T, U>::get_name() const
	{
		switch (U)
		{
			case hash_type::md5:		return hash_name::md5;
			case hash_type::sha1:		return hash_name::sha1;
			case hash_type::sha224:		return hash_name::sha224;
			case hash_type::sha256:		return hash_name::sha256;
			case hash_type::sha384:		return hash_name::sha384;
			case hash_type::sha512:		return hash_name::sha512;
			case hash_type::sha3_224:	return hash_name::sha3_224;
			case hash_type::sha3_256:	return hash_name::sha3_256;
			case hash_type::sha3_384:	return hash_name::sha3_384;
			case hash_type::sha3_512:	return hash_name::sha3_512;
			default:					return hash_name::none;
		}
	}
}

#endif
