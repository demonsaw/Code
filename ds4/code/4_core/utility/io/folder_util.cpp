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

#include <iomanip>
#include <locale>
#include <sstream>

#include "utility/io/folder_util.h"
#include "security/checksum/checksum.h"
#include "security/filter/hex.h"
#include "security/hash/md.h"

namespace eja
{
	// Utility
	bool folder_util::exists(const boost::filesystem::path& path)
	{
		boost::system::error_code error;
		return boost::filesystem::exists(path, error) && boost::filesystem::is_directory(path, error);
	}

	void folder_util::clear(const boost::filesystem::path& path)
	{
		if (exists(path))
		{
			if (boost::filesystem::remove_all(path))
				boost::filesystem::create_directory(path);
		}
	}

	bool folder_util::create(const boost::filesystem::path& path)
	{ 
		return !exists(path) ? boost::filesystem::create_directory(path) : false;
	}

	// Accessor
	u32 folder_util::get_id(const folder_component::ptr folder, const checksum::ptr checksum)
	{
#if _WSTRING
		return checksum->compute(folder->get_wpath());
#else
		return checksum->compute(folder->get_path());
#endif
	}

	std::string folder_util::get_id(const folder_component::ptr folder, const hash::ptr hash)
	{
#if _WSTRING
		return hash->compute(folder->get_wpath());
#else
		return hash->compute(folder->get_path());
#endif		
	}

	std::string folder_util::get_size(const u64 bytes)
	{
		if (!bytes || (bytes == static_cast<u64>(~0)))
			return "";

		static const u64 limits[] { 1024ull * 1024ull * 1024ull * 1024ull, 1024ull * 1024ull * 1024ull, 1024ull * 1024ull, 1024ull };
		static const char* units[] {" TB", " GB", " MB", " KB"};
		enum unit_type {tb, gb, mb, kb, max};

		for (int i = 0; i < unit_type::max; ++i)
		{
			if (bytes >= limits[i])
			{
				const auto value = static_cast<double>(bytes) / static_cast<double>(limits[i]);

				std::stringstream ss;
				ss.imbue(std::locale(""));
				ss << std::fixed << std::setfill('0') << std::setprecision(2) << value << units[i];
				return ss.str();
			}
		}

		std::stringstream ss;
		ss.imbue(std::locale(""));
		ss << std::fixed << std::setfill('0') << std::setprecision(2) << bytes << " B";
		return ss.str();
	}
}
