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

#include "color_component.h"

namespace eja
{
	// Stats
	const u32 color_component::s_num_colors = (sizeof(s_colors) / sizeof(s_colors[0]));

	// Mutator
	void color_component::set_color(const QColor& color)
	{ 
		m_color = color;
		m_pixmap.fill(m_color);
	}

	void color_component::set_color(const u8 red, const u8 green, const u8 blue)
	{
		m_color = QColor::fromRgb(red, green, blue);
		m_pixmap.fill(m_color);
	}

	void color_component::set_color(const std::string& id)
	{
		if (id == default_client::name)
		{
			m_color = QColor(default_chat::color);
		}
		else if (id.empty())
		{
			m_color = QColor(0xFF000000);
		}
		else
		{
			u32 hash = 0;
			for (const auto& ch : id)
				hash += ch;

			m_color = s_colors[hash % s_num_colors];
		}

		m_pixmap.fill(m_color);
	}

	void color_component::set_color(const char* id)
	{
		if (id == default_client::name)
		{
			m_color = QColor(default_chat::color);
		}
		else if (!strlen(id))
		{
			m_color = QColor(0xFF000000);
		}
		else
		{
			u32 hash = 0;
			for (const char* ch = id; id; ++ch)
				hash += *ch;

			m_color = s_colors[hash % s_num_colors];
		}

		m_pixmap.fill(m_color);
	}

	// Accessor
	u32 color_component::get_hash(const std::string& str)
	{
		u32 hash = 0;
		for (const auto& ch : str)
			hash += ch;

		return hash;
	}
}
