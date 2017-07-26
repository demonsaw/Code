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

#include <QEvent>
#include <QGroupBox>
#include <QKeyEvent>
#include <QLineEdit>
#include <QListView>
#include <QSpinBox>
#include <QToolButton>
#include <QVBoxLayout>

#include "client_performance_pane.h"
#include "component/client/client_option_component.h"
#include "pane/pane.h"
#include "resource/resource.h"
#include "utility/boost.h"
#include "utility/std.h"

namespace eja
{
	// Constructor
	client_performance_pane::client_performance_pane(entity::ptr entity, QWidget* parent /*= 0*/) : entity_pane(entity, parent)
	{
		create();
		layout();
		signal();
	}

	// Interface
	void client_performance_pane::create()
	{		
		// Max		
		m_max_downloads = new QSpinBox(this);
		m_max_downloads->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_max_downloads->setFixedWidth(73);
		m_max_downloads->setRange(default_client::min_downloads, default_client::max_downloads);

		m_max_downloads_refresh = new QToolButton(this);
		m_max_downloads_refresh->setObjectName("pane");
		m_max_downloads_refresh->setIcon(QIcon(resource::menu::reset));
		m_max_downloads_refresh->setToolTip("Reset");

		m_max_uploads = new QSpinBox(this);
		m_max_uploads->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_max_uploads->setFixedWidth(72);
		m_max_uploads->setRange(default_client::min_uploads, default_client::max_uploads);

		m_max_uploads_refresh = new QToolButton(this);
		m_max_uploads_refresh->setObjectName("pane");
		m_max_uploads_refresh->setIcon(QIcon(resource::menu::reset));
		m_max_uploads_refresh->setToolTip("Reset");

		m_max_retries = new QSpinBox(this);
		m_max_retries->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_max_retries->setFixedWidth(73);
		m_max_retries->setRange(default_client::min_retries, default_client::max_retries);

		m_max_retries_refresh = new QToolButton(this);
		m_max_retries_refresh->setObjectName("pane");
		m_max_retries_refresh->setIcon(QIcon(resource::menu::reset));
		m_max_retries_refresh->setToolTip("Reset");

		m_max_searches = new QSpinBox(this);
		m_max_searches->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_max_searches->setFixedWidth(73);
		m_max_searches->setRange(default_client::min_searches, default_client::max_searches);

		m_max_searches_refresh = new QToolButton(this);
		m_max_searches_refresh->setObjectName("pane");
		m_max_searches_refresh->setIcon(QIcon(resource::menu::reset));
		m_max_searches_refresh->setToolTip("Reset");

		m_max_errors = new QSpinBox(this);
		m_max_errors->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_max_errors->setFixedWidth(72);
		m_max_errors->setRange(default_error::min_errors, default_error::max_errors);

		m_max_errors_refresh = new QToolButton(this);
		m_max_errors_refresh->setObjectName("pane");
		m_max_errors_refresh->setIcon(QIcon(resource::menu::reset));
		m_max_errors_refresh->setToolTip("Reset");

		// Timeout
		m_download_timeout = new QSpinBox(this);
		m_download_timeout->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_download_timeout->setFixedWidth(73);
		m_download_timeout->setRange(default_timeout::client::min_download / 1000, default_timeout::client::max_download / 1000);
		m_download_timeout->setSuffix(" sec");

		m_download_timeout_refresh = new QToolButton(this);
		m_download_timeout_refresh->setObjectName("pane");
		m_download_timeout_refresh->setIcon(QIcon(resource::menu::reset));
		m_download_timeout_refresh->setToolTip("Reset");

		m_upload_timeout = new QSpinBox(this);
		m_upload_timeout->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_upload_timeout->setFixedWidth(72);
		m_upload_timeout->setRange(default_timeout::client::min_upload / 1000, default_timeout::client::max_upload / 1000);
		m_upload_timeout->setSuffix(" sec");

		m_upload_timeout_refresh = new QToolButton(this);
		m_upload_timeout_refresh->setObjectName("pane");
		m_upload_timeout_refresh->setIcon(QIcon(resource::menu::reset));
		m_upload_timeout_refresh->setToolTip("Reset");

		m_ping_timeout = new QSpinBox(this);
		m_ping_timeout->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_ping_timeout->setFixedWidth(73);
		m_ping_timeout->setRange(default_timeout::client::min_ping / 1000, default_timeout::client::max_ping / 1000);
		m_ping_timeout->setSuffix(" sec");
		 
		m_ping_timeout_refresh = new QToolButton(this);
		m_ping_timeout_refresh->setObjectName("pane");
		m_ping_timeout_refresh->setIcon(QIcon(resource::menu::reset));
		m_ping_timeout_refresh->setToolTip("Reset");

		m_socket_timeout = new QSpinBox(this);
		m_socket_timeout->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_socket_timeout->setFixedWidth(73);
		m_socket_timeout->setRange(default_socket::min_timeout / 1000, default_socket::max_timeout / 1000);
		m_socket_timeout->setSuffix(" sec");

		m_socket_timeout_refresh = new QToolButton(this);
		m_socket_timeout_refresh->setObjectName("pane");
		m_socket_timeout_refresh->setIcon(QIcon(resource::menu::reset));
		m_socket_timeout_refresh->setToolTip("Reset");

		// Threads
		m_download_threads = new QSpinBox(this);
		m_download_threads->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_download_threads->setFixedWidth(73);
		m_download_threads->setRange(default_client::min_download_threads, default_client::max_download_threads);

		m_download_threads_refresh = new QToolButton(this);
		m_download_threads_refresh->setObjectName("pane");
		m_download_threads_refresh->setIcon(QIcon(resource::menu::reset));
		m_download_threads_refresh->setToolTip("Reset");

		m_upload_threads = new QSpinBox(this);
		m_upload_threads->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_upload_threads->setFixedWidth(72);
		m_upload_threads->setRange(default_client::min_upload_threads, default_client::max_upload_threads);

		m_upload_threads_refresh = new QToolButton(this);
		m_upload_threads_refresh->setObjectName("pane");
		m_upload_threads_refresh->setIcon(QIcon(resource::menu::reset));
		m_upload_threads_refresh->setToolTip("Reset");

		m_thread_size = new QSpinBox(this);
		m_thread_size->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_thread_size->setFixedWidth(73);
		m_thread_size->setRange(b_to_mb(default_client::min_thread_size), b_to_mb(default_client::max_thread_size));
		m_thread_size->setSuffix(" MB");

		m_thread_size_refresh = new QToolButton(this);
		m_thread_size_refresh->setObjectName("pane");
		m_thread_size_refresh->setIcon(QIcon(resource::menu::reset));
		m_thread_size_refresh->setToolTip("Reset");

		// Priority
		m_priority_ext = new QLineEdit(this);
		m_priority_ext->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_priority_ext->setFixedWidth(230);
		m_priority_ext->setMaxLength(128);

		m_priority_ext_clear = new QToolButton(this);
		m_priority_ext_clear->setObjectName("pane");
		m_priority_ext_clear->setIcon(QIcon(resource::menu::clear));
		m_priority_ext_clear->setToolTip("Clear");

		m_priority_size = new QSpinBox(this);
		m_priority_size->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_priority_size->setFixedWidth(73);
		m_priority_size->setRange(b_to_mb(default_client::min_priority_size), b_to_mb(default_client::max_priority_size));
		m_priority_size->setSuffix(" MB");

		m_priority_size_refresh = new QToolButton(this);
		m_priority_size_refresh->setObjectName("pane");
		m_priority_size_refresh->setIcon(QIcon(resource::menu::reset));
		m_priority_size_refresh->setToolTip("Reset");

		// Default
		const auto option = m_entity->get<client_option_component>();
		if (option)
		{
			// Max
			m_max_downloads->setValue(option->get_max_downloads());
			m_max_uploads->setValue(option->get_max_uploads()); 
			m_max_retries->setValue(option->get_max_retries());
			m_max_searches->setValue(option->get_max_searches());
			m_max_errors->setValue(option->get_max_errors());

			// Priority
			m_priority_size->setValue(b_to_mb(option->get_priority_size()));

			const auto& ext = option->get_priority_ext();
			m_priority_ext->setText(QString::fromStdString(ext));

			// Timeout
			m_download_timeout->setValue(option->get_download_timeout() / 1000);			
			m_upload_timeout->setValue(option->get_upload_timeout() / 1000);
			m_ping_timeout->setValue(option->get_ping_timeout() / 1000);
			m_socket_timeout->setValue(option->get_socket_timeout() / 1000);

			// Threads
			m_download_threads->setValue(option->get_download_threads());
			m_upload_threads->setValue(option->get_upload_threads());
			m_thread_size->setValue(b_to_mb(option->get_thread_size()));
		}
	}

