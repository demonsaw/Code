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

#include <QApplication>
#include <QCheckBox>
#include <QColor>
#include <QColorDialog>
#include <QFileDialog>
#include <QFont>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStandardPaths>
#include <QString>
#include <QVBoxLayout>

#include "component/callback/callback.h"
#include "component/client/client_component.h"

#include "font/font_awesome.h"
#include "resource/resource.h"
#include "resource/word.h"
#include "utility/value.h"
#include "widget/help_dialog.h"
#include "widget/option/general_option_widget.h"
#include "window/main_window.h"

namespace eja
{
	// Constructor
	general_option_widget::general_option_widget(const entity::ptr& entity, QWidget* parent /* = nullptr */) : option_pane(entity, parent)
	{
		m_layout = resource::get_vbox_layout(this);

		layout();
		signal();

		// Event
		m_name->installEventFilter(this);

		// Color
		m_color = get_client_color();
		set_style_color();

		setLayout(m_layout);
	}

	void general_option_widget::layout()
	{
		// Font
		QFont awesome(software::font_awesome);
		awesome.setPixelSize(resource::get_icon_size());

		// Label Font
		QFont label_font = QApplication::font();
		label_font.setFamily(font_family::main);
		label_font.setPixelSize(resource::get_label_font_size());

		// Layout
		const auto layout = resource::get_vbox_layout();
		const auto client = m_entity->get<client_component>();

		const auto client_group = resource::get_vbox_layout();
		client_group->setMargin(resource::get_group_box_margin());

		// Client
		{
			const auto hlayout = resource::get_hbox_layout();
			hlayout->setSpacing(resource::get_horizontal_spacing());

			// Name
			{
				const auto vlayout = resource::get_vbox_layout();

				auto label = new QLabel("Name", this);
				label->setFont(label_font);
				vlayout->addWidget(label);
				vlayout->addSpacing(resource::get_vertical_spacing());

				m_name = new QLineEdit(this);
				m_name->setAttribute(Qt::WA_MacShowFocusRect, 0);
				m_name->setFixedWidth(459);
				m_name->setMaxLength(io::max_name_size);
				m_name->setFixedHeight(resource::get_line_edit_height());

				// Data
				m_name->setText(QString::fromStdString(client->get_name()));

				vlayout->addWidget(m_name);
				hlayout->addLayout(vlayout);
			}

			// Edit
			{
				m_name_color = new QPushButton(QString(fa::paint_brush), this);
				m_name_color->setToolTip("Set Color");
				m_name_color->setObjectName("font_awesome");
				m_name_color->setFixedSize(QSize(resource::get_fa_button_width(), resource::get_fa_button_height()));
				m_name_color->setFont(awesome);

				hlayout->addWidget(m_name_color, 0, Qt::AlignBottom);
			}

			// Default
			{
				m_random_color = new QPushButton(QString(fa::random), this);
				m_random_color->setToolTip("Random Color");
				m_random_color->setObjectName("font_awesome");
				m_random_color->setFixedSize(QSize(resource::get_fa_button_width(), resource::get_fa_button_height()));
				m_random_color->setFont(awesome);

				hlayout->addWidget(m_random_color, 0, Qt::AlignBottom);
			}

			hlayout->addStretch(1);
			client_group->addLayout(hlayout);
			client_group->addSpacing(resource::get_top_margin() * 2);
		}

		// Save
		{
			const auto hlayout = resource::get_hbox_layout();
			hlayout->setSpacing(resource::get_horizontal_spacing());

			// Path
			{
				const auto vlayout = resource::get_vbox_layout();

				auto label = new QLabel("Download Folder", this);
				label->setFont(label_font);
				vlayout->addWidget(label);
				vlayout->addSpacing(resource::get_vertical_spacing());

				m_save_path = new QLineEdit(this);
				m_save_path->setAttribute(Qt::WA_MacShowFocusRect, 0);
				m_save_path->setFixedWidth(496);
				m_save_path->setFixedHeight(resource::get_line_edit_height());

				// Data
				if (!client->has_path())
				{
					const auto qpath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
					client->set_path(qpath.toStdString());
				}
#if _WSTRING
				m_save_path->setText(QString::fromStdWString(client->get_path().wstring()));
#else
				m_save_path->setText(QString::fromStdString(client->get_path().string()));
#endif
				vlayout->addWidget(m_save_path);
				hlayout->addLayout(vlayout);
			}

			// Edit
			{
				QFont font(software::font_awesome);
				m_save_edit = new QPushButton(QString(fa::folder_open));
				m_save_edit->setToolTip("Set Folder");
				m_save_edit->setObjectName("font_awesome");
				m_save_edit->setFixedSize(QSize(resource::get_fa_button_width(), resource::get_fa_button_height()));
				m_save_edit->setFont(awesome);
				m_save_edit->setObjectName("rounded_button");

				hlayout->addWidget(m_save_edit, 0, Qt::AlignBottom);
			}

			hlayout->addStretch(1);
			client_group->addLayout(hlayout);
		}

		// Client Area
		QWidget* client_widget = new QWidget(this);
		client_widget->setObjectName("border");
		client_widget->setLayout(client_group);

		const auto hlayout = resource::get_hbox_layout();
		hlayout->setSpacing(0);

		auto label = new QLabel("User", this);
		label->setFont(label_font);
		hlayout->addWidget(label);
		hlayout->addStretch(1);

		layout->addLayout(hlayout);
		layout->addSpacing(resource::get_vertical_spacing());
		layout->addWidget(client_widget);
		layout->addSpacing(resource::get_top_margin() * 2);

		// Chat Notifications
		{
			// Title
			{
				auto label = new QLabel("Chat Notifications", this);
				label->setFont(label_font);
				layout->addWidget(label);
				layout->addSpacing(resource::get_vertical_spacing());
			}

			const auto vlayout = resource::get_vbox_layout();
			vlayout->setMargin(resource::get_group_box_margin());

			// Checkboxes
			{
				const auto hlayout = resource::get_hbox_layout();
				hlayout->setSpacing(resource::get_horizontal_spacing());

				// Browse
				m_chat_beep = new QCheckBox("Beep", this);
				m_chat_beep->setChecked(client->is_chat_beep());
				hlayout->addWidget(m_chat_beep);

				// Chat
				m_chat_flash = new QCheckBox("Flash", this);
				m_chat_flash->setChecked(client->is_chat_alert());
				hlayout->addWidget(m_chat_flash);

				// Visual
				m_chat_tab = new QCheckBox("Tab Bar", this);
				m_chat_tab->setChecked(client->is_chat_notify());
				hlayout->addWidget(m_chat_tab);

				// Tray
				m_chat_toast = new QCheckBox("Toasts", this);
				m_chat_toast->setChecked(client->is_chat_tray());
				hlayout->addWidget(m_chat_toast);

				hlayout->addStretch(2);
				vlayout->addLayout(hlayout);
			}

			const auto widget = new QWidget(this);
			widget->setObjectName("border");
			widget->setLayout(vlayout);

			const auto hlayout = resource::get_hbox_layout();
			hlayout->addWidget(widget);

			layout->addLayout(hlayout);
			layout->addSpacing(resource::get_top_margin() * 2);
		}

		// PM Notifications
		{
			// Title
			{
				auto label = new QLabel("PM Notifications", this);
				label->setFont(label_font);
				layout->addWidget(label);
				layout->addSpacing(resource::get_vertical_spacing());
			}

			const auto vlayout = resource::get_vbox_layout();
			vlayout->setMargin(resource::get_group_box_margin());

			// Checkboxes
			{
				const auto hlayout = resource::get_hbox_layout();
				hlayout->setSpacing(resource::get_horizontal_spacing());

				// Browse
				m_pm_beep = new QCheckBox("Beep", this);
				m_pm_beep->setChecked(client->is_pm_beep());
				hlayout->addWidget(m_pm_beep);

				// Chat
				m_pm_flash = new QCheckBox("Flash", this);
				m_pm_flash->setChecked(client->is_pm_alert());
				hlayout->addWidget(m_pm_flash);

				// Visual
				m_pm_tab = new QCheckBox("Tab Bar", this);
				m_pm_tab->setChecked(client->is_pm_notify());
				hlayout->addWidget(m_pm_tab);

				// Tray
				m_pm_toast = new QCheckBox("Toasts", this);
				m_pm_toast->setChecked(client->is_pm_tray());
				hlayout->addWidget(m_pm_toast);

				hlayout->addStretch(2);
				vlayout->addLayout(hlayout);
			}

			const auto widget = new QWidget(this);
			widget->setObjectName("border");
			widget->setLayout(vlayout);

			const auto hlayout = resource::get_hbox_layout();
			hlayout->addWidget(widget);

			layout->addLayout(hlayout);
			layout->addSpacing(resource::get_top_margin() * 2);
		}

		// Room Notifications
		{
			// Title
			{
				auto label = new QLabel("Room Notifications", this);
				label->setFont(label_font);
				layout->addWidget(label);
				layout->addSpacing(resource::get_vertical_spacing());
			}

			const auto vlayout = resource::get_vbox_layout();
			vlayout->setMargin(resource::get_group_box_margin());

			// Checkboxes
			{
				const auto hlayout = resource::get_hbox_layout();
				hlayout->setSpacing(resource::get_horizontal_spacing());

				// Browse
				m_room_beep = new QCheckBox("Beep", this);
				m_room_beep->setChecked(client->is_room_beep());
				hlayout->addWidget(m_room_beep);

				// Chat
				m_room_flash = new QCheckBox("Flash", this);
				m_room_flash->setChecked(client->is_room_alert());
				hlayout->addWidget(m_room_flash);

				// Visual
				m_room_tab = new QCheckBox("Tab Bar", this);
				m_room_tab->setChecked(client->is_room_notify());
				hlayout->addWidget(m_room_tab);

				// Tray
				m_room_toast = new QCheckBox("Toasts", this);
				m_room_toast->setChecked(client->is_room_tray());
				hlayout->addWidget(m_room_toast);

				hlayout->addStretch(2);
				vlayout->addLayout(hlayout);
			}

			const auto widget = new QWidget(this);
			widget->setObjectName("border");
			widget->setLayout(vlayout);

			const auto hlayout = resource::get_hbox_layout();
			hlayout->addWidget(widget);

			layout->addLayout(hlayout);
		}

		// Group
		m_client = new QGroupBox(this);
		m_client->setFlat(true);
		m_client->setLayout(layout);

		// Offset for client
		m_layout->addWidget(m_client);
	}

