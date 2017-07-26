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

#include <thread>
#include <boost/lexical_cast.hpp>

#include <QApplication>
#include <QCheckBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QSpinBox>
#include <QStandardPaths>
#include <QTextEdit>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

#include "client_network_pane.h"
#include "asio/asio_resolver.h"
#include "command/client/request/client_quit_request_command.h"
#include "component/chat_idle_component.h"
#include "component/color_component.h"
#include "component/client/client_component.h"
#include "component/client/client_idle_component.h"
#include "component/client/client_machine_component.h"
#include "component/client/client_option_component.h"
#include "component/router/router_component.h"
#include "component/router/router_security_component.h"
#include "component/transfer/transfer_idle_component.h"
#include "object/io/file.h"
#include "pane/pane.h"
#include "resource/resource.h"
#include "utility/io/file_util.h"

namespace eja
{
	// Constructor
	client_network_pane::client_network_pane(entity::ptr entity, QWidget* parent /*= 0*/) : entity_pane(entity, parent)
	{
		create();
		layout();
		signal();
	}

	// Interface
	void client_network_pane::create()
	{
		// Client
		m_enabled = new QCheckBox("Enabled", this);		
		
		m_name = new QLineEdit(this);
		m_name->setObjectName("pane");
		m_name->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_name->setFixedWidth(230);
		m_name->setMaxLength(32);

		m_name_color = new QToolButton(this);
		m_name_color->setObjectName("pane");
		m_name_color->setToolTip("Update");

		m_download_path = new QLineEdit(this);
		m_download_path->setObjectName("pane");
		m_download_path->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_download_path->setFixedWidth(230);
		m_download_path->setMaxLength(128);

		m_download_edit = new QToolButton(this);
		m_download_edit->setObjectName("pane");
		m_download_edit->setIcon(QIcon(resource::menu::edit));
		m_download_edit->setToolTip("Edit");

		m_download_refresh = new QToolButton(this);
		m_download_refresh->setObjectName("pane");
		m_download_refresh->setIcon(QIcon(resource::menu::reset));
		m_download_refresh->setToolTip("Reset");

		m_partial = new QCheckBox("Delete Partial Downloads", this);

		// Router		
		m_address = new QLineEdit(this);
		m_address->setObjectName("pane");
		m_address->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_address->setFixedWidth(166);
		m_address->setMaxLength(32);

		m_address_refresh = new QToolButton(this);
		m_address_refresh->setObjectName("pane");
		m_address_refresh->setIcon(QIcon(resource::menu::reset));
		m_address_refresh->setToolTip("Reset");

		m_port = new QSpinBox(this);
		m_port->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_port->setFixedWidth(58);
		m_port->setRange(1, 65535);

		m_port_refresh = new QToolButton(this);
		m_port_refresh->setObjectName("pane");
		m_port_refresh->setIcon(QIcon(resource::menu::reset));
		m_port_refresh->setToolTip("Reset");
		
		m_passphrase = new QTextEdit(this);
		m_passphrase->setObjectName("pane");
		m_passphrase->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_passphrase->setFixedWidth(230);
		m_passphrase->setFixedHeight(55);

		m_passphrase_clear = new QToolButton(this);
		m_passphrase_clear->setObjectName("pane");
		m_passphrase_clear->setIcon(QIcon(resource::menu::clear));
		m_passphrase_clear->setToolTip("Clear");

		m_motd = new QCheckBox("Message of the Day", this);

		// Type		
		m_browse = new QCheckBox("Browse", this);
		m_browse->setFixedWidth(73);

		m_chat = new QCheckBox("Chat", this);
		m_chat->setFixedWidth(73);

		m_message = new QCheckBox("Message", this);
		m_message->setFixedWidth(72); 
		
		m_search = new QCheckBox("Search", this);
		m_search->setFixedWidth(72);

		m_transfer = new QCheckBox("Transfer", this);
		m_transfer->setFixedWidth(73);

		// Default
		const auto option = m_entity->get<client_option_component>();
		if (option)
		{
			// Client
			m_enabled->setChecked(option->enabled());
						
			if (!option->has_download_path())
			{
				const auto download_path = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
				option->set_download_path(download_path.toStdString());
			}

			m_download_path->setText(QString::fromStdString(option->get_download_path()));
			m_partial->setChecked(option->has_partial());

			// Type
			m_browse->setChecked(option->get_browse());
			m_chat->setChecked(option->get_chat());
			m_message->setChecked(option->get_message());
			m_search->setChecked(option->get_search());
			m_transfer->setChecked(option->get_transfer());

			// Router
			m_motd->setChecked(option->get_motd());
		}

		// Client
		const auto client = m_entity->get<client_component>();
		if (client)
		{
			m_name->setText(QString::fromStdString(client->get_name()));			
		}

		// Router
		const auto router = m_entity->get<router_component>();
		if (router)
		{
			m_address->setText(QString::fromStdString(router->get_address()));
			m_port->setValue(router->get_port());
		}

		const auto security = m_entity->get<router_security_component>();
		if (security)
		{
			m_passphrase->setText(QString::fromStdString(security->get_passphrase()));
		}

		set_color();
	}

