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

#include "group.h"
#include "security/hex.h"

namespace eja
{
	// Interface
	void group::clear()
	{
		object::clear();

		m_name.clear();
	}

	// Mutator
	void group::set_id()
	{
		object::set_id();

		set_name();
	}

	void group::set_id(const std::nullptr_t null)
	{
		object::set_id(null);

		set_name();
	}

	void group::set_id(const byte* id, const size_t size)
	{
		object::set_id(id, size);

		set_name();
	}

	void group::set_id(const std::string& id)
	{
		object::set_id(id);

		set_name();
	}

	void group::set_id(const char* id)
	{
		object::set_id(id);

		set_name();
	}

	void group::set_name()
	{
		m_name = !m_id.empty() ? hex::encode(m_id, hex::default_size) : default_group::name;
	}
}
