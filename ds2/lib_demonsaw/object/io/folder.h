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

#ifndef _EJA_FOLDER_
#define _EJA_FOLDER_

#include <list>
#include <memory>
#include <string>
#include <boost/filesystem.hpp>

#include "archive.h"
#include "file.h"
#include "system/mutex/mutex_list.h"
#include "system/mutex/mutex_map.h"
#include "system/mutex/mutex_queue.h"
#include "system/mutex/mutex_vector.h"
#include "system/type.h"

namespace eja
{
	class folder;
	
	// Container
	derived_type(folder_list, mutex_list<folder>);
	derived_type(folder_map, mutex_map<std::string, folder>);
	derived_type(folder_queue, mutex_queue<folder>);
	derived_type(folder_vector, mutex_vector<folder>);

	class folder : public archive
	{
	protected:
		folder_list m_folders; 
		file_list m_files;

	public:
		using ptr = std::shared_ptr<folder>;

	public:
		folder() { }
		folder(const folder& folder);
		
		folder(const boost::filesystem::path& path) : archive(path) { }
		folder(const std::string& path) : archive(path) { }
		folder(const char* path) : archive(path) { }
		
		folder(const boost::filesystem::path& path, const u64 size) : archive(path, size) { }
		folder(const std::string& path, const u64 size) : archive(path, size) { }
		folder(const char* path, const u64 size) : archive(path, size) { }
		
		virtual ~folder() override { }

		// Operator
		folder& operator=(const folder& folder);
		folder& operator=(const boost::filesystem::path& path);
		folder& operator=(const std::string& path) { return operator=(boost::filesystem::path(path)); }
		folder& operator=(const char* path) { return operator=(boost::filesystem::path(path)); }

		// Interface
		virtual void clear() override;

		// Utility
		virtual bool exists() const override { return archive::exists() && boost::filesystem::is_directory(m_path); }
		virtual bool is_hidden() const override;
		bool is_drive() const;

		bool has_children() const;
		bool has_folders() const;
		bool has_files() const;		

		// Accessor
		const file_list& get_files(const bool refresh = false); 
		const folder_list& get_folders(const bool refresh = false);

		// Static
		static ptr create() { return std::make_shared<folder>(); }
		static ptr create(const folder& folder) { return std::make_shared<eja::folder>(folder); }
		static ptr create(const folder::ptr folder) { return std::make_shared<eja::folder>(*folder); }
		static ptr create(const boost::filesystem::path& path) { return std::make_shared<folder>(path); }
		static ptr create(const std::string& path) { return std::make_shared<folder>(path); }
		static ptr create(const char* path) { return std::make_shared<folder>(path); }
		static ptr create(const boost::filesystem::path& path, const u64 size) { return std::make_shared<folder>(path, size); }
		static ptr create(const std::string& path, const u64 size) { return std::make_shared<folder>(path, size); }
		static ptr create(const char* path, const u64 size) { return std::make_shared<folder>(path, size); }
	};
}

#endif
