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

#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QPixmap>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QSplitter>
#include <QStackedWidget>
#include <QStandardPaths>
#include <QTextEdit>
#include <QTableWidget>
#include <QTimer>
#include <QToolBar>
#include <QVBoxLayout>

#include "component/endpoint_component.h"
#include "component/name_component.h"
#include "component/callback/callback_action.h"
#include "component/message/message_acceptor_component.h"
#include "component/message/message_network_component.h"
#include "entity/entity.h"
#include "entity/entity_action.h"
#include "font/font_awesome.h"
#include "widget/router/router_option_widget.h"
#include "window/main_window.h"

namespace eja
{
	// Constructor
	router_option_widget::router_option_widget(const entity::ptr entity, QWidget* parent /*= 0*/) : entity_widget(entity, "Options", fa::cog, callback_type::option, parent)
	{
		assert(thread_info::main());

		// Menu
		add_button("Update", fa::bolt, [&]() { update(); });
		add_button("Popout", fa::clone, [&]() { popout(); });
		add_button("Close", fa::close, [&]() { close(); });

		// Layout
		auto hlayout = get_hbox_layout();
		hlayout->addWidget(m_icon);
		hlayout->addWidget(m_title);
		hlayout->addStretch(1);
		hlayout->addWidget(m_toolbar);

		m_titlebar = new QWidget(this);
		m_titlebar->installEventFilter(this);
		m_titlebar->setObjectName("menubar");
		m_titlebar->setLayout(hlayout);
		setTitleBarWidget(m_titlebar);

		// Widget
		m_layout = get_vbox_layout();
		m_layout->setMargin(1);
		m_layout->addSpacing(2);

		router();
		network();
		signal();

		m_layout->addStretch(1);

		// Callback
		add_callback(callback_action::update, [&](const entity::ptr entity) { update(); });

		const auto widget = new QWidget(this);
		widget->setLayout(m_layout);

		const auto scroll_area = new QScrollArea(this);
		scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		scroll_area->setWidget(widget);
		setWidget(scroll_area);

		// Update
		set_color();
	}

	// Interface
	void router_option_widget::on_update()
	{
		assert(thread_info::main());

		// Spam
		auto button = get_button(fa::bolt);
		if (button)
		{
			button->setEnabled(false);
			QTimer::singleShot(ui::refresh, [&]() { set_enabled(fa::bolt); });
		}

		// Service
		const auto acceptor = m_entity->get<message_acceptor_component>();
		if (acceptor->valid())
		{
			acceptor->restart();
			set_modified(false);
		}
	}

