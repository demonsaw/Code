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

#ifndef _EJA_FILE_DATA_H_
#define _EJA_FILE_DATA_H_

#include <list>
#include <memory>

#include "data/id_data.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	// Version
	make_version(file_data, 1);

	// Container
	using file_data_list = std::list<std::shared_ptr<file_data>>;

	// Message
	class file_data final : public id_data
	{
		make_factory(file_data);

	private:		
		std::string m_name;
		u64 m_size = 0;
		u32 m_weight = 0;

	public:
		file_data() { }
		file_data(const file_data& dat) : id_data(dat), m_name(dat.m_name), m_size(dat.m_size), m_weight(dat.m_weight) { }

		// Operator
		file_data& operator=(const file_data& dat);

		// Utility
		virtual bool valid() const { return has_name(); }

		void add_weight(const u32 weight = 1) { m_weight += weight; }
		void sub_weight(const u32 weight = 1) { m_weight -= weight; }

		// Has
		bool has_name() const { return !m_name.empty(); }
		bool has_size() const { return m_size != 0; }
		bool has_weight() const { return m_weight > 0; }

		// Set
		void set_name(const std::string& name) { m_name = name; }
		void set_size(const u64 size) { m_size = size; }
		void set_weight(const u32 weight) { m_weight = weight; }

		// Get
		const std::string& get_name() const { return m_name; }
		u64 get_size() const { return m_size; }
		u32 get_weight() const { return m_weight; }

		MSGPACK_DEFINE(m_id, m_name, m_size, m_weight);
	};

	// Operator
	inline file_data& file_data::operator=(const file_data& dat)
	{
		if (this != &dat)
		{
			id_data::operator=(dat);

			m_name = dat.m_name;
			m_size = dat.m_size;
			m_weight = dat.m_weight;
		}			

		return *this;
	}
}

#endif
