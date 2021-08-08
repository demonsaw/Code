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

#include <boost/algorithm/string.hpp>
#include <boost/predef.h>

#include "component/io/file_component.h"
#include "component/io/folder_component.h"
#include "system/api.h"
#include "utility/value.h"

namespace eja
{
	// Operator
	file_component& file_component::operator=(const boost::filesystem::path& path)
	{
		archive_component::operator=(path);

		set_size();
		set_time();

		return *this;
	}

	// Interface
	bool file_component::hidden() const
	{
		if (invalid())
			return false;

#if _WIN32
		DWORD attr = GetFileAttributesW(m_path.wstring().c_str());
		return FILE_ATTRIBUTE_HIDDEN == (FILE_ATTRIBUTE_HIDDEN & attr);
#else
		return boost::starts_with(m_path.string(), ".");
#endif
	}

	bool file_component::system() const
	{
		if (invalid())
			return false;

#if _WSTRING
		const auto name = get_wname();
#else
		const auto name = get_name();
#endif
		return boost::equals(name, software::config);
	}

	// Utility
	bool file_component::exists() const
	{
		if (archive_component::exists())
		{
			boost::system::error_code error;
			return !boost::filesystem::is_directory(m_path, error);
		}

		return false;
	}

	// Set
	void file_component::set_size()
	{
		if (exists())
		{
			boost::system::error_code error;
			m_size = boost::filesystem::file_size(m_path, error);
		}
		else
		{
			m_size = 0;
		}
	}

	// Get
#if _WSTRING
	std::wstring file_component::get_wdescription() const
	{
		SHFILEINFOW shFileInfo;
		const auto ext = m_path.extension().wstring();
		const auto result = ::SHGetFileInfoW(ext.c_str(), 0, &shFileInfo, sizeof(SHFILEINFOW), SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);
		if (result)
			return shFileInfo.szTypeName;

		return get_wextension();
	}

	std::wstring file_component::get_wextension() const
	{
		const auto ext = m_path.extension().wstring();
		return !ext.empty() ? ext.substr(1) : ext;
	}
#else
	std::string file_component::get_description() const
	{
		return get_extension();
	}

	std::string file_component::get_extension() const
	{
		const auto ext = m_path.extension().string();
		return !ext.empty() ? ext.substr(1) : ext;
	}
#endif
}