	// Utility
	void general_option_widget::help()
	{
		const char* text = "<b>Overview</b><br>" \
			"This tab manages general user settings.<br><br>" \
			"<b>Name</b><br>The unique user name (nick) that others will see you as.<br><br>" \
			"<b>Download Folder</b><br>The default folder your downloads will be saved to.<br><br>" \
			"<b>Notifications</b><br>You can receive messages in public chat, private conversations, or in chat rooms. &nbsp;There are three different alerts you can receive for each message type.<br><br>" \
			"1) Beep: &nbsp;An auditory alert.<br>" \
			"2) Flash: &nbsp;Flashing application taskbar/dock icon.<br>" \
			"3) Tab Bar: &nbsp;A speech bubble appearing in the client tab." \
			"<br>";

		help_dialog dlg(text, this);
		dlg.setWindowTitle("General Help");
		dlg.exec();
	}

	void general_option_widget::reset()
	{
		// Name
		const auto qname = get_random_name();
		m_name->setText(qname);
		emit m_name->textChanged(qname);

		emit m_random_color->clicked();

		// Path
		const auto qpath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
		m_save_path->setText(qpath);
		emit m_save_path->textChanged(qpath);

		// Chat
		m_chat_beep->setChecked(false);
		emit m_chat_beep->clicked(false);

		m_chat_flash->setChecked(true);
		emit m_chat_flash->clicked(true);

		m_chat_toast->setChecked(true);
		emit m_chat_toast->clicked(true);

		m_chat_tab->setChecked(true);
		emit m_chat_tab->clicked(true);

		// PM
		m_pm_beep->setChecked(true);
		emit m_pm_beep->clicked(true);

		m_pm_flash->setChecked(true);
		emit m_pm_flash->clicked(true);

		m_pm_toast->setChecked(true);
		emit m_pm_toast->clicked(true);

		m_pm_tab->setChecked(true);
		emit m_pm_tab->clicked(true);

		// Room
		m_room_beep->setChecked(false);
		emit m_room_beep->clicked(false);

		m_room_flash->setChecked(true);
		emit m_room_flash->clicked(true);

		m_room_toast->setChecked(true);
		emit m_room_toast->clicked(true);

		m_room_tab->setChecked(true);
		emit m_room_tab->clicked(true);
	}

