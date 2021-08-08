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

#ifndef _EJA_BROWSE_RESPONSE_DATA_H_
#define _EJA_BROWSE_RESPONSE_DATA_H_

#include "data/data.h"
#include "data/file_data.h"
#include "data/folder_data.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	make_using_version(browse_response_data, 1);

	class browse_response_data final : public data
	{
		make_factory(browse_response_data);

	private:
		file_data_list m_files;
		folder_data_list m_folders;

	public:
		browse_response_data() : data(data_type::browse_response) { }

		// Interface
		virtual void clear() override { m_files.clear(); m_folders.clear(); }

		// Utility
		virtual bool valid() const override { return data::valid() && (has_files() || has_folders()); }		

		void add(const file_data::ptr file) { m_files.push_back(file); }
		void add(const folder_data::ptr folder) { m_folders.push_back(folder); }		

		// Has
		bool has_files() const { return !m_files.empty(); }
		bool has_folders() const { return !m_folders.empty(); }

		// Set
		void set_files(const file_data_list& files) { m_files = files; }
		void set_folders(const folder_data_list& folders) { m_folders = folders; }

		// Get
		const file_data_list& get_files() const { return m_files; }
		const folder_data_list& get_folders() const { return m_folders; }

		size_t get_size() const { return get_file_size() + get_folder_size(); }
		size_t get_file_size() const { return m_files.size(); }
		size_t get_folder_size() const { return m_folders.size(); }

		MSGPACK_DEFINE(MSGPACK_BASE(data), m_files, m_folders);
	};
}

#endif
