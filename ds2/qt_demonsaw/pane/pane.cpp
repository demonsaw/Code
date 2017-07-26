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

#include <QHBoxLayout>
#include <QLabel>
#include <QString>
#include <QToolButton>
#include <QVBoxLayout>

#include "pane.h"

namespace eja
{
	// Static
	u32 pane::s_row = 188;
	u32 pane::s_button = 17;
	u32 pane::s_hspacing = 6;
	u32 pane::s_vspacing = 2;

	// Row
	void pane::add_row(QVBoxLayout* layout, const char* name)
	{
		QHBoxLayout* hlayout = new QHBoxLayout;
		hlayout->addWidget(new QLabel(QObject::tr(name)));
		hlayout->addStretch(1);

		layout->addLayout(hlayout);
	}

	void pane::add_row(QVBoxLayout* layout, const char* lname, const char* rname)
	{
		QHBoxLayout* hlayout = new QHBoxLayout;
		hlayout->addWidget(new QLabel(QObject::tr(lname)));
		hlayout->addSpacing(s_hspacing);
		hlayout->addWidget(new QLabel(QObject::tr(rname)));
		hlayout->addStretch(1);

		layout->addLayout(hlayout);
	}

	void pane::add_row(QVBoxLayout* layout, const char* lname, const char* cname, const char* rname)
	{
		QHBoxLayout* hlayout = new QHBoxLayout;
		hlayout->addWidget(new QLabel(QObject::tr(lname)));
		hlayout->addSpacing(s_hspacing);
		hlayout->addWidget(new QLabel(QObject::tr(cname)));
		hlayout->addSpacing(s_hspacing);
		hlayout->addWidget(new QLabel(QObject::tr(rname)));
		hlayout->addStretch(1);

		layout->addLayout(hlayout);
	}

	void pane::add_row(QVBoxLayout* layout, QWidget* widget)
	{
		QHBoxLayout* hlayout = new QHBoxLayout;
		hlayout->addWidget(widget);
		hlayout->addStretch(1);

		layout->addLayout(hlayout);
	}

	void pane::add_row(QVBoxLayout* layout, QWidget* lwidget, QWidget* rwidget)
	{
		QHBoxLayout* hlayout = new QHBoxLayout;
		hlayout->addWidget(lwidget);
		hlayout->addSpacing(s_hspacing);
		hlayout->addWidget(rwidget);
		hlayout->addStretch(1);

		layout->addLayout(hlayout);
	}

	void pane::add_row(QVBoxLayout* layout, QWidget* lwidget, QWidget* cwidget, QWidget* rwidget)
	{
		QHBoxLayout* hlayout = new QHBoxLayout;
		hlayout->addWidget(lwidget);
		hlayout->addSpacing(s_hspacing);
		hlayout->addWidget(cwidget);
		hlayout->addSpacing(s_hspacing);
		hlayout->addWidget(rwidget);
		hlayout->addStretch(1);

		layout->addLayout(hlayout);
	}

	void pane::add_row(QVBoxLayout* layout, QWidget* widget, QToolButton* button)
	{
		QHBoxLayout* nlayout = new QHBoxLayout;
		nlayout->addWidget(widget);
		nlayout->addStretch(1);
		nlayout->addWidget(button);

		layout->addLayout(nlayout);
	}

	void pane::add_row(QVBoxLayout* layout, QWidget* widget, QToolButton* lbutton, QToolButton* rbutton)
	{
		QHBoxLayout* nlayout = new QHBoxLayout;
		nlayout->addWidget(widget);
		nlayout->addStretch(1);
		nlayout->addWidget(lbutton);
		nlayout->addWidget(rbutton);

		layout->addLayout(nlayout);
	}

