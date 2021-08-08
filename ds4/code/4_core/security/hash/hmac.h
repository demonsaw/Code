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

#ifndef _EJA_HMAC_H_
#define _EJA_HMAC_H_

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
	enum class hmac_type { none, md5, sha1, sha224, sha256, sha384, sha512 };

	// Name
	namespace hmac_name
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
	class hmac : public salt
	{
	public:
		using ptr = std::shared_ptr<hmac>;

	protected:
		hmac() { }
		virtual ~hmac() { }

	public:
		// Utility
		virtual std::string random() const = 0;
		virtual size_t size() const = 0;

		// Compute		
		virtual std::string compute(const void* input, const size_t input_size) = 0;
		std::string compute(const CryptoPP::SecByteBlock& input) { return compute(input.data(), input.size()); }

		std::string compute(const std::shared_ptr<std::wstring>& input) { return compute(input->c_str(), input->size() * sizeof(wchar_t)); }
		std::string compute(const std::wstring& input) { return compute(input.c_str(), input.size() * sizeof(wchar_t)); }
		std::string compute(const wchar_t* input) { return compute(input, wcslen(input)); }

		std::string compute(const std::shared_ptr<std::string>& input) { return compute(input->c_str(), input->size()); }
		std::string compute(const std::string& input) { return compute(input.c_str(), input.size()); }
		std::string compute(const char* input) { return compute(input, strlen(input)); }

		// Update
		virtual void update(const void* input, const size_t input_size) = 0;
		void update(const CryptoPP::SecByteBlock& input) { update(input.data(), input.size()); }
		
		void update(const std::shared_ptr<std::wstring>& input) { update(input->c_str(), input->size() * sizeof(wchar_t)); }
		void update(const std::wstring& input) { update(input.c_str(), input.size() * sizeof(wchar_t)); }
		void update(const wchar_t* input) { update(input, wcslen(input)); }

		void update(const std::shared_ptr<std::string>& input) { update(input->c_str(), input->size()); }
		void update(const std::string& input) { update(input.c_str(), input.size()); }
		void update(const char* input) { update(input, strlen(input)); }

		// Finish
		virtual std::string finish() = 0;

		// Has
		//virtual bool has_key() const = 0;

		// Set
		virtual void set_key() = 0;
		virtual void set_key(const void* key, const size_t key_size) = 0;
		virtual void set_key(const CryptoPP::SecByteBlock& key) = 0;

		virtual void set_key(const std::shared_ptr<std::wstring>& key) = 0;
		virtual void set_key(const std::wstring& key) = 0; 
		virtual void set_key(const wchar_t* key) = 0;

		virtual void set_key(const std::shared_ptr<std::string>& key) = 0;
		virtual void set_key(const std::string& key) = 0;
		virtual void set_key(const char* key) = 0;

		// Create
		static ptr create(const std::string& name) { return create(name.c_str()); }
		static ptr create(const char* name);
	};

	// Implementation
	template <typename T, hmac_type U>
	class hmac_impl final : public hmac
	{
		make_factory(hmac_impl);

	private:
		T m_routine;

	public:
		hmac_impl() { set_key(); }

		// Interface
		virtual void clear() override;

		// Utility
		virtual std::string random() const override { return security::random(T::DIGESTSIZE); }
		virtual size_t size() const override { return T::DIGESTSIZE; }

		// Compute
		using hmac::compute;		
		virtual std::string compute(const void* input, const size_t input_size) override;

		// Update
		using hmac::update;
		virtual void update(const void* input, const size_t input_size) override;

		// Finish
		virtual std::string finish() override;

		// Has
		//virtual bool has_key() const override { return !m_routine.empty(); }

		// Set
		virtual void set_key() override;
		virtual void set_key(const void* key, const size_t key_size) override { m_routine.SetKey(reinterpret_cast<const byte*>(key), key_size); }
		virtual void set_key(const CryptoPP::SecByteBlock& key) override { set_key(key.data(), key.size()); }

		virtual void set_key(const std::shared_ptr<std::wstring>& key) override { set_key(key->c_str(), key->size() * sizeof(wchar_t)); }
		virtual void set_key(const std::wstring& key) override { set_key(key.c_str(), key.size() * sizeof(wchar_t)); }
		virtual void set_key(const wchar_t* key) override { set_key(key, wcslen(key)); }

		virtual void set_key(const std::shared_ptr<std::string>& key) override { set_key(key->c_str(), key->size()); }
		virtual void set_key(const std::string& key) override { set_key(key.c_str(), key.size()); }
		virtual void set_key(const char* key) override { set_key(key, strlen(key)); }

		// Get
		static const char* get_name();
		static hmac_type get_type() { return U; }

		static size_t get_min_size() { return T::MIN_KEYLENGTH; }
		static size_t get_max_size() { return T::MAX_KEYLENGTH; }
		static size_t get_size() { return T::DEFAULT_KEYLENGTH; }
	};

	// Interface
	template <typename T, hmac_type U>
	void hmac_impl<T, U>::clear()
	{ 
		hmac::clear();

		set_key(); 
	}

	// Compute	
	template <typename T, hmac_type U>
	std::string hmac_impl<T, U>::compute(const void* input, const size_t input_size)
	{
		byte digest[T::DIGESTSIZE];
		m_routine.Update(reinterpret_cast<const byte*>(input), input_size);
		m_routine.Update(m_salt.data(), m_salt.size());
		m_routine.Final(digest);

		return security::str(digest, T::DIGESTSIZE);
	}

	// Update
	template <typename T, hmac_type U>
	void hmac_impl<T, U>::update(const void* input, const size_t input_size)
	{
		m_routine.Update(reinterpret_cast<const byte*>(input), input_size);
		m_routine.Update(m_salt.data(), m_salt.size());
	}

	// Finish
	template <typename T, hmac_type U>
	std::string hmac_impl<T, U>::finish()
	{
		byte digest[T::DIGESTSIZE];
		m_routine.Final(digest);

		return security::str(digest, T::DIGESTSIZE);
	}

	// Set
	template <typename T, hmac_type U>
	void hmac_impl<T, U>::set_key()
	{
		byte key[T::DIGESTSIZE];
		memset(key, 0, T::DIGESTSIZE);
		m_routine.SetKey(key, T::DIGESTSIZE);
	}

	// Get
	template <typename T, hmac_type U>
	const char* hmac_impl<T, U>::get_name()
	{
		switch (U)
		{
			case hmac_type::md5:		return hmac_name::md5;
			case hmac_type::sha1:		return hmac_name::sha1;
			case hmac_type::sha224:		return hmac_name::sha224;
			case hmac_type::sha256:		return hmac_name::sha256;
			case hmac_type::sha384:		return hmac_name::sha384;
			case hmac_type::sha512:		return hmac_name::sha512;
			default:					return hmac_name::none;
		}
	}
}

#endif
