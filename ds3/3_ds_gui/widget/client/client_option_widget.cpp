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
#include "component/client/client_io_component.h"
#include "component/client/client_network_component.h"
#include "component/client/client_option_component.h"
#include "component/client/client_service_component.h"
#include "component/session/session_security_component.h"
#include "entity/entity.h"
#include "entity/entity_action.h"
#include "entity/entity_factory.h"
#include "entity/entity_pane.h"
#include "entity/entity_table_view.h"
#include "entity/entity_window.h"
#include "font/font_awesome.h"
#include "resource/icon_provider.h"
#include "resource/resource.h"
#include "security/cipher/cipher.h"
#include "security/hash/hash.h"
#include "security/algorithm/diffie_hellman.h"
#include "utility/io/folder_util.h"
#include "widget/client/client_option_widget.h"
#include "window/main_window.h"

Q_DECLARE_METATYPE(eja::cipher_bits);

namespace eja
{
	// Constructor
	client_option_widget::client_option_widget(const entity::ptr entity, QWidget* parent /*= 0*/) : entity_widget(entity, "Options", fa::cog, callback_type::option, parent)
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

		client();
		network();
		security();
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
	void client_option_widget::on_update()
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
		const auto service = m_entity->get<client_service_component>();
		if (service->valid())
		{
			service->restart();
			set_modified(false);
		}			
	}

	// Utility
	void client_option_widget::client()
	{
		// Font
		QFont font(demonsaw::font_awesome);

		// Layout
		const auto layout = get_vbox_layout();	
		layout->setMargin(6);
		layout->addSpacing(1);

		// Client
		{
			auto hlayout = get_hbox_layout();			

			// Name
			{
				auto vlayout = get_vbox_layout();				

				auto label = new QLabel("Name", this);				
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

		// Save
		{
			auto hlayout = get_hbox_layout();

			// Path
			{
				auto vlayout = get_vbox_layout();

				auto label = new QLabel("Save Folder", this);
				vlayout->addWidget(label);
				vlayout->addSpacing(1);
				
				m_save_path = new QLineEdit(this);
				m_save_path->setAttribute(Qt::WA_MacShowFocusRect, 0);
				m_save_path->setFixedWidth(446);
				m_save_path->setMaxLength(255);

				// Data
				const auto io = m_entity->get<client_io_component>();
				if (!io->has_path())
				{
					const auto qpath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
					io->set_path(qpath.toStdString());
				}

				m_save_path->setText(QString::fromStdString(io->get_path()));

				vlayout->addWidget(m_save_path);
				hlayout->addLayout(vlayout);
			}

			// Edit
			{
				auto vlayout = get_vbox_layout();

				auto label = new QLabel("", this);
				vlayout->addWidget(label);
				vlayout->addSpacing(1);

				QFont font(demonsaw::font_awesome);
				m_save_edit = new QPushButton(QString(fa::folderopen));
				m_save_edit->setToolTip("Choose Folder");
				m_save_edit->setObjectName("font_awesome");
				m_save_edit->setFixedSize(QSize(32, 21));
				m_save_edit->setFont(font);

				vlayout->addWidget(m_save_edit);
				hlayout->addLayout(vlayout);
			}

			// Default
			{
				auto vlayout = get_vbox_layout();

				auto label = new QLabel("", this);
				vlayout->addWidget(label);
				vlayout->addSpacing(1);

				m_save_default = new QPushButton(QString(fa::undo), this);
				m_save_default->setToolTip("Default Folder");
				m_save_default->setObjectName("font_awesome");
				m_save_default->setFixedSize(QSize(32, 21));
				m_save_default->setFont(font);				

				vlayout->addWidget(m_save_default);
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
		const auto group = new QGroupBox("Client", this);
		group->setFlat(true);
		group->setLayout(layout);

		m_layout->addWidget(group);		
		m_layout->addSpacing(8);
	}

	void client_option_widget::network()
	{
		// Layout
		const auto layout = get_vbox_layout();
		layout->setMargin(6);
		layout->addSpacing(2);

		// Transfer
		{
			// Title
			{
				auto label = new QLabel("Transfer", this);
				layout->addWidget(label);
				layout->addSpacing(1);
			}

			auto vlayout = get_vbox_layout();
			vlayout->setMargin(6);
			
			// Row 1
			{
				auto hlayout = get_hbox_layout();
				hlayout->setSpacing(6);
				
				// Buffer
				{
					auto vvlayout = get_vbox_layout();

					auto label = new QLabel("Buffer", this);
					vvlayout->addWidget(label);

					m_network_window = new QSpinBox(this);
					m_network_window->setAttribute(Qt::WA_MacShowFocusRect, 0);
					m_network_window->setFixedHeight(21);
					m_network_window->setFixedWidth(80);
					m_network_window->setRange(network::min_window, network::max_window);

					// Data
					const auto network = m_entity->get<client_network_component>();
					m_network_window->setValue(network->get_window());

					vvlayout->addWidget(m_network_window);
					hlayout->addLayout(vvlayout);
				}

				// Chunk
				{
					auto vvlayout = get_vbox_layout();

					auto label = new QLabel("Chunk", this);
					vvlayout->addWidget(label);

					m_network_chunk = new QSpinBox(this);
					m_network_chunk->setAttribute(Qt::WA_MacShowFocusRect, 0);
					m_network_chunk->setFixedHeight(21);
					m_network_chunk->setFixedWidth(80);
					m_network_chunk->setRange(b_to_kb(network::min_chunk), b_to_kb(network::max_chunk));
					m_network_chunk->setSuffix(" KB");

					// Data
					const auto network = m_entity->get<client_network_component>();
					m_network_chunk->setValue(b_to_kb(network->get_chunk()));

					vvlayout->addWidget(m_network_chunk);
					hlayout->addLayout(vvlayout);
				}

				// Downloads
				{
					auto vvlayout = get_vbox_layout();

					auto label = new QLabel("Downloads", this);
					vvlayout->addWidget(label);

					m_network_downloads = new QSpinBox(this);
					m_network_downloads->setAttribute(Qt::WA_MacShowFocusRect, 0);
					m_network_downloads->setFixedHeight(21);
					m_network_downloads->setFixedWidth(80);
					m_network_downloads->setRange(network::min_downloads, network::max_downloads);

					// Data
					const auto network = m_entity->get<client_network_component>();
					m_network_downloads->setValue(network->get_downloads());

					vvlayout->addWidget(m_network_downloads);
					hlayout->addLayout(vvlayout);
				}

				// Threads
				//{
				//	auto vvlayout = get_vbox_layout();

				//	auto label = new QLabel("Threads", this);
				//	vvlayout->addWidget(label);

				//	m_network_threads = new QSpinBox(this);
				//	m_network_threads->setAttribute(Qt::WA_MacShowFocusRect, 0);
				//	m_network_threads->setFixedHeight(21);
				//	m_network_threads->setFixedWidth(80);
				//	m_network_threads->setRange(network::min_threads, network::max_threads);

				//	// Data
				//	const auto network = m_entity->get<client_network_component>();
				//	m_network_threads->setValue(network->get_threads());

				//	vvlayout->addWidget(m_network_threads);
				//	hlayout->addLayout(vvlayout);
				//}

				// Timeout
				//{
				//	auto vvlayout = get_vbox_layout();

				//	auto label = new QLabel("Timeout", this);
				//	vvlayout->addWidget(label);

				//	m_network_timeout = new QSpinBox(this);
				//	m_network_timeout->setAttribute(Qt::WA_MacShowFocusRect, 0);
				//	m_network_timeout->setFixedHeight(21);
				//	m_network_timeout->setFixedWidth(80);
				//	m_network_timeout->setRange(ms_to_min(network::min_timeout), ms_to_min(network::max_timeout));
				//	m_network_timeout->setSuffix(" min");

				//	// Data
				//	const auto network = m_entity->get<client_network_component>();
				//	m_network_timeout->setValue(ms_to_min(network->get_timeout()));

				//	vvlayout->addWidget(m_network_timeout);
				//	hlayout->addLayout(vvlayout);
				//}
				
				// Uploads
				{
					auto vvlayout = get_vbox_layout();

					auto label = new QLabel("Uploads", this);
					vvlayout->addWidget(label);

					m_network_uploads = new QSpinBox(this);
					m_network_uploads->setAttribute(Qt::WA_MacShowFocusRect, 0);
					m_network_uploads->setFixedHeight(21);
					m_network_uploads->setFixedWidth(80);
					m_network_uploads->setRange(network::min_uploads, network::max_uploads);

					// Data
					const auto network = m_entity->get<client_network_component>();
					m_network_uploads->setValue(network->get_uploads());

					vvlayout->addWidget(m_network_uploads);
					hlayout->addLayout(vvlayout);
				}

				hlayout->addStretch(1);
				vlayout->addLayout(hlayout);
			}

			const auto widget = new QWidget(this);
			widget->setObjectName("border");
			widget->setLayout(vlayout);

			auto hhlayout = get_hbox_layout();
			hhlayout->addWidget(widget);

			layout->addLayout(hhlayout);
			layout->addSpacing(7);
		}

		// Connection
		{
			// Title
			{
				auto label = new QLabel("Connection Mode", this);
				layout->addWidget(label);
				layout->addSpacing(1);
			}
			
			auto vlayout = get_vbox_layout();
			vlayout->setSpacing(6);
			vlayout->setMargin(6);

			// Type			
			{
				auto hlayout = get_hbox_layout();
				hlayout->setSpacing(6);

				// None
				/*m_network_none = new QRadioButton("None", this);
				hlayout->addWidget(m_network_none);*/

				//// Poll
				//m_network_poll = new QRadioButton("Poll", this);
				//hlayout->addWidget(m_network_poll);

				// Tunnel
				m_network_tunnel = new QRadioButton("Tunnel", this);
				hlayout->addWidget(m_network_tunnel);

				// Server
				m_network_server = new QRadioButton("Server", this);
				hlayout->addWidget(m_network_server);

				// Data
				const auto network = m_entity->get<client_network_component>();
				//m_network_none->setChecked(network->get_mode() == network_mode::none);
				//m_network_poll->setChecked(network->get_mode() == network_mode::poll);
				m_network_tunnel->setChecked(network->get_mode() == network_mode::tunnel);
				m_network_server->setChecked(network->get_mode() == network_mode::server);

				hlayout->addStretch(1);
				vlayout->addLayout(hlayout);
			}

			// Server (Internal)
			{
				auto hlayout = get_hbox_layout();
				hlayout->setSpacing(6);

				// Address
				{
					auto vvlayout = get_vbox_layout();

					auto label = new QLabel("Internal Address", this);
					vvlayout->addWidget(label);

					m_network_address = new QLineEdit(this);
					m_network_address->setAttribute(Qt::WA_MacShowFocusRect, 0);
					m_network_address->setFixedWidth(338);
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

				hlayout->addStretch(1);
				vlayout->addLayout(hlayout);
			}

			// Server (External)
			{
				auto hlayout = get_hbox_layout();
				hlayout->setSpacing(6);

				// Address
				{
					auto vvlayout = get_vbox_layout();

					auto label = new QLabel("External Address", this);
					vvlayout->addWidget(label);

					m_network_address_ext = new QLineEdit(this);
					m_network_address_ext->setAttribute(Qt::WA_MacShowFocusRect, 0);
					m_network_address_ext->setFixedWidth(338);
					m_network_address_ext->setMaxLength(255);

					// Data
					const auto endpoint = m_entity->get<endpoint_component>();
					m_network_address_ext->setText(QString::fromStdString(endpoint->get_address_ext()));

					vvlayout->addWidget(m_network_address_ext);
					hlayout->addLayout(vvlayout);
				}

				// Port
				{
					auto vvlayout = get_vbox_layout();

					auto label = new QLabel("Port", this);
					vvlayout->addWidget(label);

					m_network_port_ext = new QSpinBox(this);
					m_network_port_ext->setAttribute(Qt::WA_MacShowFocusRect, 0);
					m_network_port_ext->setFixedHeight(21);
					m_network_port_ext->setFixedWidth(80);
					m_network_port_ext->setRange(0, 65535);

					// Data
					const auto endpoint = m_entity->get<endpoint_component>();
					m_network_port_ext->setValue(endpoint->get_port_ext());

					vvlayout->addWidget(m_network_port_ext);
					hlayout->addLayout(vvlayout);
				}

				hlayout->addStretch(1);
				vlayout->addLayout(hlayout);
			}

			// Sockets
			//{
			//	auto vvlayout = get_vbox_layout();

			//	auto label = new QLabel("Sockets", this);
			//	vvlayout->addWidget(label);

			//	m_network_sockets = new QSpinBox(this);
			//	m_network_sockets->setAttribute(Qt::WA_MacShowFocusRect, 0);
			//	m_network_sockets->setFixedHeight(21);
			//	m_network_sockets->setFixedWidth(80);
			//	m_network_sockets->setRange(network::min_sockets, network::max_sockets);

			//	// Data
			//	const auto network = m_entity->get<client_network_component>();
			//	m_network_sockets->setValue(network->get_sockets());

			//	vvlayout->addWidget(m_network_sockets);
			//	vlayout->addLayout(vvlayout);

			//}

			// Poll/Tunnel
			//{
			//	auto hlayout = get_hbox_layout();
			//	hlayout->setSpacing(6);

			//	// Interval
			//	{
			//		auto vvlayout = get_vbox_layout();

			//		auto label = new QLabel("Interval", this);
			//		vvlayout->addWidget(label);

			//		m_network_interval = new QComboBox(this);
			//		m_network_interval->setAttribute(Qt::WA_MacShowFocusRect, 0);
			//		m_network_interval->setFixedHeight(21);
			//		m_network_interval->setFixedWidth(80);

			//		m_network_interval->addItem(" 1 sec", sec_to_ms(1));
			//		m_network_interval->addItem(" 3 secs", sec_to_ms(3));
			//		m_network_interval->addItem(" 5 secs", sec_to_ms(5));
			//		m_network_interval->addItem("10 secs", sec_to_ms(10));
			//		m_network_interval->addItem("15 secs", sec_to_ms(15));
			//		m_network_interval->addItem("30 secs", sec_to_ms(30));
			//		m_network_interval->addItem("45 secs", sec_to_ms(45));
			//		m_network_interval->addItem("60 secs", sec_to_ms(60));

			//		// Data
			//		const auto network = m_entity->get<client_network_component>();
			//		const auto index = m_network_interval->findData(QVariant::fromValue<uint>(network->get_interval()));
			//		m_network_interval->setCurrentIndex(index >= 0 ? index : 0);

			//		vvlayout->addWidget(m_network_interval);
			//		hlayout->addLayout(vvlayout);
			//	}

			//	hlayout->addStretch(1);
			//	vlayout->addLayout(hlayout);
			//}
			
			const auto widget = new QWidget(this);
			widget->setObjectName("border");
			widget->setLayout(vlayout);

			auto hlayout = get_hbox_layout();
			hlayout->addWidget(widget);

			layout->addLayout(hlayout);
			layout->addSpacing(8);
		}

		// Connection
		{
			// Title
			{
				auto label = new QLabel("Router Trust Level", this);
				layout->addWidget(label);
				layout->addSpacing(1);
			}

			auto vlayout = get_vbox_layout();
			vlayout->setSpacing(6);
			vlayout->setMargin(6);

			// Type			
			{
				auto hlayout = get_hbox_layout();
				hlayout->setSpacing(6);

				// None
				m_trust_none = new QRadioButton("None", this);
				hlayout->addWidget(m_trust_none);

				// Network
				m_trust_router = new QRadioButton("Message Router", this);
				hlayout->addWidget(m_trust_router);

				// All
				/*m_trust_all = new QRadioButton("All (Message Router + Others)", this);
				hlayout->addWidget(m_trust_all);*/

				// Data
				const auto network = m_entity->get<client_network_component>();
				m_trust_none->setChecked(network->get_trust() == network_trust::none);
				m_trust_router->setChecked(network->get_trust() == network_trust::router);
				/*m_trust_all->setChecked(network->get_trust() == network_trust::all);*/

				hlayout->addStretch(1);
				vlayout->addLayout(hlayout);
			}

			const auto widget = new QWidget(this);
			widget->setObjectName("border");
			widget->setLayout(vlayout);

			auto hlayout = get_hbox_layout();
			hlayout->addWidget(widget);

			layout->addLayout(hlayout);
			layout->addSpacing(8);
		}

		// Function
		{			
			// Title
			{
				auto label = new QLabel("Ghosting", this);
				layout->addWidget(label);
				layout->addSpacing(1);
			}

			auto vlayout = get_vbox_layout();
			vlayout->setMargin(6);

			// Checkboxes			
			{
				auto hlayout = get_hbox_layout();				

				// Browse
				m_ghosting_browse = new QCheckBox("Browse", this);				
				hlayout->addWidget(m_ghosting_browse);				

				// Chat
				m_ghosting_chat = new QCheckBox("Chat", this);
				hlayout->addWidget(m_ghosting_chat);

				// PM
				m_ghosting_pm = new QCheckBox("Private Message", this);
				hlayout->addWidget(m_ghosting_pm);

				// Search
				m_ghosting_search = new QCheckBox("Search", this);
				hlayout->addWidget(m_ghosting_search);

				// Transfer
				m_ghosting_upload = new QCheckBox("Upload", this);				
				hlayout->addWidget(m_ghosting_upload);

				// Data
				const auto network = m_entity->get<client_network_component>();
				m_ghosting_browse->setChecked(network->has_browse());
				m_ghosting_chat->setChecked(network->has_chat());
				m_ghosting_pm->setChecked(network->has_pm());
				m_ghosting_search->setChecked(network->has_search()); 
				m_ghosting_upload->setChecked(network->has_upload());

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

		// Option
		{
			// Title
			{
				auto label = new QLabel("Notifications", this);
				layout->addWidget(label);
				layout->addSpacing(1);
			}

			auto vlayout = get_vbox_layout();
			vlayout->setMargin(6);

			// Checkboxes			
			{
				auto hlayout = get_hbox_layout();

				// Browse
				m_option_beep = new QCheckBox("Beep", this);
				hlayout->addWidget(m_option_beep);

				// Chat
				m_option_flash = new QCheckBox("Flash", this);
				hlayout->addWidget(m_option_flash);

				// Data
				const auto option = m_entity->get<client_option_component>();
				m_option_beep->setChecked(option->has_beep());
				m_option_flash->setChecked(option->has_flash());

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

		// Group
		const auto group = new QGroupBox("Network", this);
		group->setFlat(true);
		group->setLayout(layout);

		m_layout->addWidget(group);
		m_layout->addSpacing(7);
	}

	void client_option_widget::security()
	{
		// Layout
		const auto layout = get_vbox_layout();
		layout->setMargin(6);
		layout->addSpacing(1);

		// File
		{
			// Title
			{
				auto label = new QLabel("File", this);
				layout->addWidget(label);
				layout->addSpacing(1);
			}

			auto hlayout = get_hbox_layout();
			hlayout->setSpacing(6);

			// Hash
			{
				auto vvlayout = get_vbox_layout();
				vvlayout->setSpacing(1);

				auto label = new QLabel("Hash", this);
				vvlayout->addWidget(label);

				m_io_hash = new QComboBox(this);
				m_io_hash->setAttribute(Qt::WA_MacShowFocusRect, 0);
				m_io_hash->setFixedHeight(21);
				m_io_hash->setFixedWidth(80);

				m_io_hash->addItem(hash_name::md5);
				m_io_hash->addItem(hash_name::sha1);
				m_io_hash->addItem(hash_name::sha224);
				m_io_hash->addItem(hash_name::sha256);
				m_io_hash->addItem(hash_name::sha384);
				m_io_hash->addItem(hash_name::sha512);

				// Data
				const auto io = m_entity->get<client_io_component>();
				m_io_hash->setCurrentText(QString::fromStdString(io->get_hash()));

				vvlayout->addWidget(m_io_hash);
				vvlayout->addSpacing(2);

				hlayout->addLayout(vvlayout);
			}

			hlayout->addStretch(1);

			auto vlayout = get_vbox_layout();
			vlayout->setMargin(6);
			vlayout->addLayout(hlayout);

			// Salt
			{
				auto vvlayout = get_vbox_layout();
				vvlayout->setSpacing(1);

				auto label = new QLabel("Salt", this);
				vvlayout->addWidget(label);
				vvlayout->addSpacing(1);

				m_io_salt = new QTextEdit(this);
				m_io_salt->setAttribute(Qt::WA_MacShowFocusRect, 0);
				m_io_salt->setFixedWidth(510);
				m_io_salt->setFixedHeight(56);

				// Data
				const auto io = m_entity->get<client_io_component>();
				m_io_salt->setText(QString::fromStdString(io->get_salt()));

				vvlayout->addWidget(m_io_salt);
				vvlayout->addSpacing(2);

				vlayout->addLayout(vvlayout);
			}

			const auto widget = new QWidget(this);
			widget->setObjectName("border");
			widget->setLayout(vlayout);

			auto hhlayout = get_hbox_layout();
			hhlayout->addWidget(widget);

			layout->addLayout(hhlayout);
			layout->addSpacing(7);
		}

		// Prime
		//{
		//	// Title
		//	{
		//		auto label = new QLabel("Prime Numbers", this);
		//		layout->addWidget(label);
		//		layout->addSpacing(1);
		//	}

		//	const auto session_security = m_entity->get<session_security_component>();
		//	auto hlayout = get_hbox_layout();
		//	hlayout->setSpacing(6);

		//	// Size
		//	{
		//		auto vlayout = get_vbox_layout();
		//		vlayout->setSpacing(1);

		//		auto label = new QLabel("Size", this);
		//		vlayout->addWidget(label);

		//		m_prime_size = new QSpinBox(this);
		//		m_prime_size->setAttribute(Qt::WA_MacShowFocusRect, 0);
		//		m_prime_size->setFixedHeight(21);
		//		m_prime_size->setFixedWidth(80);
		//		m_prime_size->setRange(128, 4096);
		//		vlayout->addWidget(m_prime_size);
		//		vlayout->addSpacing(2);

		//		// Data
		//		m_prime_size->setValue(session_security->get_prime_size());

		//		hlayout->addLayout(vlayout);
		//	}

		//	// Buffer
		//	{
		//		auto vlayout = get_vbox_layout();
		//		vlayout->setSpacing(1);

		//		auto label = new QLabel("Buffer", this);
		//		vlayout->addWidget(label);

		//		m_prime_buffer = new QSpinBox(this);
		//		m_prime_buffer->setObjectName("todo");
		//		m_prime_buffer->setAttribute(Qt::WA_MacShowFocusRect, 0);
		//		m_prime_buffer->setFixedHeight(21);
		//		m_prime_buffer->setFixedWidth(80);
		//		m_prime_buffer->setRange(0, 1024);

		//		// Data
		//		//m_prime_buffer->setValue(session_security->get_buffer());

		//		vlayout->addWidget(m_prime_buffer);
		//		vlayout->addSpacing(2);

		//		hlayout->addLayout(vlayout);
		//	}

		//	// Reuse
		//	{
		//		auto vlayout = get_vbox_layout();
		//		vlayout->setSpacing(1);

		//		auto label = new QLabel("Reuse", this);
		//		vlayout->addWidget(label);

		//		m_prime_reuse = new QSpinBox(this);
		//		m_prime_reuse->setObjectName("todo");
		//		m_prime_reuse->setAttribute(Qt::WA_MacShowFocusRect, 0);
		//		m_prime_reuse->setFixedHeight(21);
		//		m_prime_reuse->setFixedWidth(80);
		//		m_prime_reuse->setRange(0, 16);

		//		// Data
		//		//m_prime_buffer->setValue(session_security->get_reuse());

		//		vlayout->addWidget(m_prime_reuse);
		//		vlayout->addSpacing(2);

		//		hlayout->addLayout(vlayout);
		//	}

		//	// Threshold
		//	{
		//		auto vlayout = get_vbox_layout();
		//		vlayout->setSpacing(1);

		//		auto label = new QLabel("Threshold", this);
		//		vlayout->addWidget(label);

		//		m_prime_threshold = new QSpinBox(this);
		//		m_prime_threshold->setObjectName("todo");
		//		m_prime_threshold->setAttribute(Qt::WA_MacShowFocusRect, 0);
		//		m_prime_threshold->setFixedHeight(21);
		//		m_prime_threshold->setFixedWidth(80);
		//		m_prime_threshold->setRange(1, 100);

		//		// Data
		//		//m_prime_threshold->setValue(session_security->get_threshold());

		//		vlayout->addWidget(m_prime_threshold);
		//		vlayout->addSpacing(2);

		//		hlayout->addLayout(vlayout);
		//	}

		//	hlayout->addStretch(1);

		//	auto vlayout = get_vbox_layout();
		//	vlayout->setMargin(6);
		//	vlayout->addLayout(hlayout);

		//	const auto widget = new QWidget(this);
		//	widget->setObjectName("border");
		//	widget->setLayout(vlayout);

		//	auto hhlayout = get_hbox_layout();
		//	hhlayout->addWidget(widget);

		//	layout->addLayout(hhlayout);
		//	layout->addSpacing(7);
		//}

		// Router
		{
			// Title
			{
				auto label = new QLabel("Session", this);
				layout->addWidget(label);
				layout->addSpacing(1);
			}			
			
			auto vlayout = get_vbox_layout();
			vlayout->setMargin(6);

			// Router
			{
				auto hlayout = get_hbox_layout();
				hlayout->setSpacing(6);

				// Algorithm
				//{
				//	auto vvlayout = get_vbox_layout();
				//	vvlayout->setSpacing(1);

				//	auto label = new QLabel("Algorithm", this);
				//	vvlayout->addWidget(label);

				//	m_session_alg = new QComboBox(this);
				//	m_session_alg->setAttribute(Qt::WA_MacShowFocusRect, 0);
				//	m_session_alg->setFixedHeight(21);
				//	m_session_alg->setFixedWidth(80);
				//	m_session_alg->addItem(diffie_hellman::get_name());

				//	// Data
				//	m_session_alg->setCurrentText(QString::fromStdString(session_security->get_algorithm()));

				//	vvlayout->addWidget(m_session_alg);
				//	vvlayout->addSpacing(2);

				//	hlayout->addLayout(vvlayout);
				//}

				// Prime
				{
					auto vlayout = get_vbox_layout();
					vlayout->setSpacing(1);

					auto label = new QLabel("Prime Size", this);
					vlayout->addWidget(label);

					m_prime_size = new QSpinBox(this);
					m_prime_size->setAttribute(Qt::WA_MacShowFocusRect, 0);
					m_prime_size->setFixedHeight(21);
					m_prime_size->setFixedWidth(80);
					m_prime_size->setRange(128, 4096);
					vlayout->addWidget(m_prime_size);
					vlayout->addSpacing(2);

					// Data
					const auto session_security = m_entity->get<session_security_component>();
					m_prime_size->setValue(session_security->get_prime_size());

					hlayout->addLayout(vlayout);
				}

				// Cipher
				{
					auto vvlayout = get_vbox_layout();
					vvlayout->setSpacing(1);

					auto label = new QLabel("Cipher", this);
					vvlayout->addWidget(label);

					m_session_cipher = new QComboBox(this);
					m_session_cipher->setAttribute(Qt::WA_MacShowFocusRect, 0);
					m_session_cipher->setFixedHeight(21);
					m_session_cipher->setFixedWidth(80);

					m_session_cipher->addItem(cipher_name::aes);
					m_session_cipher->addItem(cipher_name::mars);
					m_session_cipher->addItem(cipher_name::rc6);
					m_session_cipher->addItem(cipher_name::serpent);
					m_session_cipher->addItem(cipher_name::twofish);

					// Data
					const auto session_security = m_entity->get<session_security_component>();
					m_session_cipher->setCurrentText(QString::fromStdString(session_security->get_cipher()));

					vvlayout->addWidget(m_session_cipher);
					vvlayout->addSpacing(2);

					hlayout->addLayout(vvlayout);
				}

				// Key Size
				{
					auto vvlayout = get_vbox_layout();
					vvlayout->setSpacing(1);

					auto label = new QLabel("Key Size", this);
					vvlayout->addWidget(label);

					m_session_key_size = new QComboBox(this);
					m_session_key_size->setAttribute(Qt::WA_MacShowFocusRect, 0);
					m_session_key_size->setFixedHeight(21);
					m_session_key_size->setFixedWidth(80);

					m_session_key_size->addItem("128", QVariant::fromValue(cipher_bits::small));
					m_session_key_size->addItem("192",  QVariant::fromValue(cipher_bits::medium));
					m_session_key_size->addItem("256",  QVariant::fromValue(cipher_bits::large));

					// Data
					const auto session_security = m_entity->get<session_security_component>();
					const auto index = m_session_key_size->findData(QVariant::fromValue<uint>(session_security->get_key_size()));
					m_session_key_size->setCurrentIndex(index >= 0 ? index : 0);

					vvlayout->addWidget(m_session_key_size);
					vvlayout->addSpacing(2);

					hlayout->addLayout(vvlayout);
				}

				// Hash
				{
					auto vvlayout = get_vbox_layout();
					vvlayout->setSpacing(1);

					auto label = new QLabel("Hash", this);
					vvlayout->addWidget(label);

					m_session_hash = new QComboBox(this);
					m_session_hash->setAttribute(Qt::WA_MacShowFocusRect, 0);
					m_session_hash->setFixedHeight(21);
					m_session_hash->setFixedWidth(80);

					m_session_hash->addItem(hash_name::md5);
					m_session_hash->addItem(hash_name::sha1);
					m_session_hash->addItem(hash_name::sha224);
					m_session_hash->addItem(hash_name::sha256);
					m_session_hash->addItem(hash_name::sha384);
					m_session_hash->addItem(hash_name::sha512);

					// Data
					const auto session_security = m_entity->get<session_security_component>();
					m_session_hash->setCurrentText(QString::fromStdString(session_security->get_hash()));

					vvlayout->addWidget(m_session_hash);
					vvlayout->addSpacing(2);

					hlayout->addLayout(vvlayout);
				}

				// Iterations
				{
					auto vvlayout = get_vbox_layout();
					vvlayout->setSpacing(1);

					auto label = new QLabel("Iterations", this);
					vvlayout->addWidget(label);
					vvlayout->addSpacing(1);

					m_session_iterations = new QSpinBox(this);
					m_session_iterations->setAttribute(Qt::WA_MacShowFocusRect, 0);
					m_session_iterations->setFixedHeight(21);
					m_session_iterations->setFixedWidth(80);
					m_session_iterations->setRange(1, 1 << 17);

					// Data
					const auto session_security = m_entity->get<session_security_component>();
					m_session_iterations->setValue(session_security->get_iterations());

					vvlayout->addWidget(m_session_iterations);
					vvlayout->addSpacing(2);

					hlayout->addLayout(vvlayout);
				}

				hlayout->addStretch(1);
				vlayout->addLayout(hlayout);
			}

			// Salt
			{
				auto vvlayout = get_vbox_layout();
				vvlayout->setSpacing(1);

				auto label = new QLabel("Salt", this);
				vvlayout->addWidget(label);
				vvlayout->addSpacing(1);

				m_session_salt = new QTextEdit(this);				
				m_session_salt->setAttribute(Qt::WA_MacShowFocusRect, 0);
				m_session_salt->setFixedWidth(510);
				m_session_salt->setFixedHeight(56);

				// Data
				const auto session_security = m_entity->get<session_security_component>();
				m_session_salt->setText(QString::fromStdString(session_security->get_salt()));

				vvlayout->addWidget(m_session_salt);
				vvlayout->addSpacing(2);

				vlayout->addLayout(vvlayout);
			}

			const auto widget = new QWidget(this);
			widget->setObjectName("border");
			widget->setLayout(vlayout);

			auto hhlayout = get_hbox_layout();
			hhlayout->addWidget(widget);

			layout->addLayout(hhlayout);
			layout->addSpacing(7);
		}


		// Group
		const auto group = new QGroupBox("Security", this);
		group->setFlat(true);
		group->setLayout(layout);
		m_layout->addWidget(group);
	}

	void client_option_widget::signal()
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

			color = QColorDialog::getColor(color, this, "Client Color", QColorDialog::ShowAlphaChannel);
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
		connect(m_network_window, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto network = m_entity->get<client_network_component>();
			const auto qwindow = static_cast<size_t>(value);
			network->set_window(qwindow);
		});

		connect(m_network_chunk, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto network = m_entity->get<client_network_component>();
			const auto qchunk = kb_to_b(static_cast<size_t>(value));
			network->set_chunk(qchunk);
		});

		connect(m_network_downloads, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto network = m_entity->get<client_network_component>();
			const auto qdownloads = static_cast<size_t>(value);
			network->set_downloads(qdownloads);
		});

		/*connect(m_network_threads, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto network = m_entity->get<client_network_component>();
			const auto qthreads = static_cast<size_t>(value);
			network->set_threads(qthreads);
		});*/

		connect(m_network_uploads, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto network = m_entity->get<client_network_component>();
			const auto quploads = static_cast<size_t>(value);
			network->set_uploads(quploads);
		});

		/*connect(m_network_timeout, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto network = m_entity->get<client_network_component>();
			const auto qtimeout = static_cast<size_t>(value);
			network->set_timeout(min_to_ms(qtimeout));
		});*/

		// Connection
		/*connect(m_network_none, &QRadioButton::clicked, [=](bool checked)
		{
			const auto network = m_entity->get<client_network_component>();
			network->set_mode(network_mode::none);

			const auto service = m_entity->get<client_service_component>();
			if (service->valid())
				set_modified(true);
						
		});

		connect(m_network_poll, &QRadioButton::clicked, [=](bool checked)
		{
			const auto network = m_entity->get<client_network_component>();
			network->set_mode(network_mode::poll);

			const auto service = m_entity->get<client_service_component>();
			if (service->valid())
				set_modified(true);

		});*/

		connect(m_network_server, &QRadioButton::clicked, [=](bool checked)
		{
			const auto network = m_entity->get<client_network_component>();
			network->set_mode(network_mode::server);

			const auto service = m_entity->get<client_service_component>();
			if (service->valid())
				set_modified(true);

		});
		
		connect(m_network_tunnel, &QRadioButton::clicked, [=](bool checked)
		{
			const auto network = m_entity->get<client_network_component>();
			network->set_mode(network_mode::tunnel);

			const auto service = m_entity->get<client_service_component>();
			if (service->valid())
				set_modified(true);

		});

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

		connect(m_network_address_ext, &QLineEdit::textChanged, [=](const QString& str)
		{
			const auto endpoint = m_entity->get<endpoint_component>();
			const auto qaddress = str.simplified().toStdString();

			if (endpoint->get_address_ext() != qaddress)
			{
				endpoint->set_address_ext(qaddress);
				set_modified(true);
			}
		});

		connect(m_network_port_ext, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto endpoint = m_entity->get<endpoint_component>();
			const auto qport = static_cast<u16>(value);

			if (endpoint->get_port_ext() != qport)
			{
				endpoint->set_port_ext(qport);
				set_modified(true);
			}
		});

		/*	connect(m_network_interval, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index)
			{
				const auto variant = m_network_interval->itemData(index);
				if (!variant.isValid())
					return;

				const auto network = m_entity->get<client_network_component>();
				const auto qinterval = variant.toUInt();

				if (network->get_interval() != qinterval)
				{
					network->set_interval(qinterval);
					set_modified(true);
				}
			});*/

			/*connect(m_network_sockets, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
			{
				const auto network = m_entity->get<client_network_component>();
				const auto qsockets = static_cast<size_t>(value);

				if (network->get_sockets() != qsockets)
				{
					network->set_sockets(qsockets);
					set_modified(true);
				}
			});*/

		connect(m_trust_none, &QRadioButton::clicked, [=](bool checked)
		{
			const auto network = m_entity->get<client_network_component>();
			network->set_trust(network_trust::none);

			const auto service = m_entity->get<client_service_component>();
			if (service->valid())
				set_modified(true);

		});

		connect(m_trust_router, &QRadioButton::clicked, [=](bool checked)
		{
			const auto network = m_entity->get<client_network_component>();
			network->set_trust(network_trust::router);

			const auto service = m_entity->get<client_service_component>();
			if (service->valid())
				set_modified(true);

		});

		/*connect(m_trust_all, &QRadioButton::clicked, [=](bool checked)
		{
			const auto network = m_entity->get<client_network_component>();
			network->set_trust(network_trust::all);

			const auto service = m_entity->get<client_service_component>();
			if (service->valid())
				set_modified(true);

		});*/

		// Functions
		connect(m_ghosting_browse, &QCheckBox::stateChanged, [=](int state)
		{
			const auto network = m_entity->get<client_network_component>();
			network->set_browse(state == Qt::Checked);

			const auto service = m_entity->get<client_service_component>();
			if (service->valid())
				set_modified(true);
		});

		connect(m_ghosting_chat, &QCheckBox::stateChanged, [=](int state)
		{
			const auto network = m_entity->get<client_network_component>();
			network->set_chat(state == Qt::Checked);

			const auto service = m_entity->get<client_service_component>();
			if (service->valid())
				set_modified(true);
		});

		connect(m_ghosting_pm, &QCheckBox::stateChanged, [=](int state)
		{
			const auto network = m_entity->get<client_network_component>();
			network->set_pm(state == Qt::Checked);

			const auto service = m_entity->get<client_service_component>();
			if (service->valid())
				set_modified(true);
		});

		connect(m_ghosting_search, &QCheckBox::stateChanged, [=](int state)
		{
			const auto network = m_entity->get<client_network_component>();
			network->set_search(state == Qt::Checked);

			const auto service = m_entity->get<client_service_component>();
			if (service->valid())
				set_modified(true);
		});

		connect(m_ghosting_upload, &QCheckBox::stateChanged, [=](int state)
		{
			const auto network = m_entity->get<client_network_component>();
			network->set_upload(state == Qt::Checked);

			const auto service = m_entity->get<client_service_component>();
			if (service->valid())
				set_modified(true);
		});

		connect(m_option_beep, &QCheckBox::stateChanged, [=](int state)
		{
			const auto option = m_entity->get<client_option_component>();
			option->set_beep(state == Qt::Checked);
		});

		connect(m_option_flash, &QCheckBox::stateChanged, [=](int state)
		{
			const auto option = m_entity->get<client_option_component>();
			option->set_flash(state == Qt::Checked);
		});

		// IO
		connect(m_save_path, &QLineEdit::textChanged, [=](const QString& str)
		{
			const auto io = m_entity->get<client_io_component>();
			const auto qpath = str.simplified().toStdString();
			io->set_path(qpath);
		});

		connect(m_save_edit, &QPushButton::clicked, [this]()
		{
			QFileDialog dialog;
			dialog.setWindowTitle("Save Location");
			dialog.setOptions(QFileDialog::ShowDirsOnly | QFileDialog::ReadOnly);
			dialog.setOption(QFileDialog::DontUseNativeDialog, true);
			dialog.setFileMode(QFileDialog::Directory);
			dialog.setViewMode(QFileDialog::ViewMode::List);

			dialog.setDirectory(m_save_path->text());

			if (dialog.exec())
			{
				const auto qpaths = dialog.selectedFiles();
				const auto& qpath = qpaths.at(0);

				if (qpath != m_save_path->text())
				{
					m_save_path->setText(qpath);

					const auto io = m_entity->get<client_io_component>();
					io->set_path(qpath.toStdString());
				}
			}
		});

		connect(m_save_default, &QPushButton::clicked, [this]()
		{
			const auto qpath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
			m_save_path->setText(qpath);

			const auto io = m_entity->get<client_io_component>();
			io->set_path(qpath.toStdString());
		});

		connect(m_io_hash, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentTextChanged), [=](const QString& str)
		{
			const auto io = m_entity->get<client_io_component>();
			const auto& hash = io->get_hash();
			const auto qhash = str.toStdString();

			if (hash != qhash)
			{
				io->set_hash(qhash);
				set_modified(true);
			}
		});

		connect(m_io_salt, &QTextEdit::textChanged, [=]()
		{
			const auto io = m_entity->get<client_io_component>();
			const auto& salt = io->get_salt();
			const auto qsalt = m_io_salt->toPlainText().simplified().toStdString();

			if (salt != qsalt)
			{
				io->set_salt(qsalt);
				set_modified(true);
			}
		});

		// Prime
		connect(m_prime_size, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto session_security = m_entity->get<session_security_component>();
			const auto size = session_security->get_prime_size();
			const auto qsize = static_cast<size_t>(value);

			if (size != qsize)
			{
				session_security->set_prime_size(qsize);
				set_modified(true);
			}
		});

		//connect(m_prime_buffer, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		//{
		//	/*const auto session_security = m_entity->get<session_security_component>();
		//	const auto size = session_security->get_prime_size();
		//	const auto qsize = static_cast<size_t>(value);

		//	if (size != qsize)
		//	{
		//		session_security->set_prime_size(qsize);
		//		set_modified(true);
		//	}*/
		//});

		//connect(m_prime_reuse, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		//{
		//	/*const auto session_security = m_entity->get<session_security_component>();
		//	const auto size = session_security->get_prime_size();
		//	const auto qsize = static_cast<size_t>(value);

		//	if (size != qsize)
		//	{
		//		session_security->set_prime_size(qsize);
		//		set_modified(true);
		//	}*/
		//});

		//connect(m_prime_threshold, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		//{
		//	/*const auto session_security = m_entity->get<session_security_component>();
		//	const auto size = session_security->get_prime_size();
		//	const auto qsize = static_cast<size_t>(value);

		//	if (size != qsize)
		//	{
		//		session_security->set_prime_size(qsize);
		//		set_modified(true);
		//	}*/
		//});

		// Router
		/*connect(m_session_alg, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentTextChanged), [=](const QString& str)
		{
			const auto session_security = m_entity->get<session_security_component>();
			const auto& algorithm = session_security->get_algorithm();
			const auto qalgorithm = str.toStdString();

			if (algorithm != qalgorithm)
			{
				session_security->set_algorithm(qalgorithm);
				set_modified(true);
			}
		});*/

		connect(m_session_cipher, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentTextChanged), [=](const QString& str)
		{
			const auto session_security = m_entity->get<session_security_component>();
			const auto& cipher = session_security->get_cipher();
			const auto qcipher = str.toStdString();

			if (cipher != qcipher)
			{
				session_security->set_cipher(qcipher);
				set_modified(true);
			}
		});

		connect(m_session_key_size, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index)
		{			
			const auto variant = m_session_key_size->itemData(index);
			if (!variant.isValid())
				return;
			
			const auto session_security = m_entity->get<session_security_component>(); 
			const auto key_size = session_security->get_key_size();
			const auto qkey_size = variant.toUInt();

			if (key_size != qkey_size)
			{
				session_security->set_key_size(qkey_size);
				set_modified(true);
			}			
		});

		connect(m_session_hash, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentTextChanged), [=](const QString& str)
		{
			const auto session_security = m_entity->get<session_security_component>();
			const auto& hash = session_security->get_hash();
			const auto qhash = str.toStdString();

			if (hash != qhash)
			{
				session_security->set_hash(qhash);
				set_modified(true);
			}
		});

		connect(m_session_iterations, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto session_security = m_entity->get<session_security_component>();
			const auto iterations = session_security->get_iterations();
			const auto qiterations = static_cast<size_t>(value);

			if (iterations != qiterations)
			{
				session_security->set_iterations(qiterations);
				set_modified(true);
			}
		});

		connect(m_session_salt, &QTextEdit::textChanged, [=]()
		{
			const auto session_security = m_entity->get<session_security_component>();
			const auto& salt = session_security->get_salt();
			const auto qsalt = m_session_salt->toPlainText().simplified().toStdString();

			if (salt != qsalt)
			{
				session_security->set_salt(qsalt);
				set_modified(true);
			}
		});
	}

	// Set
	void client_option_widget::set_color()
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

		m_save_edit->setStyleSheet(style);
		m_save_default->setStyleSheet(style);

		m_theme_edit->setStyleSheet(style);
		m_theme_update->setStyleSheet(style);
		m_theme_default->setStyleSheet(style);
	}

	// Get
	QHBoxLayout* client_option_widget::get_hbox_layout() const
	{
		const auto layout = new QHBoxLayout;
		layout->setContentsMargins(0, 0, 0, 0);
		layout->setSpacing(6);
		layout->setMargin(0);

		return layout;
	}

	QVBoxLayout* client_option_widget::get_vbox_layout() const
	{
		const auto layout = new QVBoxLayout;
		layout->setContentsMargins(0, 0, 0, 0);
		layout->setSpacing(0);
		layout->setMargin(0);

		return layout;
	}
}