	// Event
	bool general_option_widget::eventFilter(QObject* object, QEvent* event)
	{
		if (object == m_name)
		{
			if (event->type() == QEvent::FocusOut)
			{
				auto qname = m_name->text().simplified();
				if (qname.isEmpty())
				{
					m_name->setFocus();
					qname = get_random_name();
					m_name->setText(qname);

					set_random_color();
					set_style_color();

					return true;
				}
			};
		};
		return false;
	}

	// Signal
	void general_option_widget::signal()
	{
		// Name
		connect(m_name, &QLineEdit::textChanged, [=](const QString& str)
		{
			const auto client = m_entity->get<client_component>();
			const auto name = str.simplified().toStdString();

			if (client->get_name() != name)
			{
				client->set_name(name);

				// Color
				if (!client->has_color())
					set_client_color();

				// Callback
				m_entity->async_call(callback::status | callback::update);
				m_entity->async_call(callback::chat | callback::update);
				m_entity->async_call(callback::pm | callback::update);
				m_entity->async_call(callback::room | callback::update);

				m_entity->async_call(callback::client | callback::update, m_entity);
				m_entity->async_call(callback::browse | callback::update, m_entity);

				emit name_changed();
			}
		});

		// Color
		connect(m_name_color, &QPushButton::clicked, [this]()
		{
			QColorDialog dialog(m_color, this);
			dialog.setWindowTitle("User Color");

			if (dialog.exec())
			{
				const auto& selected = dialog.selectedColor();
				if (m_color.rgba() != selected.rgba())
				{
					m_color = selected.rgba();
					set_client_color();
					set_style_color();

					// Callback
					m_entity->async_call(callback::status | callback::update);
					m_entity->async_call(callback::client | callback::update, m_entity);
					m_entity->async_call(callback::browse | callback::update, m_entity);
				}
			}
		});

		// Random Color
		connect(m_random_color, &QPushButton::clicked, this, [&]
		{
			set_random_color();
			set_style_color();

			// Callback
			m_entity->async_call(callback::status | callback::update);
			m_entity->async_call(callback::client | callback::update, m_entity);
			m_entity->async_call(callback::browse | callback::update, m_entity);
		});

		// Save
		connect(m_save_path, &QLineEdit::textChanged, [=](const QString& str)
		{
			const auto client = m_entity->get<client_component>();
			const auto path = str.trimmed().toStdString();
			client->set_path(path);
		});

		connect(m_save_edit, &QPushButton::clicked, [this]()
		{
			QFileDialog dialog;
			dialog.setWindowTitle("Download Folder");
			dialog.setOptions(QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog);
			dialog.setFileMode(QFileDialog::DirectoryOnly);
			dialog.setViewMode(QFileDialog::ViewMode::List);
			dialog.setDirectory(m_save_path->text());

			if (dialog.exec())
			{
				const auto qpaths = dialog.selectedFiles();
				const auto& qpath = qpaths.at(0);

				if (qpath != m_save_path->text())
				{
					m_save_path->setText(qpath);

					const auto client = m_entity->get<client_component>();
					const auto path = qpath.toStdString();
					client->set_path(path);
				}
			}
		});

		// Chat Notifications
		connect(m_chat_beep, &QCheckBox::clicked, [&]()
		{
			const auto client = m_entity->get<client_component>();
			client->set_chat_beep(m_chat_beep->isChecked());
		});

		connect(m_chat_flash, &QCheckBox::clicked, [&]()
		{
			const auto client = m_entity->get<client_component>();
			client->set_chat_alert(m_chat_flash->isChecked());
		});

		connect(m_chat_toast, &QCheckBox::clicked, [&]()
		{
			const auto client = m_entity->get<client_component>();
			client->set_chat_tray(m_chat_toast->isChecked());
		});

		connect(m_chat_tab, &QCheckBox::clicked, [&]()
		{
			const auto client = m_entity->get<client_component>();
			client->set_chat_notify(m_chat_tab->isChecked());
		});

		// PM Notifications
		connect(m_pm_beep, &QCheckBox::clicked, [&]()
		{
			const auto client = m_entity->get<client_component>();
			client->set_pm_beep(m_pm_beep->isChecked());
		});

		connect(m_pm_flash, &QCheckBox::clicked, [&]()
		{
			const auto client = m_entity->get<client_component>();
			client->set_pm_alert(m_pm_flash->isChecked());
		});

		connect(m_pm_toast, &QCheckBox::clicked, [&]()
		{
			const auto client = m_entity->get<client_component>();
			client->set_pm_tray(m_pm_toast->isChecked());
		});

		connect(m_pm_tab, &QCheckBox::clicked, [&]()
		{
			const auto client = m_entity->get<client_component>();
			client->set_pm_notify(m_pm_tab->isChecked());
		});

		// Room Notifications
		connect(m_room_beep, &QCheckBox::clicked, [&]()
		{
			const auto client = m_entity->get<client_component>();
			client->set_room_beep(m_room_beep->isChecked());
		});

		connect(m_room_flash, &QCheckBox::clicked, [&]()
		{
			const auto client = m_entity->get<client_component>();
			client->set_room_alert(m_room_flash->isChecked());
		});

		connect(m_room_toast, &QCheckBox::clicked, [&]()
		{
			const auto client = m_entity->get<client_component>();
			client->set_room_tray(m_room_toast->isChecked());
		});

		connect(m_room_tab, &QCheckBox::clicked, [&]()
		{
			const auto client = m_entity->get<client_component>();
			client->set_room_notify(m_room_tab->isChecked());
		});
	}

