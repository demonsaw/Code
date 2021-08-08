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

#ifndef _EJA_ROOM_COMPONENT_H_
#define _EJA_ROOM_COMPONENT_H_

#include <deque>
#include <map>
#include <string>

#include "component/name_component.h"

#include "system/type.h"
#include "thread/thread_safe.h"

namespace eja
{
	class entity;

	make_thread_safe_component(room_list_component, std::deque<std::shared_ptr<entity>>);
	make_thread_safe_component(room_map_component, std::map<std::string, std::shared_ptr<entity>>);

	class room_component final : public name_component
	{
		make_factory(room_component);

	private:
		size_t m_size = 0;

	public:
		room_component() { }

		// Interface
		bool empty() const { return !has_size(); }

		// Utility		
		void add_size(const size_t size = 1) { m_size += size; }
		void sub_size(const size_t size = 1) { m_size -= size; }

		// Has		
		bool has_size() const { return m_size > 0; }

		// Set
		void set_size(const size_t size) { m_size = size; }

		// Get
		size_t get_size() const { return m_size; }
	};
}

#endif
