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

#ifndef _EJA_DOWNLOAD_REQUEST_DATA_H_
#define _EJA_DOWNLOAD_REQUEST_DATA_H_

#include <memory>
#include <set>

#include "data/id_data.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	// Version
	make_using_version(download_request_data, 1);

	// Data
	class download_request_data final : public id_data
	{
		make_factory(download_request_data);

		enum bits : u8 { none, remove = bit(1) };
		
	private:
		std::set<u32> m_chunks;

	public:
		download_request_data() : id_data(data_type::download_request) { }
		download_request_data(const u8 bits) : id_data(data_type::download_request, bits) { }
		
		// Has
		bool has_chunks() const { return !m_chunks.empty(); }

		// Is
		bool is_remove() const { return is_mode(bits::remove); }

		// Set
		template <typename T>
		void set_chunks(const T& chunks);
		
		void set_remove(const bool value) { set_mode(bits::remove, value); }

		// Get
		const std::set<u32>& get_chunks() const { return m_chunks; }
		std::set<u32>& get_chunks() { return m_chunks; }

		MSGPACK_DEFINE(MSGPACK_BASE(id_data), m_chunks);
	};

	// Set
	template <typename T>
	void download_request_data::set_chunks(const T& t)
	{ 
		m_chunks.clear();
		m_chunks.insert(t.begin(), t.end());
	}
}

#endif