	void pane::add_row(QVBoxLayout* layout, QWidget* widget, QToolButton* lbutton, QToolButton* cbutton, QToolButton* rbutton)
	{
		QHBoxLayout* nlayout = new QHBoxLayout;
		nlayout->addWidget(widget);
		nlayout->addStretch(1);
		nlayout->addWidget(lbutton);
		nlayout->addWidget(cbutton);
		nlayout->addWidget(rbutton);

		layout->addLayout(nlayout);
	}

	void pane::add_row(QVBoxLayout* layout, const char* name, QWidget* widget)
	{
		QHBoxLayout* nlayout = new QHBoxLayout;
		nlayout->addWidget(new QLabel(QObject::tr(name)));
		nlayout->addStretch(1);

		QHBoxLayout* wlayout = new QHBoxLayout;
		wlayout->addWidget(widget);
		wlayout->addStretch(1);

		layout->addLayout(nlayout);
		layout->addLayout(wlayout);
	}

	void pane::add_row(QVBoxLayout* layout, const char* name, QWidget* widget, QLabel* button)
	{
		QHBoxLayout* nlayout = new QHBoxLayout;
		QLabel* label = new QLabel(QObject::tr(name));
		label->setFixedWidth(widget->width() - s_button);

		nlayout->addWidget(label);
		nlayout->addWidget(button);
		nlayout->addStretch(1);

		QHBoxLayout* wlayout = new QHBoxLayout;
		wlayout->addWidget(widget);
		wlayout->addStretch(1);

		layout->addLayout(nlayout);
		layout->addLayout(wlayout);
	}

	void pane::add_row(QVBoxLayout* layout, const char* name, QWidget* widget, QToolButton* button)
	{
		QHBoxLayout* nlayout = new QHBoxLayout;
		QLabel* label = new QLabel(QObject::tr(name));
		label->setFixedWidth(widget->width() - s_button);
		
		nlayout->addWidget(label);
		nlayout->addWidget(button);
		nlayout->addStretch(1);

		QHBoxLayout* wlayout = new QHBoxLayout;
		wlayout->addWidget(widget);
		wlayout->addStretch(1);

		layout->addLayout(nlayout);
		layout->addLayout(wlayout);
	}

	void pane::add_row(QVBoxLayout* layout, const char* name, QWidget* widget, QToolButton* lbutton, QToolButton* rbutton)
	{
		QHBoxLayout* nlayout = new QHBoxLayout;		
		QLabel* label = new QLabel(QObject::tr(name));
		label->setFixedWidth(widget->width() - (s_button * 2));
		nlayout->addWidget(label);

		nlayout->addWidget(lbutton);
		nlayout->addWidget(rbutton);
		nlayout->addStretch(1);

		QHBoxLayout* wlayout = new QHBoxLayout;
		wlayout->addWidget(widget);
		wlayout->addStretch(1);

		layout->addLayout(nlayout);
		layout->addLayout(wlayout);
	}

	void pane::add_row(QVBoxLayout* layout, const char* name, QWidget* widget, QToolButton* lbutton, QToolButton* cbutton, QToolButton* rbutton)
	{
		QHBoxLayout* nlayout = new QHBoxLayout;
		QLabel* label = new QLabel(QObject::tr(name));
		label->setFixedWidth(widget->width() - (s_button * 3));
		nlayout->addWidget(label);

		nlayout->addWidget(lbutton);
		nlayout->addWidget(cbutton);
		nlayout->addWidget(rbutton);
		nlayout->addStretch(1);

		QHBoxLayout* wlayout = new QHBoxLayout;
		wlayout->addWidget(widget);
		wlayout->addStretch(1);

		layout->addLayout(nlayout);
		layout->addLayout(wlayout);
	}

