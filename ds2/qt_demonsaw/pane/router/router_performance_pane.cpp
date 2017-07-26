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
#include <QLineEdit>
#include <QSpinBox>
#include <QToolButton>
#include <QVBoxLayout>

#include "router_performance_pane.h"
#include "component/router/router_option_component.h"
#include "pane/pane.h"
#include "resource/resource.h"
#include "system/type.h"
#include "utility/std.h"

namespace eja
{
	// Constructor
	router_performance_pane::router_performance_pane(entity::ptr entity, QWidget* parent /*= 0*/) : entity_pane(entity, parent)
	{
		create();
		layout();
		signal();
	}

	// Interface
	void router_performance_pane::create()
	{		
		// Max
		m_max_threads = new QSpinBox(this);
		m_max_threads->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_max_threads->setFixedWidth(73);
		m_max_threads->setRange(default_router::min_threads, default_router::max_threads);

		m_max_threads_refresh = new QToolButton(this);
		m_max_threads_refresh->setObjectName("pane");
		m_max_threads_refresh->setIcon(QIcon(resource::menu::reset));
		m_max_threads_refresh->setToolTip("Reset");

		m_max_transfers = new QSpinBox(this);
		m_max_transfers->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_max_transfers->setFixedWidth(72);
		m_max_transfers->setRange(default_router::min_transfers, default_router::max_transfers);

		m_max_transfers_refresh = new QToolButton(this);
		m_max_transfers_refresh->setObjectName("pane");
		m_max_transfers_refresh->setIcon(QIcon(resource::menu::reset));
		m_max_transfers_refresh->setToolTip("Reset");
		
		m_max_errors = new QSpinBox(this);
		m_max_errors->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_max_errors->setFixedWidth(73);
		m_max_errors->setRange(default_error::min_errors, default_error::max_errors);

		m_max_errors_refresh = new QToolButton(this);
		m_max_errors_refresh->setObjectName("pane");
		m_max_errors_refresh->setIcon(QIcon(resource::menu::reset));
		m_max_errors_refresh->setToolTip("Reset");

		// Timeout		
		m_client_timeout = new QSpinBox(this);
		m_client_timeout->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_client_timeout->setFixedWidth(73);
		m_client_timeout->setRange(default_timeout::router::min_client / 1000 / 60, default_timeout::router::max_client / 1000 / 60);
		m_client_timeout->setSuffix(" min");

		m_client_timeout_refresh = new QToolButton(this);
		m_client_timeout_refresh->setObjectName("pane");
		m_client_timeout_refresh->setIcon(QIcon(resource::menu::reset));
		m_client_timeout_refresh->setToolTip("Reset");

		m_queue_timeout = new QSpinBox(this);
		m_queue_timeout->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_queue_timeout->setFixedWidth(72);
		m_queue_timeout->setRange(default_timeout::router::min_queue / 1000 / 60, default_timeout::router::max_queue / 1000 / 60);
		m_queue_timeout->setSuffix(" min");

		m_queue_timeout_refresh = new QToolButton(this);
		m_queue_timeout_refresh->setObjectName("pane");
		m_queue_timeout_refresh->setIcon(QIcon(resource::menu::reset));
		m_queue_timeout_refresh->setToolTip("Reset");

		m_socket_timeout = new QSpinBox(this);
		m_socket_timeout->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_socket_timeout->setFixedWidth(73);
		m_socket_timeout->setRange(default_socket::min_timeout / 1000, default_socket::max_timeout / 1000);
		m_socket_timeout->setSuffix(" sec");

		m_socket_timeout_refresh = new QToolButton(this);
		m_socket_timeout_refresh->setObjectName("pane");
		m_socket_timeout_refresh->setIcon(QIcon(resource::menu::reset));
		m_socket_timeout_refresh->setToolTip("Reset");

		m_spam_timeout = new QSpinBox(this);
		m_spam_timeout->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_spam_timeout->setFixedWidth(73);
		m_spam_timeout->setRange(default_timeout::router::min_spam / 1000 / 60, default_timeout::router::max_spam / 1000 / 60);
		m_spam_timeout->setSuffix(" min");

		m_spam_timeout_refresh = new QToolButton(this);
		m_spam_timeout_refresh->setObjectName("pane");
		m_spam_timeout_refresh->setIcon(QIcon(resource::menu::reset));
		m_spam_timeout_refresh->setToolTip("Reset");

		m_transfer_timeout = new QSpinBox(this);
		m_transfer_timeout->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_transfer_timeout->setFixedWidth(72);
		m_transfer_timeout->setRange(default_timeout::router::min_transfer / 1000 / 60, default_timeout::router::max_transfer / 1000 / 60);
		m_transfer_timeout->setSuffix(" min");

		m_transfer_timeout_refresh = new QToolButton(this);
		m_transfer_timeout_refresh->setObjectName("pane");
		m_transfer_timeout_refresh->setIcon(QIcon(resource::menu::reset));
		m_transfer_timeout_refresh->setToolTip("Reset");

		// Transfer
		m_chunk_size = new QSpinBox(this);
		m_chunk_size->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_chunk_size->setFixedWidth(73);
		m_chunk_size->setRange(b_to_kb(default_chunk::min_size), b_to_kb(default_chunk::max_size));
		m_chunk_size->setSuffix(" KB");

		m_chunk_size_refresh = new QToolButton(this);
		m_chunk_size_refresh->setObjectName("pane");
		m_chunk_size_refresh->setIcon(QIcon(resource::menu::reset));
		m_chunk_size_refresh->setToolTip("Reset");

		m_chunk_buffer = new QSpinBox(this);
		m_chunk_buffer->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_chunk_buffer->setFixedWidth(72);
		m_chunk_buffer->setRange(b_to_kb(default_chunk::min_buffer), b_to_kb(default_chunk::max_buffer));
		m_chunk_buffer->setSuffix(" KB");

		m_chunk_buffer_refresh = new QToolButton(this);
		m_chunk_buffer_refresh->setObjectName("pane");
		m_chunk_buffer_refresh->setIcon(QIcon(resource::menu::reset));
		m_chunk_buffer_refresh->setToolTip("Reset");

		m_chunk_drift = new QSpinBox(this);
		m_chunk_drift->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_chunk_drift->setFixedWidth(73);
		m_chunk_drift->setRange(b_to_kb(default_chunk::min_drift), b_to_kb(default_chunk::max_drift));
		m_chunk_drift->setSuffix(" KB");

		m_chunk_drift_refresh = new QToolButton(this);
		m_chunk_drift_refresh->setObjectName("pane");
		m_chunk_drift_refresh->setIcon(QIcon(resource::menu::reset));
		m_chunk_drift_refresh->setToolTip("Reset");				

		// Default
		const auto option = m_entity->get<router_option_component>();
		if (option)
		{
			// Max
			m_max_threads->setValue(option->get_max_threads());
			m_max_transfers->setValue(option->get_max_transfers());
			m_max_errors->setValue(option->get_max_errors());

			// Timeout			
			m_client_timeout->setValue(option->get_client_timeout() / 1000 / 60);			
			m_queue_timeout->setValue(option->get_queue_timeout() / 1000 / 60);
			m_socket_timeout->setValue(option->get_socket_timeout() / 1000); 
			m_spam_timeout->setValue(option->get_spam_timeout() / 1000 / 60);
			m_transfer_timeout->setValue(option->get_transfer_timeout() / 1000 / 60);

			// Transfer
			m_chunk_size->setValue(b_to_kb(option->get_chunk_size()));
			m_chunk_buffer->setValue(b_to_kb(option->get_chunk_buffer()));
			m_chunk_drift->setValue(b_to_kb(option->get_chunk_drift()));
		}
	}

