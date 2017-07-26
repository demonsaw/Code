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
#include <boost/filesystem/operations.hpp>

#include "folder.h"
#include "system/api.h"

namespace eja
{
	// Constructor
	folder::folder(const folder& folder) : archive(folder)
	{
		m_folders = folder.m_folders;
		m_files = folder.m_files;
	}
	
	// Operator
	folder& folder::operator=(const folder& folder)
	{
		if (this != &folder)
		{
			archive::operator=(folder);

			m_folders = folder.m_folders;
			m_files = folder.m_files;
		}			

		return *this;
	}

	folder& folder::operator=(const boost::filesystem::path& path)
	{
		m_path = path;
		m_size = 0;

		return *this;
	}

	// Interface
	void folder::clear()
	{
		archive::clear();

		m_folders.clear();
		m_files.clear();
	}

	// Utility
	bool folder::is_hidden() const
	{
		if (empty())
			return false;

#if _WIN32
		const auto path = m_path.string();
		DWORD attr = GetFileAttributesA(path.c_str());
		return FILE_ATTRIBUTE_HIDDEN == (FILE_ATTRIBUTE_HIDDEN & attr);
#else
		const auto path = m_path.string();
		return boost::starts_with(path, ".");
#endif
	}

	bool folder::is_drive() const
	{
		if (empty())
			return false;

#if _WIN32
		const auto path = m_path.string();
		return (path.size() == 3) && (path[1] == ':');
#else
		const auto path = m_path.string();
		return path.size() == 1 && path[0] == '/';
#endif
	}

	bool folder::has_children() const
	{
		try
		{
			if (exists())
			{
				boost::filesystem::directory_iterator it(m_path);
				if (it != boost::filesystem::directory_iterator())
					return true;
			}
		}
		catch (...) { }

		return false;
	}

	bool folder::has_folders() const
	{
		try
		{
			if (exists())
			{
				boost::filesystem::directory_iterator it(m_path);

				while (it != boost::filesystem::directory_iterator())
				{
					const boost::filesystem::directory_entry& entry = *it;
					const boost::filesystem::path& path = entry.path();

					if (boost::filesystem::is_directory(path))
						return true;

					++it;
				}
			}
		}
		catch (...) {}

		return false;
	}

	bool folder::has_files() const
	{
		try
		{
			if (exists())
			{
				boost::filesystem::directory_iterator it(m_path);

				while (it != boost::filesystem::directory_iterator())
				{
					const boost::filesystem::directory_entry& entry = *it;
					const boost::filesystem::path& path = entry.path();

					if (!boost::filesystem::is_directory(path))
						return true;

					++it;
				}
			}
		}
		catch (...) { }

		return false;
	}

	// Accessor
	const folder_list& folder::get_folders(const bool refresh /*= false*/)
	{
		try
		{
			if (refresh)
				m_folders.clear();

			if (m_folders.empty() && exists())
			{
				boost::filesystem::directory_iterator it(m_path);

				while (it != boost::filesystem::directory_iterator())
				{
					const boost::filesystem::directory_entry& entry = *it;

					while (it != boost::filesystem::directory_iterator())
					{
						const boost::filesystem::directory_entry& entry = *it;
						const boost::filesystem::path& path = entry.path();

						if (boost::filesystem::is_directory(path))
						{
							const auto folder = folder::create(path);
							if (!folder->is_hidden())
								m_folders.add(folder);
						}

						++it;
					}
				}
			}
		}
		catch (...) {}

		return m_folders;
	}

	const file_list& folder::get_files(const bool refresh /*= false*/)
	{
		try
		{
			if (refresh)
				m_files.clear();

			if (m_files.empty() && exists())
			{
				boost::filesystem::directory_iterator it(m_path);

				while (it != boost::filesystem::directory_iterator())
				{
					const boost::filesystem::directory_entry& entry = *it;
					const boost::filesystem::path& path = entry.path();

					if (!boost::filesystem::is_directory(path))
					{
						const auto file = file::create(path);
						if (!file->is_hidden() && !file->is_config())
							m_files.add(file);
					}

					++it;
				}
			}
		}
		catch (...) {}

		return m_files;
	}
}
