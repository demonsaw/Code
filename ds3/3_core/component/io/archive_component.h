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

#ifndef _EJA_ARCHIVE_COMPONENT_H_
#define _EJA_ARCHIVE_COMPONENT_H_

#include <string>
#include <boost/filesystem.hpp>

#include "component/id_component.h"
#include "thread/thread_safe.h"
#include "system/type.h"

namespace eja
{
	make_using_component(archive);

	class archive_component : public id_component
	{
		make_factory(archive_component);

	protected:
		boost::filesystem::path m_path;		
		u64 m_size = 0;

	public:
		archive_component() { }
		archive_component(const archive_component& comp) : id_component(comp), m_path(comp.m_path), m_size(comp.m_size) { }

		archive_component(const boost::filesystem::path& path) : m_path(path) { }
		archive_component(const std::string& path) : m_path(path) { }
		archive_component(const char* path) : m_path(path) { }

		archive_component(const boost::filesystem::path& path, const u64 size) : m_path(path), m_size(size) { }
		archive_component(const std::string& path, const u64 size) : m_path(path), m_size(size) { }
		archive_component(const char* path, const u64 size) : m_path(path), m_size(size) { }

		// Operator
		archive_component& operator=(const archive_component& comp);

		// Interface
		virtual void clear() override;

		// Utility
		virtual bool valid() const override { return has_path(); }
		virtual bool hidden() const { return false; }
		virtual bool exists() const;
		void clean();

		// Has
		bool has_path() const { return !m_path.empty(); }
		bool has_size() const { return m_size > 0; }

		// Set
		void set_path(const boost::filesystem::path& path) { m_path = path; }
		void set_path(const std::string& path) { m_path = path; }
		void set_path(const char* path) { m_path = path; }

		void set_size(const u64 size) { m_size = size; }
		void add_size(const u64 size) { m_size += size; }
		void sub_size(const u64 size) { m_size -= size; }

		// Get		
		const boost::filesystem::path& get_data() const { return m_path; }
		boost::filesystem::path& get_data() { return m_path; }
		u64 get_size() const { return m_size; }

		std::string get_path() const { return m_path.string(); }
		std::string get_name() const { return m_path.filename().string(); }
		std::string get_stem() const { return m_path.stem().string(); }
		std::string get_folder() const { return m_path.parent_path().string(); }

		// Static
		static ptr create(const boost::filesystem::path& path) { return ptr(new archive_component(path)); }
		static ptr create(const std::string& path) { return ptr(new archive_component(path)); }
		static ptr create(const char* path) { return ptr(new archive_component(path)); }

		static ptr create(const boost::filesystem::path& path, const u64 size) { return ptr(new archive_component(path, size)); }
		static ptr create(const std::string& path, const u64 size) { return ptr(new archive_component(path, size)); }
		static ptr create(const char* path, const u64 size) { return ptr(new archive_component(path, size)); }
	};
}

#endif
