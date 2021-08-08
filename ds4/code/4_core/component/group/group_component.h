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

#ifndef _EJA_GROUP_COMPONENT_H_
#define _EJA_GROUP_COMPONENT_H_

#include <deque>
#include <map>
#include <string>
#include <cryptopp/secblock.h>

#include "component/component.h"
#include "system/type.h"
#include "thread/thread_safe.h"

namespace eja
{
	class entity;
	class entity_map_component;
	class cipher;

	make_thread_safe_component(group_list_component, std::deque<std::shared_ptr<entity>>);	
	make_thread_safe_component(group_map_component, std::map<std::string, std::shared_ptr<entity>>);

	class group_component final : public thread_safe_ex<component, std::list<std::shared_ptr<cipher>>>
	{
		make_factory(group_component);

	public:
		group_component() { }

		// Utility
		virtual bool valid() const override { return has_id() && !empty(); }

		// Encrypt
		std::string encrypt(const void* input, const size_t input_size) const;
		std::string encrypt(const CryptoPP::SecByteBlock& input) const { return encrypt(input.data(), input.size()); }
		std::string encrypt(const string_ptr& input) const { return encrypt(input->c_str(), input->size()); }
		std::string encrypt(const std::string& input) const { return encrypt(input.c_str(), input.size()); }
		std::string encrypt(const char* input) const { return encrypt(input, strlen(input)); }

		// Decrypt
		std::string decrypt(const void* input, const size_t input_size) const;
		std::string decrypt(const CryptoPP::SecByteBlock& input) const { return decrypt(input.data(), input.size()); }
		std::string decrypt(const string_ptr& input) const { return decrypt(input->c_str(), input->size()); }
		std::string decrypt(const std::string& input) const { return decrypt(input.c_str(), input.size()); }
		std::string decrypt(const char* input) const { return decrypt(input, strlen(input)); }
	};
}

#endif
