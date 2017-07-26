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

#ifndef _EJA_GROUP_CIPHER_
#define _EJA_GROUP_CIPHER_

#include <string>

#include "object/cipher.h"
#include "system/type.h"

namespace eja
{
	class group_cipher : public cipher
	{
	protected:
		std::string m_data;

	public:
		using ptr = std::shared_ptr<group_cipher>;

	public:
		group_cipher() : cipher() { }
		group_cipher(const eja::group_cipher& cipher) : eja::cipher(cipher), m_data(cipher.m_data) { }
		group_cipher(const size_t size) : cipher(size) { }
		group_cipher(const std::string& id) : cipher(id) { }
		group_cipher(const char* id) : cipher(id) { }
		virtual ~group_cipher() override { }

		// Utility
		bool has_data() const { return !m_data.empty(); }

		// Mutator
		void set_data(const std::string& data) { m_data = data; }
		void set_data(const char* data) { m_data = data; }
		void set_data() { m_data.clear(); }

		// Accessor
		const std::string& get_data() const { return m_data; }

		// Static
		static ptr create() { return std::make_shared<group_cipher>(); }
		static ptr create(const eja::group_cipher& group_cipher) { return std::make_shared<eja::group_cipher>(group_cipher); }
		static ptr create(const group_cipher::ptr group_cipher) { return std::make_shared<eja::group_cipher>(*group_cipher); }
		static ptr create(const std::string& id) { return std::make_shared<group_cipher>(id); }
		static ptr create(const char* id) { return std::make_shared<group_cipher>(id); }

	};
}

#endif
