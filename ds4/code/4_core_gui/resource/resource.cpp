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

#include <algorithm>

#include <QApplication>
#include <QColor>
#include <QDesktopWidget>
#include <QFont>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QPalette>
#include <QRect>
#include <QVBoxLayout>
#include <QWidget>

#include "resource/resource.h"
#include "utility/value.h"

namespace eja
{
	// Static
	const char* resource::icon = ":/icon.ico";
	const char* resource::logo = ":/logo.svg";
	const char* resource::logo_rgb = ":/logo_rgb.png";
	const char* resource::logo_black = ":/logo_black.png";
	const char* resource::logo_gray = ":/logo_gray.png";
	const char* resource::logo_white = ":/logo_white.png";
	const char* resource::title = ":/title.svg";

	const char* resource::drag_drop = ":/drag_drop.png";
	const char* resource::style = ":/style.css";

	const char* resource::font = ":/font/sans.ttf";
	const char* resource::font_bold = ":/font/sans_bold.ttf";
	const char* resource::font_mono = ":/font/mono.ttf";
	const char* resource::font_awesome = ":/font/font_awesome.otf";

#if !_WIN32 && !_MACX
	const char* resource::font_emoji = ":/font/emoji.ttf";
	//const char* resource::font_emoji_color = ":/font/emoji_color.ttf";
#endif
	// Main
	double resource::s_ratio = 1.0;
	size_t resource::s_font_size = 13;

	// Generic
	size_t resource::s_circle_size = 20;
	size_t resource::s_icon_size = 16;
	size_t resource::s_icon_padding = 4;
	size_t resource::s_large_top_margin = 8;
	size_t resource::s_row_height = 24;
	size_t resource::s_small_padding = 3;
	size_t resource::s_small_top_margin = 4;
	size_t resource::s_top_margin = 6;

	// Delegate
	size_t resource::s_bar_margin = 1;
	size_t resource::s_bar_width = 4;
	size_t resource::s_delegate_bottom = 1;
	size_t resource::s_delegate_invalid = 98;
	size_t resource::s_delegate_left = 5 + resource::s_bar_width;
	size_t resource::s_delegate_right = 9;
	size_t resource::s_delegate_top = 5;
	size_t resource::s_delegate_width = 822;
	size_t resource::s_menu_width = s_row_height * 5;

	// Search Weight Delegate
	size_t resource::s_search_text = 20;

	// Chat
	size_t resource::s_chat_verified_width = 25;

	// Layout
	size_t resource::s_combo_box_height = 28;
	size_t resource::s_combo_box_width = 90;
	size_t resource::s_group_box_margin = 8;
	size_t resource::s_text_edit_height = 65;
	size_t resource::s_text_edit_width = 528;
	size_t resource::s_tree_view_indent = 20;
	size_t resource::s_horizontal_spacing = 6;
	size_t resource::s_vertical_spacing = 2;

	// Interface
	void resource::init(size_t font_size /* = 0 */)
	{
		// Calculate Ratio
		s_font_size = (font_size >= ui::min_font_size && font_size <= ui::max_font_size) ? font_size : get_default_font_size();
		s_ratio = s_font_size / 13.0;

		// Font
		QFont font = QFont(font_family::main);
		font.setPixelSize(s_font_size);
		QFontMetrics metrics(font);

		// Apply Ratio
		s_circle_size *= s_ratio;
		s_icon_size *= s_ratio;
		s_icon_padding *= s_ratio;
		s_large_top_margin *= s_ratio;
		s_row_height *= s_ratio;
		s_small_padding *= s_ratio;
		s_small_top_margin *= s_ratio;
		s_top_margin *= s_ratio;

		s_bar_margin *= s_ratio;
		s_bar_width *= s_ratio;
		s_delegate_bottom *= s_ratio;
		s_delegate_invalid *= s_ratio;
		s_delegate_left *= s_ratio;
		s_delegate_right *= s_ratio;
		s_delegate_top *= s_ratio;
		s_delegate_width *= s_ratio;
		s_menu_width = metrics.width("Download To...");

		s_search_text *= s_ratio;
		s_chat_verified_width *= s_ratio;

		s_combo_box_height *= s_ratio;
		s_combo_box_width *= s_ratio;
		s_group_box_margin *= s_ratio;
		s_text_edit_height *= s_ratio;
		s_text_edit_width *= s_ratio;
		s_horizontal_spacing *= s_ratio;
		s_vertical_spacing *= s_ratio;
	}

	double resource::get_default_ratio()
	{
		return get_default_font_size() / 13.0;
	}

	size_t resource::get_default_font_size()
	{
		// Find Resolution
		const auto desktop = QApplication::desktop();
		const auto rect = desktop->availableGeometry(desktop->primaryScreen());

		// Determine Font Size
		const auto multiplier = std::max(static_cast<size_t>(1), static_cast<size_t>(std::round(static_cast<double>(rect.height()) / 540.0)));
		return clamp(static_cast<size_t>(10), static_cast<size_t>(7 + (multiplier * 3)), static_cast<size_t>(24));
	}

	// Chat
	size_t resource::get_chat_edit_margin()
	{
		return s_font_size * 0.2;
	}

	// Size
	size_t resource::get_fa_button_height()
	{
		return s_font_size * 2;
	}

	size_t resource::get_fa_button_width()
	{
		return s_font_size * 2;
	}

	size_t resource::get_label_font_size()
	{
		return s_font_size * 1.20;
	}

	size_t resource::get_large_icon_size()
	{
		return s_icon_size * 1.35;
	}

	size_t resource::get_small_icon_size()
	{
		return s_icon_size * 0.90;
	}

	size_t resource::get_line_edit_height()
	{
		return s_font_size * 2;
	}

	size_t resource::get_small_font_size()
	{
		return s_font_size * 0.90;
	}

	size_t resource::get_large_font_size()
	{
		return s_font_size * 1.35;
	}

	size_t resource::get_chat_box_height()
	{
		return (s_font_size * 2) + get_chat_edit_margin();
	}

	// Layout
	QHBoxLayout* resource::get_hbox_layout(QWidget* parent /*= nullptr*/)
	{
		const auto layout = new QHBoxLayout(parent);
		layout->setContentsMargins(0, 0, 0, 0);
		layout->setSpacing(0);
		layout->setMargin(0);

		return layout;
	}

	QVBoxLayout* resource::get_vbox_layout(QWidget* parent /*= nullptr*/)
	{
		const auto layout = new QVBoxLayout(parent);
		layout->setContentsMargins(0, 0, 0, 0);
		layout->setSpacing(0);
		layout->setMargin(0);

		return layout;
	}
}
