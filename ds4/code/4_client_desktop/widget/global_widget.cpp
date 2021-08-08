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

#include <boost/filesystem.hpp>

#include <QCheckBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPainter>
#include <QPaintEvent>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QStandardPaths>
#include <QStringList>
#include <QVBoxLayout>

#include "config/qt_application.h"
#include "font/font_awesome.h"
#include "resource/resource.h"
#include "utility/value.h"
#include "widget/help_dialog.h"
#include "widget/global_widget.h"
#include "window/main_window.h"

namespace eja
{
	global_widget::global_widget(QWidget* parent /* = nullptr */) : QWidget(parent)
	{
		setObjectName("global_settings");
		setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

		// App
		const auto& app = main_window::get_app();

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
			layout->addSpacing(resource::get_large_top_margin() * 4);
		}

		// Theme Label
		{
			const auto hlayout = resource::get_hbox_layout();
			hlayout->setSpacing(resource::get_horizontal_spacing());

			auto label = new QLabel("Theme", this);
			label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			label->setFont(label_font);
			hlayout->addWidget(label, 1, Qt::AlignBottom);
			hlayout->addStretch(1);

			// Reset
			m_reset = new QPushButton(QString(fa::undo), this);
			m_reset->setObjectName("rounded_button");
			m_reset->setToolTip("Restore Defaults");
			m_reset->setFixedSize(QSize(resource::get_fa_button_width(), resource::get_fa_button_height()));
			m_reset->setFont(awesome);

			auto vvlayout = resource::get_vbox_layout();
			vvlayout->addWidget(m_reset);
			vvlayout->addSpacing(2);
			hlayout->addLayout(vvlayout);

			// Help
			m_help = new QPushButton(QString(fa::question), this);
			m_help->setObjectName("rounded_button");
			m_help->setToolTip("Help");
			m_help->setFixedSize(QSize(resource::get_fa_button_width(), resource::get_fa_button_height()));
			m_help->setFont(awesome);

			vvlayout = resource::get_vbox_layout();
			vvlayout->addWidget(m_help);
			vvlayout->addSpacing(2);
			hlayout->addLayout(vvlayout);

			layout->addLayout(hlayout);
			layout->addSpacing(resource::get_vertical_spacing());
		}

		// Theme LineEdit, Buttons
		{
			const auto hlayout = resource::get_hbox_layout();
			hlayout->setSpacing(resource::get_horizontal_spacing());

			// Theme
			{
				m_theme = new QLineEdit(this);
				m_theme->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
				m_theme->setAttribute(Qt::WA_MacShowFocusRect, 0);
				m_theme->setMinimumWidth(386);
				m_theme->setFixedHeight(resource::get_line_edit_height());

				const auto& theme = app.get_theme();
#if _WSTRING
				m_theme->setText(QString::fromStdWString(theme.wstring()));
#else
				m_theme->setText(QString::fromStdString(theme.string()));
#endif
				hlayout->addWidget(m_theme);
			}

			// Buttons
			{
				// Update
				m_theme_update = new QPushButton(QString(fa::refresh), this);
				m_theme_update->setObjectName("rounded_button");
				m_theme_update->setToolTip("Refresh Theme");
				m_theme_update->setFixedSize(QSize(resource::get_fa_button_width(), resource::get_fa_button_height()));
				m_theme_update->setFont(awesome);

				// Edit
				m_theme_edit = new QPushButton(QString(fa::file), this);
				m_theme_edit->setObjectName("rounded_button");
				m_theme_edit->setToolTip("Set Theme");
				m_theme_edit->setFixedSize(QSize(resource::get_fa_button_width(), resource::get_fa_button_height()));
				m_theme_edit->setFont(awesome);

				hlayout->addWidget(m_theme_update);
				hlayout->addWidget(m_theme_edit);

				set_color();
			}

			layout->addLayout(hlayout);
			layout->addSpacing(resource::get_large_top_margin() * 2);
		}

		// Scaling
		{
			const auto vvlayout = resource::get_vbox_layout();

			QString text = "Font Size: " + QString::number((int)resource::get_font_size()) + "px";
			m_scaling_label = new QLabel(text, this);
			m_scaling_label->setFont(label_font);
			vvlayout->addWidget(m_scaling_label, 0, Qt::AlignBottom);
			vvlayout->addSpacing(resource::get_vertical_spacing());

			m_scaling = new QSlider(Qt::Horizontal, this);
			m_scaling->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			m_scaling->setMinimum(ui::min_font_size);
			m_scaling->setMaximum(ui::max_font_size);
			m_scaling->setMinimumWidth(462);
			m_scaling->setTickInterval(1);
			m_scaling->setTickPosition(QSlider::TicksBothSides);
			m_scaling->setValue(resource::get_font_size());
			vvlayout->addWidget(m_scaling, 1);

			layout->addLayout(vvlayout);
			layout->addSpacing(resource::get_large_top_margin() * 2);
		}

