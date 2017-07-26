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

#include "component/group/group_component.h"
#include "security/cipher/cipher.h"

namespace eja
{
	// Operator
	group_component& group_component::operator=(const group_component& comp)
	{
		if (this != &comp)
			thread_safe_ex::operator=(comp);
		
		return *this;
	}

	// Encrypt
	std::string group_component::encrypt(const void* input, const size_t input_size) const
	{
		std::string output(reinterpret_cast<const char*>(input), input_size);

		thread_lock(this);

		for (auto it = begin(); it != end(); ++it)
		{
			const auto& cipher = *it;
			output = cipher->encrypt(output);
		}			

		return output;
	}

	// Decrypt
	std::string group_component::decrypt(const void* input, const size_t input_size) const
	{
		std::string output(reinterpret_cast<const char*>(input), input_size);

		thread_lock(this);

		for (auto it = rbegin(); it != rend(); ++it)
		{
			const auto& cipher = *it;
			output = cipher->decrypt(output);
		}

		return output;
	}
}
