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

#ifndef _EJA_FILE_COMPONENT_H_
#define _EJA_FILE_COMPONENT_H_

#include <list>
#include <map>
#include <string>

#include "component/io/archive_component.h"
#include "thread/thread_safe.h"
#include "system/type.h"

namespace eja
{
	class entity;	

	make_using_component(file);
	make_thread_safe_component(file_list_component, std::list<std::shared_ptr<entity>>);
	make_thread_safe_component(file_map_component, std::map<std::string, std::shared_ptr<entity>>);
	make_thread_safe_component(file_multimap_component, std::multimap<std::string, std::shared_ptr<entity>>);

	class file_component final : public archive_component
	{
		make_factory(file_component);

	public:
		file_component() { }
		file_component(const file_component& comp) : archive_component(comp) { }

		file_component(const boost::filesystem::path& path) : archive_component(path) { set_size(); }
		file_component(const std::string& path) : archive_component(path) { set_size(); }
		file_component(const char* path) : archive_component(path) { set_size(); }

		file_component(const boost::filesystem::path& path, const u64 size) : archive_component(path, size) { }
		file_component(const std::string& path, const u64 size) : archive_component(path, size) { }
		file_component(const char* path, const u64 size) : archive_component(path, size) { }

		// Operator
		file_component& operator=(const file_component& comp);
		file_component& operator=(const boost::filesystem::path& path);
		file_component& operator=(const std::string& path) { return operator=(boost::filesystem::path(path)); }
		file_component& operator=(const char* path) { return operator=(boost::filesystem::path(path)); }

		// Utility
		virtual bool exists() const override { return archive_component::exists() && !boost::filesystem::is_directory(m_path); }
		virtual bool hidden() const override;
		bool system() const;

		std::string read(u64 input_size = 0, u64 offset = 0) const;
		u64 read(std::string& input, u64 input_size = 0, u64 offset = 0) const;

		u64 write(const char* output, const u64 output_size, u64 offset = 0) const;
		u64 write(const std::string& output, u64 offset = 0) const { return write(output.c_str(), output.size(), offset); }
		u64 write(const char* output, u64 offset = 0) const { return write(output, strlen(output), offset); }

		u64 append(const char* output, const u64 output_size);
		u64 append(const std::string& output) { return append(output.c_str(), output.size()); }
		u64 append(const char* output) { return append(output, strlen(output)); }

		// Set
		using archive_component::set_size;
		void set_size() { m_size = exists() ? boost::filesystem::file_size(m_path) : 0; }		

		// Get		
		std::string get_suffix() const { return m_path.extension().string(); }
		std::string get_description() const;
		std::string get_extension() const;		

		// Static
		static ptr create(const boost::filesystem::path& path) { return ptr(new file_component(path)); }
		static ptr create(const std::string& path) { return ptr(new file_component(path)); }
		static ptr create(const char* path) { return ptr(new file_component(path)); }

		static ptr create(const boost::filesystem::path& path, const u64 size) { return ptr(new file_component(path, size)); }
		static ptr create(const std::string& path, const u64 size) { return ptr(new file_component(path, size)); }
		static ptr create(const char* path, const u64 size) { return ptr(new file_component(path, size)); }
	};
}

#endif
