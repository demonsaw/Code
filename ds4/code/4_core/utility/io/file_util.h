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

#ifndef _EJA_FILE_UTIL_H_
#define _EJA_FILE_UTIL_H_

#include <memory>
#include <string>
#include <boost/filesystem.hpp>

#include "component/io/file_component.h"
#include "system/type.h"

namespace eja
{
	class hash;

	class file_util final
	{
	private:
		file_util() = delete;
		~file_util() = delete;

	public:
		// Utility
		static void create(const boost::filesystem::path& path, const u64 bytes = 0);
		static void create(const std::wstring& path, const u64 bytes = 0) { return create(boost::filesystem::path(path), bytes); }
		static void create(const std::string& path, const u64 bytes = 0) { return create(boost::filesystem::path(path), bytes); }
		static void create(const char* path, const u64 bytes = 0) { return create(boost::filesystem::path(path), bytes); }
		static void create(const file_component::ptr file, const u64 bytes = 0) { return create(file->get_data(), bytes); }
		static void create(const file_component& file, const u64 bytes = 0) { return create(file.get_data(), bytes); }

		static void clear(const boost::filesystem::path& path) { resize(path, 0); }
		static void clear(const std::wstring& path) { clear(boost::filesystem::path(path)); }
		static void clear(const std::string& path) { clear(boost::filesystem::path(path)); }
		static void clear(const char* path) { clear(boost::filesystem::path(path)); }
		static void clear(const file_component::ptr file) { clear(file->get_data()); }
		static void clear(const file_component& file) { clear(file.get_data()); }

		static void copy(const boost::filesystem::path& source, const boost::filesystem::path& target) { boost::filesystem::copy_file(source, target, boost::filesystem::copy_option::overwrite_if_exists); }
		static void copy(const std::wstring& source, const std::wstring& target) { copy(boost::filesystem::path(source), boost::filesystem::path(target)); }
		static void copy(const std::string& source, const std::string& target) { copy(boost::filesystem::path(source), boost::filesystem::path(target)); }
		static void copy(const char* source, const char* target) { copy(boost::filesystem::path(source), boost::filesystem::path(target)); }
		static void copy(const file_component::ptr source, const file_component::ptr target) { copy(source->get_data(), target->get_data()); }
		static void copy(const file_component& source, const file_component& target) { copy(source.get_data(), target.get_data()); }

		static bool empty(const boost::filesystem::path& path) { return file_util::size(path) == 0; }
		static bool empty(const std::wstring& path) { return empty(boost::filesystem::path(path)); }
		static bool empty(const std::string& path) { return empty(boost::filesystem::path(path)); }
		static bool empty(const char* path) { return empty(boost::filesystem::path(path)); }
		static bool empty(const file_component::ptr file) { return empty(file->get_data()); }
		static bool empty(const file_component& file) { return empty(file.get_data()); }

		static bool exists(const boost::filesystem::path& path);
		static bool exists(const std::wstring& path) { return exists(boost::filesystem::path(path)); }
		static bool exists(const std::string& path) { return exists(boost::filesystem::path(path)); }
		static bool exists(const char* path) { return exists(boost::filesystem::path(path)); }
		static bool exists(const file_component::ptr file) { return exists(file->get_data()); }
		static bool exists(const file_component& file) { return exists(file.get_data()); }

		static void move(const boost::filesystem::path& source, const boost::filesystem::path& target) { boost::filesystem::rename(source, target); }
		static void move(const std::wstring& source, const std::wstring& target) { move(boost::filesystem::path(source), boost::filesystem::path(target)); }
		static void move(const std::string& source, const std::string& target) { move(boost::filesystem::path(source), boost::filesystem::path(target)); }
		static void move(const char* source, const char* target) { move(boost::filesystem::path(source), boost::filesystem::path(target)); }
		static void move(const file_component::ptr source, const file_component::ptr target) { move(source->get_data(), target->get_data()); }
		static void move(const file_component& source, const file_component& target) { move(source.get_data(), target.get_data()); }

		static void remove(const boost::filesystem::path& path) { boost::filesystem::remove(path); }
		static void remove(const std::wstring& path) { remove(boost::filesystem::path(path)); }
		static void remove(const std::string& path) { remove(boost::filesystem::path(path)); }
		static void remove(const char* path) { remove(boost::filesystem::path(path)); }
		static void remove(const file_component::ptr file) { remove(file->get_data()); }
		static void remove(const file_component& file) { remove(file.get_data()); }

		static void resize(const boost::filesystem::path& path, const u64 bytes = 0) { boost::filesystem::resize_file(path, bytes); }
		static void resize(const std::wstring& path, const u64 bytes = 0) { resize(boost::filesystem::path(path), bytes); }
		static void resize(const std::string& path, const u64 bytes = 0) { resize(boost::filesystem::path(path), bytes); }
		static void resize(const char* path, const u64 bytes = 0) { resize(boost::filesystem::path(path), bytes); }
		static void resize(const file_component::ptr file, const u64 bytes = 0) { resize(file->get_data(), bytes); }
		static void resize(const file_component& file, const u64 bytes = 0) { resize(file.get_data(), bytes); }

		static u64 size(const boost::filesystem::path& path);
		static u64 size(const std::wstring& path) { return size(boost::filesystem::path(path)); }
		static u64 size(const std::string& path) { return size(boost::filesystem::path(path)); }
		static u64 size(const char* path) { return size(boost::filesystem::path(path)); }
		static u64 size(const file_component::ptr file) { return size(file->get_data()); }
		static u64 size(const file_component& file) { return size(file.get_data()); }

