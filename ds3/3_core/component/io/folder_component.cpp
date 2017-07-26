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

#include <boost/predef.h>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>

#include "component/io/folder_component.h"
#include "system/api.h"

namespace eja
{
	// Constructor
	folder_component::folder_component(const folder_component& comp) : archive_component(comp)
	{
		m_folders = comp.m_folders;
		m_files = comp.m_files;
	}

	// Operator
	folder_component& folder_component::operator=(const folder_component& comp)
	{
		if (this != &comp)
		{
			archive_component::operator=(comp);

			m_folders = comp.m_folders;
			m_files = comp.m_files;
		}

		return *this;
	}

	folder_component& folder_component::operator=(const boost::filesystem::path& path)
	{
		m_path = path;
		m_size = 0;

		return *this;
	}

	// Interface
	void folder_component::clear()
	{
		archive_component::clear();

		m_folders.clear();
		m_files.clear();
	}

	void folder_component::clear_children()
	{
		m_size = 0;
		m_folders.clear();
		m_files.clear();
	}

	// Utility
	bool folder_component::hidden() const
	{
		if (invalid())
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

	bool folder_component::drive() const
	{
		if (invalid())
			return false;

#if _WIN32
		const auto path = m_path.string();
		return (path.size() <= 3) && (path[1] == ':');
#else
		const auto path = m_path.string();
		return path.size() == 1 && path[0] == '/';
#endif
	}

	bool folder_component::has_children() const
	{
		if (exists())
		{
			boost::system::error_code error;
			boost::filesystem::directory_iterator it(m_path, error);

			if (it != boost::filesystem::directory_iterator())
				return true;
		}

		return false;
	}

	bool folder_component::has_folders() const
	{
		if (exists())
		{
			boost::system::error_code error;
			boost::filesystem::directory_iterator it(m_path, error);

			while (it != boost::filesystem::directory_iterator())
			{
				const boost::filesystem::directory_entry& entry = *it;
				const boost::filesystem::path& path = entry.path();

				if (boost::filesystem::is_directory(path))
					return true;

				++it;
			}
		}

		return false;
	}

	bool folder_component::has_files() const
	{
		if (exists())
		{
			boost::system::error_code error;
			boost::filesystem::directory_iterator it(m_path, error);

			while (it != boost::filesystem::directory_iterator())
			{
				const boost::filesystem::directory_entry& entry = *it;
				const boost::filesystem::path& path = entry.path();

				if (!boost::filesystem::is_directory(path))
					return true;

				++it;
			}
		}

		return false;
	}

	// Get
	const entity_list& folder_component::get_folders(const bool refresh /*= false*/)
	{
		if (refresh)
			m_folders.clear();

		if (m_folders.empty() && exists())
		{
			boost::system::error_code error;
			boost::filesystem::directory_iterator it(m_path, error);

			while (it != boost::filesystem::directory_iterator())
			{
				const boost::filesystem::directory_entry& entry = *it;

				while (it != boost::filesystem::directory_iterator())
				{
					const boost::filesystem::directory_entry& entry = *it;
					const boost::filesystem::path& path = entry.path();

					if (boost::filesystem::is_directory(path))
					{
						const auto folder = folder_component::create(path);						
						if (!folder->hidden())
						{
							const auto owner = get_entity();
							if (owner)
							{
								const auto entity = entity::create(owner, folder);
								m_folders.push_back(entity);
							}							
						}							
					}

					++it;
				}
			}
		}

		return m_folders;
	}

	const entity_list& folder_component::get_files(const bool refresh /*= false*/)
	{
		if (refresh)
			m_files.clear();

		if (m_files.empty() && exists())
		{
			boost::system::error_code error;
			boost::filesystem::directory_iterator it(m_path, error);

			while (it != boost::filesystem::directory_iterator())
			{
				const boost::filesystem::directory_entry& entry = *it;
				const boost::filesystem::path& path = entry.path();

				if (!boost::filesystem::is_directory(path))
				{
					const auto file = file_component::create(path);
					if (!file->hidden() && !file->system())
					{
						const auto owner = get_entity();
						if (owner)
						{
							const auto entity = entity::create(owner, file);
							m_files.push_back(entity);
						}						
					}
				}

				++it;
			}
		}

		return m_files;
	}
}