		// Checkboxes
		{
			const auto hlayout = resource::get_hbox_layout();
			hlayout->setSpacing(resource::get_horizontal_spacing());

			// Auto Open
			m_auto_open = new QCheckBox("Auto Open Tabs", this);
			m_auto_open->setCheckState(app.is_auto_open() ? Qt::Checked : Qt::Unchecked);
			hlayout->addWidget(m_auto_open);

			// Statusbar
			m_statusbar = new QCheckBox("Status Bar", this);
			m_statusbar->setCheckState(app.is_statusbar() ? Qt::Checked : Qt::Unchecked);
			hlayout->addWidget(m_statusbar);

			// System Tray
			m_system_tray = new QCheckBox("System Tray", this);
			m_system_tray->setCheckState(app.is_system_tray() ? Qt::Checked : Qt::Unchecked);
			hlayout->addWidget(m_system_tray);

			// Timestamp
			m_timestamp = new QCheckBox("Timestamps", this);
			m_timestamp->setCheckState(app.is_timestamps() ? Qt::Checked : Qt::Unchecked);
			hlayout->addWidget(m_timestamp);

			// User Colors
			m_user_colors = new QCheckBox("User Colors", this);
			m_user_colors->setCheckState(app.is_user_colors() ? Qt::Checked : Qt::Unchecked);
			hlayout->addWidget(m_user_colors);

			hlayout->addStretch(1);
			layout->addLayout(hlayout);
			layout->addSpacing((resource::get_large_top_margin() * 2) - (resource::get_icon_padding() / 2));
		}

		// Transfers
		{
			const auto hlayout = resource::get_hbox_layout();
			hlayout->setSpacing(resource::get_horizontal_spacing() * 2);

			// Downloads
			{
				const auto vvlayout = resource::get_vbox_layout();

				const auto label = new QLabel("Max Download", this);
				label->setFont(label_font);
				vvlayout->addWidget(label);
				vvlayout->addSpacing(resource::get_vertical_spacing());

				m_downloads = new QSpinBox(this);
				m_downloads->setAttribute(Qt::WA_MacShowFocusRect, 0);
				m_downloads->setFixedHeight(resource::get_combo_box_height());
				m_downloads->setMinimumWidth(resource::get_combo_box_width() * 1.25);
				m_downloads->setSuffix(" Mbps");
				m_downloads->setSingleStep(1);

				const auto& throttle = http_download_throttle::get();
				m_downloads->setRange(0, download::max_throttle_size);
				m_downloads->setValue(b_to_mb(throttle->get_max_size() << 3));

				vvlayout->addWidget(m_downloads);
				hlayout->addLayout(vvlayout);
			}

			// Uploads
			{
				const auto vvlayout = resource::get_vbox_layout();

				const auto label = new QLabel("Max Upload", this);
				label->setFont(label_font);
				vvlayout->addWidget(label);
				vvlayout->addSpacing(resource::get_vertical_spacing());

				m_uploads = new QSpinBox(this);
				m_uploads->setAttribute(Qt::WA_MacShowFocusRect, 0);
				m_uploads->setFixedHeight(resource::get_combo_box_height());
				m_uploads->setMinimumWidth(resource::get_combo_box_width() * 1.25);
				m_uploads->setSuffix(" Mbps");
				m_uploads->setSingleStep(1);

				const auto& throttle = http_upload_throttle::get();
				m_uploads->setRange(0, upload::max_throttle_size);
				m_uploads->setValue(b_to_mb(throttle->get_max_size() << 3));

				vvlayout->addWidget(m_uploads);
				hlayout->addLayout(vvlayout);
			}

			hlayout->addStretch(1);

			layout->addLayout(hlayout);
			layout->addSpacing(resource::get_large_top_margin() * 2);
		}

		// Unsaved
		m_unsaved = new QLabel("Please restart the application for these changes to take effect.", this);
		m_unsaved->setObjectName("option_unsaved");

		layout->addSpacing(resource::get_top_margin());
		layout->addWidget(m_unsaved);
		layout->addStretch(1);

		m_unsaved->hide();

		const auto group = new QGroupBox("", this);
		group->setObjectName("wizard");
		group->setFlat(true);

