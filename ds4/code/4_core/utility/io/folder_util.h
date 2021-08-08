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

#ifndef _EJA_FOLDER_UTIL_
#define _EJA_FOLDER_UTIL_

#include <memory>
#include <string>
#include <boost/filesystem.hpp>

#include "utility/io/file_util.h"
#include "component/io/folder_component.h"

namespace eja
{
	class checksum;
	class hash;

	class folder_util final
	{
	private:
		folder_util() = delete;
		~folder_util() = delete;

	public:
		// Utility
		static bool create(const boost::filesystem::path& path);
		static bool create(const std::wstring& path) { return create(boost::filesystem::path(path)); }
		static bool create(const std::string& path) { return create(boost::filesystem::path(path)); }
		static bool create(const char* path) { return create(boost::filesystem::path(path)); }
		static bool create(const folder_component::ptr folder) { return create(folder->get_data()); }
		static bool create(const folder_component& folder) { return create(folder.get_data()); }

		static u64 remove(const boost::filesystem::path& path) { return boost::filesystem::remove_all(path); }
		static u64 remove(const std::wstring& path) { return remove(boost::filesystem::path(path)); }
		static u64 remove(const std::string& path) { return remove(boost::filesystem::path(path)); }
		static u64 remove(const char* path) { return remove(boost::filesystem::path(path)); }
		static u64 remove(const folder_component::ptr folder) { return remove(folder->get_data()); }
		static u64 remove(const folder_component& folder) { return remove(folder.get_data()); }

		static void clear(const boost::filesystem::path& path);
		static void clear(const std::wstring& path) { clear(boost::filesystem::path(path)); }
		static void clear(const std::string& path) { clear(boost::filesystem::path(path)); }
		static void clear(const char* path) { clear(boost::filesystem::path(path)); }
		static void clear(const folder_component::ptr folder) { clear(folder->get_data()); }
		static void clear(const folder_component& folder) { clear(folder.get_data()); }

		static bool exists(const boost::filesystem::path& path);
		static bool exists(const std::wstring& path) { return exists(boost::filesystem::path(path)); }
		static bool exists(const std::string& path) { return exists(boost::filesystem::path(path)); }
		static bool exists(const char* path) { return exists(boost::filesystem::path(path)); }
		static bool exists(const folder_component::ptr folder) { return exists(folder->get_data()); }
		static bool exists(const folder_component& folder) { return exists(folder.get_data()); }

		// Accessor
		static std::string get_current() { return boost::filesystem::current_path().string(); }
		static u32 get_id(const folder_component::ptr folder, const std::shared_ptr<checksum> checksum);
		static std::string get_id(const folder_component::ptr folder, const std::shared_ptr<hash> hash);
		static std::string get_size(const u64 bytes);
	};
}

#endif
		