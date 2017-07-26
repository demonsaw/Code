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

#ifndef _EJA_FILE_COMPONENT_
#define _EJA_FILE_COMPONENT_

#include <memory>
#include <string>
#include <boost/filesystem.hpp>

#include "component/component.h"
#include "entity/entity.h"
#include "object/io/file.h"
#include "system/type.h"

namespace eja
{
	// Component
	class file_component final : public file, public component
	{
	public:
		using ptr = std::shared_ptr<file_component>;

	private:
		file_component() { }
		file_component(const file_component& file) : eja::file(file) { }
		file_component(const file& file) : eja::file(file) { }

		file_component(const boost::filesystem::path& path) : file(path) { }
		file_component(const std::string& path) : file(path) { }
		file_component(const char* path) : file(path) { }

		file_component(const boost::filesystem::path& path, const u64 size) : file(path, size) { }
		file_component(const std::string& path, const u64 size) : file(path, size) { }
		file_component(const char* path, const u64 size) : file(path, size) { }

	public:		
		virtual ~file_component() override { }

		// Static
		static ptr create() { return ptr(new file_component()); }
		static ptr create(const file_component::ptr file) { return ptr(new file_component(*file)); }
		static ptr create(const file::ptr file) { return ptr(new file_component(*file)); }
		static ptr create(const file& file) { return ptr(new file_component(file)); }

		static ptr create(const boost::filesystem::path& path) { return ptr(new file_component(path)); }
		static ptr create(const std::string& path) { return ptr(new file_component(path)); }
		static ptr create(const char* path) { return ptr(new file_component(path)); }

		static ptr create(const boost::filesystem::path& path, const u64 size) { return ptr(new file_component(path, size)); }
		static ptr create(const std::string& path, const u64 size) { return ptr(new file_component(path, size)); }
		static ptr create(const char* path, const u64 size) { return ptr(new file_component(path, size)); }
	};

	// Container
	derived_type(file_list_component, list_component<file>);
	derived_type(file_map_component, map_component<std::string, file>);
	derived_type(file_queue_component, queue_component<file>);
	derived_type(file_vector_component, vector_component<file>);

	derived_type(file_entity_list_component, entity_list_component);
	derived_type(file_entity_map_component, entity_map_component);
	derived_type(file_entity_queue_component, entity_queue_component);
	derived_type(file_entity_vector_component, entity_vector_component);
}

#endif
