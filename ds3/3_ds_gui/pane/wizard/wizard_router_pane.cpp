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

#include <QColorDialog>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QTimer>
#include <QVBoxLayout>

#include "component/endpoint_component.h"
#include "dialog/wizard_dialog.h"
#include "entity/entity.h"
#include "pane/wizard/wizard_router_pane.h"
#include "resource/icon_provider.h"
#include "font/font_awesome.h"
#include "resource/resource.h"
#include "utility/value.h"

namespace eja
{
	// Constructor
	wizard_router_pane::wizard_router_pane(QWidget* parent /*= 0*/) : QWidget(parent)
	{
		m_dialog = qobject_cast<wizard_dialog*>(parent);

		setObjectName("wizard");

		// Font
		QFont font(demonsaw::font_awesome);

		// Layout
		auto layout = new QVBoxLayout;
		layout->setContentsMargins(0, 0, 0, 0);
		layout->setSpacing(0);
		layout->setMargin(0);

		// Group
		{
			auto vlayout = new QVBoxLayout;
			vlayout->setContentsMargins(0, 0, 0, 0);
			vlayout->setSpacing(0);
			vlayout->setMargin(8);

			// Router
			{
				auto hlayout = new QHBoxLayout;
				hlayout->setContentsMargins(0, 0, 0, 0);
				hlayout->setSpacing(8);
				hlayout->setMargin(0);

				// Name
				{
					auto vvlayout = new QVBoxLayout;

					auto label = new QLabel("Name", this);
					vvlayout->addWidget(label);

					m_name = new QLineEdit(this);
					m_name->setText(router::name);
					m_name->setAttribute(Qt::WA_MacShowFocusRect, 0);
					m_name->setFixedWidth(350);
					m_name->setMaxLength(64);

					vvlayout->addWidget(m_name);
					hlayout->addLayout(vvlayout);
				}

				// Edit
				{
					auto vvlayout = new QVBoxLayout;

					auto label = new QLabel("", this);
					vvlayout->addWidget(label);
					vvlayout->addSpacing(1);

					m_name_color = new QPushButton(QString(fa::paintbrush), this);
					m_name_color->setFocusPolicy(Qt::NoFocus);
					m_name_color->setToolTip("Choose Color");
					m_name_color->setObjectName("font_awesome");
					m_name_color->setFixedSize(QSize(32, 21));
					m_name_color->setFont(font);

					vvlayout->addWidget(m_name_color);
					hlayout->addLayout(vvlayout);
				}

				// Default
				{
					auto vvlayout = new QVBoxLayout;

					auto label = new QLabel("", this);
					vvlayout->addWidget(label);
					vvlayout->addSpacing(1);

					m_name_default = new QPushButton(QString(fa::undo), this);
					m_name_default->setFocusPolicy(Qt::NoFocus);
					m_name_default->setToolTip("Default Color");
					m_name_default->setObjectName("font_awesome");
					m_name_default->setFixedSize(QSize(32, 21));
					m_name_default->setFont(font);

					vvlayout->addWidget(m_name_default);
					hlayout->addLayout(vvlayout);
				}

				hlayout->addStretch(1);
				vlayout->addLayout(hlayout);
				vlayout->addSpacing(8);
			}

			// Network
			{
				auto hlayout = new QHBoxLayout;
				hlayout->setContentsMargins(0, 0, 0, 0);
				hlayout->setSpacing(0);
				hlayout->setMargin(0);

				// Address
				{
					auto vvlayout = new QVBoxLayout;

					auto label = new QLabel("Address", this);
					vvlayout->addWidget(label);

					m_address = new QLineEdit(this);
					m_address->setText(router::localhost);
					m_address->setAttribute(Qt::WA_MacShowFocusRect, 0);
					m_address->setFixedWidth(350);
					m_address->setMaxLength(255);
					vvlayout->addWidget(m_address);

					hlayout->addLayout(vvlayout);
				}

				hlayout->addSpacing(8);

				// Port
				{
					auto vvlayout = new QVBoxLayout;

					auto label = new QLabel("Port", this);
					vvlayout->addWidget(label);

					m_port = new QLineEdit(this);
					m_port->setText(QString::number(router::port));
					m_port->setAttribute(Qt::WA_MacShowFocusRect, 0);
					m_port->setFixedWidth(72);
					m_port->setMaxLength(5);
					vvlayout->addWidget(m_port);

					hlayout->addLayout(vvlayout);
				}

				hlayout->addStretch(1);
				vlayout->addLayout(hlayout);
				vlayout->addSpacing(8);
			}

			const auto group = new QGroupBox("Router", this);
			group->setObjectName("wizard");
			group->setFlat(true);
			group->setLayout(vlayout);

			layout->addWidget(group, 1, Qt::AlignTop);
			layout->addStretch(1);
		}

		// Info
		{
			m_info = new QLabel("Routers connect clients to Demonsaw networks. They are responsible for keeping track of clients and routing all data. Routers can be configured to send messages, files, or both.", this);
			m_info->setObjectName("wizard_info");
			m_info->setWordWrap(true);

			layout->addWidget(m_info);
			layout->addStretch(1);
		}

		// Buttons
		{
			auto hlayout = new QHBoxLayout;
			hlayout->setContentsMargins(0, 0, 0, 0);
			hlayout->setSpacing(8);
			hlayout->setMargin(0);

			auto label = new QLabel(this);
			label->setFont(QFont(demonsaw::font_awesome));
			label->setObjectName("wizard_image");
			label->setText(QString(fa::server));
			hlayout->addWidget(label, 0, Qt::AlignBottom);
			
			hlayout->addStretch(1);

			m_back = new QPushButton(this);
			m_back->setObjectName("wizard");
			m_back->setFixedWidth(80);
			m_back->setIcon(icon_provider::get_icon(16, fa::arrowleft, 82, 193, 117));
			m_back->setText("Back");
			hlayout->addWidget(m_back, 0, Qt::AlignBottom);

			m_done = new QPushButton(this);			
			m_done->setObjectName("wizard");
			m_done->setFixedWidth(80);
			m_done->setIcon(icon_provider::get_icon(16, fa::check, 82, 193, 117));
			m_done->setText("Done");			
			hlayout->addWidget(m_done, 0, Qt::AlignBottom);

			layout->addLayout(hlayout);
		}

		setLayout(layout);

		// Signal
		connect(m_name, &QLineEdit::textChanged, [this](const QString& str)
		{
			if (!m_user_modified)
			{
				m_user_modified = true;
				m_user_color = false;
			}

			set_color();
		});

		connect(m_done, &QPushButton::clicked, [this]() { m_dialog->done(); }); 
		connect(m_back, &QPushButton::clicked, [this]()
		{
			m_name->setText(router::name);
			m_address->setText(router::localhost);
			m_port->setText(QString::number(router::port));

			m_user_color = true;
			m_user_modified = false;

			// Color
			const u32 m_num_colors = (sizeof(s_colors) / sizeof(s_colors[0]));
			const auto color = s_colors[rand() % m_num_colors];
			m_color.setRgba(color);

			set_color();

			m_dialog->back();
		});

		// Color
		connect(m_name_color, &QPushButton::clicked, [this]()
		{
			QColorDialog dialog(m_color, this);
			dialog.setWindowTitle("Router Color");
			dialog.setOptions(QColorDialog::ShowAlphaChannel);

			if (dialog.exec())
			{
				m_user_color = true;
				const auto& selected = dialog.selectedColor();
				m_color = selected.rgba();
				set_color();
			}
		});

		connect(m_name_default, &QPushButton::clicked, [this]()
		{
			m_user_color = false;
			set_color();
		});

		// Color
		const u32 m_num_colors = (sizeof(s_colors) / sizeof(s_colors[0]));
		const auto color = s_colors[rand() % m_num_colors];
		m_color.setRgba(color);

		set_color();
	}