	// Utility
	void router_option_widget::router()
	{
		// Font
		QFont font(demonsaw::font_awesome);

		// Layout
		const auto layout = get_vbox_layout();
		layout->setMargin(6);
		layout->addSpacing(1);

		// Router
		{
			auto hlayout = get_hbox_layout();

			// Name
			{
				auto vlayout = get_vbox_layout();

				auto label = new QLabel("Router", this);
				vlayout->addWidget(label);
				vlayout->addSpacing(1);

				m_name = new QLineEdit(this);
				m_name->setAttribute(Qt::WA_MacShowFocusRect, 0);
				m_name->setFixedWidth(446);
				m_name->setMaxLength(64);

				// Data
				const auto endpoint = m_entity->get<endpoint_component>();
				m_name->setText(QString::fromStdString(endpoint->get_name()));

				vlayout->addWidget(m_name);
				hlayout->addLayout(vlayout);
			}

			// Edit
			{
				auto vlayout = get_vbox_layout();

				auto label = new QLabel("", this);
				vlayout->addWidget(label);
				vlayout->addSpacing(1);

				m_name_color = new QPushButton(QString(fa::paintbrush), this);
				m_name_color->setToolTip("Choose Color");
				m_name_color->setObjectName("font_awesome");
				m_name_color->setFixedSize(QSize(32, 21));
				m_name_color->setFont(font);

				vlayout->addWidget(m_name_color);
				hlayout->addLayout(vlayout);
			}

			// Default
			{
				auto vlayout = get_vbox_layout();

				auto label = new QLabel("", this);
				vlayout->addWidget(label);
				vlayout->addSpacing(1);

				m_name_default = new QPushButton(QString(fa::undo), this);
				m_name_default->setToolTip("Default Color");
				m_name_default->setObjectName("font_awesome");
				m_name_default->setFixedSize(QSize(32, 21));
				m_name_default->setFont(font);

				vlayout->addWidget(m_name_default);
				hlayout->addLayout(vlayout);
			}

			hlayout->addStretch(1);
			layout->addLayout(hlayout);
			layout->addSpacing(7);
		}

		// Theme
		{
			auto hlayout = get_hbox_layout();

			// Theme
			{
				auto vlayout = get_vbox_layout();

				auto label = new QLabel("Theme", this);
				vlayout->addWidget(label);
				vlayout->addSpacing(1);

				m_theme = new QLineEdit(this);
				m_theme->setAttribute(Qt::WA_MacShowFocusRect, 0);
				m_theme->setFixedWidth(408);
				m_theme->setMaxLength(64);

				// Data
				const auto window = main_window::get();
				auto& app = window->get_application();
				m_theme->setText(QString::fromStdString(app.get_theme()));

				vlayout->addWidget(m_theme);
				hlayout->addLayout(vlayout);
			}

			// Update
			{
				auto vlayout = get_vbox_layout();

				auto label = new QLabel("", this);
				vlayout->addWidget(label);
				vlayout->addSpacing(1);

				m_theme_update = new QPushButton(QString(fa::bolt), this);
				m_theme_update->setToolTip("Update Theme");
				m_theme_update->setObjectName("font_awesome");
				m_theme_update->setFixedSize(QSize(32, 21));
				m_theme_update->setFont(font);

				vlayout->addWidget(m_theme_update);
				hlayout->addLayout(vlayout);
			}

			// Edit
			{
				auto vlayout = get_vbox_layout();

				auto label = new QLabel("", this);
				vlayout->addWidget(label);
				vlayout->addSpacing(1);

				m_theme_edit = new QPushButton(QString(fa::file), this);
				m_theme_edit->setToolTip("Choose Theme");
				m_theme_edit->setObjectName("font_awesome");
				m_theme_edit->setFixedSize(QSize(32, 21));
				m_theme_edit->setFont(font);

				vlayout->addWidget(m_theme_edit);
				hlayout->addLayout(vlayout);
			}

			// Default
			{
				auto vlayout = get_vbox_layout();

				auto label = new QLabel("", this);
				vlayout->addWidget(label);
				vlayout->addSpacing(1);

				m_theme_default = new QPushButton(QString(fa::undo), this);
				m_theme_default->setToolTip("Default Theme");
				m_theme_default->setObjectName("font_awesome");
				m_theme_default->setFixedSize(QSize(32, 21));
				m_theme_default->setFont(font);

				vlayout->addWidget(m_theme_default);
				hlayout->addLayout(vlayout);
			}

			hlayout->addStretch(1);
			layout->addLayout(hlayout);
			layout->addSpacing(7);
		}

		// Group
		const auto group = new QGroupBox("Router", this);
		group->setFlat(true);
		group->setLayout(layout);

		m_layout->addWidget(group);
		m_layout->addSpacing(8);
	}

