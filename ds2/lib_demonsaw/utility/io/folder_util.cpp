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

#include "folder_util.h"
#include "security/hex.h"
#include "security/md.h"

namespace eja
{
	// Utility
	void folder_util::clear(const boost::filesystem::path& path)
	{
		if (boost::filesystem::exists(path))
		{
			if (boost::filesystem::remove_all(path))
				boost::filesystem::create_directory(path);
		}
	}

	bool folder_util::create(const boost::filesystem::path& path)
	{ 
		if (!exists(path))
		{
			boost::system::error_code error;
			return boost::filesystem::create_directory(path, error);
		}

		return false;
	}

	// Accessor
	std::string folder_util::get_id(const folder::ptr folder, const hash::ptr hash)
	{
		const auto id = hash->compute(folder->get_path());
		return hex::encode(id);
	}

	std::string folder_util::get_size(const size_t size)
	{
		if (!size || (size == static_cast<size_t>(~0)))
			return "";

		static const size_t limits[] {1024 * 1024 * 1024, 1024 * 1024, 1024};
		static const char* units[] {" GB", " MB", " KB"};
		enum unit_type {gb, mb, kb, max};

		for (int i = 0; i < unit_type::max; ++i)
		{
			if (size >= limits[i])
			{
				const auto value = static_cast<double>(size) / static_cast<double>(limits[i]);

				std::stringstream ss;
				ss.imbue(std::locale(""));
				ss << std::fixed << std::setfill('0') << std::setprecision(2) << value << units[i];
				return ss.str();
			}
		}

		std::stringstream ss;
		ss.imbue(std::locale(""));
		ss << std::fixed << std::setfill('0') << std::setprecision(2) << size << " B";
		return ss.str();
	}
}