	// Event
	void wizard_router_pane::showEvent(QShowEvent* event)
	{
		if (!visibleRegion().isEmpty())
		{
			// Timer
			QTimer::singleShot(0, m_name, SLOT(setFocus()));
			QTimer::singleShot(0, m_name, SLOT(selectAll()));
		}

		// Event
		QWidget::showEvent(event);
	}

	// Set
	void wizard_router_pane::set_color()
	{
		if (!m_user_color)
		{
			endpoint_component endpoint;
			const auto name = m_name->text().toStdString();
			endpoint.set_name(name);

			m_color.setRgba(endpoint.get_color());
		}

		const auto background = palette().color(QPalette::Base);
		if (m_color == background)
			m_color = palette().color(QPalette::Text);

		const auto rgba = QString("rgba(%1, %2, %3, %4)").arg(m_color.red()).arg(m_color.green()).arg(m_color.blue()).arg(m_color.alpha());
		const auto style = QString("QPushButton { font-size: 14px; color: %2; background: %3; border: 0; padding: 0; } QPushButton:hover { color: %3; background: %2; border: 1px solid %3; } QPushButton:pressed { color: %1; border: 1px solid %1; }").arg(m_color.name()).arg(background.name()).arg(rgba);

		m_name_color->setStyleSheet(style);
		m_name_default->setStyleSheet(style);
	}
}
