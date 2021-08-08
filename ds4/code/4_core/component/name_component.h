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

#ifndef _EJA_NAME_COMPONENT_H_
#define _EJA_NAME_COMPONENT_H_

#include <deque>
#include <map>
#include <string>

#include "component/color_component.h"
#include "system/type.h"
#include "thread/thread_safe.h"

namespace eja
{
	class entity;

	make_thread_safe_component(name_list_component, std::deque<std::shared_ptr<entity>>);
	make_thread_safe_component(name_map_component, std::map<std::string, std::shared_ptr<entity>>);

	class name_component : public color_component
	{
		make_factory(name_component);

	protected:
		std::string m_name;

	public:
		name_component() { }
		name_component(const name_component& comp) : color_component(comp), m_name(comp.m_name) { }
		name_component(const size_t value) : color_component(value) { }

		// Operator
		name_component& operator=(const name_component& comp);

		// Has
		bool has_name() const { return !m_name.empty(); }

		// Set
		void set_name() { m_name.clear(); }
		void set_name(std::string&& name) { m_name = std::move(name); }
		void set_name(const std::string& name) { m_name = name; }

		// Get
		using color_component::get_color;
		virtual u32 get_color() const override;

		const std::string& get_name() const { return m_name; }
		std::string& get_name() { return m_name; }
	};

	// Operator
	inline name_component& name_component::operator=(const name_component& comp)
	{
		if (this != &comp)
		{
			color_component::operator=(comp);

			m_name = comp.m_name;
		}

		return *this;
	}
}

#endif
