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
#include <QFile>
#include <QLabel>
#include <QTextDocument>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "about_dialog.h"
#include "resource/resource.h"
#include "utility/default_value.h"

namespace eja
{
	// Constructor
	about_dialog::about_dialog(QWidget* parent /*= 0*/) : QDialog(parent)
	{
		// Widget
		m_widget = new QWidget(this);
		m_widget->setMinimumSize(QSize(512, 384));
		m_widget->installEventFilter(this);

		QMargins margins(8, 0, 0, 2);
		QFont font("Raleway SemiBold", 10, QFont::Normal);
		font.setLetterSpacing(QFont::SpacingType::AbsoluteSpacing, 1.2);

		const char* white = "QLabel { background: transparent; color: white; border: 0px; }";
		const char* gray = "QLabel { background: transparent; color: #979797; border: 0px;}";
		const char* green = "QLabel { background: transparent; color: #00d087; border: 0px; }";
		const char* widget = "QWidget { background-image: url(:/demonsaw/splash.png); border: 0px; margin: 0px; spacing: 0px; }";

		// Version
		QLabel* version = new QLabel(default_value::title, this);
		version->setContentsMargins(margins);
		version->setAttribute(Qt::WA_TranslucentBackground);
		version->setStyleSheet(white);
		version->setFont(font);

		// Quote
		QLabel* quote = new QLabel("Believe in the Right to Share", this);
		quote->setContentsMargins(margins);
		quote->setAttribute(Qt::WA_TranslucentBackground);
		quote->setStyleSheet(white);
		quote->setFont(font);

		// Website
		QLabel* website = new QLabel(this);
		website->setContentsMargins(margins);
		quote->setStyleSheet(white);
		website->setText("<html><a style='color:#00d087; text-decoration:none; margin-left: 8px;' href='https://www.demonsaw.com'>www.demonsaw.com</a></html>");
		website->setAttribute(Qt::WA_TranslucentBackground);
		website->setFont(font);

		website->setTextFormat(Qt::RichText);
		website->setTextInteractionFlags(Qt::TextBrowserInteraction);
		website->setOpenExternalLinks(true);

		// Twitter
		QLabel* twitter = new QLabel(this);
		twitter->setContentsMargins(QMargins(8, 0, 0, 8));
		twitter->setText("<html><a style='color:#00d087; text-decoration:none; margin-left: 8px;' href='https://www.twitter.com/demon_saw'>@demon_saw</a></html>");
		twitter->setAttribute(Qt::WA_TranslucentBackground);
		twitter->setFont(font);

		twitter->setTextFormat(Qt::RichText);
		twitter->setTextInteractionFlags(Qt::TextBrowserInteraction);
		twitter->setOpenExternalLinks(true);

		// Author
		QLabel* author = new QLabel(this);
		author->setContentsMargins(QMargins(0, 0, 8, 2));
		author->setText("<html><a style='color:#00d087; text-decoration:none;' href='https://www.twitter.com/demon_saw'>Eric J Anderson (Eijah)</a></html>");
		author->setAlignment(Qt::AlignRight | Qt::AlignJustify);
		author->setAttribute(Qt::WA_TranslucentBackground);
		author->setFont(font);

		author->setTextFormat(Qt::RichText);
		author->setTextInteractionFlags(Qt::TextBrowserInteraction);
		author->setOpenExternalLinks(true);

		// Build
		QLabel* build = new QLabel(default_value::date, this);
		build->setContentsMargins(QMargins(0, 0, 8, 8));
		build->setAlignment(Qt::AlignRight | Qt::AlignJustify); 
		build->setAttribute(Qt::WA_TranslucentBackground);
		build->setStyleSheet(gray);
		build->setFont(font);		

		QWidget* right = new QWidget(this);
		right->setAttribute(Qt::WA_TranslucentBackground);

		QVBoxLayout* rvlayout = new QVBoxLayout(right);
		rvlayout->setSpacing(0);
		rvlayout->setMargin(0);

		rvlayout->addWidget(author);
		rvlayout->addWidget(build);
		right->setLayout(rvlayout);

		// Layout	
		QVBoxLayout* vlayout = new QVBoxLayout(this);
		vlayout->setSpacing(0);
		vlayout->setMargin(0);

		vlayout->addWidget(m_widget);
		vlayout->addStretch();
		vlayout->addWidget(version);
		vlayout->addWidget(quote);

		// left
		QWidget* left = new QWidget(this);
		left->setAttribute(Qt::WA_TranslucentBackground);

		QVBoxLayout* lvlayout = new QVBoxLayout(left);
		lvlayout->setSpacing(0);
		lvlayout->setMargin(0);

		lvlayout->addWidget(website);
		lvlayout->addWidget(twitter);
		left->setLayout(lvlayout);
		
		// Footer
		QWidget* footer = new QWidget(this);
		footer->setAttribute(Qt::WA_TranslucentBackground);

		QHBoxLayout* hlayout = new QHBoxLayout(footer);
		hlayout->setSpacing(0);
		hlayout->setMargin(0);

		hlayout->addWidget(left);
		hlayout->addWidget(right);
		footer->setLayout(hlayout);

		vlayout->addWidget(footer);

		setMinimumSize(QSize(512, 384));
		setMaximumSize(QSize(512, 384));
		setStyleSheet(widget);
		setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
		setAttribute(Qt::WA_NoSystemBackground);
		setAttribute(Qt::WA_TranslucentBackground);
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
