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

#include <random>
#include <boost/format.hpp>

#include <QColorDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QMainWindow>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>

#include "component/name_component.h"
#include "font/font_awesome.h"
#include "resource/icon_provider.h"
#include "resource/resource.h"
#include "resource/word.h"
#include "utility/value.h"
#include "widget/wizard_widget.h"
#include "window/main_window.h"

namespace eja
{
	wizard_widget::wizard_widget(QWidget* parent /* = nullptr */) : QWidget(parent)
	{
		setObjectName("wizard");
		setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

		// Ratio
		const auto ratio = resource::get_ratio();

		// Font
		auto font = QApplication::font();
		font.setFamily(font_family::main);
		setFont(font);

		QFont awesome(software::font_awesome);
		awesome.setPixelSize(resource::get_icon_size());

		// Label Font
		QFont label_font = QApplication::font();
		label_font.setFamily(font_family::main);
		label_font.setPixelSize(resource::get_label_font_size());

		// Pixmap
		QPixmap pixmap(":/logo_black.svg");
		m_background = pixmap.scaledToWidth(800 * ratio, Qt::SmoothTransformation);

		// Layout
		const auto layout = resource::get_vbox_layout();
		layout->setMargin(resource::get_large_top_margin());
		layout->addSpacing(resource::get_large_top_margin() * 5);

		// Groupbox
		{
			// Logo
			auto title = QPixmap(resource::title);
			auto scaled_title = title.scaledToWidth(448 * ratio, Qt::SmoothTransformation);
			auto label = new QLabel(this);
			label->setPixmap(scaled_title);
			label->setAlignment(Qt::AlignHCenter);
			label->setObjectName("wizard_image");
			label->setToolTip(software::author);
			label->setToolTipDuration(0);

			layout->addWidget(label);
			layout->addSpacing(resource::get_large_top_margin() * 2);

			// Info
			{
				const auto hlayout = resource::get_hbox_layout();

				// Instructions Font
				QFont instructions_font = QApplication::font();
				instructions_font.setFamily(font_family::main);
				instructions_font.setPixelSize(resource::get_small_font_size());

				const char* psz = "Demonsaw is an encrypted communications platform that allows you to chat, message, and transfer files without fear of data collection or surveillance. &nbsp;" \
					"For more information, please visit: <html><a style='color:#0c9bdc; text-decoration:none;' href='https://www.demonsaw.com'>demonsaw.com</a></html>";

				m_info = new QLabel(psz, this);
				m_info->setTextFormat(Qt::RichText);
				m_info->setTextInteractionFlags(Qt::TextBrowserInteraction);
				m_info->setOpenExternalLinks(true);

				m_info->setObjectName("wizard_info");
				m_info->setWordWrap(true);
				m_info->setFixedWidth(448 * ratio);
				m_info->setAlignment(Qt::AlignCenter);
				m_info->setFont(instructions_font);

				hlayout->addWidget(m_info);
				hlayout->addStretch(1);

				layout->addLayout(hlayout);
				layout->addSpacing(resource::get_large_top_margin() * 4);
			}

			// Client
			{
				const auto hlayout = resource::get_hbox_layout();
				hlayout->setSpacing(resource::get_horizontal_spacing());

				// Name
				{
					const auto vvlayout = resource::get_vbox_layout();

					auto label = new QLabel("Name", this);
					label->setFont(label_font);
					vvlayout->addWidget(label);
					vvlayout->addSpacing(resource::get_vertical_spacing());

					m_name = new QLineEdit(this);
					m_name->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
					m_name->setText(get_random_name());
					m_name->setAttribute(Qt::WA_MacShowFocusRect, 0);
					m_name->setMinimumWidth(386 * ratio);
					m_name->setMaxLength(io::max_name_size);
					m_name->setFixedHeight(resource::get_line_edit_height());
					vvlayout->addWidget(m_name, 16);

					hlayout->addLayout(vvlayout);
				}

				// Edit
				{
					m_name_color = new QPushButton(QString(fa::paint_brush), this);
					m_name_color->setToolTip("Set Color");
					m_name_color->setFixedSize(QSize(resource::get_fa_button_width(), resource::get_fa_button_height()));
					m_name_color->setFont(awesome);
					hlayout->addWidget(m_name_color, 0, Qt::AlignBottom);
				}

				// Random
				{
					m_random = new QPushButton(QString(fa::random), this);
					m_random->setToolTip("Random Color");
					m_random->setFixedSize(QSize(resource::get_fa_button_width(), resource::get_fa_button_height()));
					m_random->setFont(awesome);

					hlayout->addWidget(m_random, 0, Qt::AlignBottom);
				}

				hlayout->addStretch(1);
				layout->addLayout(hlayout);
				layout->addSpacing(resource::get_top_margin() * 2);
			}

			// Network
			{
				const auto hlayout = resource::get_hbox_layout();
				hlayout->setSpacing(resource::get_horizontal_spacing());

				// Address
				{
					const auto vvlayout = resource::get_vbox_layout();

					auto label = new QLabel("Router Address", this);
					label->setFont(label_font);
					vvlayout->addWidget(label);
					vvlayout->addSpacing(resource::get_vertical_spacing());

					m_address = new QLineEdit(this);
					m_address->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
					m_address->setText(router::address);
					m_address->setAttribute(Qt::WA_MacShowFocusRect, 0);
					m_address->setMinimumWidth(386 * ratio);
					m_address->setMaxLength(io::max_address_size);
					m_address->setFixedHeight(resource::get_line_edit_height());
					vvlayout->addWidget(m_address);

					hlayout->addLayout(vvlayout, 16);
				}

				// Port
				{
					const auto vvlayout = resource::get_vbox_layout();

					auto label = new QLabel("Port", this);
					label->setFont(label_font);
					vvlayout->addWidget(label);
					vvlayout->addSpacing(resource::get_vertical_spacing());

					auto validator = new QIntValidator(1, 65535, this);

					m_port = new QLineEdit(this);
					m_port->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
					m_port->setText(QString::number(router::port));
					m_port->setAttribute(Qt::WA_MacShowFocusRect, 0);
					m_port->setFixedWidth(70 * ratio);
					m_port->setMaxLength(io::max_port_size);
					m_port->setValidator(validator);
					m_port->setFixedHeight(resource::get_line_edit_height());
					vvlayout->addWidget(m_port);

					hlayout->addLayout(vvlayout);
				}

				hlayout->addStretch(1);
				layout->addLayout(hlayout);
				layout->addSpacing(resource::get_top_margin() * 2);
			}

			// Password
			{
				const auto vvlayout = resource::get_vbox_layout();

				auto label = new QLabel("Password", this);
				label->setFont(label_font);
				vvlayout->addWidget(label);
				vvlayout->addSpacing(resource::get_vertical_spacing());

				m_password = new QTextEdit(this);
				m_password->setPlaceholderText("None");
				m_password->setAttribute(Qt::WA_MacShowFocusRect, 0);
				m_password->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
				m_password->setFixedWidth(462 * ratio);
				m_password->setFixedHeight(resource::get_text_edit_height());

				vvlayout->addWidget(m_password);
				layout->addLayout(vvlayout);
				layout->addSpacing(resource::get_large_top_margin());
			}

			// Start Connected
			{
				const auto hlayout = resource::get_hbox_layout();

				// Checkbox
				{
					QFont small_font = QApplication::font();
					small_font.setFamily(font_family::main);
					small_font.setPixelSize(resource::get_small_font_size());

					m_start_checkbox = new QCheckBox("Start Connected", this);
					m_start_checkbox->setCheckState(Qt::Checked);
					m_start_checkbox->setFont(small_font);

					hlayout->addWidget(m_start_checkbox, 0, Qt::AlignLeft);
					hlayout->addStretch(1);
				}

				layout->addLayout(hlayout);
			}

			layout->addSpacing(resource::get_large_top_margin() * 4);
		}

		// Button
		{
			m_start_button = new QPushButton(this);
			m_start_button->setAutoDefault(true);
			m_start_button->setFixedSize(112 * ratio, 40 * ratio);
			m_start_button->setText("Start");

			const auto hlayout = resource::get_hbox_layout();
			hlayout->addWidget(m_start_button, 0, Qt::AlignTop);

			layout->addLayout(hlayout);
			layout->addStretch(4);
		}

		// Signal
		connect(m_name, &QLineEdit::textEdited, [this](const QString& str)
		{
			if (!m_user_modified)
			{
				m_user_modified = true;
				m_user_color = false;
			}

			const auto qname = str.simplified();
			m_start_button->setVisible(!qname.isEmpty());

			set_color();
		});

		// Color
		connect(m_name_color, &QPushButton::clicked, [this]()
		{
			QColorDialog dialog(m_color, this);
			dialog.setWindowTitle("User Color");

			if (dialog.exec())
			{
				m_user_color = true;
				const auto& selected = dialog.selectedColor();
				m_color = selected.rgba();
				set_color();

				m_name->setFocus();
				m_name->selectAll();
			}
		});

		// Random Color
		connect(m_random, &QPushButton::clicked, [this]()
		{
			set_random_color();

			m_name->setFocus();
			m_name->selectAll();
		});

		// Start
		connect(m_start_button, &QPushButton::clicked, [this]() { emit done(); });

		// Color
		set_random_color();

		// Event
		installEventFilter(this);

		// Center
		const auto hlayout = resource::get_hbox_layout();
		hlayout->setSpacing(resource::get_horizontal_spacing());
		hlayout->addStretch(1);
		hlayout->addLayout(layout);
		hlayout->addStretch(1);
		setLayout(hlayout);

		// Tab Order
		setTabOrder(m_name, m_address);
		setTabOrder(m_address, m_port);

		show();
	}

