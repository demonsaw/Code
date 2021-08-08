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

#ifndef _EJA_COLOR_COMPONENT_H_
#define _EJA_COLOR_COMPONENT_H_

#include <string>

#include "component/component.h"
#include "system/type.h"

namespace eja
{
	class color_component : public component
	{
		make_factory(color_component);

		enum mask : u32 { alpha = 0xFF000000, red = 0x00FF0000, green = 0x0000FF00, blue = 0x000000FF };

	protected:
		static const u32 s_num_colors;

		u32 m_color = 0;

	public:
		color_component() { }
		color_component(const color_component& comp) : component(comp), m_color(comp.m_color) { }
		color_component(const size_t value) : component(value) { }

		// Operator
		color_component& operator=(const color_component& comp);

		// Has
		bool has_color() const { return m_color > 0; }

		// Is
		bool is_dark() const { return (((get_red() * 0.299 + get_green() * 0.587 + get_blue() * 0.114) / 255) <= 0.5); }
		bool is_light() const { return !is_dark(); }

		// Set
		void set_color();
		void set_color(const u32 color) { m_color = color; }
		void set_color(const u8 red, const u8 green, const u8 blue) { m_color = (0xFF << 24) + (red << 16) + (green << 8) + blue; }

		void set_alpha(const u8 alpha) { m_color = ((m_color & ~mask::alpha) | (static_cast<u32>(alpha) << 24)); }
		void set_red(const u8 red) { m_color = ((m_color & ~mask::red) | (static_cast<u32>(red) << 16)); }
		void set_green(const u8 green) { m_color = ((m_color & ~mask::green) | (static_cast<u32>(green) << 8)); }
		void set_blue(const u8 blue) { m_color = ((m_color & ~mask::blue) | static_cast<u32>(blue)); }

		// Get
		virtual u32 get_color() const { return m_color; }
		u32 get_color(const u8 alpha) const;

		u32 get_font_color() const;
		u32 get_reverse_color() const { return get_reverse_color(get_alpha()); }
		u32 get_reverse_color(const u8 alpha) const;

		u8 get_alpha() const { return ((m_color >> 24) & 0xFF); }
		u8 get_red() const { return ((m_color >> 16) & 0xFF); }
		u8 get_green() const { return ((m_color >> 8) & 0xFF); }
		u8 get_blue() const { return (m_color & 0xFF); }
	};

	// Operator
	inline color_component& color_component::operator=(const color_component& comp)
	{
		if (this != &comp)
		{
			component::operator=(comp);

			m_color = comp.m_color;
		}

		return *this;
	}
}

#endif
