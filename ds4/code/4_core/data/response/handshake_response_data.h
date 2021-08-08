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

#ifndef _EJA_HANDSHAKE_RESPONSE_DATA_H_
#define _EJA_HANDSHAKE_RESPONSE_DATA_H_

#include <string>

#include "data/id_data.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	make_using_version(handshake_response_data, 1);

	class handshake_response_data final : public id_data
	{
		make_factory(handshake_response_data);

	private:
		std::string m_public_key;

	public:
		handshake_response_data() : id_data(data_type::handshake_response) { }

		// Utility
		virtual bool valid() const override { return id_data::valid() && has_public_key(); }

		// Has
		bool has_public_key() const { return !m_public_key.empty(); }

		// Set
		void set_public_key() { m_public_key.clear(); }
		void set_public_key(std::string&& key) { m_public_key = std::move(key); }
		void set_public_key(const std::string& key) { m_public_key = key; }

		// Get
		const std::string& get_public_key() const { return m_public_key; }

		MSGPACK_DEFINE(MSGPACK_BASE(id_data), m_public_key);
	};
}

#endif
