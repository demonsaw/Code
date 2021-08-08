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
#include <QBitmap>
#include <QColor>
#include <QFont>
#include <QPaintEvent>
#include <QPen>
#include <QPoint>
#include <QStylePainter>
#include <QRect>

#include "component/callback/callback.h"
#include "component/client/client_component.h"
#include "component/client/client_service_component.h"
#include "component/status/status_component.h"

#include "font/font_awesome.h"
#include "resource/icon_provider.h"
#include "resource/resource.h"
#include "thread/thread_info.h"
#include "utility/value.h"
#include "widget/tab_widget.h"
#include "window/client_window.h"
#include "window/main_window.h"

namespace eja
{
	// Constructor
	tab_widget::tab_widget(const entity::ptr& entity, QWidget* parent /*= nullptr*/) : entity_callback(entity), QWidget(parent)
	{
		// Ratio
		const auto ratio = resource::get_ratio();
		setFixedSize(188 * ratio, 30 * ratio);

		// Signal
		connect(this, &tab_widget::clear, this, &tab_widget::on_clear);
		connect(this, &tab_widget::clear_chat, this, &tab_widget::on_clear_chat);

		connect(this, static_cast<void (tab_widget::*)(const entity::ptr& entity)>(&tab_widget::clear_pm), this, static_cast<void (tab_widget::*)(const entity::ptr& entity)>(&tab_widget::on_clear_pm));
		connect(this, static_cast<void (tab_widget::*)()>(&tab_widget::clear_pm), this, static_cast<void (tab_widget::*)()>(&tab_widget::on_clear_pm));

		connect(this, static_cast<void (tab_widget::*)(const entity::ptr& entity)>(&tab_widget::clear_room), this, static_cast<void (tab_widget::*)(const entity::ptr& entity)>(&tab_widget::on_clear_room));
		connect(this, static_cast<void (tab_widget::*)()>(&tab_widget::clear_room), this, static_cast<void (tab_widget::*)()>(&tab_widget::on_clear_room));

		connect(this, &tab_widget::chat, this, &tab_widget::on_chat);
		connect(this, &tab_widget::pm, this, &tab_widget::on_pm);
		connect(this, &tab_widget::room, this, &tab_widget::on_room);

		// Callback
		add_callback(callback::status | callback::clear, [&]() { clear(); });
		add_callback(callback::status | callback::update, [&]() { update(); });

		add_callback(callback::chat | callback::add, [&](const entity::ptr entity) { chat(entity); });
		add_callback(callback::chat | callback::clear, [&]() { emit clear_chat(); });

		add_callback(callback::pm | callback::add, [&](const entity::ptr entity) { pm(entity); });
		add_callback(callback::pm | callback::clear, [&]() { emit clear_pm(); });

		add_callback(callback::room | callback::add, [&](const entity::ptr entity) { room(entity); });
		add_callback(callback::room | callback::clear, [&]() { emit clear_room(); });
	}

	// Interface
	void tab_widget::on_clear()
	{
		assert(thread_info::main());

		if (m_chat || m_pm || m_room)
		{
			m_chat = nullptr;
			m_pm = nullptr;
			m_room = nullptr;
			update();
		}
	}

	// Utility
	void tab_widget::on_clear_chat()
	{
		assert(thread_info::main());

		if (m_chat)
		{
			m_chat = nullptr;
			update();
		}
	}

	void tab_widget::on_clear_pm(const entity::ptr& entity)
	{
		assert(thread_info::main());

		if (m_pm == entity)
			on_clear_pm();
	}

	void tab_widget::on_clear_pm()
	{
		assert(thread_info::main());

		if (m_pm)
		{
			m_pm = nullptr;
			update();
		}
	}

	void tab_widget::on_clear_room(const entity::ptr& entity)
	{
		assert(thread_info::main());

		if (m_room == entity)
			on_clear_room();
	}

	void tab_widget::on_clear_room()
	{
		assert(thread_info::main());

		if (m_room)
		{
			m_room = nullptr;
			update();
		}
	}

	void tab_widget::on_chat(const entity::ptr& entity)
	{
		assert(thread_info::main());

		const auto parent = entity->get_owner();

		if (parent && parent->has_owner())
		{
			const auto main = main_window::get();
			const auto owner = parent->get_owner();
			const auto window = main->get_window(owner);

			if (window)
			{
				if (main->isActiveWindow())
				{
					if (!window->isVisible() || !window->is_chat_visible())
					{
						const auto client = owner->get<client_component>();
						if (client->is_chat_notify() && (m_chat != parent))
						{
							m_chat = parent;
							repaint();
						}

						if (client->is_chat_tray())
							main->toast(entity, fa::comment);
					}
				}
				else
				{
					const auto client = owner->get<client_component>();
					if (client->is_chat_notify() && (m_chat != parent))
					{
						m_chat = parent;
						update();
					}

					if (client->is_chat_alert())
						QApplication::alert(window, sec_to_ms(1));

					if (client->is_chat_beep())
						QApplication::beep();

					if (client->is_chat_tray())
						main->toast(entity, fa::comment);
				}
			}
		}
	}