	void client_performance_pane::layout()
	{
		// Max
		QVBoxLayout* max_layout = new QVBoxLayout(this);
		max_layout->setSpacing(0);
		max_layout->setMargin(6);

		pane::add_row(max_layout, "Download", m_max_downloads, m_max_downloads_refresh, "Upload", m_max_uploads, m_max_uploads_refresh, "Retries", m_max_retries, m_max_retries_refresh);
		pane::add_spacing(max_layout, 2);
		pane::add_row(max_layout, "Search", m_max_searches, m_max_searches_refresh, "Error", m_max_errors, m_max_errors_refresh);

		QGroupBox* max_group = new QGroupBox("Max", this);
		max_group->setLayout(max_layout);

		// Timeout
		QVBoxLayout* timeout_layout = new QVBoxLayout(this);
		timeout_layout->setSpacing(0);
		timeout_layout->setMargin(6);

		pane::add_row(timeout_layout, "Download", m_download_timeout, m_download_timeout_refresh, "Upload", m_upload_timeout, m_upload_timeout_refresh, "Socket", m_socket_timeout, m_socket_timeout_refresh);
		pane::add_spacing(max_layout, 2);
		pane::add_row(timeout_layout, "Ping", m_ping_timeout, m_ping_timeout_refresh);

		QGroupBox* timeout_group = new QGroupBox("Timeout", this);
		timeout_group->setLayout(timeout_layout);

		// Threads
		QVBoxLayout* threads_layout = new QVBoxLayout(this);
		threads_layout->setSpacing(0);
		threads_layout->setMargin(6);

		pane::add_row(threads_layout, "Download", m_download_threads, m_download_threads_refresh, "Upload", m_upload_threads, m_upload_threads_refresh, "Min Size", m_thread_size, m_thread_size_refresh);

		QGroupBox* threads_group = new QGroupBox("Threads", this);
		threads_group->setLayout(threads_layout);

		// Priority
		QVBoxLayout* uploads_layout = new QVBoxLayout(this);
		uploads_layout->setSpacing(0);
		uploads_layout->setMargin(6);

		pane::add_row(uploads_layout, "File Extensions (abc, xyz)", m_priority_ext, m_priority_ext_clear); 
		pane::add_spacing(uploads_layout, 2);
		pane::add_row(uploads_layout, "Max Size", m_priority_size, m_priority_size_refresh);

		QGroupBox* uploads_group = new QGroupBox("Upload Priority", this);
		uploads_group->setLayout(uploads_layout);

		// Layout
		QVBoxLayout* layout = new QVBoxLayout(this);
		layout->setContentsMargins(6, 0, 0, 0);
		layout->setSpacing(0);

		layout->addWidget(max_group);
		layout->addSpacing(6);

		layout->addWidget(timeout_group);
		layout->addSpacing(6);

		layout->addWidget(threads_group);
		layout->addSpacing(6);

		layout->addWidget(uploads_group);
		layout->addStretch(1);

		setLayout(layout);
	}

