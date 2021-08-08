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

#ifndef _EJA_EXCLUDE_H_
#define _EJA_EXCLUDE_H_

#include <string>
#include <cryptopp/integer.h>
#include <cryptopp/secblock.h>

#include "security/security.h"

namespace eja
{
	class exclusive final
	{
	public:
		static const size_t default_size = 16;

	private:
		exclusive() = delete;
		exclusive(const exclusive&) = delete;
		~exclusive() = delete;

		// Operator
		exclusive& operator=(const exclusive&) = delete;

	public:
		// Compute
		static std::string compute(const void* input, const size_t input_size, const void* aux, const size_t aux_size);
		static std::string compute(const CryptoPP::Integer& input, const CryptoPP::Integer& aux) { return compute(security::str(input), security::str(aux)); }
		static std::string compute(const CryptoPP::SecByteBlock& input, const CryptoPP::SecByteBlock& aux) { return compute(input.data(), input.size(), aux.data(), aux.size()); }

		static std::string compute(const std::shared_ptr<std::wstring>& input, const std::shared_ptr<std::wstring>& aux) { return compute(input->c_str(), input->size() * sizeof(wchar_t), aux->c_str(), aux->size() * sizeof(wchar_t)); }
		static std::string compute(const std::wstring& input, const std::wstring& aux) { return compute(input.c_str(), input.size() * sizeof(wchar_t), aux.c_str(), aux.size() * sizeof(wchar_t)); }
		static std::string compute(const wchar_t* input, const wchar_t* aux) { return compute(input, wcslen(input), aux, wcslen(aux)); }

		static std::string compute(const std::shared_ptr<std::string>& input, const std::shared_ptr<std::string>& aux) { return compute(input->c_str(), input->size(), aux->c_str(), aux->size()); }
		static std::string compute(const std::string& input, const std::string& aux) { return compute(input.c_str(), input.size(), aux.c_str(), aux.size()); }
		static std::string compute(const char* input, const char* aux) { return compute(input, strlen(input), aux, strlen(aux)); }
	};
}

#endif
