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

#ifndef _EJA_ARCHIVE_
#define _EJA_ARCHIVE_

#include <memory>
#include <string>
#include <boost/filesystem.hpp>

#include "object/object.h"
#include "system/mutex/mutex_list.h"
#include "system/mutex/mutex_map.h"
#include "system/mutex/mutex_queue.h"
#include "system/mutex/mutex_vector.h"
#include "system/type.h"

namespace eja
{
	class archive : public object
	{
	protected:
		boost::filesystem::path m_path;
		u64 m_size = 0;

	public:
		using ptr = std::shared_ptr<archive>;

	protected:
		archive() { }
		archive(const archive& archive) : object(archive), m_path(archive.m_path), m_size(archive.m_size) { }
		
		archive(const boost::filesystem::path& path) : m_path(path) { }
		archive(const std::string& path) : m_path(path) { }
		archive(const char* path) : m_path(path) { }
	
		archive(const boost::filesystem::path& path, const u64 size) : m_path(path), m_size(size) { }
		archive(const std::string& path, const u64 size) : m_path(path), m_size(size) { }
		archive(const char* path, const u64 size) : m_path(path), m_size(size) { }

	public:		
		virtual ~archive() override { }

		// Operator
		archive& operator=(const archive& archive);
		archive& operator=(const archive::ptr archive) { return operator=(*archive); }

		// Interface
		virtual void clear() override;

		// Utility
		void clean();		
		bool empty() const { return !has_path(); }
		virtual bool exists() const;
		virtual bool is_hidden() const = 0;

		bool has_path() const { return !m_path.empty(); }
		bool has_size() const { return m_size > 0; }

		// Mutator
		void set_path(const boost::filesystem::path& path) { m_path = path; }
		void set_path(const std::string& path) { m_path = path; }
		void set_path(const char* path) { m_path = path; }
		
		void set_size(const u64 size) { m_size = size; }
		void add_size(const u64 size) { m_size += size; }
		void sub_size(const u64 size) { m_size -= size; }

		// Accessor
		const boost::filesystem::path& get_data() const { return m_path; }
		boost::filesystem::path& get_data() { return m_path; }

		std::string get_path() const { return m_path.string(); }
		std::string get_name() const { return m_path.filename().string(); }
		std::string get_stem() const { return m_path.stem().string(); }
		std::string get_parent() const { return m_path.parent_path().string(); }
		u64 get_size() const { return m_size; }
	};

	// Container
	derived_type(archive_list, mutex_list<archive>);
	derived_type(archive_map, mutex_map<std::string, archive>);
	derived_type(archive_queue, mutex_queue<archive>);
	derived_type(archive_vector, mutex_vector<archive>);
}

#endif
