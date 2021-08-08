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

#include <cassert>

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QLocale>
#include <QPushButton>
#include <QTimer>

#include "component/browse_component.h"
#include "component/chat_component.h"
#include "component/room_component.h"
#include "component/callback/callback.h"
#include "component/client/client_component.h"
#include "component/transfer/download_component.h"
#include "component/transfer/finished_component.h"
#include "component/transfer/transfer_component.h"
#include "component/transfer/upload_component.h"

#include "dialog/about_dialog.h"
#include "entity/entity.h"
#include "font/font_awesome.h"
#include "resource/resource.h"
#include "thread/thread_info.h"
#include "utility/value.h"
#include "widget/statusbar_widget.h"

namespace eja
{
	statusbar_widget::statusbar_widget(const entity::ptr& entity, QWidget* parent /*= nullptr*/) : QStatusBar(parent), entity_callback(entity)
	{
		assert(thread_info::main());

		// Ratio
		const auto ratio = resource::get_ratio();
		setMinimumHeight(24 * ratio);

		// Timer
		m_timer = new QTimer(this);
		m_timer->setSingleShot(true);

		// Layout
		const auto hlayout = resource::get_hbox_layout();
		hlayout->setSpacing(resource::get_icon_padding());
		hlayout->addSpacing(-resource::get_icon_padding() / 2);

		// Activity
		m_activity = new QPushButton(this);
		m_activity->setObjectName("status_about");
		m_activity->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		m_activity->setIconSize(QSize(resource::get_large_icon_size(), resource::get_large_icon_size()));
		m_activity->setToolTip("Network Activity");
		m_activity->setToolTipDuration(0);
		hlayout->addWidget(m_activity, Qt::AlignLeft);

		// Copyright
		auto font = QApplication::font();
		font.setFamily(font_family::main);
		font.setPixelSize(resource::get_small_font_size());

		m_author = new QLabel(software::motto, this);
		m_author->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		m_author->setFont(font);

		// Icon
		m_active = QPixmap(resource::logo_rgb).scaledToWidth(resource::get_icon_size() + 1, Qt::TransformationMode::SmoothTransformation);
		m_inactive = QPixmap(resource::logo_black).scaledToWidth(resource::get_icon_size() + 1, Qt::TransformationMode::SmoothTransformation);
		m_activity->setIcon(m_inactive);

		hlayout->addWidget(m_author, Qt::AlignLeft);
		hlayout->addStretch(1);

		// Create
		static const int images[] = { fa::user, fa::folder_open, fa::comment, fa::hourglass_start, fa::arrow_down, fa::arrow_up, fa::check, fa::server };
		static const char* tooltip[] = { "Active Clients", "Sharing Clients", "Chat Messages", "Queued Transfers", "Active Downloads", "Active Uploads", "Finished Transfers", "Transfer Routers" };
		static const char* text[] = { "0", "0", "0", "0", "0", "0", "0", "0" };

		m_pane = new statusbar_pane*[pane::max];

		for (auto i = 0; i < pane::max; ++i)
		{
			m_pane[i] = new statusbar_pane(this);
			m_pane[i]->set_image(images[i]);
			m_pane[i]->set_tooltip(tooltip[i]);
			m_pane[i]->set_text(text[i]);

			hlayout->addWidget(m_pane[i]->get_image());
			hlayout->addSpacing(resource::get_icon_padding());
			hlayout->addWidget(m_pane[i]->get_text());
		}

		const auto widget = new QWidget(this);
		widget->setLayout(hlayout);
		addPermanentWidget(widget, 1);

		// Signal
		connect(this, static_cast<void (statusbar_widget::*)()>(&statusbar_widget::add), this, static_cast<void (statusbar_widget::*)()>(&statusbar_widget::on_add));
		connect(this, static_cast<void (statusbar_widget::*)()>(&statusbar_widget::remove), this, static_cast<void (statusbar_widget::*)()>(&statusbar_widget::on_remove));
		connect(this, static_cast<void (statusbar_widget::*)()>(&statusbar_widget::update), this, static_cast<void (statusbar_widget::*)()>(&statusbar_widget::on_update));
		connect(this, static_cast<void (statusbar_widget::*)()>(&statusbar_widget::clear), this, static_cast<void (statusbar_widget::*)()>(&statusbar_widget::on_clear));

		connect(m_activity, &QPushButton::clicked, this, &statusbar_widget::on_about);
		connect(m_timer, &QTimer::timeout, this, &statusbar_widget::on_timeout);

		// Callback
		add_callback(callback::statusbar | callback::add, [&]() { add(); });
		add_callback(callback::statusbar | callback::remove, [&]() { remove(); });
		add_callback(callback::statusbar | callback::update, [&]() { update(); });
		add_callback(callback::statusbar | callback::clear, [&]() { clear(); });
	}

	statusbar_widget::~statusbar_widget()
	{
		for (auto i = 0; i < pane::max; ++i)
			disconnect(m_pane[i]->get_image());

		delete m_pane;
	}

	// Interface
	void statusbar_widget::on_add()
	{
		assert(thread_info::main());

		// Client
		const auto room_map = m_entity->get<room_map_component>();
		const auto client_list = m_entity->get<client_list_component>();
		auto str = QLocale::system().toString(static_cast<uint>(!client_list->empty() ? (client_list->size() - room_map->size()) : 0));
		m_pane[pane::client]->set_text(str);

		// Browse
		const auto browse_list = m_entity->get<browse_list_component>();
		str = QLocale::system().toString(static_cast<uint>(browse_list->size()));
		m_pane[pane::browse]->set_text(str);

		// Chat
		const auto chat_list = m_entity->get<chat_list_component>();
		str = QLocale::system().toString(static_cast<uint>(chat_list->get_rows()));
		m_pane[pane::chat]->set_text(str);

		// Queued
		const auto download_list = m_entity->get<download_list_component>();
		str = QLocale::system().toString(static_cast<uint>(download_list->size()));
		m_pane[pane::queued]->set_text(str);

		// Download
		const auto client = m_entity->get<client_component>();
		const auto download_size = std::min(client->get_downloads(), download_list->size());
		str = QLocale::system().toString(static_cast<uint>(download_size));
		m_pane[pane::download]->set_text(str);

		// Upload
		const auto upload_list = m_entity->get<upload_list_component>();
		str = QLocale::system().toString(static_cast<uint>(upload_list->size()));
		m_pane[pane::upload]->set_text(str);

		// Upload
		const auto finished_list = m_entity->get<finished_list_component>();
		str = QLocale::system().toString(static_cast<uint>(finished_list->size()));
		m_pane[pane::finished]->set_text(str);

		// Router
		const auto transfer_list = m_entity->get<transfer_list_component>();
		str = QLocale::system().toString(static_cast<uint>(transfer_list->size()));
		m_pane[pane::router]->set_text(str);
	}

	void statusbar_widget::on_update()
	{
		assert(thread_info::main());

		m_activity->setIcon(m_active);

		// Timer
		m_timer->start(milliseconds::statusbar);
	}

	void statusbar_widget::on_clear()
	{
		assert(thread_info::main());

		for (size_t i = 0; i < pane::max; ++i)
		{
			auto& pane = m_pane[i];
			if (pane->has_text())
				pane->set_text("0");
		}
	}

	void statusbar_widget::on_about()
	{
		about_dialog dlg(this);
		dlg.exec();
	}

	void statusbar_widget::on_timeout()
	{
		m_activity->setIcon(m_inactive);
	}
}