	// Activate
	void wizard_widget::activate()
	{
		const auto qname = get_random_name();
		m_start_button->setVisible(!qname.isEmpty());

		m_name->setText(qname);
		m_address->setText(router::address);
		m_password->setText(router::password);
		m_port->setText(QString::number(router::port));
		m_start_checkbox->setChecked(true);

		// Color
		m_user_color = true;
		m_user_modified = false;
		set_random_color();

		m_name->setFocus();
		m_name->selectAll();
	}

	// Event
	bool wizard_widget::eventFilter(QObject* watched, QEvent* event)
	{
		if (event->type() == QEvent::KeyPress)
		{
			QKeyEvent* key = static_cast<QKeyEvent*>(event);
			const auto key_value = key->key();

			if ((key_value == Qt::Key_Enter) || (key_value == Qt::Key_Return))
			{
				// Enter or return was pressed
				const auto text = m_name->text().simplified();
				if (!text.isEmpty())
					emit done();
			}
			else
			{
				return QObject::eventFilter(watched, event);
			}
			return true;
		}
		else
		{
			return QObject::eventFilter(watched, event);
		}

		return false;
	}

	// Paint
	void wizard_widget::paintEvent(QPaintEvent *event)
	{
		QPainter painter(this);
		QPoint center(width() / 2 - m_background.width() / 2, 0);
		painter.setOpacity(0.035);
		painter.drawPixmap(center, m_background);
	}

