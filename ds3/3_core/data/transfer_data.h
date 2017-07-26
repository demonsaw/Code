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

#ifndef _EJA_CHUNK_DATA_H_
#define _EJA_CHUNK_DATA_H_

#include "data/data.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	// Version
	make_version(transfer_data, 1);

	// Message
	class transfer_data final : public data
	{
		make_factory(transfer_data);

	private:
		std::string m_data;		
		u32 m_index = 0;
		u32 m_size = 0;

	public:
		transfer_data() : data(data_type::transfer) { }
		transfer_data(const transfer_data& dat) : data(dat), m_data(dat.m_data), m_index(dat.m_index), m_size(dat.m_size) { }

		// Operator
		transfer_data& operator=(const transfer_data& dat);

		// Utility
		virtual bool valid() const { return has_data(); }

		// Has
		bool has_data() const { return !m_data.empty(); }
		bool has_size() const { return m_size > 0; }

		// Set
		void set_data(const std::string& data) { m_data = data; }
		void set_data(std::string&& data) { m_data = std::move(data); }
		void set_index(const u32 index) { m_index = index; }
		void set_size(const u32 size) { m_size = size; }

		// Get
		const std::string& get_data() const { return m_data; }
		std::string& get_data() { return m_data; }
		u32 get_index() const { return m_index; }
		u32 get_size() const { return m_size; }

		MSGPACK_DEFINE(m_data, m_index, m_size);
	};

	// Operator
	inline transfer_data& transfer_data::operator=(const transfer_data& dat)
	{
		if (this != &dat)
		{
			data::operator=(dat);

			m_data = dat.m_data;
			m_index = dat.m_index;
			m_size = dat.m_size;
		}

		return *this;
	}
}

#endif
