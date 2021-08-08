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

#ifndef _EJA_ARCHIVE_COMPONENT_H_
#define _EJA_ARCHIVE_COMPONENT_H_

#include <string>
#include <boost/atomic.hpp>
#include <boost/filesystem.hpp>

#include "component/component.h"
#include "thread/thread_safe.h"
#include "system/type.h"
#include "utility/utf.h"

namespace eja
{
	make_using_component(archive);

	class archive_component : public component
	{
		make_factory(archive_component);

	protected:
		boost::filesystem::path m_path;
		boost::atomic<u64> m_size;
		u64 m_time = 0;

	public:
		archive_component() : m_size(0) { }
		archive_component(const archive_component& comp) : component(comp), m_path(comp.m_path), m_size(comp.m_size.load()), m_time(comp.m_time) { }

		template <typename T>
		archive_component(const T& path) : m_size(0) { set_path(path); }
		archive_component(const boost::filesystem::path& path) : m_path(path), m_size(0) { }

		template <typename T>
		archive_component(const T& path, const u64 size) : m_size(size) { set_path(path); }
		archive_component(const boost::filesystem::path& path, const u64 size) : m_path(path), m_size(size) { }

		// Operator
		template <typename T>
		archive_component& operator=(const T& path) { return operator=(boost::filesystem::path(path)); }
		archive_component& operator=(const boost::filesystem::path& path);
		archive_component& operator=(const archive_component& comp);

		// Interface
		virtual void clear() override;

		// Utility
		virtual bool valid() const override { return has_path(); }
		virtual bool hidden() const { return false; }
		virtual bool exists() const;
		//void clean();

		// Has
		bool has_path() const { return !m_path.empty(); }
		bool has_size() const { return m_size > 0; }
		bool has_time() const { return m_time > 0; }

		// Set
		template <typename T>
		void set_path(const T& path);
		void set_path(const boost::filesystem::path& path) { m_path = path; }
		WIN32_ONLY(void set_path(const std::wstring& path) { m_path = path; })

		virtual void set_size() { m_size = 0; }
		void set_size(const u64 size) { m_size = size; }
		void add_size(const u64 size) { m_size += size; }
		void sub_size(const u64 size) { m_size -= size; }

		void set_time();
		void set_time(const u64 time) { m_time = time; }

		// Get
		const boost::filesystem::path& get_data() const { return m_path; }
		boost::filesystem::path& get_data() { return m_path; }
		u64 get_size() const { return m_size.load(); }
		u64 get_time() const { return m_time; }

#if _WSTRING
		std::wstring get_wfolder() const { return m_path.parent_path().wstring(); }
		std::wstring get_wname() const { return m_path.filename().wstring(); }
		const std::wstring& get_wpath() const { return m_path.wstring(); }
		std::wstring get_wstem() const { return m_path.stem().wstring(); }

		std::string get_folder() const { return utf::to_utf8(get_wfolder()); }
		std::string get_name() const { return utf::to_utf8(get_wname()); }
		std::string get_path() const { return utf::to_utf8(get_wpath()); }
		std::string get_stem() const { return utf::to_utf8(get_wstem()); }
#else
		std::string get_folder() const { return m_path.parent_path().string(); }
		std::string get_name() const { return m_path.filename().string(); }
		const std::string& get_path() const { return m_path.string(); }
		std::string get_stem() const { return m_path.stem().string(); }
#endif
		// Static
		template <typename T>
		static ptr create(const T& path) { return std::make_shared<archive_component>(path); }
		static ptr create(const boost::filesystem::path& path) { return std::make_shared<archive_component>(path); }

		template <typename T>
		static ptr create(const T& path, const u64 size) { return std::make_shared<archive_component>(path, size); }
		static ptr create(const boost::filesystem::path& path, const u64 size) { return std::make_shared<archive_component>(path, size); }
	};

	// Set
	template <typename T>
	void archive_component::set_path(const T& path)
	{
#if _WSTRING
		m_path = utf::to_utf16(path);
#else
		m_path = path;
#endif
	}
}

#endif