	void pane::add_row(QVBoxLayout* layout, const char* lname, QWidget* lwidget, const char* rname, QWidget* rwidget)
	{
		QHBoxLayout* nlayout = new QHBoxLayout;

		QLabel* llabel = new QLabel(QObject::tr(lname));
		llabel->setFixedWidth(lwidget->width());
		nlayout->addWidget(llabel);
		nlayout->addSpacing(s_hspacing);

		QLabel* rlabel = new QLabel(QObject::tr(rname));
		rlabel->setFixedWidth(rwidget->width());
		nlayout->addWidget(rlabel);
		nlayout->addStretch(1);

		QHBoxLayout* wlayout = new QHBoxLayout;
		wlayout->addWidget(lwidget);
		wlayout->addSpacing(s_hspacing);
		wlayout->addWidget(rwidget);
		wlayout->addStretch(1);

		layout->addLayout(nlayout);
		layout->addLayout(wlayout);
	}

	void pane::add_row(QVBoxLayout* layout, const char* lname, QWidget* lwidget, const char* rname, QWidget* rwidget, QToolButton* rbutton)
	{
		QHBoxLayout* nlayout = new QHBoxLayout;

		QLabel* llabel = new QLabel(QObject::tr(lname));
		llabel->setFixedWidth(lwidget->width());
		nlayout->addWidget(llabel);
		nlayout->addSpacing(s_hspacing);

		QLabel* rlabel = new QLabel(QObject::tr(rname));
		rlabel->setFixedWidth(rwidget->width() - s_button);
		nlayout->addWidget(rlabel);
		nlayout->addWidget(rbutton);
		nlayout->addStretch(1);

		QHBoxLayout* wlayout = new QHBoxLayout;
		wlayout->addWidget(lwidget);
		wlayout->addSpacing(s_hspacing);
		wlayout->addWidget(rwidget);
		wlayout->addStretch(1);

		layout->addLayout(nlayout);
		layout->addLayout(wlayout);
	}

	void pane::add_row(QVBoxLayout* layout, const char* lname, QWidget* lwidget, QToolButton* lbutton, const char* rname, QWidget* rwidget, QToolButton* rbutton)
	{
		QHBoxLayout* nlayout = new QHBoxLayout;

		QLabel* llabel = new QLabel(QObject::tr(lname));
		llabel->setFixedWidth(lwidget->width() - s_button);
		nlayout->addWidget(llabel);
		nlayout->addWidget(lbutton);
		nlayout->addSpacing(s_hspacing);

		QLabel* rlabel = new QLabel(QObject::tr(rname));
		rlabel->setFixedWidth(rwidget->width() - s_button);
		nlayout->addWidget(rlabel);
		nlayout->addWidget(rbutton);
		nlayout->addStretch(1);

		QHBoxLayout* wlayout = new QHBoxLayout;
		wlayout->addWidget(lwidget);
		wlayout->addSpacing(s_hspacing);
		wlayout->addWidget(rwidget);
		wlayout->addStretch(1);

		layout->addLayout(nlayout);
		layout->addLayout(wlayout);
	}

	void pane::add_row(QVBoxLayout* layout, const char* lname, QWidget* lwidget, QToolButton* lbutton, QToolButton* rbutton, const char* rname, QWidget* rwidget)
	{
		QHBoxLayout* nlayout = new QHBoxLayout;

		QLabel* llabel = new QLabel(QObject::tr(lname));
		llabel->setFixedWidth(lwidget->width() - (s_button * 2));
		nlayout->addWidget(llabel);
		nlayout->addWidget(lbutton);
		nlayout->addWidget(rbutton); 
		nlayout->addSpacing(s_hspacing);

		QLabel* rlabel = new QLabel(QObject::tr(rname));
		rlabel->setFixedWidth(rwidget->width());
		nlayout->addWidget(rlabel);
		nlayout->addStretch(1);

		QHBoxLayout* wlayout = new QHBoxLayout;
		wlayout->addWidget(lwidget);
		wlayout->addSpacing(s_hspacing);
		wlayout->addWidget(rwidget);
		wlayout->addStretch(1);

		layout->addLayout(nlayout);
		layout->addLayout(wlayout);
	}

