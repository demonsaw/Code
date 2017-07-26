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

#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QTimer>
#include <QVBoxLayout>

#include "dialog/wizard_dialog.h"
#include "entity/entity.h"
#include "pane/wizard/wizard_welcome_pane.h"
#include "resource/icon_provider.h"
#include "font/font_awesome.h"
#include "resource/resource.h"
#include "utility/value.h"

namespace eja
{
	// Constructor
	wizard_welcome_pane::wizard_welcome_pane(QWidget* parent /*= 0*/) : QWidget(parent)
	{
		m_dialog = qobject_cast<wizard_dialog*>(parent);

		setObjectName("wizard");

		// Layout
		auto layout = new QVBoxLayout;
		layout->setContentsMargins(0, 0, 0, 0);
		layout->setSpacing(0);
		layout->setMargin(0);

		// Title
		{
			m_title = new QLabel(this);
			m_title->setAttribute(Qt::WA_TranslucentBackground);
			QPixmap pixmap(resource::title);
			m_title->setPixmap(pixmap);

			layout->addWidget(m_title, 0, Qt::AlignCenter);
		}		

		// Motto
		{
			m_motto = new QLabel(this);
			m_motto->setObjectName("wizard");
			m_motto->setText(demonsaw::motto);

			layout->addWidget(m_motto, 0, Qt::AlignCenter);
			layout->addStretch(1);
		}		

		// Group
		{
			auto vlayout = new QVBoxLayout;
			vlayout->setContentsMargins(0, 0, 0, 0);
			vlayout->setSpacing(12);
			vlayout->setMargin(8);

			vlayout->addSpacing(2);

			m_client = new QRadioButton("Client", this);
			m_client->setCheckable(true);
			m_client->setChecked(true);
			vlayout->addWidget(m_client);

			m_router = new QRadioButton("Router", this);
			m_router->setCheckable(true);
			m_router->setChecked(false);
			vlayout->addWidget(m_router);

			vlayout->addSpacing(2);

			const auto group = new QGroupBox("What would you like to create?", this);
			group->setObjectName("wizard");
			group->setFlat(true);
			group->setLayout(vlayout);

			layout->addWidget(group, 1, Qt::AlignVCenter);
			layout->addStretch(1);
		}		

		// Buttons
		{
			auto hlayout = new QHBoxLayout;
			hlayout->setContentsMargins(0, 0, 0, 0);
			hlayout->setSpacing(8);
			hlayout->setMargin(0);

			QImage image(resource::logo_rgb);
			const auto pixmap = QPixmap::fromImage(image.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));

			auto label = new QLabel(this);
			label->setPixmap(pixmap);
			hlayout->addWidget(label, 0, Qt::AlignBottom);

			hlayout->addStretch(1);

			m_skip = new QPushButton(this);			
			m_skip->setObjectName("wizard");
			m_skip->setFixedWidth(80);
			m_skip->setIcon(icon_provider::get_icon(16, fa::bolt, 82, 193, 117));
			m_skip->setText("Quick");
			hlayout->addWidget(m_skip, 0, Qt::AlignBottom);

			m_next = new QPushButton(this);			
			m_next->setObjectName("wizard");
			m_next->setFixedWidth(80);
			m_next->setIcon(icon_provider::get_icon(16, fa::arrowright, 82, 193, 117));
			m_next->setText("Next");
			hlayout->addWidget(m_next, 0, Qt::AlignBottom);

			layout->addLayout(hlayout);
		}

		setLayout(layout);

		// Signal
		connect(m_skip, &QPushButton::clicked, [&]() { m_dialog->quick(); });
		connect(m_next, &QPushButton::clicked, [&]() { m_dialog->next(); });
	}

	// Event
	void wizard_welcome_pane::showEvent(QShowEvent* event)
	{
		if (!visibleRegion().isEmpty())
		{
			// Timer
			QTimer::singleShot(0, m_client, SLOT(setFocus()));
		}

		// Event
		QWidget::showEvent(event);
	}
}