	void router_performance_pane::layout()
	{
		// Max
		QVBoxLayout* max_layout = new QVBoxLayout(this);
		max_layout->setSpacing(0);
		max_layout->setMargin(6);

		pane::add_row(max_layout, "Thread", m_max_threads, m_max_threads_refresh, "Transfer", m_max_transfers, m_max_transfers_refresh, "Error", m_max_errors, m_max_errors_refresh);

		QGroupBox* max_group = new QGroupBox("Max", this);
		max_group->setLayout(max_layout);

		// Timeout
		QVBoxLayout* timeout_layout = new QVBoxLayout(this);
		timeout_layout->setSpacing(0);
		timeout_layout->setMargin(6);

		pane::add_row(timeout_layout, "Client", m_client_timeout, m_client_timeout_refresh, "Queue", m_queue_timeout, m_queue_timeout_refresh, "Socket", m_socket_timeout, m_socket_timeout_refresh);
		pane::add_spacing(timeout_layout, 2);
		pane::add_row(timeout_layout, "Spam", m_spam_timeout, m_spam_timeout_refresh, "Transfer", m_transfer_timeout, m_transfer_timeout_refresh);

		QGroupBox* timeout_group = new QGroupBox("Timeout", this);
		timeout_group->setLayout(timeout_layout);

		// Transfer
		QVBoxLayout* transfer_layout = new QVBoxLayout(this);
		transfer_layout->setSpacing(0);
		transfer_layout->setMargin(6);

		pane::add_row(transfer_layout, "Size", m_chunk_size, m_chunk_size_refresh, "Buffer", m_chunk_buffer, m_chunk_buffer_refresh, "Drift", m_chunk_drift, m_chunk_drift_refresh);

		QGroupBox* transfer_group = new QGroupBox("Chunk", this);
		transfer_group->setLayout(transfer_layout);

		// Layout
		QVBoxLayout* layout = new QVBoxLayout(this);
		layout->setContentsMargins(6, 0, 0, 0);
		layout->setSpacing(0);		

		layout->addWidget(max_group);
		layout->addSpacing(6);

		layout->addWidget(timeout_group);
		layout->addSpacing(6);

		layout->addWidget(transfer_group);
		layout->addStretch(1);

		setLayout(layout);
	}