	void client_network_pane::layout()
	{
		// Client
		QVBoxLayout* client_layout = new QVBoxLayout(this);
		client_layout->setSpacing(0);
		client_layout->setMargin(6);

		pane::add_row(client_layout, "Name", m_name, m_name_color);
		pane::add_spacing(client_layout, 2);

		pane::add_row(client_layout, "Download Folder", m_download_path, m_download_edit, m_download_refresh);
		pane::add_spacing(client_layout, 4);

		pane::add_row(client_layout, m_partial);

		QGroupBox* client_group = new QGroupBox("Client", this);
		client_group->setLayout(client_layout);

		// Router
		QVBoxLayout* router_layout = new QVBoxLayout(this);
		router_layout->setSpacing(0);
		router_layout->setMargin(6);

		pane::add_row(router_layout, "Address", m_address, m_address_refresh, "Port", m_port, m_port_refresh);
		pane::add_spacing(router_layout, 2);

		pane::add_row(router_layout, "Passphrase", m_passphrase, m_passphrase_clear);
		pane::add_spacing(router_layout, 4);

		pane::add_row(router_layout, m_motd);

		QGroupBox* router_group = new QGroupBox("Router", this);
		router_group->setLayout(router_layout);

		// Type
		QVBoxLayout* feature_layout = new QVBoxLayout(this);
		feature_layout->setSpacing(0);
		feature_layout->setMargin(6);

		pane::add_row(feature_layout, m_browse, m_chat, m_message);
		pane::add_row(feature_layout, m_search, m_transfer);

		QGroupBox* feature_group = new QGroupBox("Type", this);
		feature_group->setLayout(feature_layout);

		// Layout
		QVBoxLayout* layout = new QVBoxLayout(this);
		layout->setContentsMargins(6, 0, 0, 0);
		layout->setSpacing(0);

		pane::add_row(layout, m_enabled);
		layout->addSpacing(6);

		layout->addWidget(client_group);
		layout->addSpacing(6);

		layout->addWidget(router_group);
		layout->addSpacing(6);
		
		layout->addWidget(feature_group);
		layout->addStretch(1);

		setLayout(layout);
	}

