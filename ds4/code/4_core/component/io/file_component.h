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

	class file_component final : public archive_component
	{
		make_factory(file_component);

	public:
		file_component() { }

		template <typename T>
		file_component(const T& path) : archive_component(path) { set_size(); set_time(); }
		file_component(const boost::filesystem::path& path) : archive_component(path) { set_size(); set_time(); }

		template <typename T>
		file_component(const T& path, const u64 size) : archive_component(path, size) { }
		file_component(const boost::filesystem::path& path, const u64 size) : archive_component(path, size) { }

		// Operator
		template <typename T>
		file_component& operator=(const T& path) { return operator=(boost::filesystem::path(path)); }
		file_component& operator=(const boost::filesystem::path& path);

		// Utility
		virtual bool exists() const override;
		virtual bool hidden() const override;
		bool system() const;

		// Set
		using archive_component::set_size;
		virtual void set_size() override;

#if _WSTRING
		// Get
		std::wstring get_wdescription() const;
		std::wstring get_wextension() const;
		std::wstring get_wsuffix() const { return m_path.extension().wstring(); }

		std::string get_description() const { return utf::to_utf8(get_wdescription()); }
		std::string get_extension() const { return utf::to_utf8(get_wextension()); }
		std::string get_suffix() const { return utf::to_utf8(get_wsuffix()); }
#else
		std::string get_description() const;
		std::string get_extension() const;
		std::string get_suffix() const { return m_path.extension().string(); }
#endif
		// Static
		template <typename T>
		static ptr create(const T& path) { return std::make_shared<file_component>(path); }
		static ptr create(const boost::filesystem::path& path) { return std::make_shared<file_component>(path); }

		template <typename T>
		static ptr create(const T& path, const u64 size) { return std::make_shared<file_component>(path, size); }
		static ptr create(const boost::filesystem::path& path, const u64 size) { return std::make_shared<file_component>(path, size); }
	};
}

#endif
