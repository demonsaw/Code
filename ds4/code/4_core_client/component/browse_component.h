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

#ifndef _EJA_BROWSE_COMPONENT_H_
#define _EJA_BROWSE_COMPONENT_H_

#include <deque>
#include <map>
#include <string>
#include <boost/filesystem.hpp>

#include "component/component.h"

#include "system/type.h"
#include "thread/thread_safe.h"

namespace eja
{
	class entity;

	make_component(browse_component);
	make_thread_safe_component(browse_map_component, std::map<std::string, std::shared_ptr<entity>>);

	class browse_list_component : public thread_safe_ex<component, std::deque<std::shared_ptr<entity>>>
	{
		make_factory(browse_list_component);

		enum bits : size_t { none, download = bit(1), expanded = bit(2), search = bit(3) };

	private:
		boost::filesystem::path m_path;

	public:
		browse_list_component() { }

		// Interface
		virtual void clear() override;

		// Has
		bool has_path() const { return !m_path.empty(); }

		// Is
		bool is_download() const { return is_mode(bits::download); }
		bool is_expanded() const { return is_mode(bits::expanded); }
		bool is_search() const { return is_mode(bits::search); }

		// Set
		void set_path(const boost::filesystem::path& path) { m_path = path; }
		void set_path(const std::wstring& path) { m_path = path; }
		void set_path(const std::string& path) { m_path = path; }
		void set_path(const char* path) { m_path = path; }

		void set_download(const bool value) { set_mode(bits::download, value); }
		void set_expanded(const bool value) { set_mode(bits::expanded, value); }
		void set_search(const bool value) { set_mode(bits::search, value); }

		// Get		
		const boost::filesystem::path& get_path() const { return m_path; }
		boost::filesystem::path& get_path() { return m_path; }
	};
}

#endif
