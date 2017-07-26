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

#include <QEvent>
#include <QLabel>
#include <QPainter>
#include <QVBoxLayout>

#include "dialog/about_dialog.h"
#include "utility/value.h"

namespace eja
{
	// Constructor
	about_dialog::about_dialog(QWidget* parent /*= 0*/) : QDialog(parent, Qt::FramelessWindowHint)
	{
		setAttribute(Qt::WA_TranslucentBackground);
		setStyleSheet("QWidget { background:transparent; background-image: url(:/logo.png); border: 0px; margin: 0px; spacing: 0px; }");
		setFixedSize(QSize(384, 384));

		// Widget
		m_widget = new QWidget(this);
		m_widget->setFixedSize(QSize(384, 384));
		m_widget->installEventFilter(this);

		// Version
		QLabel* version = new QLabel(version::app, this);
		version->setAlignment(Qt::AlignHCenter | Qt::AlignJustify);
		version->setStyleSheet("QLabel { background:transparent; color:white; border:0px; font-size:24px; }");

		// Quote
		QLabel* quote = new QLabel(demonsaw::motto, this);
		quote->setAlignment(Qt::AlignHCenter | Qt::AlignJustify);
		quote->setStyleSheet("QLabel { background:transparent; color:white; border:0px; font-size:12px; }");

		// Build
		QLabel* build = new QLabel(demonsaw::date, this);
		build->setAlignment(Qt::AlignHCenter | Qt::AlignJustify);
		build->setStyleSheet("QLabel { background:transparent; color:white; border:0px; font-size:12px; }");

		// Author
		QLabel* author = new QLabel(demonsaw::author, this);
		author->setAlignment(Qt::AlignHCenter | Qt::AlignJustify);
		author->setStyleSheet("QLabel { background:transparent; color:#a0a0a0; border:0px; font-size:12px; }");

		// Website
		QLabel* website = new QLabel(this);
		website->setAlignment(Qt::AlignHCenter | Qt::AlignJustify);
		website->setText("<html><a style='color:#52c175; font-size:12px; text-decoration:none;' href='https://www.demonsaw.com'>www.demonsaw.com</a></html>");

		website->setTextFormat(Qt::RichText);
		website->setTextInteractionFlags(Qt::TextBrowserInteraction);
		website->setOpenExternalLinks(true);

		// Twitter
		QLabel* twitter = new QLabel(this);
		twitter->setAlignment(Qt::AlignHCenter | Qt::AlignJustify);
		twitter->setText("<html><a style='color:#52c175; font-size:12px; text-decoration:none;' href='https://www.twitter.com/demon_saw'>@demon_saw</a></html>");

		twitter->setTextFormat(Qt::RichText);
		twitter->setTextInteractionFlags(Qt::TextBrowserInteraction);
		twitter->setOpenExternalLinks(true);

		// Layout	
		QVBoxLayout* vlayout = new QVBoxLayout(this);
		vlayout->setSpacing(0);
		vlayout->setMargin(0);
		vlayout->addSpacing(32);

		vlayout->addWidget(version);		
		vlayout->addWidget(quote);
		vlayout->addSpacing(2);

		vlayout->addWidget(build);
		vlayout->addSpacing(60);

		vlayout->addWidget(author);
		vlayout->addSpacing(2);

		vlayout->addWidget(website);
		vlayout->addSpacing(2);

		vlayout->addWidget(twitter);
		vlayout->addStretch();
		setLayout(vlayout);
	}

	// Event
	bool about_dialog::eventFilter(QObject* obj, QEvent* e)
	{
		if ((e->type() == QEvent::MouseButtonRelease) && (obj == m_widget))
			close();

		return false;
	}
}
