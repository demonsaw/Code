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

#pragma once

#ifndef _EJA_MESSAGE_BUFFER_H_
#define _EJA_MESSAGE_BUFFER_H_

#include "system/type.h"

namespace eja
{
	class message_buffer final : public std::string
	{
		make_factory(message_buffer);

	public:
		message_buffer() { }
		message_buffer(const size_t bytes) { reserve(bytes); }
		message_buffer(const std::string& input) : std::string(input) { }
		message_buffer(std::string&& input) : std::string(std::move(input)) { }

		// Write
		void write(const void* input, const size_t input_size) { append(reinterpret_cast<const char*>(input), input_size); }
		void write(const std::string& input) { write(input.data(), input.size()); }
		void write(const char* input) { write(input, strlen(input)); }

		// Create
		static ptr create(const size_t bytes) { return std::make_shared<message_buffer>(bytes); }
		static ptr create(const std::string& input) { return std::make_shared<message_buffer>(input); }
		static ptr create(std::string&& input) { return std::make_shared<message_buffer>(input); }
	};
}

#endif
