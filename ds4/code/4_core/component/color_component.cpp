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

#include <random>

#include "component/color_component.h"
#include "resource/color.h"

namespace eja
{
	// Static
	const u32 color_component::s_num_colors = (sizeof(s_colors) / sizeof(u32));

	// Set
	void color_component::set_color()
	{ 
		std::random_device rnd;
		std::uniform_int_distribution<u32> dist(0, s_num_colors - 1);
		m_color = s_colors[dist(rnd)];
	}

	// Get
	u32 color_component::get_color(const u8 alpha) const
	{
		auto color = get_color() & ~mask::alpha;
		return color |= (alpha << 24);
	}

	u32 color_component::get_font_color() const
	{
		return is_dark() ? 0xffffffff : 0xff232323;
	}

	u32 color_component::get_reverse_color(const u8 alpha) const
	{
		const auto color = (alpha << 24) + ((0xFF - get_red()) << 16) + ((0xFF - get_green()) << 8) + (0xFF - get_blue());
		return color;
	}
}
