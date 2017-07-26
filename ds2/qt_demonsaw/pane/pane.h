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

#ifndef _EJA_PANE_
#define _EJA_PANE_

#include <QLabel>
#include <QMargins>
#include <QMetaType>
#include "entity/entity.h"

class QToolButton;
class QVBoxLayout;

namespace eja
{
	enum class pane_alignment { left, center, right };

	class pane final
	{
	private:
		static u32 s_row; 
		static u32 s_button;
		static u32 s_hspacing;
		static u32 s_vspacing;

	private:
		pane() { }
		~pane() { }

	public:
		// Utility
		static void add_row(QVBoxLayout* layout, const char* name);
		static void add_row(QVBoxLayout* layout, const char* lname, const char* rname);
		static void add_row(QVBoxLayout* layout, const char* lname, const char* cname, const char* rname);

		static void add_row(QVBoxLayout* layout, QWidget* widget);
		static void add_row(QVBoxLayout* layout, QWidget* lwidget, QWidget* rwidget);
		static void add_row(QVBoxLayout* layout, QWidget* lwidget, QWidget* cwidget, QWidget* rwidget);

		static void add_row(QVBoxLayout* layout, QWidget* widget, QToolButton* button); 
		static void add_row(QVBoxLayout* layout, QWidget* widget, QToolButton* lbutton, QToolButton* rbutton);
		static void add_row(QVBoxLayout* layout, QWidget* widget, QToolButton* lbutton, QToolButton* cbutton, QToolButton* rbutton);		

		static void add_row(QVBoxLayout* layout, const char* name, QWidget* widget);
		static void add_row(QVBoxLayout* layout, const char* name, QWidget* widget, QLabel* button);
		static void add_row(QVBoxLayout* layout, const char* name, QWidget* widget, QToolButton* button);
		static void add_row(QVBoxLayout* layout, const char* name, QWidget* widget, QToolButton* lbutton, QToolButton* rbutton);
		static void add_row(QVBoxLayout* layout, const char* name, QWidget* widget, QToolButton* lbutton, QToolButton* cbutton, QToolButton* rbutton);
		static void add_row(QVBoxLayout* layout, const char* lname, QWidget* lwidget, const char* rname, QWidget* rwidget);
		static void add_row(QVBoxLayout* layout, const char* lname, QWidget* lwidget, const char* rname, QWidget* rwidget, QToolButton* rbutton);
		static void add_row(QVBoxLayout* layout, const char* lname, QWidget* lwidget, QToolButton* lbutton, const char* rname, QWidget* rwidget, QToolButton* rbutton);
		static void add_row(QVBoxLayout* layout, const char* lname, QWidget* lwidget, QToolButton* lbutton, QToolButton* rbutton, const char* rname, QWidget* rwidget);
		static void add_row(QVBoxLayout* layout, const char* lname, QWidget* lwidget, QToolButton* lbutton, QToolButton* rbutton, const char* rname, QWidget* rwidget, QToolButton* rrbutton);
		static void add_row(QVBoxLayout* layout, const char* lname, QWidget* lwidget, QToolButton* lbutton, const char* cname, QWidget* cwidget, QToolButton* cbutton, const char* rname, QWidget* rwidget, QToolButton* rbutton);
		static void add_row(QVBoxLayout* layout, const char* lname, QWidget* lwidget, const char* cname, QWidget* cwidget, const char* rname, QWidget* rwidget);

		// Spacing
		static void add_spacing(QVBoxLayout* layout, const size_t value) { layout->addSpacing(value); }
		static void add_stretch(QVBoxLayout* layout, const size_t value = 1) { layout->addStretch(value); }

	};
}

#endif
