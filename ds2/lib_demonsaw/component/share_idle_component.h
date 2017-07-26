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

#ifndef _EJA_SHARE_IDLE_COMPONENT_
#define _EJA_SHARE_IDLE_COMPONENT_

#include <memory>
#include <boost/asio.hpp>

#include "component/idle_component.h"
#include "component/io/file_component.h"
#include "component/io/folder_component.h"
#include "entity/entity.h"
#include "security/hash.h"
#include "utility/default_value.h"

namespace eja
{
	class share_idle_component final : public idle_component
	{
	public:
		using ptr = std::shared_ptr<share_idle_component>;
		using io_service_ptr = std::shared_ptr<boost::asio::io_service>;

	private:
		io_service_ptr m_service = io_service_ptr(new boost::asio::io_service());
		folder_list m_folders;
		file_list m_files;

	private:
		share_idle_component() : idle_component(default_timeout::client::share) { }

		// Callback		
		virtual bool on_run() override;

	public:
		virtual ~share_idle_component() override { }

		// Interface
		virtual void init() override;
		virtual void shutdown() override;
		virtual void update() override;

		// Utility
		void clear();

		void add(const std::string& path, const hash::ptr hash);
		void add(const std::string& path) { add(path, get_hash()); }

		void add(const file::ptr file, const hash::ptr hash, const bool share = true);
		void add(const file::ptr file, const bool share = true) { add(file, get_hash(), share); }

		void add(const folder::ptr folder, const hash::ptr hash, const bool share = true);
		void add(const folder::ptr folder, const bool share = true) { add(folder, get_hash(), share); }

		void remove(const std::string& path, const hash::ptr hash);
		void remove(const std::string& path) { remove(path, get_hash()); }

		void remove(const folder::ptr folder, const hash::ptr hash, const bool share = true);
		void remove(const folder::ptr folder, const bool share = true) { remove(folder, get_hash(), share); }

		void remove(const file::ptr file, const hash::ptr hash, const bool share = true);
		void remove(const file::ptr file, const bool share = true) { remove(file, get_hash(), share); }

		// Accessor
		hash::ptr get_hash() const;
		const folder_list& get_folders() const { return m_folders; }
		const file_list& get_files() const { return m_files; }

		// Static
		static ptr create() { return ptr(new share_idle_component()); }
	};

	// Container
	derived_type(share_entity_list_component, entity_list_component);
	derived_type(share_entity_map_component, entity_map_component);
	derived_type(share_entity_queue_component, entity_queue_component);
	derived_type(share_entity_vector_component, entity_vector_component);
}

#endif
