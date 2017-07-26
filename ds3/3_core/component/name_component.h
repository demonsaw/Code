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

#include <string>

#include "component/id_component.h"
#include "resource/color.h"
#include "system/type.h"
#include "thread/thread_safe.h"

namespace eja
{
	class entity;

	class name_component : public id_component
	{
		make_factory(name_component);

	private:
		const u32 m_num_colors = (sizeof(s_colors) / sizeof(s_colors[0]));

	protected:
		std::string m_name;
		u32 m_color = 0;		

	public:
		name_component() { }
		name_component(const name_component& comp) : id_component(comp), m_color(comp.m_color), m_name(comp.m_name) { }

		// Operator
		name_component& operator=(const name_component& comp);

		// Interface
		virtual void clear() override;

		// Has
		bool has_name() const { return !m_name.empty(); }
		bool has_color() const { return m_color > 0; }

		// Set		
		void set_name() { m_name.clear(); }
		void set_name(const std::string& name) { m_name = name; }		

		void set_color() { m_color = s_colors[rand() % m_num_colors]; }
		void set_color(const u32 color) { m_color = color; }
		void set_color(const u8 red, const u8 green, const u8 blue) { m_color = (0xFF << 24) + (red << 16) + (green << 8) + (blue << 0); }

		void set_alpha(const u8 alpha) { m_color &= (static_cast<u32>(alpha) << 24); }
		void set_red(const u8 red) { m_color &= (static_cast<u32>(red) << 16); }
		void set_green(const u8 green) { m_color &= (static_cast<u32>(green) << 8); }
		void set_blue(const u8 blue) { m_color &= (static_cast<u32>(blue) << 0); }

		// Get
		const std::string& get_name() const { return m_name; }

		u32 get_color();
		u32 get_color() const { return const_cast<name_component*>(this)->get_color(); }				
		
		u8 get_alpha() const { return ((m_color >> 24) & 0xFF); }
		u8 get_red() const { return ((m_color >> 16) & 0xFF); }
		u8 get_green() const { return ((m_color >> 8) & 0xFF); }
		u8 get_blue() const { return (m_color & 0xFF); }
	};
}

#endif
