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

#include "component/verified/verified_option_component.h"

#include "security/hash/hash.h"
#include "security/hash/pbkdf.h"

namespace eja
{
	// Constructor
	verified_option_component::verified_option_component()
	{
		m_hash = hash_name::sha1;
		m_iterations = pbkdf::get_min_iterations(); 
		m_key_size = security::get_size();
	}

	// Interface
	void verified_option_component::clear()
	{
		component::clear();

		m_hash = hash_name::sha1;
		m_iterations = pbkdf::get_min_iterations();
		m_key.clear();
		m_key_size = security::get_size();
		m_salt.clear();
	}
}
