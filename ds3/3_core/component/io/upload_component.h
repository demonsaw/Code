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

#ifndef _EJA_UPLOAD_COMPONENT_H_
#define _EJA_UPLOAD_COMPONENT_H_

#include <deque>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <boost/atomic.hpp>

#include "component/component.h"
#include "thread/thread_safe.h"

namespace eja
{
	class entity;

	make_thread_safe_component(upload_index_map_component, std::multimap<std::shared_ptr<entity>, u32>);
	make_thread_safe_component(upload_index_set_component, std::set<u32>);
	make_thread_safe_component(upload_list_component, std::vector<std::shared_ptr<entity>>);
	make_thread_safe_component(upload_map_component, std::map<std::string, std::shared_ptr<entity>>);
	make_thread_safe_component(upload_multimap_component, std::multimap<std::string, std::shared_ptr<entity>>);

	struct upload_data
	{
		u32 index;
		std::string data;
		using ptr = std::shared_ptr<upload_data>;

		// Static
		static ptr create() { return ptr(new upload_data()); }
	};

	class upload_data_list_component : public thread_safe_ex<component, std::deque<std::shared_ptr<upload_data>>>
	{
		make_factory(upload_data_list_component);

	private:
		boost::atomic<size_t> m_buffer;

	public:
		upload_data_list_component() : m_buffer(0) { }
		upload_data_list_component(const upload_data_list_component& comp) : thread_safe_ex(comp) { m_buffer.store(comp.m_buffer); }

		// Operator
		upload_data_list_component& operator=(const upload_data_list_component& comp);

		// Utility
		virtual bool valid() const override { return !empty(); }

		void add_buffer(const size_t buffer) { m_buffer += buffer; }
		void sub_buffer(const size_t buffer) { m_buffer = std::max(static_cast<int>(m_buffer - buffer), 0); }

		// Has
		bool has_buffer() const { return m_buffer > 0; }

		// Set
		void set_buffer(const size_t buffer) { m_buffer = buffer; }

		// Get
		size_t get_buffer() const { return m_buffer; }
	};
}

#endif