	void tab_widget::on_pm(const entity::ptr& entity)
	{
		assert(thread_info::main());

		const auto parent = entity->get_owner();

		if (parent && parent->has_owner())
		{
			const auto main = main_window::get();
			const auto owner = parent->get_owner();
			const auto window = main->get_window(owner);

			if (window)
			{
				if (main->isActiveWindow())
				{
					if (!window->isVisible() || !window->is_pm_visible(parent))
					{
						const auto client = owner->get<client_component>();
						if (client->is_pm_notify() && (m_pm != parent))
						{
							m_pm = parent;
							repaint();
						}

						if (client->is_pm_tray())
							main->toast(entity, fa::envelope);
					}
				}
				else
				{
					const auto client = owner->get<client_component>();
					if (client->is_pm_notify() && (m_pm != parent))
					{
						m_pm = parent;
						update();
					}

					if (client->is_pm_alert())
						QApplication::alert(window, sec_to_ms(1));

					if (client->is_pm_beep())
						QApplication::beep();

					if (client->is_pm_tray())
						main->toast(entity, fa::envelope);
				}
			}
		}
	}

	void tab_widget::on_room(const entity::ptr& entity)
	{
		assert(thread_info::main());

		const auto parent = entity->get_owner();

		if (parent && parent->has_owner())
		{
			const auto main = main_window::get();
			const auto owner = parent->get_owner();
			const auto window = main->get_window(owner);

			if (window)
			{
				if (main->isActiveWindow())
				{
					if (!window->isVisible() || !window->is_room_visible(entity))
					{
						const auto client = owner->get<client_component>();
						if (client->is_room_notify() && (m_room != parent))
						{
							m_room = parent;
							repaint();
						}

						if (client->is_room_tray())
							main->toast(entity, fa::hash_tag);
					}
				}
				else
				{
					const auto client = owner->get<client_component>();
					if (client->is_room_notify() && (m_room != parent))
					{
						m_room = parent;
						update();
					}

					if (client->is_room_alert())
						QApplication::alert(window, sec_to_ms(1));

					if (client->is_room_beep())
						QApplication::beep();

					if (client->is_room_tray())
						main->toast(entity, fa::hash_tag);
				}
			}
		}
	}

	// Event
	void tab_widget::paintEvent(QPaintEvent *event)
	{
		QStylePainter painter(this);
		painter.setRenderHint(QPainter::Antialiasing, true);

		// Ratio
		const auto ratio = resource::get_ratio();

		// Name
		const auto client = m_entity->get<client_component>();
		auto qname = QString::fromStdString(client->get_name());
		if (client->is_troll())
			qname += suffix::troll;

		QFontMetrics metrics(font());
		const auto elided = metrics.elidedText(qname, Qt::ElideRight, 98 * ratio);

		const auto point = QPoint(32 * ratio, 20 * ratio);
		painter.drawText(point, elided);

		// Color
		const auto font_pen = painter.pen();
		QFont font(software::font_awesome);
		font.setPixelSize(resource::get_circle_size());

		// Icon
		auto fa = fa::circle_o;
		const auto hoffset = 5 * ratio;

		const auto client_service = m_entity->get<client_service_component>();
		if (client_service->valid())
		{
			if (client->is_verified())
				fa = fa::check_circle;
			else if (client->is_troll())
				fa = fa::question_circle;
			else
				fa = fa::circle;
		}

		painter.setFont(font);
		painter.setPen(QColor(client->get_color()));
		painter.drawText(QRect(hoffset, 4 * ratio, resource::get_circle_size(), resource::get_circle_size()), Qt::TextSingleLine | Qt::AlignCenter, QString(fa));

		// Notifications
		font.setPixelSize(resource::get_small_font_size());
		painter.setFont(font);

		if (m_room)
		{
			const auto& app = main_window::get_app();
			if (app.is_user_colors())
			{
				const auto client = m_room->get<client_component>();
				painter.setPen(QColor(client->get_color()));
			}
			else
			{
				painter.setPen(font_pen);
			}

			painter.drawText(QRect(129 * ratio, 9 * ratio, 14 * ratio, 14 * ratio), Qt::TextSingleLine | Qt::AlignCenter, QString(fa::hash_tag));
		}

		if (m_pm)
		{
			const auto& app = main_window::get_app();
			if (app.is_user_colors())
			{
				const auto client = m_pm->get<client_component>();
				painter.setPen(QColor(client->get_color()));
			}
			else
			{
				painter.setPen(font_pen);
			}

			painter.drawText(QRect(145 * ratio, 8 * ratio, 14 * ratio, 14 * ratio), Qt::TextSingleLine | Qt::AlignCenter, QString(fa::envelope));
		}

		if (m_chat)
		{
			const auto& app = main_window::get_app();
			if (app.is_user_colors())
			{
				const auto client = m_chat->get<client_component>();
				painter.setPen(QColor(client->get_color()));
			}
			else
			{
				painter.setPen(font_pen);
			}

			painter.drawText(QRect(161 * ratio, 8 * ratio, 14 * ratio, 14 * ratio), Qt::TextSingleLine | Qt::AlignCenter, QString(fa::comment));
		}

		// Status
		const auto status = m_entity->get<status_component>();
		QPen pen(QColor(status->get_color()));
		pen.setWidth(4 * ratio);

		painter.setPen(pen);
		painter.drawArc(QRect(hoffset, 12 * ratio, resource::get_circle_size() - 1, resource::get_circle_size() - hoffset), -30 * 16, -120 * 16);

		painter.end();
	}
}