	void router_option_widget::network()
	{
		// Layout
		const auto layout = get_vbox_layout();
		layout->setMargin(6);
		layout->addSpacing(2);

		// Server
		{
			auto hlayout = get_hbox_layout();
			hlayout->setSpacing(6);

			// Address
			{
				auto vvlayout = get_vbox_layout();

				auto label = new QLabel("Address", this);
				vvlayout->addWidget(label);

				m_network_address = new QLineEdit(this);
				m_network_address->setAttribute(Qt::WA_MacShowFocusRect, 0);
				m_network_address->setFixedWidth(350);
				m_network_address->setMaxLength(255);

				// Data
				const auto endpoint = m_entity->get<endpoint_component>();
				m_network_address->setText(QString::fromStdString(endpoint->get_address()));

				vvlayout->addWidget(m_network_address);
				hlayout->addLayout(vvlayout);
			}

			// Port
			{
				auto vvlayout = get_vbox_layout();

				auto label = new QLabel("Port", this);
				vvlayout->addWidget(label);

				m_network_port = new QSpinBox(this);
				m_network_port->setAttribute(Qt::WA_MacShowFocusRect, 0);
				m_network_port->setFixedHeight(21);
				m_network_port->setFixedWidth(80);
				m_network_port->setRange(0, 65535);

				// Data
				const auto endpoint = m_entity->get<endpoint_component>();
				m_network_port->setValue(endpoint->get_port());

				vvlayout->addWidget(m_network_port);
				hlayout->addLayout(vvlayout);
			}

			// Buffer
			{
				auto vvlayout = get_vbox_layout();

				auto label = new QLabel("Buffer", this);
				vvlayout->addWidget(label);

				m_network_buffer = new QSpinBox(this);
				m_network_buffer->setAttribute(Qt::WA_MacShowFocusRect, 0);
				m_network_buffer->setFixedHeight(21);
				m_network_buffer->setFixedWidth(80);
				m_network_buffer->setRange(b_to_mb(network::min_buffer), b_to_mb(network::max_buffer));
				m_network_buffer->setSuffix(" MB");

				// Data
				const auto network = m_entity->get<message_network_component>();
				m_network_buffer->setValue(b_to_mb(network->get_buffer()));

				vvlayout->addWidget(m_network_buffer);
				hlayout->addLayout(vvlayout);

			}

			hlayout->addStretch(1);
			layout->addLayout(hlayout);
			layout->addSpacing(8);
		}

		// Type
		{
			// Title
			{
				auto label = new QLabel("Router Type", this);
				layout->addWidget(label);
				layout->addSpacing(1);
			}

			auto vlayout = get_vbox_layout();
			vlayout->setMargin(6);

			// Checkboxes			
			{
				auto hlayout = get_hbox_layout();

				m_type_message = new QCheckBox("Message", this);
				hlayout->addWidget(m_type_message);

				m_type_transfer = new QCheckBox("Transfer", this);
				hlayout->addWidget(m_type_transfer);

				m_type_open = new QCheckBox("Public", this);
				hlayout->addWidget(m_type_open);

				// Data
				const auto network = m_entity->get<message_network_component>();
				m_type_message->setChecked(network->has_message());
				m_type_transfer->setChecked(network->has_transfer());
				m_type_open->setChecked(network->has_open());

				hlayout->addStretch(1);
				vlayout->addLayout(hlayout);
			}

			const auto widget = new QWidget(this);
			widget->setObjectName("border");
			widget->setLayout(vlayout);

			auto hlayout = get_hbox_layout();
			hlayout->addWidget(widget);

			layout->addLayout(hlayout);
			layout->addSpacing(7);
		}

		// Redirect
		{
			auto vlayout = get_vbox_layout();

			auto label = new QLabel("Redirect", this);
			vlayout->addWidget(label);
			vlayout->addSpacing(1);

			m_network_redirect = new QLineEdit(this);
			m_network_redirect->setAttribute(Qt::WA_MacShowFocusRect, 0);
			m_network_redirect->setFixedWidth(522);
			m_network_redirect->setMaxLength(255);

			// Data
			const auto network = m_entity->get<message_network_component>();
			m_network_redirect->setText(QString::fromStdString(network->get_redirect()));

			vlayout->addWidget(m_network_redirect);
			layout->addLayout(vlayout);
			layout->addSpacing(7);
		}

		// MOTD
		{
			auto vlayout = get_vbox_layout();

			auto label = new QLabel("Message of the Day", this);
			vlayout->addWidget(label);
			vlayout->addSpacing(1);

			m_network_motd = new QTextEdit(this);
			m_network_motd->setAttribute(Qt::WA_MacShowFocusRect, 0);
			m_network_motd->setFixedWidth(522);
			m_network_motd->setFixedHeight(56);

			// Data
			const auto network = m_entity->get<message_network_component>();
			m_network_motd->setText(QString::fromStdString(network->get_motd()));

			vlayout->addWidget(m_network_motd);
			layout->addLayout(vlayout);
			layout->addSpacing(7);
		}

		// Group
		const auto group = new QGroupBox("Network", this);
		group->setFlat(true);
		group->setLayout(layout);

		m_layout->addWidget(group);
		m_layout->addSpacing(7);
	}