		layout->addWidget(group, 0, Qt::AlignTop);
		layout->addSpacing(resource::get_vertical_spacing());

		// Center
		const auto hlayout = resource::get_hbox_layout();
		hlayout->setSpacing(resource::get_horizontal_spacing());
		hlayout->addStretch(1);
		hlayout->addLayout(layout);
		hlayout->addStretch(1);
		setLayout(hlayout);

		// Signal
		signal();

		show();
	}

	// Signal
	void global_widget::signal()
	{
		// Help
		connect(m_help, &QPushButton::clicked, this, [&]
		{
			const char* text = "<b>Overview</b><br>" \
				"This tab manages global settings which apply to all users.<br><br>" \
				"<b>Theme</b><br>Select a CSS file which controls how Demonsaw looks.  Demonsaw comes with a light theme (default) and a dark theme. &nbsp;Try searching for themes created by other users, or create your own!<br><br>" \
				"<b>Font Size</b><br>The general size of all text in Demonsaw. &nbsp;Changing this requires restarting the application.<br><br>" \
				"<b>Auto Open Tabs</b><br>Automatically open the relevant tab when something happens. &nbsp;For example, if you begin a download, open the download tab automatically.<br><br>" \
				"<b>Display Status Bar</b><br>Show a status bar at the bottom of the window which displays general statistics.<br><br>" \
				"<b>Display User Colors</b><br>Show custom user colors in chat and notifications. &nbsp;Turning this off puts Demonsaw in monochrome mode.<br><br>" \
				"<b>Max Download, Max Upload</b><br>Internet bandwidth rate limiting for your downloads/uploads. &nbsp;Set these limits to avoid saturating your Internet connection." \
				"<br>";

			help_dialog dlg(text, this);
			dlg.setWindowTitle("Global Help");
			dlg.exec();
		});

		connect(m_reset, &QPushButton::clicked, [&]()
		{
			const auto font_size = resource::get_default_font_size();
			m_scaling->setValue(font_size);
			emit m_scaling->valueChanged(font_size);

			m_downloads->setValue(0);
			emit m_downloads->valueChanged(0);

			m_uploads->setValue(0);
			emit m_uploads->valueChanged(0);

			m_auto_open->setChecked(true);
			emit m_auto_open->clicked(true);

			m_statusbar->setChecked(true);
			emit m_statusbar->clicked(true);

			m_system_tray->setChecked(true);
			emit m_system_tray->clicked(true);

			m_timestamp->setChecked(true);
			emit m_timestamp->clicked(true);

			m_user_colors->setChecked(true);
			emit m_user_colors->clicked(true);

			// Theme
			m_theme->clear();

			auto& app = main_window::get_app();
			if (app.has_theme())
			{
				app.set_theme();

				const auto window = main_window::get();
				window->update_theme();
			}
		});

		// Theme
		connect(m_theme_edit, &QPushButton::clicked, [&]()
		{
			QFileDialog dialog;
			dialog.setWindowTitle("Theme Location");
			dialog.setOptions(QFileDialog::ReadOnly | QFileDialog::DontUseNativeDialog);
			dialog.setFileMode(QFileDialog::ExistingFile);
			dialog.setViewMode(QFileDialog::ViewMode::List);

			QStringList filters;
			filters << "CSS (*.css)" << "All Files (*)";
			dialog.setNameFilters(filters);
			dialog.selectNameFilter("CSS (*.css)");

			QFileInfo info(m_theme->text());
			QDir dir = info.dir();
			dialog.setDirectory(dir.exists() ? dir.absolutePath() : QCoreApplication::applicationDirPath());

			if (dialog.exec())
			{
				const auto qpaths = dialog.selectedFiles();
				const auto& qpath = qpaths.at(0);

				if (qpath != m_theme->text())
				{
					// Theme
					auto& app = main_window::get_app();
					app.set_theme(qpath.toStdString());
					m_theme->setText(qpath);

					// Update Widgets
					const auto window = main_window::get();
					window->update_theme();
				}
			}
		});

		connect(m_theme_update, &QPushButton::clicked, [&]()
		{
			auto& app = main_window::get_app();
			const auto& qpath = m_theme->text();

			if (!qpath.isEmpty())
			{
				// Theme
#if _WSTRING
				app.set_theme(qpath.toStdWString());
#else
				app.set_theme(qpath.toStdString());
#endif
				// Update Widgets
				const auto window = main_window::get();
				window->update_theme();
			}
			else
			{
				if (app.has_theme())
				{
					app.set_theme();

					// Update Widgets
					const auto window = main_window::get();
					window->update_theme();
				}
			}
		});

		// Unsaved Scaling
		connect(m_scaling, &QSlider::valueChanged, [&](int value)
		{
			// Make slider to follow the mouse directly and not by pageStep steps
			auto sliderPosUnderMouse = value;
			const auto btns = QApplication::mouseButtons();
			const auto localMousePos = m_scaling->mapFromGlobal(QCursor::pos());
			const bool clickOnSlider = (btns & Qt::LeftButton) && ((localMousePos.x() >= 0) && (localMousePos.y() >= 0) && (localMousePos.x() < m_scaling->size().width()) && (localMousePos.y() < m_scaling->size().height()));

			if (clickOnSlider)
			{
				// Attention! The following works only for Horizontal, Left-to-right sliders
				float posRatio = localMousePos.x() / (float)m_scaling->size().width();
				int sliderRange = m_scaling->maximum() - m_scaling->minimum();
				sliderPosUnderMouse = m_scaling->minimum() + sliderRange * posRatio;
				if (sliderPosUnderMouse != value)
				{
					m_scaling->setValue(sliderPosUnderMouse);
				}
			}

			// Set Font
			auto& app = main_window::get_app();
			if (app.get_font_size() != sliderPosUnderMouse)
			{
				app.set_font_size(sliderPosUnderMouse);

				// Update Text
				m_scaling_label->setText("Font Size: " + QString::number(sliderPosUnderMouse) + "px");

				// Show Warning
				m_unsaved->show();
			}
		});

		// Transfer
		connect(m_downloads, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [](int value)
		{
			const auto& throttle = http_download_throttle::get();
			const auto size = mb_to_b(static_cast<size_t>(value)) >> 3;
			throttle->set_max_size(size);
		});

		connect(m_uploads, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [](int value)
		{
			const auto& throttle = http_upload_throttle::get();
			const auto size = mb_to_b(static_cast<size_t>(value)) >> 3;
			throttle->set_max_size(size);
		});

		// Tabs
		connect(m_auto_open, &QCheckBox::clicked, [&]()
		{
			auto& app = main_window::get_app();
			app.set_auto_open(m_auto_open->isChecked());
		});

		connect(m_statusbar, &QCheckBox::clicked, [&]()
		{
			const auto window = main_window::get();
			window->set_statusbar(m_statusbar->isChecked());

			auto& app = main_window::get_app();
			app.set_statusbar(m_statusbar->isChecked());
		});

		connect(m_system_tray, &QCheckBox::clicked, [&]()
		{
			auto& app = main_window::get_app();
			app.set_system_tray(m_system_tray->isChecked());

			// Show Warning
			m_unsaved->show();
		});

		connect(m_timestamp, &QCheckBox::clicked, [&]()
		{
			auto& app = main_window::get_app();
			app.set_timestamps(m_timestamp->isChecked());
		});

		connect(m_user_colors, &QCheckBox::clicked, [&]()
		{
			auto& app = main_window::get_app();
			app.set_user_colors(m_user_colors->isChecked());

			// Update Widgets
			const auto window = main_window::get();
			window->update_color();
		});
	}

	// Paint
	void global_widget::paintEvent(QPaintEvent *event)
	{
		QPainter painter(this);
		QPoint center(width() / 2 - m_background.width() / 2, 0);
		painter.setOpacity(0.035);
		painter.drawPixmap(center, m_background);
	}

	// Set
	void global_widget::set_color()
	{
		QColor color(color::blue);
		const auto font_color = get_font_color(color);;
		const auto font_rgba = QString("rgba(%1, %2, %3, %4)").arg(font_color.red()).arg(font_color.green()).arg(font_color.blue()).arg(font_color.alpha());
		const auto rgba = QString("rgba(%1, %2, %3, %4)").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha());
		const auto style = QString("QPushButton {color: %3; background: %2; border-radius: 4px; border: 0; padding: 3px; margin-bottom: 1px;} QPushButton:hover { color: %2; background: %3; border: 1px solid %2; } QPushButton:pressed { color: %1; border: 1px solid %1; }").arg(m_color.name()).arg(rgba).arg(font_rgba);

		m_help->setStyleSheet(style);
		m_reset->setStyleSheet(style);

		m_theme_edit->setStyleSheet(style);
		m_theme_update->setStyleSheet(style);
	}

	// Get
	QColor global_widget::get_font_color(const QColor& background)
	{
		if ((background.red() * 0.299 + background.green() * 0.587 + background.blue() * 0.114) / 255 <= 0.5)
			return QColor(255, 255, 255);
		else
			return QColor(35, 35, 35);
	}
}
