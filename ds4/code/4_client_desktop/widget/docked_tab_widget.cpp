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

#include <QAction>
#include <QApplication>
#include <QColor>
#include <QFont>
#include <QKeyEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPen>
#include <QStylePainter>
#include <QRect>

#include "component/room_component.h"
#include "component/client/client_component.h"

#include "font/font_awesome.h"
#include "resource/resource.h"
#include "thread/thread_info.h"
#include "widget/docked_tab_widget.h"
#include "window/client_window.h"
#include "window/main_window.h"

namespace eja
{
	// Constructor
	docked_tab_widget::docked_tab_widget(const QString& text, QWidget* parent /*= nullptr*/) : entity_widget(parent)
	{
		setFixedWidth(resource::get_row_height() * 5);
		setFixedHeight(resource::get_row_height());
		setObjectName("docked_tab");
		setContextMenuPolicy(Qt::CustomContextMenu);

		// Font
		m_font = QApplication::font();
		m_font.setFamily(font_family::main);
		m_font.setPixelSize(resource::get_small_icon_size());
		setFont(m_font);

		// Action
		m_close_action = make_action(" Close", fa::close);

		// Event
		installEventFilter(this);

		// Signal
		connect(this, &docked_tab_widget::customContextMenuRequested, this, &docked_tab_widget::show_menu);
		connect(m_close_action, &QAction::triggered, this, &docked_tab_widget::on_close);

		set_text(text);
	}


	// Utility
	void docked_tab_widget::on_close()
	{
		const auto main = main_window::get();
		const auto window = main->get_window();
		if (window)
			window->close(this);
	}

	// Event
	bool docked_tab_widget::eventFilter(QObject* object, QEvent* event)
	{
		if (event->type() == QEvent::MouseButtonRelease)
		{
			QMouseEvent* mouse_event = static_cast<QMouseEvent*> (event);
			if (mouse_event->button() == Qt::MiddleButton)
				on_close();
		}

		return QObject::eventFilter(object, event);
	}

	void docked_tab_widget::paintEvent(QPaintEvent *event)
	{
		QStylePainter painter(this);
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.setFont(m_font);

		// Ratio
		const auto ratio = resource::get_ratio();

		QRect text_rect(resource::get_row_height(), 0, resource::get_row_height() * 5, resource::get_row_height());
		painter.drawText(text_rect, Qt::AlignLeft | Qt::AlignVCenter, m_elided_text);

		QRect rect(0, 0, resource::get_row_height(), resource::get_row_height());
		QFont font_awesome = QFont(font_family::font_awesome);
		font_awesome.setPixelSize(resource::get_icon_size());
		painter.setFont(font_awesome);

		const auto& app = main_window::get_app();
		if (app.is_user_colors())
		{
			if (m_icon == fa::envelope)
			{
				const auto client = m_entity->get<client_component>();
				painter.setPen(client->get_color());
			}
			else if (m_icon == fa::hash_tag)
			{
				const auto room = m_entity->get<room_component>();
				if (room->has_color())
					painter.setPen(room->get_color());
			}
		}

		painter.drawText(rect, Qt::AlignVCenter | Qt::AlignHCenter, QString(m_icon));

		painter.end();
	}

	// Menu
	void docked_tab_widget::show_menu(const QPoint& pos)
	{
		assert(thread_info::main());

		QMenu menu;
		menu.setStyleSheet(QString("QMenu::item { height:%1px; width:%2px; }").arg(resource::get_row_height()).arg(resource::get_menu_width()));
		menu.addAction(m_close_action);
		menu.exec(mapToGlobal(pos));
	}

	// Set
	void docked_tab_widget::set_text(const QString& text)
	{
		m_text = text;

		QFontMetrics metrics(m_font);
		m_elided_text = metrics.elidedText(text, Qt::ElideRight, resource::get_row_height() * 5);
	}
}
