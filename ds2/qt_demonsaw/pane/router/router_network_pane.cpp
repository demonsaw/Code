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

#include <boost/lexical_cast.hpp>

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QListView>
#include <QSpinBox>
#include <QTextEdit>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

#include "router_network_pane.h"
#include "asio/asio_resolver.h"
#include "component/router/router_component.h"
#include "component/router/router_idle_component.h"
#include "component/router/router_machine_component.h"
#include "component/router/router_option_component.h"
#include "component/router/router_security_component.h"
#include "pane/pane.h"
#include "resource/resource.h"

namespace eja
{	 
	// Constructor
	router_network_pane::router_network_pane(entity::ptr entity, QWidget* parent /*= 0*/) : entity_pane(entity, parent)
	{
		create();
		layout();
		signal();
	}

	// Interface
	void router_network_pane::create()
	{
		// Router				
		m_enabled = new QCheckBox("Enabled", this);
		
		m_name = new QLineEdit(this);
		m_name->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_name->setFixedWidth(230);
		m_name->setMaxLength(32);

		m_address = new QComboBox(this);
		m_address->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_address->setFixedWidth(166);
		m_address->setEditable(true);

		QListView* address_view = new QListView(this);
		address_view->setObjectName("border");
		m_address->setView(address_view);

		auto addresses = asio_resolver::get_addresses_v4();
		for (const auto& address : addresses)
		{
			const auto& addr = address.to_string();
			m_address->addItem(QString::fromStdString(addr));
		}

		addresses = asio_resolver::get_addresses_v6();
		for (const auto& address : addresses)
		{
			const auto& addr = address.to_string();
			m_address->addItem(QString::fromStdString(addr));
		}

		m_port = new QSpinBox(this);
		m_port->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_port->setFixedWidth(58);
		m_port->setRange(1, 65535);

		// Security		
		m_passphrase = new QTextEdit(this);
		m_passphrase->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_passphrase->setFixedWidth(230);
		m_passphrase->setFixedHeight(55);

		m_passphrase_clear = new QToolButton(this);
		m_passphrase_clear->setObjectName("pane");
		m_passphrase_clear->setIcon(QIcon(resource::menu::clear));
		m_passphrase_clear->setToolTip("Clear");

		// Type
		m_message = new QCheckBox("Message", this);
		m_message->setFixedWidth(73);

		m_public_group = new QCheckBox("Public", this);
		m_public_group->setFixedWidth(72);

		m_transfer = new QCheckBox("Transfer", this);
		m_transfer->setFixedWidth(73);

		// Options
		m_motd = new QTextEdit(this);
		m_motd->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_motd->setFixedWidth(230);
		m_motd->setFixedHeight(55);

		m_motd_clear = new QToolButton(this);
		m_motd_clear->setObjectName("pane");
		m_motd_clear->setIcon(QIcon(resource::menu::clear));
		m_motd_clear->setToolTip("Clear");

		m_redirect = new QLineEdit(this);
		m_redirect->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_redirect->setFixedWidth(230);
		m_redirect->setMaxLength(128);

		m_redirect_clear = new QToolButton(this);
		m_redirect_clear->setObjectName("pane");
		m_redirect_clear->setIcon(QIcon(resource::menu::clear));
		m_redirect_clear->setToolTip("Clear");

		// Default
		const auto option = m_entity->get<router_option_component>();
		if (option)
		{
			// Router
			m_enabled->setChecked(option->enabled());

			// Type
			m_message->setChecked(option->get_message_router());
			m_public_group->setChecked(option->get_public_router());
			m_transfer->setChecked(option->get_transfer_router());

			// Options
			m_motd->setText(QString::fromStdString(option->get_message()));
			m_redirect->setText(QString::fromStdString(option->get_redirect()));
		}

		// Router
		const auto router = m_entity->get<router_component>();
		if (router)
		{
			m_name->setText(QString::fromStdString(router->get_name()));

			const auto qaddress = QString::fromStdString(router->get_address());
			const auto index = m_address->findText(qaddress);
			if (index == -1)
				m_address->addItem(qaddress);

			m_address->setCurrentText(qaddress);

			m_port->setValue(router->get_port());
		}

		const auto security = m_entity->get<router_security_component>();
		if (security)
		{
			m_passphrase->setText(QString::fromStdString(security->get_passphrase()));
		}
	}

	void router_network_pane::layout()
	{
		// Router
		QVBoxLayout* router_layout = new QVBoxLayout(this);
		router_layout->setSpacing(0);
		router_layout->setMargin(6);

		pane::add_row(router_layout, "Name", m_name);
		pane::add_spacing(router_layout, 2);

		pane::add_row(router_layout, "Address", m_address, "Port", m_port);
		pane::add_spacing(router_layout, 2);

		pane::add_row(router_layout, "Passphrase", m_passphrase, m_passphrase_clear);

		QGroupBox* router_group = new QGroupBox("Router", this);
		router_group->setLayout(router_layout);

		// Options
		QVBoxLayout* option_layout = new QVBoxLayout(this);
		option_layout->setSpacing(0);
		option_layout->setMargin(6);

		pane::add_row(option_layout, "Redirect URI", m_redirect, m_redirect_clear);
		pane::add_spacing(option_layout, 2);

		pane::add_row(option_layout, "Message of the Day", m_motd, m_motd_clear);

		QGroupBox* option_group = new QGroupBox("Options", this);
		option_group->setLayout(option_layout);

		// Feature		
		QVBoxLayout* feature_layout = new QVBoxLayout(this);
		feature_layout->setSpacing(0);
		feature_layout->setMargin(6);

		pane::add_row(feature_layout, m_message, m_public_group, m_transfer);

		QGroupBox* feature_group = new QGroupBox("Type", this);
		feature_group->setLayout(feature_layout);

		// Layout
		QVBoxLayout* layout = new QVBoxLayout(this);
		layout->setContentsMargins(6, 0, 0, 0);
		layout->setSpacing(0);

		pane::add_row(layout, m_enabled);
		layout->addSpacing(6);

		layout->addWidget(router_group);
		layout->addSpacing(6);

		layout->addWidget(option_group);
		layout->addSpacing(6);

		layout->addWidget(feature_group);
		layout->addStretch(1);

		setLayout(layout);
	}

