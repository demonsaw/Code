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

#ifndef _EJA_PBKDF_H_
#define _EJA_PBKDF_H_

#undef MAINTAIN_BACKWARDS_COMPATIBILITY_562

#include <memory>
#include <string>
#include <cryptopp/osrng.h>
#include <cryptopp/pwdbased.h>
#include <cryptopp/secblock.h>

#include "security/salt.h"
#include "security/security.h"
#include "system/type.h"

namespace eja
{
	// Type
	enum class pbkdf_type { none, md5, sha1, sha224, sha256, sha384, sha512 };

	// Name
	namespace pbkdf_name
	{
		static const char* none = "none";
		static const char* md5 = "md5";
		static const char* sha1 = "sha1";
		static const char* sha224 = "sha224";
		static const char* sha256 = "sha256";
		static const char* sha384 = "sha384";
		static const char* sha512 = "sha512";
	}

	// Base
	class pbkdf : public salt
	{
	protected:
		size_t m_iterations = get_min_iterations();

	public:
		using ptr = std::shared_ptr<pbkdf>;		

	protected:
		pbkdf() { }
		pbkdf(const pbkdf& p) : salt(p), m_iterations(p.m_iterations) { }
		virtual ~pbkdf() { };

		// Operator
		pbkdf& operator=(const pbkdf& p);

	public:
		// Interface
		virtual void clear();
		virtual ptr copy() const = 0;

		// Utility
		virtual size_t size() const = 0;

		// Compute
		virtual std::string compute(const void* input, const size_t input_size, const size_t output_size) const = 0;
		virtual std::string compute(const CryptoPP::SecByteBlock& input, const size_t output_size) const { return compute(input.data(), input.size(), output_size); }
		virtual std::string compute(const std::string& input, const size_t output_size) const { return compute(input.c_str(), input.size(), output_size); }
		virtual std::string compute(const char* input, const size_t output_size) const { return compute(input, strlen(input), output_size); }

		// Set
		void set_iterations() { m_iterations = get_min_iterations(); }
		void set_iterations(const size_t iterations) { m_iterations = iterations; }

		// Get
		size_t get_iterations() const { return m_iterations; }
		virtual const char* get_name() const = 0;
		virtual pbkdf_type get_type() const = 0;

		virtual size_t get_min_size() const = 0;
		virtual size_t get_max_size() const = 0;
		virtual size_t get_size() const = 0;

		// Static
		static size_t get_min_iterations() { return 1; }
		static size_t get_max_iterations() { return size_t_max; }

		// Create
		static ptr create(const std::string& name) { return create(name.c_str()); }
		static ptr create(const char* name);
	};

	// Implementation
	template <typename T, typename U, pbkdf_type V>
	class pbkdf_impl final : public pbkdf
	{
		make_nocopy_factory(pbkdf_impl);

	private:		
		T m_routine;

	public:
		pbkdf_impl() { }
		pbkdf_impl(const pbkdf_impl& impl) : pbkdf(impl), m_routine(impl.m_routine) { }

		// Operator
		pbkdf_impl& operator=(const pbkdf_impl& impl);

		// Interface
		virtual pbkdf::ptr copy() const override { return create(*this); }

		// Utility
		static std::string random(const size_t output_size = U::DIGESTSIZE) { return security::random(output_size); }
		virtual size_t size() const override { return U::DIGESTSIZE; }

		// Compute
		using pbkdf::compute;
		virtual std::string compute(const void* input, const size_t input_size, const size_t output_size = U::DIGESTSIZE) const override;

		// Get
		virtual const char* get_name() const override;
		virtual pbkdf_type get_type() const override { return V; }

		virtual size_t get_min_size() const override { return U::DIGESTSIZE; }
		virtual size_t get_max_size() const override { return U::DIGESTSIZE; }
		virtual size_t get_size() const override { return U::DIGESTSIZE; }
	};

	// Operator
	template <typename T, typename U, pbkdf_type V>
	pbkdf_impl<T, U, V>& pbkdf_impl<T, U, V>::operator=(const pbkdf_impl& impl)
	{
		if (this != &impl)
		{
			eja::pbkdf::operator=(impl);

			m_routine = impl.m_routine;
		}

		return *this;
	}

	// Compute
	template <typename T, typename U, pbkdf_type V>
	std::string pbkdf_impl<T, U, V>::compute(const void* input, const size_t input_size, const size_t output_size /*= U::DIGESTSIZE*/) const
	{
		CryptoPP::SecByteBlock block(output_size);
		m_routine.DeriveKey(block, block.size(), 0x00, reinterpret_cast<const byte*>(input), input_size, m_salt, m_salt.size(), static_cast<int>(m_iterations), 0);
		return security::str(block);
	}

	// Get
	template <typename T, typename U, pbkdf_type V>
	const char* pbkdf_impl<T, U, V>::get_name() const
	{
		switch (V)
		{
			case pbkdf_type::md5:		return pbkdf_name::md5;
			case pbkdf_type::sha1:		return pbkdf_name::sha1;
			case pbkdf_type::sha224:	return pbkdf_name::sha224;
			case pbkdf_type::sha256:	return pbkdf_name::sha256;
			case pbkdf_type::sha384:	return pbkdf_name::sha384;
			case pbkdf_type::sha512:	return pbkdf_name::sha512;
			default:					return pbkdf_name::none;
		}
	}
}

#endif
