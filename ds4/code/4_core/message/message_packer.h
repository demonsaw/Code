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

#ifndef _EJA_PACKER_COMPONENT_H_
#define _EJA_PACKER_COMPONENT_H_

#include <list>
#include <string>

#include "component/component.h"
#include "message/message.h"
#include "system/type.h"

namespace eja
{
	class entity;

	class message_packer final : public message_list
	{
		make_factory(message_packer);

	public:
		message_packer() { }
		message_packer(const message_packer& comp) : message_list(comp) { }
		template <typename T> message_packer(const T& t) { push_back(t); }

		// Operator
		message_packer& operator=(const message_packer& comp);
		template <typename T> message_packer& operator=(const T& t);

		// Utility
		bool valid() const { return !empty(); }
		bool empty() const { return message_list::empty(); }

		std::string pack(const size_t size = 128) const;

		// Static
		template <typename T>
		static ptr create(const T& t);
	};

	// Operator
	template <typename T>
	message_packer& message_packer::operator=(const T& t)
	{
		assign(1, t);

		return *this;
	}

	// Static
	template <typename T>
	message_packer::ptr message_packer::create(const T& t)
	{
		const auto packer = std::make_shared<message_packer>();
		packer->push_back(t);

		return packer;
	}
}

#endif