	void pane::add_row(QVBoxLayout* layout, const char* lname, QWidget* lwidget, QToolButton* lbutton, QToolButton* rbutton, const char* rname, QWidget* rwidget, QToolButton* rrbutton)
	{
		QHBoxLayout* nlayout = new QHBoxLayout;

		QLabel* llabel = new QLabel(QObject::tr(lname));
		llabel->setFixedWidth(lwidget->width() - (s_button * 2));
		nlayout->addWidget(llabel);
		nlayout->addWidget(lbutton);
		nlayout->addWidget(rbutton);
		nlayout->addSpacing(s_hspacing);

		QLabel* rlabel = new QLabel(QObject::tr(rname));
		rlabel->setFixedWidth(rwidget->width() - s_button);
		nlayout->addWidget(rlabel);
		nlayout->addWidget(rrbutton);
		nlayout->addStretch(1);

		QHBoxLayout* wlayout = new QHBoxLayout;
		wlayout->addWidget(lwidget);
		wlayout->addSpacing(s_hspacing);
		wlayout->addWidget(rwidget);
		wlayout->addStretch(1);

		layout->addLayout(nlayout);
		layout->addLayout(wlayout);
	}

	void pane::add_row(QVBoxLayout* layout, const char* lname, QWidget* lwidget, const char* cname, QWidget* cwidget, const char* rname, QWidget* rwidget)
	{
		QHBoxLayout* nlayout = new QHBoxLayout;

		QLabel* llabel = new QLabel(QObject::tr(lname));
		llabel->setFixedWidth(lwidget->width());
		nlayout->addWidget(llabel);
		nlayout->addSpacing(s_hspacing);

		QLabel* clabel = new QLabel(QObject::tr(cname));
		clabel->setFixedWidth(cwidget->width());
		nlayout->addWidget(clabel);
		nlayout->addStretch(1);

		QLabel* rlabel = new QLabel(QObject::tr(rname));
		rlabel->setFixedWidth(rwidget->width());
		nlayout->addWidget(rlabel);
		nlayout->addStretch(1);

		QHBoxLayout* wlayout = new QHBoxLayout;
		wlayout->addWidget(lwidget);
		wlayout->addSpacing(s_hspacing);
		wlayout->addWidget(cwidget);
		wlayout->addSpacing(s_hspacing);
		wlayout->addWidget(rwidget);
		wlayout->addStretch(1);

		layout->addLayout(nlayout);
		layout->addLayout(wlayout);
	}

	// 3
	void pane::add_row(QVBoxLayout* layout, const char* lname, QWidget* lwidget, QToolButton* lbutton, const char* cname, QWidget* cwidget, QToolButton* cbutton, const char* rname, QWidget* rwidget, QToolButton* rbutton)
	{
		QHBoxLayout* nlayout = new QHBoxLayout;

		QLabel* llabel = new QLabel(QObject::tr(lname));
		llabel->setFixedWidth(lwidget->width() - s_button);
		nlayout->addWidget(llabel);
		nlayout->addWidget(lbutton);
		nlayout->addSpacing(s_hspacing);

		QLabel* clabel = new QLabel(QObject::tr(cname));
		clabel->setFixedWidth(cwidget->width() - s_button);
		nlayout->addWidget(clabel);
		nlayout->addWidget(cbutton);
		nlayout->addSpacing(s_hspacing);

		QLabel* rlabel = new QLabel(QObject::tr(rname));
		rlabel->setFixedWidth(rwidget->width() - s_button);
		nlayout->addWidget(rlabel);
		nlayout->addWidget(rbutton);
		nlayout->addStretch(1);

		QHBoxLayout* wlayout = new QHBoxLayout;
		wlayout->addWidget(lwidget);
		wlayout->addSpacing(s_hspacing);
		wlayout->addWidget(cwidget); 
		wlayout->addSpacing(s_hspacing);
		wlayout->addWidget(rwidget);
		wlayout->addStretch(1);

		layout->addLayout(nlayout);
		layout->addLayout(wlayout);
	}
}
