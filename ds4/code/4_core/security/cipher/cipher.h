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

#ifndef _EJA_CIPHER_H_
#define _EJA_CIPHER_H_

#undef MAINTAIN_BACKWARDS_COMPATIBILITY_562

#include <memory>
#include <string>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/secblock.h>

#include "security/security.h"
#include "system/type.h"

namespace eja
{
	// Type
	enum class cipher_type { none, aes, mars, rc6, serpent, twofish };

	// Name
	namespace cipher_name
	{
		static const char* none = "none";
		static const char* aes = "aes";
		static const char* mars = "mars";
		static const char* rc6 = "rc6";
		static const char* serpent = "serpent";
		static const char* twofish = "twofish";		
	}

	// Size
	enum class cipher_bits { small = 128, medium = 192, large = 256 };
	enum class cipher_bytes { small = 16, medium = 24, large = 32 };

	// Base
	class cipher
	{
	protected:
		CryptoPP::SecByteBlock m_iv;
		CryptoPP::SecByteBlock m_key; 

	public:
		using ptr = std::shared_ptr<cipher>;		

	protected:
		cipher() { }
		virtual ~cipher() { }

	public:
		// Interface
		virtual void clear();

		// Utility
		virtual std::string random(const size_t output_size) const = 0;
		size_t size() { return m_key.size(); }

		// Encrypt
		virtual std::string encrypt(const void* input, const size_t input_size) const = 0;
		std::string encrypt(const CryptoPP::SecByteBlock& input) const { return encrypt(input.data(), input.size()); }

		std::string encrypt(const std::shared_ptr<std::wstring>& input) const { return encrypt(input->c_str(), input->size() * sizeof(wchar_t)); }
		std::string encrypt(const std::wstring& input) const { return encrypt(input.c_str(), input.size() * sizeof(wchar_t)); }
		std::string encrypt(const wchar_t* input) const { return encrypt(input, wcslen(input)); }

		std::string encrypt(const std::shared_ptr<std::string>& input) const { return encrypt(input->c_str(), input->size()); }
		std::string encrypt(const std::string& input) const { return encrypt(input.c_str(), input.size()); }
		std::string encrypt(const char* input) const { return encrypt(input, strlen(input)); }

		// Decrypt
		virtual std::string decrypt(const void* input, const size_t input_size) const = 0;
		std::string decrypt(const CryptoPP::SecByteBlock& input) const { return decrypt(input.data(), input.size()); }

		std::string decrypt(const std::shared_ptr<std::wstring>& input) const { return decrypt(input->c_str(), input->size() * sizeof(wchar_t)); }
		std::string decrypt(const std::wstring& input) const { return decrypt(input.c_str(), input.size() * sizeof(wchar_t)); }
		std::string decrypt(const wchar_t* input) const { return decrypt(input, wcslen(input)); }

		std::string decrypt(const std::shared_ptr<std::string>& input) const { return decrypt(input->c_str(), input->size()); }
		std::string decrypt(const std::string& input) const { return decrypt(input.c_str(), input.size()); }
		std::string decrypt(const char* input) const { return decrypt(input, strlen(input)); }

		// Has
		bool has_iv() const { return !m_iv.empty(); }
		bool has_key() const { return !m_key.empty(); }

		// Set
		void set_iv() { m_iv.resize(0); }
		virtual void set_iv(const void* iv, const size_t iv_size) = 0;
		void set_iv(const CryptoPP::SecByteBlock& iv) { set_iv(iv.data(), iv.size()); }

		void set_iv(const std::shared_ptr<std::wstring>& iv) { set_iv(iv->c_str(), iv->size() * sizeof(wchar_t)); }
		void set_iv(const std::wstring& iv) { set_iv(iv.c_str(), iv.size() * sizeof(wchar_t)); }
		void set_iv(const wchar_t* iv) { set_iv(iv, wcslen(iv)); }

		void set_iv(const std::shared_ptr<std::string>& iv) { set_iv(iv->c_str(), iv->size()); }
		void set_iv(const std::string& iv) { set_iv(iv.c_str(), iv.size()); }
		void set_iv(const char* iv) { set_iv(iv, strlen(iv)); }

		void set_key() { m_key.resize(0); }
		void set_key(const void* key, const size_t key_size);
		void set_key(const CryptoPP::SecByteBlock& key) { set_key(key.data(), key.size()); }

		void set_key(const std::shared_ptr<std::wstring>& key) { set_key(key->c_str(), key->size() * sizeof(wchar_t)); }
		void set_key(const std::wstring& key) { set_key(key.c_str(), key.size() * sizeof(wchar_t)); }
		void set_key(const wchar_t* key) { set_key(key, wcslen(key)); }

		void set_key(const std::shared_ptr<std::string>& key) { set_key(key->c_str(), key->size()); }
		void set_key(const std::string& key) { set_key(key.c_str(), key.size()); }
		void set_key(const char* key) { set_key(key, strlen(key)); }