		// Read
		static std::string read(const boost::filesystem::path& path, u64 input_size = 0, u64 offset = 0);
		static std::string read(const std::wstring& path, u64 input_size = 0, u64 offset = 0) { return read(boost::filesystem::path(path), input_size, offset); }
		static std::string read(const std::string& path, u64 input_size = 0, u64 offset = 0) { return read(boost::filesystem::path(path), input_size, offset); }
		static std::string read(const char* path, u64 input_size = 0, u64 offset = 0) { return read(boost::filesystem::path(path), input_size, offset); }
		static std::string read(const file_component::ptr file, u64 input_size = 0, u64 offset = 0) { return read(file->get_data(), input_size, offset); }
		static std::string read(const file_component& file, u64 input_size = 0, u64 offset = 0) { return read(file.get_data(), input_size, offset); }

		static u64 read(const boost::filesystem::path& path, std::string& input, u64 input_size = 0, u64 offset = 0);
		static u64 read(const std::wstring& path, std::string& input, u64 input_size = 0, u64 offset = 0) { return read(boost::filesystem::path(path), input, input_size, offset); }
		static u64 read(const std::string& path, std::string& input, u64 input_size = 0, u64 offset = 0) { return read(boost::filesystem::path(path), input, input_size, offset); }
		static u64 read(const char* path, std::string& input, u64 input_size = 0, u64 offset = 0) { return read(boost::filesystem::path(path), input, input_size, offset); }
		static u64 read(const file_component::ptr file, std::string& input, u64 input_size = 0, u64 offset = 0) { return read(file->get_data(), input, input_size, offset); }
		static u64 read(const file_component& file, std::string& input, u64 input_size = 0, u64 offset = 0) { return read(file.get_data(), input, input_size, offset); }

		// Write
		static u64 write(const boost::filesystem::path& path, const byte* output, const u64 output_size, u64 offset = 0);
		static u64 write(const std::wstring& path, const std::string& output, u64 offset = 0) { return write(boost::filesystem::path(path), reinterpret_cast<const byte*>(output.c_str()), output.size(), offset); }
		static u64 write(const std::wstring& path, const char* output, u64 offset = 0) { return write(boost::filesystem::path(path), reinterpret_cast<const byte*>(output), strlen(output), offset); }
		static u64 write(const std::string& path, const std::string& output, u64 offset = 0) { return write(boost::filesystem::path(path), reinterpret_cast<const byte*>(output.c_str()), output.size(), offset); }
		static u64 write(const std::string& path, const char* output, u64 offset = 0) { return write(boost::filesystem::path(path), reinterpret_cast<const byte*>(output), strlen(output), offset); }
		static u64 write(const char* path, const std::string& output, u64 offset = 0) { return write(boost::filesystem::path(path), reinterpret_cast<const byte*>(output.c_str()), output.size(), offset); }
		static u64 write(const char* path, const char* output, u64 offset = 0) { return write(boost::filesystem::path(path), reinterpret_cast<const byte*>(output), strlen(output), offset); }
		static u64 write(const file_component::ptr file, const std::string& output, u64 offset = 0) { return write(file->get_data(), reinterpret_cast<const byte*>(output.c_str()), output.size(), offset); }
		static u64 write(const file_component::ptr file, const char* output, u64 offset = 0) { return write(file->get_data(), reinterpret_cast<const byte*>(output), strlen(output), offset); }
		static u64 write(const file_component& file, const std::string& output, u64 offset = 0) { return write(file.get_data(), reinterpret_cast<const byte*>(output.c_str()), output.size(), offset); }
		static u64 write(const file_component& file, const char* output, u64 offset = 0) { return write(file.get_data(), reinterpret_cast<const byte*>(output), strlen(output), offset); }

		static u64 append(const boost::filesystem::path& path, const byte* output, const u64 output_size);
		static u64 append(const std::wstring& path, const std::string& output) { return append(boost::filesystem::path(path), reinterpret_cast<const byte*>(output.c_str()), output.size()); }
		static u64 append(const std::wstring& path, const char* output) { return append(boost::filesystem::path(path), reinterpret_cast<const byte*>(output), strlen(output)); }
		static u64 append(const std::string& path, const std::string& output) { return append(boost::filesystem::path(path), reinterpret_cast<const byte*>(output.c_str()), output.size()); }
		static u64 append(const std::string& path, const char* output) { return append(boost::filesystem::path(path), reinterpret_cast<const byte*>(output), strlen(output)); }
		static u64 append(const char* path, const std::string& output) { return append(boost::filesystem::path(path), reinterpret_cast<const byte*>(output.c_str()), output.size()); }
		static u64 append(const char* path, const char* output) { return append(boost::filesystem::path(path), reinterpret_cast<const byte*>(output), strlen(output)); }
		static u64 append(const file_component::ptr file, const std::string& output) { return append(file->get_data(), reinterpret_cast<const byte*>(output.c_str()), output.size()); }
		static u64 append(const file_component::ptr file, const char* output) { return append(file->get_data(), reinterpret_cast<const byte*>(output), strlen(output)); }
		static u64 append(const file_component& file, const std::string& output) { return append(file.get_data(), reinterpret_cast<const byte*>(output.c_str()), output.size()); }
		static u64 append(const file_component& file, const char* output) { return append(file.get_data(), reinterpret_cast<const byte*>(output), strlen(output)); }

		// Accessor
		static std::string get_id(const file_component::ptr file, const std::shared_ptr<hash> hash);
		static std::string get_size(const u64 bytes);
		static std::string get_speed(const u64 bytes);
	};
}

#endif