	void router_option_widget::signal()
	{
		// Name
		connect(m_name, &QLineEdit::textChanged, [=](const QString& str)
		{
			const auto endpoint = m_entity->get<endpoint_component>();
			const auto qname = str.simplified().toStdString();

			if (endpoint->get_name() != qname)
			{
				endpoint->set_name(qname);

				// Action
				const auto window = main_window::get();
				const auto action = window->get_action();
				action->set_text(endpoint->get_name());

				// Color
				if (!endpoint->has_color())
					set_color();

				set_modified(true);
			}
		});

		// Color
		connect(m_name_color, &QPushButton::clicked, [this]()
		{
			QColor color;
			const auto endpoint = m_entity->get<endpoint_component>();
			color.setRgba(endpoint->get_color());

			color = QColorDialog::getColor(color, this, "Router Color", QColorDialog::ShowAlphaChannel);
			if (color.isValid())
			{
				endpoint->set_color(color.rgba());
				set_modified(true);
				set_color();
			}
		});

		connect(m_name_default, &QPushButton::clicked, [this]()
		{
			const auto endpoint = m_entity->get<endpoint_component>();
			endpoint->set_color(0);

			set_modified(true);
			set_color();
		});

		// Theme
		connect(m_theme_edit, &QPushButton::clicked, [this]()
		{
			QFileDialog dialog;
			dialog.setWindowTitle("Theme Location");
			dialog.setOptions(QFileDialog::ReadOnly);
			dialog.setOption(QFileDialog::DontUseNativeDialog, true);
			dialog.setFileMode(QFileDialog::ExistingFile);
			dialog.setViewMode(QFileDialog::ViewMode::List);

			QStringList filters;
			filters << "CSS (*.css)" << "All Files (*)";
			dialog.setNameFilters(filters);
			dialog.selectNameFilter("CSS (*.css)");

			const auto& qpath = m_theme->text();
			if (!qpath.isEmpty())
			{
				boost::filesystem::path path = qpath.toStdString();
				const auto parent_path = path.parent_path();
				dialog.setDirectory(QString::fromStdString(parent_path.string()));
			}
			else
			{
				// Theme
				const auto qparent = qApp->applicationDirPath();
				const QString qtheme = qparent + QDir::separator() + demonsaw::theme;
				dialog.setDirectory(QFileInfo::exists(qtheme) ? qtheme : qparent);
			}

			if (dialog.exec())
			{
				const auto qpaths = dialog.selectedFiles();
				const auto& qpath = qpaths.at(0);

				if (qpath != m_theme->text())
				{
					// Theme
					const auto window = main_window::get();
					auto& app = window->get_application();
					app.set_theme(qpath.toStdString());

					m_theme->setText(qpath);
				}
			}
		});

		connect(m_theme_update, &QPushButton::clicked, [this]()
		{
			const auto& qpath = m_theme->text();
			if (!qpath.isEmpty())
			{
				// Theme
				const auto window = main_window::get();
				auto& app = window->get_application();
				app.set_theme(qpath.toStdString());
			}
		});

		connect(m_theme_default, &QPushButton::clicked, [this]()
		{
			// Theme
			const auto window = main_window::get();
			auto& app = window->get_application();
			app.set_theme();

			m_theme->clear();
		});

		// Network
		connect(m_network_address, &QLineEdit::textChanged, [=](const QString& str)
		{
			const auto endpoint = m_entity->get<endpoint_component>();
			const auto qaddress = str.simplified().toStdString();

			if (endpoint->get_address() != qaddress)
			{
				endpoint->set_address(qaddress);
				set_modified(true);
			}
		});

		connect(m_network_port, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto endpoint = m_entity->get<endpoint_component>();
			const auto qport = static_cast<u16>(value);

			if (endpoint->get_port() != qport)
			{
				endpoint->set_port(qport);
				set_modified(true);
			}
		});