	void client_network_pane::signal()
	{
		// Client
		connect(m_enabled, &QCheckBox::stateChanged, [=](int state)
		{
			const auto option = m_entity->get<client_option_component>();
			if (!option)
				return;

			switch (state)
			{
				case Qt::Checked:
				{
					if (option->enabled())
						return;

					m_enabled->setEnabled(false);
					QTimer::singleShot(default_timeout::client::enabled, [this]() { m_enabled->setEnabled(true); });

					// Machine
					const auto machine = m_entity->get<client_machine_component>();
					if (machine)
						machine->start();

					// Client Idle
					const auto client_idle = m_entity->get<client_idle_component>();
					if (client_idle)
						client_idle->start();

					// Chat Idle
					const auto chat_idle = m_entity->get<chat_idle_component>();
					if (chat_idle)
						chat_idle->start();

					// Transfer
					const auto transfer = m_entity->get<transfer_idle_component>();
					if (transfer)
						transfer->start();

					option->set_enabled();

					break;
				}
				case Qt::Unchecked:
				{
					if (option->disabled())
						return;

					m_enabled->setEnabled(false);
					QTimer::singleShot(default_timeout::client::enabled, [this]() { m_enabled->setEnabled(true); });

					// Transfer
					const auto transfer = m_entity->get<transfer_idle_component>();
					if (transfer)
						transfer->stop();

					// Client Idle
					const auto client_idle = m_entity->get<client_idle_component>();
					if (client_idle)
						client_idle->stop();

					// Chat Idle
					const auto chat_idle = m_entity->get<chat_idle_component>();
					if (chat_idle)
						chat_idle->stop();

					// Machine
					std::thread thread([this, option]()
					{												
						const auto machine = m_entity->get<client_machine_component>();
						if (machine)
						{
							if (machine->success())
							{
								const auto socket = http_socket::create();

								try
								{
									const auto router = m_entity->get<router_component>();
									if (router)
									{
										// Socket
										socket->set_timeout(option->get_socket_timeout());
										socket->open(router->get_address(), router->get_port());

										// Quit
										const auto request_command = client_quit_request_command::create(m_entity, socket);
										request_command->execute();
									}
								}
								catch (...) {}

								socket->close();
							}

							machine->stop();
						}
					});

					thread.detach();

					// Feature
					m_entity->call(function_type::client, function_action::clear, m_entity);
					m_entity->call(function_type::search, function_action::clear, m_entity);
					m_entity->call(function_type::browse, function_action::clear, m_entity);
					m_entity->call(function_type::group, function_action::clear, m_entity);
					m_entity->call(function_type::share, function_action::clear, m_entity);
					m_entity->call(function_type::chat, function_action::clear, m_entity);
					m_entity->call(function_type::message, function_action::clear, m_entity);

					m_entity->call(function_type::download, function_action::clear, m_entity);
					m_entity->call(function_type::upload, function_action::clear, m_entity);
					m_entity->call(function_type::transfer, function_action::clear, m_entity);
					m_entity->call(function_type::finished, function_action::clear, m_entity);
					m_entity->call(function_type::error, function_action::clear, m_entity);

					option->set_disabled();

					break;
				}
			}
		});

		connect(m_name, &QLineEdit::textChanged, [=](const QString& str)
		{
			const auto client = m_entity->get<client_component>();
			if (!client)
				return;

			const auto& name = client->get_name();
			const auto qname = str.trimmed().toStdString();

			if (name != qname)
			{
				set_color();

				const auto machine = m_entity->get<client_machine_component>();
				if (machine)
					set_modified(machine->success());

				client->set_name(qname);
			}

			// Action
			const auto action = get_action();
			if (action)
			{
				action->setText(str);
				action->setIconText(str);
			}
		});

		connect(m_name_color, &QToolButton::clicked, [this]()
		{
			const auto option = m_entity->get<client_option_component>();
			if (!option)
				return;

			if (option->disabled())
				return;

			m_name_color->setEnabled(false);
			QApplication::setOverrideCursor(Qt::WaitCursor);

			QTimer::singleShot(default_timeout::client::enabled, [this]()
			{
				m_name_color->setEnabled(true);
				QApplication::restoreOverrideCursor();
			});

			// Machine
			const auto machine = m_entity->get<client_machine_component>();
			if (machine)
				machine->restart();
		});

		connect(m_download_path, &QLineEdit::textChanged, [=](const QString& str)
		{
			const auto option = m_entity->get<client_option_component>();
			if (!option)
				return;

			const auto path = str.toStdString();
			if (path != option->get_download_path())
			{
				option->set_download_path(path);

				// TODO: Update status
				//
				const auto machine = m_entity->get<client_machine_component>();
				if (machine)
					set_modified(machine->success());
			}
		});

		connect(m_download_edit, &QToolButton::clicked, [this]()
		{
			QFileDialog dialog;
			dialog.setWindowTitle("Download Path");
			dialog.setFileMode(QFileDialog::DirectoryOnly);
			dialog.setOptions(QFileDialog::ShowDirsOnly | QFileDialog::ReadOnly);
			dialog.setViewMode(QFileDialog::ViewMode::List);
			dialog.setDirectory(m_download_path->text());

			if (dialog.exec())
			{
				const auto qpaths = dialog.selectedFiles();
				const auto& qpath = qpaths.at(0);

				if (qpath != m_download_path->text())
				{
					m_download_path->setText(qpath);

					const auto option = m_entity->get<client_option_component>();
					if (option)
						option->set_download_path(qpath.toStdString());
				}
			}
		});

		connect(m_download_refresh, &QToolButton::clicked, [this]()
		{
			const auto path = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
			m_download_path->setText(path);

			const auto option = m_entity->get<client_option_component>();
			if (option)
				option->set_download_path(path.toStdString());
		});

		connect(m_partial, &QCheckBox::stateChanged, [=](int state)
		{
			const auto option = m_entity->get<client_option_component>();
			if (!option)
				return;

			const auto partial = (state == Qt::Checked);
			option->set_partial(partial);
		});

		// Router
		connect(m_address, &QLineEdit::textChanged, [=](const QString& str)
		{
			const auto router = m_entity->get<router_component>();
			if (!router)
				return;

			const auto& address = router->get_address();
			const auto qaddress = str.trimmed().toStdString();

			if (address != qaddress)
			{
				const auto machine = m_entity->get<client_machine_component>();
				if (machine)
					set_modified(machine->success());

				router->set_address(qaddress);
			}
		});

		connect(m_address_refresh, &QToolButton::clicked, [this]()
		{
			m_address->setText(default_network::address);

			const auto router = m_entity->get<router_component>();
			if (router)
				router->set_address(default_network::address);
		});

		connect(m_port, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto router = m_entity->get<router_component>();
			if (!router)
				return; 
			
			const auto& port = router->get_port();
			const auto qport = static_cast<u16>(value);

			if (port != qport)
			{
				const auto machine = m_entity->get<client_machine_component>();
				if (machine)
					set_modified(machine->success());

				router->set_port(qport);
			}
		});

