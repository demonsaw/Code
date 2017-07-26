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

#include <QFont>
#include <QHBoxLayout>
#include <QLocale>
#include <QTimer>

#include "component/browse_component.h"
#include "component/chat_component.h"
#include "component/endpoint_component.h"
#include "component/io/download_component.h"
#include "component/io/queue_component.h"
#include "component/io/upload_component.h"
#include "component/search/search_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/status/statusbar_component.h"
#include "font/font_awesome.h"
#include "pane/client/client_status_pane.h"
#include "resource/icon_provider.h"
#include "utility/value.h"
#include "window/client/client_window.h"

namespace eja
{
	// Constructor
	status_pane::status_pane(QWidget* parent /*= 0*/) : QWidget(parent)
	{
		// Image
		m_image = new QPushButton(parent);
		m_image->setObjectName("status_image");

		// Text
		m_text = new QLabel(parent);
		m_text->setObjectName("status_text");
		m_text->setMinimumWidth(48);

		// Timer
		m_timer = new QTimer(this);
		m_timer->setSingleShot(true);

		// Signal
		connect(m_timer, &QTimer::timeout, [&]() { m_image->setStyleSheet(""); });
	}
	
	status_pane::status_pane(const int image, const char* psz, QWidget* parent /*= 0*/) : QWidget(parent)
	{
		set_image(image);
		set_text(psz);
	}
	
	client_status_pane::client_status_pane(const entity::ptr entity, QWidget* parent /*= 0*/) : QStatusBar(parent), entity_callback(entity, callback_type::status)
	{
		assert(thread_info::main());

		setMinimumHeight(24);

		// Layout
		QHBoxLayout* layout = new QHBoxLayout;
		layout->setContentsMargins(0, 0, 0, 0);
		layout->setSpacing(4);
		layout->setMargin(0);

		layout->addStretch(1);

		// Create
		const int images[] = { fa::user, fa::sitemap, fa::comment, fa::hourglassstart, fa::arrowdown, fa::arrowup, fa::server, fa::exchange };
		const char* tooltips[] = { "Clients", "Browsable Clients", "Chats", "Queue", "Downloads", "Uploads", "Message Routers", "Transfer Routers" };

		m_size = sizeof(images) / sizeof(int);
		m_pane = new status_pane*[m_size];

		for (auto i = 0; i < m_size; ++i)
		{
			m_pane[i] = new status_pane(this);
			m_pane[i]->set_tooltip(tooltips[i]);
			m_pane[i]->set_image(images[i]);
			m_pane[i]->set_text("0");

			const auto image = images[i];
			connect(m_pane[i]->get_image(), &QPushButton::clicked, [this, image]() { on_clicked(image); });

			layout->addWidget(m_pane[i]->get_image());
			layout->addWidget(m_pane[i]->get_text());
		}

		const auto widget = new QWidget(this);
		widget->setLayout(layout);

		addPermanentWidget(widget);

		// Signal		
		connect(this, static_cast<void (client_status_pane::*)(const entity::ptr)>(&client_status_pane::add), this, static_cast<void (client_status_pane::*)(const entity::ptr)>(&client_status_pane::on_add));
		connect(this, static_cast<void (client_status_pane::*)(const entity::ptr)>(&client_status_pane::remove), this, static_cast<void (client_status_pane::*)(const entity::ptr)>(&client_status_pane::on_remove));
		connect(this, static_cast<void (client_status_pane::*)(const entity::ptr)>(&client_status_pane::update), this, static_cast<void (client_status_pane::*)(const entity::ptr)>(&client_status_pane::on_update));
		connect(this, static_cast<void (client_status_pane::*)(const entity::ptr)>(&client_status_pane::clear), this, static_cast<void (client_status_pane::*)(const entity::ptr)>(&client_status_pane::on_clear));

		// Callback		
		add_callback(callback_action::add, [&](const entity::ptr entity) { add(entity); }); 
		add_callback(callback_action::remove, [&](const entity::ptr entity) { remove(entity); });
		add_callback(callback_action::update, [&](const entity::ptr entity) { update(entity); });
		add_callback(callback_action::clear, [&](const entity::ptr entity) { clear(entity); });
	}

