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

#ifndef _EJA_FILE_REQUEST_DATA_H_
#define _EJA_FILE_REQUEST_DATA_H_

#include "data/data.h"
#include "data/data_type.h"
#include "data/router_data.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	// Version
	make_version(file_request_data, 1);

	// Data
	class file_request_data final : public data
	{
		make_factory(file_request_data);

	private:		
		std::string m_file;		
		router_data_list m_routers;
		u32 m_chunk = 0;

	public:
		file_request_data() : data(data_type::file_request) { }
		file_request_data(const file_request_data& dat) : data(dat), m_chunk(dat.m_chunk), m_file(dat.m_file), m_routers(dat.m_routers) { }

		// Operator
		file_request_data& operator=(const file_request_data& dat);

		// Utility
		virtual bool valid() const { return has_chunk() && has_file() && has_routers(); }

		void add(const router_data::ptr router) { m_routers.push_back(router); }

		// Has
		bool has_chunk() const { return m_chunk > 0; }
		bool has_file() const { return !m_file.empty(); }
		bool has_routers() const { return !m_routers.empty(); }

		// Set
		void set_chunk(const u32 chunk) { m_chunk = chunk; }
		void set_file(const std::string& file) { m_file = file; }
		void set_routers(const router_data_list& routers) { m_routers = routers; }

		// Get
		u32 get_chunk() const { return m_chunk; }
		const std::string& get_file() const { return m_file; }
		const router_data_list& get_routers() const { return m_routers; }

		MSGPACK_DEFINE(m_file, m_routers, m_chunk);
	};

	// Operator
	inline file_request_data& file_request_data::operator=(const file_request_data& dat)
	{
		if (this != &dat)
		{
			data::operator=(dat);
			
			m_chunk = dat.m_chunk;
			m_file = dat.m_file;
			m_routers = dat.m_routers;			
		}

		return *this;
	}
}

#endif