		// Get
		const CryptoPP::SecByteBlock& get_key() const { return m_key; }
		
		// Create
		static ptr create(const std::string& name) { return create(name.c_str()); }
		static ptr create(const char* name);
	};

	// Implementation
	template <typename T, cipher_type U, typename V = CryptoPP::CBC_Mode<T>>
	class cipher_impl final : public cipher
	{
		make_factory(cipher_impl);

	public:
		cipher_impl() { }

		// Utility
		virtual std::string random(const size_t output_size = T::DEFAULT_KEYLENGTH) const override { return security::random<T>(output_size); }

		// Encrypt
		using cipher::encrypt;
		virtual std::string encrypt(const void* input, const size_t input_size) const override;

		// Decrypt
		using cipher::decrypt;
		virtual std::string decrypt(const void* input, const size_t input_size) const override;

		// Set
		using cipher::set_iv;
		virtual void set_iv(const void* iv, const size_t iv_size) override;

		// Get
		const CryptoPP::SecByteBlock& get_iv() const { return m_iv; }

		static const char* get_name();
		static cipher_type get_type() { return U; }
		static size_t get_block_size() { return T::BLOCKSIZE; }
		
		static size_t get_min_size() { return T::MIN_KEYLENGTH; }
		static size_t get_max_size() { return T::MAX_KEYLENGTH; }
		static size_t get_size() { return T::DEFAULT_KEYLENGTH; }
	};

	// Encrypt	
	template <typename T, cipher_type U, typename V>
	std::string cipher_impl<T, U, V>::encrypt(const void* input, const size_t input_size) const
	{
		std::string output;

		if (has_iv())
		{
			typename V::Encryption encryptor(m_key, m_key.size(), m_iv);
			CryptoPP::StreamTransformationFilter filter(encryptor, new CryptoPP::StringSink(output));
			filter.Put(reinterpret_cast<const byte*>(input), input_size);
			filter.MessageEnd();
		}
		else
		{
			// IV
			CryptoPP::SecByteBlock iv(T::BLOCKSIZE);
			CryptoPP::AutoSeededX917RNG<T> rng;
			rng.GenerateBlock(iv, iv.size());
			
			const auto output_size = (input_size + (2 * iv.size())) & ~static_cast<size_t>(iv.size() - 1);
			output.resize(output_size);

			// NOTE: Store the IV as the first 16 bytes of the cipher text
			memcpy(const_cast<char*>(output.data()), iv.data(), iv.size());

			typename V::Encryption encryptor(m_key, m_key.size(), iv);
			CryptoPP::StreamTransformationFilter filter(encryptor, new CryptoPP::ArraySink(reinterpret_cast<byte*>(const_cast<char*>(output.data())) + iv.size(), output.size() - iv.size()));
			filter.Put(reinterpret_cast<const byte*>(input), input_size);
			filter.MessageEnd();
		}

		return output;
	}

	// Decrypt	
	template <typename T, cipher_type U, typename V>
	std::string cipher_impl<T, U, V>::decrypt(const void* input, const size_t input_size) const
	{
		std::string output;

		if (has_iv())
		{
			typename V::Decryption decryptor(m_key, m_key.size(), m_iv);
			CryptoPP::StreamTransformationFilter filter(decryptor, new CryptoPP::StringSink(output));
			filter.Put(reinterpret_cast<const byte*>(input), input_size);
			filter.MessageEnd();
		}
		else if (input_size > T::BLOCKSIZE)
		{
			// IV
			CryptoPP::SecByteBlock iv(T::BLOCKSIZE);
			memcpy(iv.data(), input, iv.size());

			typename V::Decryption decryptor(m_key, m_key.size(), iv);
			CryptoPP::StreamTransformationFilter filter(decryptor, new CryptoPP::StringSink(output));
			filter.Put(reinterpret_cast<const byte*>(input) + iv.size(), input_size - iv.size());
			filter.MessageEnd();
		}

		return output;
	}

	// Set
	template <typename T, cipher_type U, typename V>
	void cipher_impl<T, U, V>::set_iv(const void* iv, const size_t iv_size)
	{
		assert(iv && (iv_size >= T::BLOCKSIZE));

		m_iv.Assign(reinterpret_cast<const byte*>(iv), iv_size);
	}

	// Get
	template <typename T, cipher_type U, typename V>
	const char* cipher_impl<T, U, V>::get_name()
	{
		switch (U)
		{
			case cipher_type::aes:		return cipher_name::aes;
			case cipher_type::mars:		return cipher_name::mars;
			case cipher_type::rc6:		return cipher_name::rc6;
			case cipher_type::serpent:	return cipher_name::serpent;
			case cipher_type::twofish:	return cipher_name::twofish;
			default:					return cipher_name::none;
		}
	}
}

#endif