		connect(m_network_buffer, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto network = m_entity->get<message_network_component>();
			const auto qbuffer = mb_to_b(static_cast<size_t>(value));
			network->set_buffer(qbuffer);
		});

		connect(m_type_message, &QCheckBox::stateChanged, [=](int state)
		{
			const auto network = m_entity->get<message_network_component>();
			network->set_message(state == Qt::Checked);

			const auto acceptor = m_entity->get<message_acceptor_component>();
			if (acceptor->valid())
				set_modified(true);
		});

		connect(m_type_transfer, &QCheckBox::stateChanged, [=](int state)
		{
			const auto network = m_entity->get<message_network_component>();
			network->set_transfer(state == Qt::Checked);

			const auto acceptor = m_entity->get<message_acceptor_component>();
			if (acceptor->valid())
				set_modified(true);
		});

		connect(m_type_open, &QCheckBox::stateChanged, [=](int state)
		{
			const auto network = m_entity->get<message_network_component>();
			network->set_open(state == Qt::Checked);

			const auto acceptor = m_entity->get<message_acceptor_component>();
			if (acceptor->valid())
				set_modified(true);
		});

		connect(m_network_redirect, &QLineEdit::textChanged, [=](const QString& str)
		{
			const auto network = m_entity->get<message_network_component>();
			const auto qredirect = str.simplified().toStdString();
			network->set_redirect(qredirect);
		});

		connect(m_network_motd, &QTextEdit::textChanged, [=]()
		{
			const auto network = m_entity->get<message_network_component>();
			const auto qmotd = m_network_motd->toPlainText().simplified().toStdString();
			network->set_motd(qmotd);
		});
	}

	// Set
	void router_option_widget::set_color()
	{
		QColor color;
		const auto endpoint = m_entity->get<endpoint_component>();
		color.setRgba(endpoint->get_color());

		const auto background = palette().color(QPalette::Base);
		if (color == background)
			color = palette().color(QPalette::Text);

		const auto color_rgba = QString("rgba(%1, %2, %3, %4%)").arg(color.red()).arg(color.green()).arg(color.blue()).arg(static_cast<size_t>((static_cast<float>(color.alpha()) / 255.0) * 100.0));
		const auto background_rgba = QString("rgba(%1, %2, %3, %4%)").arg(background.red()).arg(background.green()).arg(background.blue()).arg(static_cast<size_t>((static_cast<float>(background.alpha()) / 255.0) * 100.0));
		const auto style = QString("QPushButton { font-size: 14px; color: %2; background: %1; border: 0; padding: 0; } QPushButton:hover { color: %1; background: %2; border: 1px solid %1; } QPushButton:pressed { color: %1; border: 1px solid %1; }").arg(color_rgba).arg(background_rgba);

		m_name_color->setStyleSheet(style);
		m_name_default->setStyleSheet(style);

		m_theme_edit->setStyleSheet(style);
		m_theme_update->setStyleSheet(style);
		m_theme_default->setStyleSheet(style);
	}

	// Get
	QHBoxLayout* router_option_widget::get_hbox_layout() const
	{
		const auto layout = new QHBoxLayout;
		layout->setContentsMargins(0, 0, 0, 0);
		layout->setSpacing(6);
		layout->setMargin(0);

		return layout;
	}

	QVBoxLayout* router_option_widget::get_vbox_layout() const
	{
		const auto layout = new QVBoxLayout;
		layout->setContentsMargins(0, 0, 0, 0);
		layout->setSpacing(0);
		layout->setMargin(0);

		return layout;
	}
}