	void client_performance_pane::signal()
	{
		// Max
		connect(m_max_downloads, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto option = m_entity->get<client_option_component>();			
			if (option)
				option->set_max_downloads(value);
		});

		connect(m_max_downloads_refresh, &QToolButton::clicked, [this]()
		{
			m_max_downloads->setValue(default_client::num_downloads);
		});

		connect(m_max_uploads, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto option = m_entity->get<client_option_component>();
			if (option)
				option->set_max_uploads(value);
		});

		connect(m_max_uploads_refresh, &QToolButton::clicked, [this]()
		{
			m_max_uploads->setValue(default_client::num_uploads);
		}); 
		
		connect(m_max_retries, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto option = m_entity->get<client_option_component>();
			if (option)
				option->set_max_retries(value);
		});

		connect(m_max_retries_refresh, &QToolButton::clicked, [this]()
		{
			m_max_retries->setValue(default_client::num_retries);
		});

		connect(m_max_searches, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto option = m_entity->get<client_option_component>();
			if (option)
				option->set_max_searches(value);
		});

		connect(m_max_searches_refresh, &QToolButton::clicked, [this]()
		{
			m_max_searches->setValue(default_client::num_searches);
		});

		connect(m_max_errors, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto option = m_entity->get<client_option_component>();
			if (option)
				option->set_max_errors(value);
		});

		connect(m_max_errors_refresh, &QToolButton::clicked, [this]()
		{
			m_max_errors->setValue(default_error::num_errors);
		});

		// Timeout
		connect(m_download_timeout, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto option = m_entity->get<client_option_component>();
			if (option)
				option->set_download_timeout(value * 1000);
		});

		connect(m_download_timeout_refresh, &QToolButton::clicked, [this]()
		{
			m_download_timeout->setValue(default_timeout::client::num_download / 1000);
		});

		connect(m_upload_timeout, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto option = m_entity->get<client_option_component>();
			if (option)
				option->set_upload_timeout(value * 1000);
		});

		connect(m_upload_timeout_refresh, &QToolButton::clicked, [this]()
		{
			m_upload_timeout->setValue(default_timeout::client::num_upload / 1000);
		});

		connect(m_socket_timeout, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto option = m_entity->get<client_option_component>();
			if (option)
				option->set_socket_timeout(value * 1000);
		});

		connect(m_socket_timeout_refresh, &QToolButton::clicked, [this]()
		{
			m_socket_timeout->setValue(default_socket::num_timeout / 1000);
		});

		connect(m_ping_timeout, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto option = m_entity->get<client_option_component>();
			if (option)
				option->set_ping_timeout(value * 1000);
		});

		connect(m_ping_timeout_refresh, &QToolButton::clicked, [this]()
		{
			m_ping_timeout->setValue(default_timeout::client::num_ping / 1000);
		});

		// Threads
		connect(m_download_threads, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto option = m_entity->get<client_option_component>();
			if (option)
				option->set_download_threads(value);
		});

		connect(m_download_threads_refresh, &QToolButton::clicked, [this]()
		{
			m_download_threads->setValue(default_client::num_download_threads);
		});

		connect(m_upload_threads, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto option = m_entity->get<client_option_component>();
			if (option)
				option->set_upload_threads(value);
		});

		connect(m_upload_threads_refresh, &QToolButton::clicked, [this]()
		{
			m_upload_threads->setValue(default_client::num_upload_threads);
		});

		connect(m_thread_size, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto option = m_entity->get<client_option_component>();
			if (option)
				option->set_thread_size(mb_to_b(value));
		});

		connect(m_thread_size_refresh, &QToolButton::clicked, [this]()
		{
			m_thread_size->setValue(b_to_mb(default_client::num_thread_size));
		});

		// Priority
		connect(m_priority_size, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
		{
			const auto option = m_entity->get<client_option_component>();
			if (option)
				option->set_priority_size(mb_to_b(value));
		});

		connect(m_priority_size_refresh, &QToolButton::clicked, [this]()
		{
			m_priority_size->setValue(b_to_mb(default_client::num_priority_size));
		});

		connect(m_priority_ext, &QLineEdit::textChanged, [=](const QString& str)
		{
			const auto option = m_entity->get<client_option_component>();
			if (!option)
				return;

			const auto qext = str.trimmed().toStdString();
			option->set_priority_ext(qext);
		});

		connect(m_priority_ext_clear, &QToolButton::clicked, [this]()
		{
			m_priority_ext->clear();
			
			const auto option = m_entity->get<client_option_component>();
			if (option)
				option->set_priority_ext();
		});
	}
}
