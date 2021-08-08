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

#include <QApplication>
#include <QFont>

#include "pane/statusbar_pane.h"
#include "resource/resource.h"
#include "utility/value.h"

namespace eja
{
	// Constructor
	statusbar_pane::statusbar_pane(QWidget* parent /*= nullptr*/) : QWidget(parent)
	{
		// Ratio
		const auto ratio = resource::get_ratio();

		// Font
		auto font = QApplication::font();
		font.setFamily(font_family::main);
		font.setPixelSize(resource::get_small_font_size());
		setFont(font);

		// Image Font
		auto image_font = QFont(software::font_awesome);
		image_font.setPixelSize(resource::get_icon_size());

		// Image
		m_image = new QLabel(parent);
		m_image->setFont(image_font);
		m_image->setObjectName("status_image");

		// Text Font
		auto text_font = font;
		text_font.setPixelSize(resource::get_small_font_size());

		// Text
		m_text = new QLabel(parent);
		m_text->setFont(text_font);
		m_text->setMinimumWidth(48 * ratio);
	}
	
	statusbar_pane::statusbar_pane(const int image, const char* psz, QWidget* parent /*= nullptr*/) : QWidget(parent)
	{
		set_image(image);
		set_text(psz);
	}
	
	// Set
	void statusbar_pane::set(const int image, const char* text)
	{ 
		set_image(image); 
		set_text(text); 
	}
	
	void statusbar_pane::set_image(const int image)
	{
		m_image->setText(QString(image));		
	}
}