		connect(m_port_refresh, &QToolButton::clicked, [this]()
		{
			m_port->setValue(default_network::port);

			const auto router = m_entity->get<router_component>();
			if (router)
				router->set_port(default_network::port);
		});

		connect(m_passphrase, &QTextEdit::textChanged, [=]()
		{
			const auto security = m_entity->get<router_security_component>();
			if (!security)
				return;
			
			const auto& passphrase = security->get_passphrase();
			const auto qpassphrase = m_passphrase->toPlainText().trimmed().toStdString();

			if (passphrase != qpassphrase)
			{
				const auto machine = m_entity->get<client_machine_component>();
				if (machine)
					set_modified(machine->success());

				security->set_passphrase(qpassphrase);
			}
		});

		connect(m_passphrase_clear, &QToolButton::clicked, [this]()
		{
			const auto qpasshrase = m_passphrase->toPlainText();
			if (qpasshrase.isEmpty())
				return;

			m_passphrase->clear();
			const auto security = m_entity->get<router_security_component>();
			if (security)
				security->clear();

			const auto machine = m_entity->get<client_machine_component>();
			if (machine)
				set_modified(machine->success());
		});

		// Client
		connect(m_motd, &QCheckBox::stateChanged, [=](int state)
		{
			const auto option = m_entity->get<client_option_component>();
			if (!option)
				return;

			const auto motd = (state == Qt::Checked);
			option->set_motd(motd);
		});

		// Type
		connect(m_browse, &QCheckBox::stateChanged, [=](int state)
		{
			const auto option = m_entity->get<client_option_component>();
			if (option)
			{
				set_modified(true);

				const auto value = state == Qt::CheckState::Checked;
				option->set_browse(value);
			}
		});

		connect(m_chat, &QCheckBox::stateChanged, [=](int state)
		{
			const auto option = m_entity->get<client_option_component>();
			if (option)
			{
				set_modified(true);

				const auto value = state == Qt::CheckState::Checked;
				option->set_chat(value);
			}
		});

		connect(m_message, &QCheckBox::stateChanged, [=](int state)
		{
			const auto option = m_entity->get<client_option_component>();
			if (option)
			{
				set_modified(true);

				const auto value = state == Qt::CheckState::Checked;
				option->set_message(value);
			}
		});

		connect(m_search, &QCheckBox::stateChanged, [=](int state)
		{
			const auto option = m_entity->get<client_option_component>();
			if (option)
			{
				set_modified(true);

				const auto value = state == Qt::CheckState::Checked;
				option->set_search(value);
			}
		});

		connect(m_transfer, &QCheckBox::stateChanged, [=](int state)
		{
			auto option = m_entity->get<client_option_component>();
			if (option)
			{
				set_modified(true);

				const auto value = state == Qt::CheckState::Checked;
				option->set_transfer(value);
			}
		});		
	}

	void client_network_pane::set_color()
	{
		const auto qname = m_name->text().trimmed();
		const auto name = qname.toStdString();
		QPixmap pixmap(13, 13);

		if (name == default_client::name)
		{
			const auto color = QColor(default_chat::color);
			pixmap.fill(color);
		}
		else if (name.empty())
		{
			const auto color = QColor(0xFF000000);
			pixmap.fill(color);
		}
		else
		{
			const auto& color = color_component::get_color(name);
			pixmap.fill(color);
		}

		m_name_color->setIcon(QIcon(pixmap));
	}
}