	// Set
	void general_option_widget::set_client_color()
	{
		auto component = m_entity->get<client_component>();
		component->set_color(m_color.red(), m_color.green(), m_color.blue());

		emit color_changed(m_color);
	}

	void general_option_widget::set_random_color()
	{
		std::random_device rnd;
		std::uniform_int_distribution<int> dist(0, 255);
		m_color.setRgb(dist(rnd), dist(rnd), dist(rnd));

		set_client_color();
		set_style_color();
	}

	void general_option_widget::set_style_color()
	{
		const auto client = m_entity->get<client_component>();

		QColor font_color(client->get_font_color());;
		const auto font_rgb = QString("rgba(%1, %2, %3)").arg(font_color.red()).arg(font_color.green()).arg(font_color.blue());
		const auto text_rgb = QString("rgba(%1, %2, %3, %4)").arg(client->get_red()).arg(client->get_green()).arg(client->get_blue()).arg(client->get_alpha());
		const auto style = QString("QPushButton {color: %2; background: %1; border-radius: 4px; border: 0; padding: 4px; margin: 1px 0;} QPushButton:hover, QPushButton:pressed { color: %1; background: %2; border: 1px solid %1; }").arg(text_rgb).arg(font_rgb);

		m_name_color->setStyleSheet(style);
		m_random_color->setStyleSheet(style);
		m_save_edit->setStyleSheet(style);
	}

	// Get
	QColor general_option_widget::get_client_color()
	{
		const auto client = m_entity->get<client_component>();

		return QColor(client->get_color());
	}

	QString general_option_widget::get_random_name() const
	{
		std::random_device rnd;
		std::uniform_int_distribution<size_t> adj_dist(0, (sizeof(s_adjectives) / sizeof(char*)) - 1);
		const auto adjective = s_adjectives[adj_dist(rnd)];

		std::uniform_int_distribution<size_t> noun_dist(0, (sizeof(s_nouns) / sizeof(char*)) - 1);
		const auto noun = s_nouns[noun_dist(rnd)];

		return QString("%1 %2").arg(adjective).arg(noun);
	}
}