	// Set
	void wizard_widget::set_color()
	{
		if (!m_user_color)
		{
			name_component name;
			name.set_name(m_name->text().toStdString());
			m_color.setRgba(name.get_color());
		}

		const auto ratio = resource::get_ratio();
		const auto font_color = get_font_color(m_color);

		const auto font_rgba = QString("rgba(%1, %2, %3, %4)").arg(font_color.red()).arg(font_color.green()).arg(font_color.blue()).arg(font_color.alpha());
		const auto rgba = QString("rgba(%1, %2, %3, %4)").arg(m_color.red()).arg(m_color.green()).arg(m_color.blue()).arg(m_color.alpha());
		const auto style = QString("QPushButton {color: %3; background: %2; border-radius: 4px; border: 0; padding: 3px; margin-bottom: 1px;} QPushButton:hover { color: %2; background: %3; border: 1px solid %2; } QPushButton:pressed { color: %1; border: 1px solid %1; }").arg(m_color.name()).arg(rgba).arg(font_rgba);
		const auto button_style = QString("QPushButton { font-size: %4px; font-weight: bold; border-radius: 8px; border: 0; padding: 0; padding-bottom: 3px; color: %3; background: %2; } QPushButton:hover { color: %2; background: %3; border: 2px solid %2; } QPushButton:pressed { color: %1; border: 2px solid %1; }").arg(m_color.name()).arg(rgba).arg(font_rgba).arg(static_cast<size_t>(22 * ratio));

		m_name_color->setStyleSheet(style);
		m_random->setStyleSheet(style);
		m_start_button->setStyleSheet(button_style);
	}


	void wizard_widget::set_random_color()
	{
		m_user_color = true;
		m_user_modified = false;

		std::random_device rnd;
		std::uniform_int_distribution<int> dist(0, 255);
		m_color.setRgb(dist(rnd), dist(rnd), dist(rnd));

		set_color();
	}

	// Get
	QColor wizard_widget::get_font_color(const QColor& background)
	{
		if ((background.red() * 0.299 + background.green() * 0.587 + background.blue() * 0.114) / 255 <= 0.5)
			return QColor(255, 255, 255);
		else
			return QColor(35, 35, 35);
	}

	QString wizard_widget::get_random_name() const
	{
		std::random_device rnd;
		std::uniform_int_distribution<size_t> adj_dist(0, (sizeof(s_adjectives) / sizeof(char*)) - 1);
		const auto adjective = s_adjectives[adj_dist(rnd)];

		std::uniform_int_distribution<size_t> noun_dist(0, (sizeof(s_nouns) / sizeof(char*)) - 1);
		const auto noun = s_nouns[noun_dist(rnd)];

		return QString("%1 %2").arg(adjective).arg(noun);
	}
}
