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

#ifndef _EJA_FILE_
#define _EJA_FILE_

#include <memory>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>

#include "archive.h"
#include "system/mutex/mutex_list.h"
#include "system/mutex/mutex_map.h"
#include "system/mutex/mutex_queue.h"
#include "system/mutex/mutex_vector.h"
#include "system/type.h"

namespace eja
{
	// Filter
	enum file_filter { none, audio, image, video };

	// Object
	class file : public archive
	{
	public:		
		using ptr = std::shared_ptr<file>;

	public:
		file() { }
		file(const file& file) : archive(file) { }

		file(const boost::filesystem::path& path) : archive(path) { set_size(); }
		file(const std::string& path) : archive(path) { set_size(); }
		file(const char* path) : archive(path) { set_size(); }
		
		file(const boost::filesystem::path& path, const u64 size) : archive(path, size) { }
		file(const std::string& path, const u64 size) : archive(path, size) { }
		file(const char* path, const u64 size) : archive(path, size) { }
		
		virtual ~file() override { }

		// Operator
		file& operator=(const file& file);
		file& operator=(const file::ptr file) { return operator=(*file); }
		file& operator=(const boost::filesystem::path& path);
		file& operator=(const std::string& path) { return operator=(boost::filesystem::path(path)); }
		file& operator=(const char* path) { return operator=(boost::filesystem::path(path)); }

		// Utility
		virtual bool exists() const override { return archive::exists() && !boost::filesystem::is_directory(m_path); }
		virtual bool is_hidden() const override;
		bool is_config() const;

		std::string read(u64 input_size = 0, u64 offset = 0) const;
		u64 read(std::string& input, u64 input_size = 0, u64 offset = 0) const;

		u64 write(const byte* output, const u64 output_size, u64 offset = 0) const;
		u64 write(const std::string& output, u64 offset = 0) const { return write(reinterpret_cast<const byte*>(output.c_str()), output.size(), offset); }
		u64 write(const char* output, u64 offset = 0) const { return write(reinterpret_cast<const byte*>(output), strlen(output), offset); }

		u64 append(const byte* output, const u64 output_size);
		u64 append(const std::string& output) { return append(reinterpret_cast<const byte*>(output.c_str()), output.size()); }
		u64 append(const char* output) { return append(reinterpret_cast<const byte*>(output), strlen(output)); }

		// Mutator
		using archive::set_size;
		void set_size() { m_size = exists() ? boost::filesystem::file_size(m_path) : 0; }

		// Accessor		
		std::string get_suffix() const { return m_path.extension().string(); }
		std::string get_description() const;
		std::string get_extension() const;

		// Static
		static ptr create() { return std::make_shared<file>(); }
		static ptr create(const file& file) { return std::make_shared<eja::file>(file); }
		static ptr create(const file::ptr file) { return std::make_shared<eja::file>(*file); }
		static ptr create(const boost::filesystem::path& path) { return std::make_shared<file>(path); }
		static ptr create(const std::string& path) { return std::make_shared<file>(path); }
		static ptr create(const char* path) { return std::make_shared<file>(path); }
		static ptr create(const boost::filesystem::path& path, const u64 size) { return std::make_shared<file>(path, size); }
		static ptr create(const std::string& path, const u64 size) { return std::make_shared<file>(path, size); }
		static ptr create(const char* path, const u64 size) { return std::make_shared<file>(path, size); }
	};

	// Container
	derived_type(file_list, mutex_list<file>);
	derived_type(file_map, mutex_map<std::string, file>);
	derived_type(file_queue, mutex_queue<file>);
	derived_type(file_vector, mutex_vector<file>);
}

#endif
