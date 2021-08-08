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

#ifndef _EJA_FOLDER_COMPONENT_H_
#define _EJA_FOLDER_COMPONENT_H_

#include <list>
#include <map>
#include <string>

#include "component/io/archive_component.h"
#include "component/io/file_component.h"
#include "entity/entity.h"
#include "thread/thread_safe.h"
#include "system/type.h"

namespace eja
{
	make_using_component(folder);
	make_thread_safe_component(folder_list_component, std::list<std::shared_ptr<entity>>);
	make_thread_safe_component(folder_map_component, std::map<std::string, std::shared_ptr<entity>>);

	class folder_component final : public archive_component
	{
		make_factory(folder_component);

		enum bits : size_t { none, drive = bit(1) };

	private:
		entity_list m_files;
		entity_list m_folders;

	public:
		folder_component() { }

		template <typename T>
		folder_component(const T& path) : archive_component(path) { set_time(); }
		folder_component(const boost::filesystem::path& path) : archive_component(path) { set_time(); }

		template <typename T>
		folder_component(const T& path, const u64 size) : archive_component(path, size) { }
		folder_component(const boost::filesystem::path& path, const u64 size) : archive_component(path, size) { }

		// Operator
		template <typename T>
		folder_component& operator=(const T& path) { return operator=(boost::filesystem::path(path)); }
		folder_component& operator=(const boost::filesystem::path& path);

		// Interface
		virtual void clear() override;
		void clear_children();

		// Utility
		virtual bool exists() const override;
		virtual bool hidden() const override;

		// Has
		bool has_children() const;
		bool has_folders() const;
		bool has_files() const;

		// Is
		bool is_drive() const;

		// Set
		void set_drive(const bool value) { set_mode(bits::drive, value); }

		// Get
		const entity_list& get_files(const bool refresh = false);
		const entity_list& get_folders(const bool refresh = false);

		// Static
		template <typename T>
		static ptr create(const T& path) { return std::make_shared<folder_component>(path); }
		static ptr create(const boost::filesystem::path& path) { return std::make_shared<folder_component>(path); }

		template <typename T>
		static ptr create(const T& path, const u64 size) { return std::make_shared<folder_component>(path, size); }
		static ptr create(const boost::filesystem::path& path, const u64 size) { return std::make_shared<folder_component>(path, size); }
	};
}

#endif
