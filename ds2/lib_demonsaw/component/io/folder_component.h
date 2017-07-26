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

#ifndef _EJA_FOLDER_COMPONENT_
#define _EJA_FOLDER_COMPONENT_

#include <memory>
#include <string>
#include <boost/filesystem.hpp>

#include "component/component.h"
#include "entity/entity.h"
#include "object/io/folder.h"

namespace eja
{
	class folder_component final : public folder, public component
	{
	public:
		using ptr = std::shared_ptr<folder_component>;

	private:
		folder_component() { }
		folder_component(const folder_component& folder) : eja::folder(folder) { }
		folder_component(const folder& folder) : eja::folder(folder) { }

		folder_component(const boost::filesystem::path& path) : folder(path) { }
		folder_component(const std::string& path) : folder(path) { }
		folder_component(const char* path) : folder(path) { }

		folder_component(const boost::filesystem::path& path, const u64 size) : folder(path, size) { }
		folder_component(const std::string& path, const u64 size) : folder(path, size) { }
		folder_component(const char* path, const u64 size) : folder(path, size) { }

	public:		
		virtual ~folder_component() override { }

		// Static
		static ptr create() { return ptr(new folder_component()); }
		static ptr create(const folder_component::ptr folder) { return ptr(new folder_component(*folder)); }
		static ptr create(const folder::ptr folder) { return ptr(new folder_component(*folder)); }
		static ptr create(const folder& folder) { return ptr(new folder_component(folder)); }

		static ptr create(const boost::filesystem::path& path) { return ptr(new folder_component(path)); }
		static ptr create(const std::string& path) { return ptr(new folder_component(path)); }
		static ptr create(const char* path) { return ptr(new folder_component(path)); }

		static ptr create(const boost::filesystem::path& path, const u64 size) { return ptr(new folder_component(path, size)); }
		static ptr create(const std::string& path, const u64 size) { return ptr(new folder_component(path, size)); }
		static ptr create(const char* path, const u64 size) { return ptr(new folder_component(path, size)); }
	};

	derived_type(folder_list_component, list_component<folder>);
	derived_type(folder_map_component, map_component<std::string, folder>);
	derived_type(folder_queue_component, queue_component<folder>);
	derived_type(folder_vector_component, vector_component<folder>);

	derived_type(folder_entity_list_component, entity_list_component);
	derived_type(folder_entity_map_component, entity_map_component);
	derived_type(folder_entity_queue_component, entity_queue_component);
	derived_type(folder_entity_vector_component, entity_vector_component);
}

#endif
