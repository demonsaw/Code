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

#include <boost/network/uri.hpp>

#include "component/group/group_security_component.h"
#include "security/cipher/cipher.h"
#include "security/hash/pbkdf.h"
#include "utility/value.h"
#include "utility/io/file_util.h"

namespace eja
{
	// Constructor
	group_security_component::group_security_component()
	{
		m_type = group::type;
		m_percent = group::percent;

		m_cipher = group::cipher;
		m_key_size = group::key_size;

		m_hash = group::hash;
		m_iterations = group::iterations;
	}

	// Operator
	group_security_component& group_security_component::operator=(const group_security_component& comp)
	{
		if (this != &comp)
		{
			security_component::operator=(comp);
			
			m_entropy = comp.m_entropy;			
			m_percent = comp.m_percent;
			m_type = comp.m_type;

			m_data = comp.m_data;
			m_size = comp.m_size;
		}

		return *this;
	}

	// Interface
	void group_security_component::clear()
	{
		security_component::clear();
		
		m_entropy.clear();		
		m_type = group_security_type::none;
		m_percent = 0;

		m_data.clear();
		m_size = 0;
	}

	// Set
	void group_security_component::set_data()
	{		
		m_data.clear();
		m_data.shrink_to_fit();
		//m_size = 0;
	}

	// Get
	cipher::ptr group_security_component::create_cipher() const
	{
		// PBKDF
		const auto pbkdf = pbkdf::create(get_hash());
		pbkdf->set_iterations(get_iterations());
		pbkdf->set_salt(get_salt());

		// Key
		const auto key_size = get_key_size() >> 3;
		const auto& data = has_data() ? get_data() : get_entropy();
		const auto key = pbkdf->compute(data, key_size);
		//const auto hex = hex::encode(key);

		// Cipher
		const auto cipher = cipher::create(get_cipher());
		cipher->set_key(key);

		return cipher;
	}
}
