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

#ifndef _EJA_UPLOAD_RESPONSE_DATA_H_
#define _EJA_UPLOAD_RESPONSE_DATA_H_

#include "data/data.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	make_using_version(upload_response_data, 1);

	class upload_response_data final : public data
	{
		make_factory(upload_response_data);

	private:
		u32 m_sequence = static_cast<u32>(~0);

	public:
		upload_response_data() : data(data_type::upload_response) { }

		// Has
		bool has_sequence() const { return m_sequence < static_cast<u32>(~0); }

		// Set
		void set_sequence(const u32 sequence) { m_sequence = sequence; }

		// Get
		u32 get_sequence() const { return m_sequence; }

		MSGPACK_DEFINE(MSGPACK_BASE(data), m_sequence);
	};
}

#endif
