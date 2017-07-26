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

#include <QDesktopServices>
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QToolBar>
#include <QUrl>

#include "component/callback/callback_type.h"
#include "entity/entity.h"
#include "entity/entity_table_view.h"
#include "font/font_awesome.h"
#include "widget/client/client_help_widget.h"

namespace eja
{
	// Constructor
	client_help_widget::client_help_widget(const entity::ptr entity, QWidget* parent /*= 0*/) : entity_widget(entity, "Help", fa::question, callback_type::help, parent)
	{
		assert(thread_info::main());

		// Menu
		add_button("Popout", fa::clone, [&]() { popout(); });
		add_button("Close", fa::close, [&]() { close(); });

		// Layout
		auto hlayout = new QHBoxLayout;
		hlayout->setSpacing(0);
		hlayout->setMargin(0);

		hlayout->addWidget(m_icon);
		hlayout->addWidget(m_title);
		hlayout->addStretch(1);
		hlayout->addWidget(m_toolbar);

		m_titlebar = new QWidget(this);
		m_titlebar->installEventFilter(this);
		m_titlebar->setObjectName("menubar");
		m_titlebar->setLayout(hlayout);
		setTitleBarWidget(m_titlebar);

		// Quote
		QLabel* tutorial = new QLabel("Tutorials", this);
		tutorial->setAlignment(Qt::AlignHCenter);
		tutorial->setObjectName("help");

		// Intro
		QPushButton* intro = new QPushButton(this);
		connect(intro, &QPushButton::clicked, this, [&] { QDesktopServices::openUrl(QUrl("https://www.demonsaw.com/help/intro.mp4")); });
		intro->setText("Intro");

		// Overview
		QPushButton* overview = new QPushButton(this);
		connect(overview, &QPushButton::clicked, this, [&] { QDesktopServices::openUrl(QUrl("https://www.demonsaw.com/help/overview.mp4")); });
		overview->setText("Overview");

		// Share
		QPushButton* share = new QPushButton(this);
		connect(share, &QPushButton::clicked, this, [&] { QDesktopServices::openUrl(QUrl("https://www.demonsaw.com/help/shares.mp4")); });
		share->setText("Sharing Files");

		// Transfer
		QPushButton* transfer = new QPushButton(this);
		connect(transfer, &QPushButton::clicked, this, [&] { QDesktopServices::openUrl(QUrl("https://www.demonsaw.com/help/download.mp4")); });
		transfer->setText("Downloading Files");

		// Chat
		QPushButton* chat = new QPushButton(this);
		connect(chat, &QPushButton::clicked, this, [&] { QDesktopServices::openUrl(QUrl("https://www.demonsaw.com/help/chat.mp4")); });
		chat->setText("Chat && Messaging");

		// Groups
		QPushButton* group = new QPushButton(this);
		connect(group, &QPushButton::clicked, this, [&] { QDesktopServices::openUrl(QUrl("https://www.demonsaw.com/help/groups.mp4")); });
		group->setText("Secure Groups");

		// Option
		QPushButton* option = new QPushButton(this);
		connect(option, &QPushButton::clicked, this, [&] { QDesktopServices::openUrl(QUrl("https://www.demonsaw.com/help/options.mp4")); });
		option->setText("Advanced Options");

		// Routers
		QPushButton* router = new QPushButton(this);
		connect(router, &QPushButton::clicked, this, [&] { QDesktopServices::openUrl(QUrl("https://www.demonsaw.com/help/routers.mp4")); });
		router->setText("Hosting Routers");

		// Theme
		QPushButton* theme = new QPushButton(this);
		connect(theme, &QPushButton::clicked, this, [&] { QDesktopServices::openUrl(QUrl("https://www.demonsaw.com/help/themes.mp4")); });
		theme->setText("Making Themes");

		// Beta
		QPushButton* beta = new QPushButton(this);
		connect(beta, &QPushButton::clicked, this, [&] { QDesktopServices::openUrl(QUrl("https://www.demonsaw.com/help/betas.mp4")); });
		beta->setText("Beta Builds");

		// Layout
		QVBoxLayout* vlayout = new QVBoxLayout;
		vlayout->setSpacing(0);
		vlayout->setMargin(6);

		vlayout->addWidget(tutorial);
		vlayout->addSpacing(4);

		vlayout->addWidget(intro);
		vlayout->addSpacing(4);

		vlayout->addWidget(overview);
		vlayout->addSpacing(4);

		vlayout->addWidget(share);
		vlayout->addSpacing(4);

		vlayout->addWidget(transfer);
		vlayout->addSpacing(4);

		vlayout->addWidget(chat);
		vlayout->addSpacing(4);

		vlayout->addWidget(group);
		vlayout->addSpacing(4);

		vlayout->addWidget(option);
		vlayout->addSpacing(4);

		vlayout->addWidget(router);
		vlayout->addSpacing(4);

		vlayout->addWidget(theme);
		vlayout->addSpacing(4);

		vlayout->addWidget(beta);
		vlayout->addStretch();

		const auto widget = new QWidget(this);
		widget->setObjectName("border");
		widget->setLayout(vlayout);
		setWidget(widget);
	}
}
