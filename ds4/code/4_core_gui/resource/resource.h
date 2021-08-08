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

#ifndef _EJA_RESOURCE_H_
#define _EJA_RESOURCE_H_

#include <QString>

#include "system/type.h"

class QHBoxLayout;
class QVBoxLayout;

namespace eja
{
	namespace font_family
	{
		static const char* font_awesome = "FontAwesome";

#if _WIN32
		static const char* main = "Noto Sans";
		static const char* emoji = "Segoe UI Emoji";
#elif _MACX
		static const char* main = "Noto Sans";
		static const char* emoji = "Helvetica Neue";
#else
		static const char* main = "Noto Sans";
		static const char* emoji = "Emoji One";
#endif
	}

	class resource final
	{
	private:
		// Main
		static double s_ratio;
		static size_t s_font_size;

		// Generic
		static size_t s_circle_size;
		static size_t s_icon_size;
		static size_t s_icon_padding;
		static size_t s_large_top_margin;
		static size_t s_row_height;
		static size_t s_small_padding;
		static size_t s_small_top_margin;
		static size_t s_top_margin;

		// Delegate
		static size_t s_bar_margin;
		static size_t s_bar_width;
		static size_t s_delegate_bottom;
		static size_t s_delegate_invalid;
		static size_t s_delegate_left;
		static size_t s_delegate_right;
		static size_t s_delegate_top;
		static size_t s_delegate_width;
		static size_t s_menu_width;

		// Search Weight Delegate
		static size_t s_search_text;

		// Chat
		static size_t s_chat_edit_margin;
		static size_t s_chat_verified_width;

		// Layout
		static size_t s_combo_box_height;
		static size_t s_combo_box_width;
		static size_t s_group_box_margin;
		static size_t s_text_edit_height;
		static size_t s_text_edit_width;
		static size_t s_tree_view_indent;
		static size_t s_horizontal_spacing;
		static size_t s_vertical_spacing;

	public:
		static const char* drag_drop;
		static const char* icon;
		static const char* logo;
		static const char* logo_rgb;
		static const char* logo_black;
		static const char* logo_gray;
		static const char* logo_white;
		static const char* style;
		static const char* title;

		static const char* font;
		static const char* font_bold;
		static const char* font_mono;
		static const char* font_awesome;

#if !_WIN32 && !_MACX
		static const char* font_emoji;
		//static const char* font_emoji_color;
#endif

	private:
		resource() = delete;
		~resource() = delete;

	public:
		// Interface
		static void init(size_t font_size = 0);

		// Main
		static double get_ratio() { return s_ratio; }
		static double get_default_ratio();
		static size_t get_font_size() { return s_font_size; }
		static size_t get_default_font_size();

		// Generic
		static size_t get_fa_button_height();
		static size_t get_fa_button_width();

		static size_t get_circle_size() { return s_circle_size; }
		static size_t get_icon_size() { return s_icon_size; }
		static size_t get_icon_padding() { return s_icon_padding; }
		static size_t get_large_icon_size();
		static size_t get_small_icon_size();

		static size_t get_large_top_margin() { return s_large_top_margin; }
		static size_t get_row_height() { return s_row_height; }
		static size_t get_small_padding() { return s_small_padding; }
		static size_t get_small_top_margin() { return s_small_top_margin; }
		static size_t get_top_margin() { return s_top_margin; }

		// Delegate
		static size_t get_delegate_bar_margin() { return s_bar_margin; }
		static size_t get_delegate_bar_width() { return s_bar_width; }
		static size_t get_delegate_bottom() { return s_delegate_bottom; }
		static size_t get_delegate_invalid() { return s_delegate_invalid; }
		static size_t get_delegate_left() { return s_delegate_left; }
		static size_t get_delegate_right() { return s_delegate_right; }
		static size_t get_delegate_top() { return s_delegate_top; }
		static size_t get_delegate_width() { return s_delegate_width; }
		static size_t get_menu_width() { return s_menu_width; }

		// Search Weight Delegate
		static size_t get_search_text() { return s_search_text; }

		// Chat
		static size_t get_chat_box_height();
		static size_t get_chat_edit_margin();
		static size_t get_chat_verified_width() { return s_chat_verified_width; }

		// Layout
		static size_t get_combo_box_height() { return s_combo_box_height; }
		static size_t get_combo_box_width() { return s_combo_box_width; }
		static size_t get_group_box_margin() { return s_group_box_margin; }
		static size_t get_text_edit_height() { return s_text_edit_height; }
		static size_t get_text_edit_width() { return s_text_edit_width; }
		static size_t get_tree_view_indent() { return s_tree_view_indent; }
		static size_t get_horizontal_spacing() { return s_horizontal_spacing; }
		static size_t get_vertical_spacing() { return s_vertical_spacing; }

		//static QPixmap get_pixmap(const char* svg, size_t width, size_t height);
		static size_t get_label_font_size();
		static size_t get_line_edit_height();
		static size_t get_small_font_size();
		static size_t get_large_font_size();
		static QHBoxLayout* get_hbox_layout(QWidget* parent = nullptr);
		static QVBoxLayout* get_vbox_layout(QWidget* parent = nullptr);
	};
}

#endif
