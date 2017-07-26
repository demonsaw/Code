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

#ifndef _EJA_COLOR_COMPONENT_
#define _EJA_COLOR_COMPONENT_

#include <memory>

#include <QColor>
#include <QPixmap>
#include <QImage>

#include "component/component.h"
#include "resource/color.h"

namespace eja
{
	class color_component final : public component
	{
	private:
		QColor m_color;
		QPixmap m_pixmap;

	private:
		static const u32 s_num_colors;
		static const u32 width = 16;
		static const u32 height = 16;

	public:
		using ptr = std::shared_ptr<color_component>;

	public:
		color_component() : m_pixmap(width, height) { set_color(0xFF000000); }
		color_component(const std::string& id) : m_pixmap(width, height) { set_color(id); }
		color_component(const char* id) : m_pixmap(width, height) { set_color(id); }
		color_component(const QColor& color) : m_pixmap(width, height) { set_color(color); }
		color_component(const u8 red, const u8 green, const u8 blue) : m_pixmap(width, height) { set_color(red, green, blue); }
		virtual ~color_component() override { }

		// Mutator
		void set_color(const QColor& color);
		void set_color(const u8 red, const u8 green, const u8 blue);
		void set_color(const std::string& id);
		void set_color(const char* id);

		// Accessor
		const QColor& get_color() const { return m_color; }
		const QPixmap& get_pixmap() const { return m_pixmap; }

		// Static
		static ptr create() { return ptr(new color_component()); }
		static ptr create(const std::string& id) { return ptr(new color_component(id)); }
		static ptr create(const char* id) { return ptr(new color_component(id)); }
		static ptr create(const QColor& color) { return  ptr(new color_component(color)); }
		static ptr create(const u8 red, const u8 green, const u8 blue) { return ptr(new color_component(red, green, blue)); }

		// TODO: Remove me
		static u32 get_hash(const std::string& name);
		static const QColor& get_color(const std::string& color) { return s_colors[get_hash(color) % s_num_colors]; }
		static const QColor& get_color(u32 color) { return s_colors[color % s_num_colors]; }
		
	};
}

#endif