	void router_network_pane::signal()
	{
		// Router
		connect(m_enabled, &QCheckBox::stateChanged, [=](int state)
		{
			const auto option = m_entity->get<router_option_component>();
			if (!option)
				return;

			switch (state)
			{
				case Qt::Checked:
				{
					if (option->enabled())
						return;

					m_enabled->setEnabled(false);
					QTimer::singleShot(default_timeout::router::enabled, [this]() { m_enabled->setEnabled(true); });

					// Machine
					const auto machine = m_entity->get<router_machine_component>();
					if (machine)
						machine->start();

					// Idle
					const auto idle = m_entity->get<router_idle_component>();
					if (idle)
						idle->start();

					option->set_enabled();

					break;
				}
				case Qt::Unchecked:
				{
					if (option->disabled())
						return;

					m_enabled->setEnabled(false);
					QTimer::singleShot(default_timeout::router::enabled, [this]() { m_enabled->setEnabled(true); });

					// Idle
					const auto idle = m_entity->get<router_idle_component>();
					if (idle)
						idle->stop();

					// Machine
					const auto machine = m_entity->get<router_machine_component>();
					if (machine)
						machine->stop();

					// Feature
					m_entity->call(function_type::client, function_action::clear, m_entity);
					m_entity->call(function_type::transfer, function_action::clear, m_entity);
					m_entity->call(function_type::group, function_action::clear, m_entity);
					m_entity->call(function_type::error, function_action::clear, m_entity);

					option->set_disabled();

					break;
				}
			}
		});

		connect(m_name, &QLineEdit::textChanged, [=](const QString& str)
		{	
			const auto router = m_entity->get<router_component>();
			if (!router)
				return;

			const auto qname = str.trimmed().toStdString();
			router->set_name(qname);

			// Action
			const auto action = get_action();
			if (action)
			{
				action->setText(str);
				action->setIconText(str);
			}
		});

		connect(m_address, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentTextChanged), [=](const QString& str)
		{
			const auto router = m_entity->get<router_component>();
			if (!router)
				return;

			const auto& address = router->get_address();
			const auto qaddress = str.trimmed().toStdString();

			if (address != qaddress)
			{
				// TODO: Update status
				//
				const auto machine = m_entity->get<router_machine_component>();
				if (machine)
					set_modified(machine->success());
				
				router->set_address(qaddress);
			}
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
				// TODO: Update status
				//
				const auto machine = m_entity->get<router_machine_component>();
				if (machine)
					set_modified(machine->success());

				router->set_port(qport);
			}
		});

		// Security
		connect(m_passphrase, &QTextEdit::textChanged, [=]()
		{
			const auto security = m_entity->get<router_security_component>();
			if (security)
			{
				const auto passphrase = m_passphrase->toPlainText().toStdString();
				security->set_passphrase(passphrase);
			}
		});

		connect(m_passphrase_clear, &QToolButton::clicked, [this]()
		{
			m_passphrase->clear();
			const auto security = m_entity->get<router_security_component>();
			if (security)
				security->clear();
		});

		// Type		
		connect(m_message, &QCheckBox::stateChanged, [=](int state)
		{
			const auto option = m_entity->get<router_option_component>();
			if (option)
			{
				const auto value = state == Qt::CheckState::Checked;
				option->set_message_router(value);
			}
		});

		connect(m_public_group, &QCheckBox::stateChanged, [=](int state)
		{
			const auto option = m_entity->get<router_option_component>();
			if (option)
			{
				const auto value = state == Qt::CheckState::Checked;
				option->set_public_router(value);
			}
		});

		connect(m_transfer, &QCheckBox::stateChanged, [=](int state)
		{
			auto option = m_entity->get<router_option_component>();
			if (option)
			{
				const auto value = state == Qt::CheckState::Checked;
				option->set_transfer_router(value);
			}
		});

		// Options
		connect(m_motd, &QTextEdit::textChanged, [=]()
		{
			auto option = m_entity->get<router_option_component>();
			if (option)
			{
				const auto motd = m_motd->toPlainText().toStdString();
				option->set_message(motd);
			}
		});

		connect(m_motd_clear, &QToolButton::clicked, [this]()
		{
			m_motd->clear();

			auto option = m_entity->get<router_option_component>();
			if (option)
				option->set_message();
		});

		connect(m_redirect, &QLineEdit::textChanged, [=](const QString& str)
		{
			auto option = m_entity->get<router_option_component>();
			if (option)
			{
				const auto location = str.trimmed().toStdString();
				option->set_redirect(location);
			}
		});

		connect(m_redirect_clear, &QToolButton::clicked, [this]()
		{
			m_redirect->clear();

			auto option = m_entity->get<router_option_component>();
			if (option)
				option->set_redirect();
		});
	}
}