	client_status_pane::~client_status_pane()
	{
		for (auto i = 0; i < m_size; ++i)
		{
			disconnect(m_pane[i]->get_image());
			delete m_pane[i];
		}			

		delete m_pane;
	}

	// Interface
	void client_status_pane::on_text(const std::shared_ptr<entity> entity)
	{
		assert(thread_info::main());

		if (entity)
		{
			// Color
			const auto statusbar = entity->get<statusbar_component>();
			const auto pane = m_pane[static_cast<int>(statusbar->get())];

			switch (statusbar->get())
			{
				case eja::statusbar::client:
				{
					const auto client_list = m_entity->get<client_list_component>();
					const auto str = QLocale::system().toString(static_cast<uint>(client_list->size()));
					pane->set_text(str);

					break;
				}
				case eja::statusbar::browse:
				{
					const auto browse_list = m_entity->get<browse_list_component>();
					const auto str = QLocale::system().toString(static_cast<uint>(browse_list->size()));
					pane->set_text(str);

					break;
				}
				case eja::statusbar::chat:
				{
					const auto chat_list = m_entity->get<chat_list_component>();
					const auto str = QLocale::system().toString(static_cast<uint>(chat_list->size()));
					pane->set_text(str);

					break;
				}
				case eja::statusbar::queue:
				{
					const auto queue_list = m_entity->get<queue_list_component>();
					const auto str = QLocale::system().toString(static_cast<uint>(queue_list->size()));
					pane->set_text(str);

					break;
				}
				case eja::statusbar::download:
				{
					// NOTE: Not thread-safe but should be fine
					const auto download_map = m_entity->get<download_map_component>();
					const auto str = QLocale::system().toString(static_cast<uint>(download_map->size()));
					pane->set_text(str);

					break;
				}
				case eja::statusbar::upload:
				{
					// NOTE: Not thread-safe but should be fine
					const auto upload_map = m_entity->get<upload_map_component>();
					const auto str = QLocale::system().toString(static_cast<uint>(upload_map->size()));
					pane->set_text(str);

					break;
				}
				case eja::statusbar::message:
				{
					const auto message_list = m_entity->get<message_list_component>();
					const auto str = QLocale::system().toString(static_cast<uint>(message_list->size()));
					pane->set_text(str);

					break;
				}
				case eja::statusbar::transfer:
				{
					const auto transfer_list = m_entity->get<transfer_list_component>();
					const auto str = QLocale::system().toString(static_cast<uint>(transfer_list->size()));
					pane->set_text(str);

					break;
				}
				default:
				{
					break;
				}
			}
		}
	}

	void client_status_pane::on_update(const std::shared_ptr<entity> entity)
	{
		assert(thread_info::main());

		if (entity)
		{
			// Color
			// TODO: Use CSS somehow?
			//
			const auto statusbar = entity->get<statusbar_component>();
			const auto pane = m_pane[static_cast<int>(statusbar->get())];
			const auto image = pane->get_image();
			image->setStyleSheet("QPushButton { color: #52C175; }");

			// Timer
			const auto timer = pane->get_timer();
			timer->start(milliseconds::status);
		}
	}

	void client_status_pane::on_clear(const std::shared_ptr<entity> entity)
	{
		assert(thread_info::main());

		if (entity)
		{
			// Color
			const auto statusbar = entity->get<statusbar_component>();
			const auto pane = m_pane[static_cast<int>(statusbar->get())];
			pane->set_text("0");
		}
	}

	// Utility
	void client_status_pane::on_clicked(const int image)
	{
		const auto window = qobject_cast<client_window*>(parent());
		if (window)
			window->on_click(image);
	}

	// Set
	void status_pane::set(const int image, const char* text)
	{ 
		set_image(image); 
		set_text(text); 
	}
	
	void status_pane::set_image(const int image)
	{
		QFont font(demonsaw::font_awesome);
		font.setPixelSize(14);

		m_image->setFont(font);
		m_image->setText(QString(image));		
	}
}
