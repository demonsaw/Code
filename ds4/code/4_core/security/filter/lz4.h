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

#ifndef _EJA_LZ4_H_
#define _EJA_LZ4_H_

#include <string>
#include <cryptopp/secblock.h>

namespace eja
{
	class lz4 final
	{
	private:
		lz4() = delete;
		lz4(const lz4&) = delete;
		~lz4() = delete;

		// Operator
		lz4& operator=(const lz4&) = delete;

	public:
		// Encode
		template <typename T>
		static std::string encode(const T& t) { return encode(&t, sizeof(T)); }
		static std::string encode(const void* input, const size_t input_size);
		static std::string encode(const CryptoPP::SecByteBlock& input) { return encode(input.data(), input.size()); }

		static std::string encode(const std::shared_ptr<std::wstring>& input) { return encode(input->c_str(), input->size() * sizeof(wchar_t)); }
		static std::string encode(const std::wstring& input) { return encode(input.c_str(), input.size() * sizeof(wchar_t)); }
		static std::string encode(const wchar_t* input) { return encode(input, wcslen(input)); }

		static std::string encode(const std::shared_ptr<std::string>& input) { return encode(input->c_str(), input->size()); }
		static std::string encode(const std::string& input) { return encode(input.c_str(), input.size()); }
		static std::string encode(const char* input) { return encode(input, strlen(input)); }

		// Decode		
		template <typename T>
		static std::string decode(const T& t) { return decode(&t, sizeof(T)); }
		static std::string decode(const void* input, const size_t input_size);
		static std::string decode(const CryptoPP::SecByteBlock& input) { return decode(input.data(), input.size()); }

		static std::string decode(const std::shared_ptr<std::wstring>& input) { return decode(input->c_str(), input->size() * sizeof(wchar_t)); }
		static std::string decode(const std::wstring& input) { return decode(input.c_str(), input.size() * sizeof(wchar_t)); }
		static std::string decode(const wchar_t* input) { return decode(input, wcslen(input)); }

		static std::string decode(const std::shared_ptr<std::string>& input) { return decode(input->c_str(), input->size()); }
		static std::string decode(const std::string& input) { return decode(input.c_str(), input.size()); }
		static std::string decode(const char* input) { return decode(input, strlen(input)); }
	};

	//class lz4hc final
	//{
	//private:
	//	lz4hc() = delete;
	//	lz4hc(const lz4hc&) = delete;
	//	~lz4hc() = delete;

	//	// Operator
	//	lz4hc& operator=(const lz4hc&) = delete;

	//public:
	//	// Encode
	//	template <typename T>
	//	static std::string encode(const T& t) { return encode(&t, sizeof(T)); }
	//	static std::string encode(const void* input, const size_t input_size);
	//	static std::string encode(const CryptoPP::SecByteBlock& input) { return encode(input.data(), input.size()); }
	//
	//	static std::string encode(const std::shared_ptr<std::wstring>& input) { return encode(input->c_str(), input->size() * sizeof(wchar_t)); }
	//	static std::string encode(const std::wstring& input) { return encode(input.c_str(), input.size() * sizeof(wchar_t)); }
	//	static std::string encode(const wchar_t* input) { return encode(input, wcslen(input)); }
	//
	//	static std::string encode(const std::shared_ptr<std::string>& input) { return encode(input->c_str(), input->size()); }
	//	static std::string encode(const std::string& input) { return encode(input.c_str(), input.size()); }
	//	static std::string encode(const char* input) { return encode(input, strlen(input)); }

	//	// Decode		
	//	template <typename T>
	//	static std::string decode(const T& t) { return decode(&t, sizeof(T)); }
	//	static std::string decode(const void* input, const size_t input_size) { return lz4::decode(input, input_size); }
	//	static std::string decode(const CryptoPP::SecByteBlock& input) { return decode(input.data(), input.size()); }
	//
	//	static std::string decode(const std::shared_ptr<std::wstring>& input) { return decode(input->c_str(), input->size() * sizeof(wchar_t)); }
	//	static std::string decode(const std::wstring& input) { return decode(input.c_str(), input.size() * sizeof(wchar_t)); }
	//	static std::string decode(const wchar_t* input) { return decode(input, wcslen(input)); }

	//	static std::string decode(const std::shared_ptr<std::string>& input) { return decode(input->c_str(), input->size()); }
	//	static std::string decode(const std::string& input) { return decode(input.c_str(), input.size()); }
	//	static std::string decode(const char* input) { return decode(input, strlen(input)); }
	//};
}

#endif