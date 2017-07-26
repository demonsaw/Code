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

#ifndef _EJA_SEARCH_RESPONSE_DATA_H_
#define _EJA_SEARCH_RESPONSE_DATA_H_

#include "data/data.h"
#include "data/file_data.h"
#include "data/data_type.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	// Version
	make_version(search_response_data, 1);

	// Data
	class search_response_data final : public data
	{
		make_factory(search_response_data);

	private:
		file_data_list m_files;

	public:
		search_response_data() : data(data_type::search_response) { }
		search_response_data(const search_response_data& dat) : data(dat), m_files(dat.m_files) { }

		// Operator
		search_response_data& operator=(const search_response_data& dat);

		// Utility
		void add(const file_data::ptr file) { m_files.push_back(file); }

		// Has
		bool has_files() const { return !m_files.empty(); }

		// Set
		void set_files(const file_data_list& files) { m_files = files; }

		// Get
		const file_data_list& get_files() const { return m_files; }

		MSGPACK_DEFINE(m_files);
	};

	// Operator
	inline search_response_data& search_response_data::operator=(const search_response_data& dat)
	{
		if (this != &dat)
		{
			data::operator=(dat);

			m_files = dat.m_files;
		}

		return *this;
	}
}

#endif
