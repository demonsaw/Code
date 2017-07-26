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

#ifndef _EJA_JSON_KEY_
#define _EJA_JSON_KEY_

#include <memory>
#include <string>
#include <cryptopp/integer.h>
#include <cryptopp/secblock.h>

#include "json/json.h"
#include "json/json_lib.h"
#include "json/json_value.h"
#include "security/security.h"

namespace eja
{
	class json_key final : public json_value
	{
	public:
		using ptr = std::shared_ptr<json_key>;

	private:
		json_key() { }
		json_key(const std::string& input) { parse(input); }
		json_key(const char* input) { parse(input); }

		json_key(const json_value::ptr value) : json_value(value) { }
		json_key(const json_value::ptr value, const char* key) : json_value(value, key) { }

		json_key(const Json::Value& value) : json_value(value) { }
		json_key(const Json::Value& value, const char* key) : json_value(value, key) { }

	public:		
		virtual ~json_key() override { }

		// Utility
		virtual bool valid() const override { return json_value::valid() && has_public_key(); }

		bool has_size() const { return get_value().isMember(json::size) && !get_value()[json::size].empty() && get_size(); }
		bool has_base() const { return get_value().isMember(json::base) && !get_value()[json::base].empty() && get_base(); }
		bool has_prime() const { return get_value().isMember(json::prime) && !get_value()[json::prime].empty() && strlen(get_prime()); }
		bool has_public_key() const { return get_value().isMember(json::public_key) && !get_value()[json::public_key].empty() && strlen(get_public_key()); }		

		// Mutator
		void set_size(const size_t size) { get_value()[json::size] = static_cast<Json::Value::UInt64>(size); }

		void set_base(const CryptoPP::Integer& base) { get_value()[json::base] = static_cast<Json::Value::UInt64>(base.ConvertToLong()); }
		void set_base(const size_t base) { get_value()[json::base] = static_cast<Json::Value::UInt64>(base); }

		void set_prime(const byte* prime, const size_t prime_size) { get_value()[json::prime] = security::str(prime, prime_size); }
		void set_prime(const CryptoPP::SecByteBlock& prime) { get_value()[json::prime] = security::str(prime); }
		void set_prime(const CryptoPP::Integer& prime) { get_value()[json::prime] = security::str(prime); }
		void set_prime(const std::string& prime) { get_value()[json::prime] = prime; }
		void set_prime(const char* prime) { get_value()[json::prime] = prime; }

		void set_public_key(const byte* key, const size_t key_size) { get_value()[json::public_key] = security::str(key, key_size); }
		void set_public_key(const CryptoPP::SecByteBlock& key) { get_value()[json::public_key] = security::str(key); }
		void set_public_key(const std::string& key) { get_value()[json::public_key] = key; }
		void set_public_key(const char* key) { get_value()[json::public_key] = key; }

		// Accessor
		size_t get_size() const { return static_cast<size_t>(get_value()[json::size].asUInt64()); }
		size_t get_base() const { return static_cast<size_t>(get_value()[json::base].asUInt64()); }
		const char* get_prime() const { return get_value()[json::prime].asCString(); }
		const char* get_public_key() const { return get_value()[json::public_key].asCString(); }		

		// Static
		static ptr create() { return ptr(new json_key()); }
		static ptr create(const std::string& input) { return ptr(new json_key(input)); }
		static ptr create(const char* input) { return ptr(new json_key(input)); }

		static ptr create(const json_value::ptr value) { return ptr(new json_key(value)); }
		static ptr create(const json_value::ptr value, const char* key) { return ptr(new json_key(value, key)); }

		static ptr create(const Json::Value& value) { return ptr(new json_key(value)); }
		static ptr create(const Json::Value& value, const char* key) { return ptr(new json_key(value, key)); }
	};
}

#endif