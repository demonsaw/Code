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
#include <QBitmap>
#include <QDesktopWidget>
#include <QEvent>
#include <QLabel>
#include <QPainter>
#include <QVBoxLayout>

#include "dialog/about_dialog.h"
#include "resource/resource.h"
#include "utility/value.h"

namespace eja
{
	// Constructor
	about_dialog::about_dialog(QWidget* parent /*= nullptr*/) : QDialog(parent, Qt::FramelessWindowHint | Qt::Popup | Qt::NoDropShadowWindowHint)
	{
		// Ratio
		const auto ratio = resource::get_ratio();

		setAttribute(Qt::WA_TranslucentBackground);
		setFixedSize(QSize(448 * ratio, 448 * ratio));

		// Event
		installEventFilter(this);

		// Font
		auto font = QApplication::font();
		font.setFamily(font_family::main);
		setFont(font);

		// Pixmap
		QPixmap pixmap(resource::logo);
		m_background = pixmap.scaledToWidth(448 * ratio, Qt::SmoothTransformation);
		setMask(m_background.mask());

		// Version
		QLabel* version = new QLabel(software::build, this);
		version->setAlignment(Qt::AlignCenter);
		version->setStyleSheet(QString("QLabel { background:transparent; color:#ffffff; border:0px; font-size:%1px; font-weight:bold; }").arg(static_cast<size_t>(14 * ratio)));

		// Quote
		QLabel* quote = new QLabel(software::motto, this);
		quote->setAlignment(Qt::AlignCenter);
		quote->setStyleSheet(QString("QLabel { background:transparent; color:#ffffff; border:0px; font-size:%1px; }").arg(static_cast<size_t>(14 * ratio)));

		// Edition
		const auto hlayout = resource::get_hbox_layout();

#if _WIN32
		QLabel* edition_bolt = new QLabel(QString::fromStdWString(L"\xD83D\xDD25"), this);
#else
		QLabel* edition_bolt = new QLabel("\uD83D\uDD25", this);
#endif
		edition_bolt->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
		edition_bolt->setStyleSheet(QString("QLabel { background:transparent; color:#f2c80f; border:0px; font-size:%1px; }").arg(static_cast<size_t>(18 * ratio)));
		hlayout->addWidget(edition_bolt);
		hlayout->addSpacing(4 * ratio);

		QLabel* edition_text = new QLabel("Reefer Madness\nCammy Sadness Edition", this);
		edition_text->setAlignment(Qt::AlignCenter);
		edition_text->setStyleSheet(QString("QLabel { background:transparent; color:#ffffff; border:0px; font-size:%1px; }").arg(static_cast<size_t>(14 * ratio)));
		hlayout->addWidget(edition_text);

		const auto edition = new QWidget(this);
		edition->setStyleSheet("QWidget { background:transparent; }");
		edition->setLayout(hlayout);

		// Data
		QLabel* date = new QLabel(software::date, this);
		date->setAlignment(Qt::AlignCenter);
		date->setStyleSheet(QString("QLabel { background:transparent; color:#ffffff; border:0px; font-size:%1px; }").arg(static_cast<size_t>(14 * ratio)));

		// Credit
		QLabel* credit = new QLabel(this);
		credit->setAlignment(Qt::AlignCenter);
		credit->setText(QString("<html><a style='color:#52c175; font-size:%1px; font-weight:bold; text-decoration:none;' href='https://www.eijah.com'>An Eijah Production</a></html>").arg(static_cast<size_t>(16 * ratio)));
		credit->setFocusPolicy(Qt::FocusPolicy::NoFocus);

		credit->setTextFormat(Qt::RichText);
		credit->setTextInteractionFlags(Qt::TextBrowserInteraction);
		credit->setOpenExternalLinks(true);

		// Layout
		QVBoxLayout* vlayout = resource::get_vbox_layout(this);
		vlayout->setContentsMargins(0, 0, 0, 0);
		vlayout->setSpacing(2 * ratio);
		vlayout->setMargin(0);

		vlayout->addSpacing(40 * ratio);
		vlayout->addWidget(version);
		vlayout->addWidget(quote);
		vlayout->addWidget(date);

		vlayout->addSpacing(65 * ratio);
		vlayout->addWidget(credit);

		vlayout->addSpacing(5 * ratio);
		vlayout->addWidget(edition, 0, Qt::AlignCenter);
		vlayout->addStretch();

		setLayout(vlayout);
	}

	// Event
	bool about_dialog::eventFilter(QObject* obj, QEvent* e)
	{
		if ((e->type() == QEvent::MouseButtonRelease) || (e->type() == QEvent::KeyPress))
		{
			close();

			return true;
		}

		return QDialog::eventFilter(obj, e);
	}

	// Paint
	void about_dialog::paintEvent(QPaintEvent *event)
	{
		QPainter painter(this);
		QPoint center(width() / 2 - m_background.width() / 2, 0);
		painter.drawPixmap(center, m_background);
	}
}
