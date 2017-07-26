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

#ifndef _EJA_NETWORK_URI_
#define _EJA_NETWORK_URI_

#include <string>
#include <boost/format.hpp>
#include <cryptopp/secblock.h>

#include "http/http.h"
#include "system/type.h"
#include "utility/default_value.h"

namespace eja
{
	class uri final
	{
	private:
		std::string m_scheme = http::scheme;
		std::string m_address; 
		u16 m_port = http::port;
		std::string m_resource = http::root;

	public:
		uri() { }
		explicit uri(const std::string& path) { parse(path); }
		~uri() { }

		// Interface
		void clear();
		bool parse(const std::string& path);

		// Utility
		std::string str() const { return boost::str(boost::format("%s://%s:%s%s") % m_scheme % m_address % m_port % m_resource); }
		
		bool is_http() const { return m_scheme == http::scheme; }
		bool is_https() const { return m_scheme == http::scheme_secure; }
		bool has_address() const { return !m_address.empty(); }

		// Accessor
		const std::string& get_scheme() const { return m_scheme; }
		const std::string& get_address() const { return m_address; }
		u16 get_port() const { return m_port; }
		const std::string& get_resource() const { return m_resource; }

		// Encode
		static std::string encode(const byte* input, const size_t input_size);
		static std::string encode(const CryptoPP::SecByteBlock& input) { return encode(input.data(), input.size()); }
		static std::string encode(const std::string& input) { return encode(reinterpret_cast<const byte*>(input.c_str()), input.size()); }
		static std::string encode(const char* input) { return encode(reinterpret_cast<const byte*>(input), strlen(input)); }

		// Decode
		static std::string decode(const byte* input, const size_t input_size);
		static std::string decode(const CryptoPP::SecByteBlock& input) { return decode(input.data(), input.size()); }
		static std::string decode(const std::string& input) { return decode(reinterpret_cast<const byte*>(input.c_str()), input.size()); }
		static std::string decode(const char* input) { return decode(reinterpret_cast<const byte*>(input), strlen(input)); }
	};
}

#endif