	void router_performance_pane::signal()
	{
		// Max
		connect(m_max_threads, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto option = m_entity->get<router_option_component>();
			if (option)
				option->set_max_threads(value);
		});

		connect(m_max_threads_refresh, &QToolButton::clicked, [this]()
		{
			m_max_threads->setValue(default_router::num_threads);
		});

		connect(m_max_transfers, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto option = m_entity->get<router_option_component>();
			if (option)
				option->set_max_transfers(value);
		});

		connect(m_max_transfers_refresh, &QToolButton::clicked, [this]()
		{
			m_max_transfers->setValue(default_router::num_transfers);
		});		

		connect(m_max_errors, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto option = m_entity->get<router_option_component>();
			if (option)
				option->set_max_errors(value);
		});

		connect(m_max_errors_refresh, &QToolButton::clicked, [this]()
		{
			m_max_errors->setValue(default_error::num_errors);
		});

		// Timeout
		connect(m_client_timeout, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto option = m_entity->get<router_option_component>();
			if (option)
				option->set_client_timeout(value * 1000 * 60);
		});

		connect(m_client_timeout_refresh, &QToolButton::clicked, [this]()
		{
			m_client_timeout->setValue(default_timeout::router::num_client / 1000 / 60);
		});

		connect(m_queue_timeout, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto option = m_entity->get<router_option_component>();
			if (option)
				option->set_queue_timeout(value * 1000 * 60);
		});

		connect(m_queue_timeout_refresh, &QToolButton::clicked, [this]()
		{
			m_queue_timeout->setValue(default_timeout::router::num_queue / 1000 / 60);
		});

		connect(m_socket_timeout, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto option = m_entity->get<router_option_component>();
			if (option)
				option->set_socket_timeout(value * 1000);
		});

		connect(m_socket_timeout_refresh, &QToolButton::clicked, [this]()
		{
			m_socket_timeout->setValue(default_socket::num_timeout / 1000);
		});

		connect(m_spam_timeout, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto option = m_entity->get<router_option_component>();
			if (option)
				option->set_spam_timeout(value * 1000 * 60);
		});

		connect(m_spam_timeout_refresh, &QToolButton::clicked, [this]()
		{
			m_spam_timeout->setValue(default_timeout::router::num_spam / 1000 / 60);
		});

		connect(m_transfer_timeout, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto option = m_entity->get<router_option_component>();
			if (option)
				option->set_transfer_timeout(value * 1000 * 60);
		});

		connect(m_transfer_timeout_refresh, &QToolButton::clicked, [this]()
		{
			m_transfer_timeout->setValue(default_timeout::router::num_transfer / 1000 / 60);
		});

		// Transfer
		connect(m_chunk_size, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto option = m_entity->get<router_option_component>();
			if (option)
				option->set_chunk_size(kb_to_b(value));
		});

		connect(m_chunk_size_refresh, &QToolButton::clicked, [this]()
		{
			m_chunk_size->setValue(b_to_kb(default_chunk::num_size));
		}); 
		
		connect(m_chunk_buffer, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto option = m_entity->get<router_option_component>();
			if (option)
				option->set_chunk_buffer(kb_to_b(value));
		});

		connect(m_chunk_buffer_refresh, &QToolButton::clicked, [this]()
		{
			m_chunk_buffer->setValue(b_to_kb(default_chunk::num_buffer));
		});

		connect(m_chunk_drift, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto option = m_entity->get<router_option_component>();
			if (option)
				option->set_chunk_drift(kb_to_b(value));
		});

		connect(m_chunk_drift_refresh, &QToolButton::clicked, [this]()
		{
			m_chunk_drift->setValue(b_to_kb(default_chunk::num_drift));
		});
	}
}
