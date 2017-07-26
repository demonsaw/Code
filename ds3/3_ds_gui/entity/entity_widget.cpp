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
#include <QContextMenuEvent>
#include <QEvent>
#include <QFont>
#include <QLabel>
#include <QMenu>
#include <QString>
#include <QToolBar>
#include <QToolTip>

#include "component/endpoint_component.h"
#include "entity/entity.h"
#include "entity/entity_widget.h"
#include "font/font_button.h"
#include "font/font_trigger.h"
#include "font/font_awesome.h"
#include "resource/icon_provider.h"
#include "thread/thread_info.h"
#include "utility/value.h"
#include "window/main_window.h"

namespace eja
{
	// Constructor
	entity_widget::entity_widget(const QString& title, const int fa, const callback_type type /*= callback_type::none*/, QWidget* parent /*= 0*/) : entity_callback(type), QDockWidget(parent)
	{
		// Title
		m_title_text = title;
		m_title = new QLabel(title, this);
		m_title->setObjectName("menubar");

		const auto icon_title = QString(fa);
		setWindowTitle(icon_title);

		// Icon
		m_icon = new QLabel(this);
		m_icon->setText(icon_title);
		m_icon->setObjectName("menubar_icon");
		m_icon->setFont(QFont(demonsaw::font_awesome));
		m_icon->setToolTip(title);

		// Menu
		m_toolbar = new QToolBar(this);
		m_toolbar->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextOnly);
		m_toolbar->setFont(QFont(demonsaw::font_awesome));
		m_toolbar->setObjectName("menubar");

		// Spacer
		QWidget* spacer = new QWidget(m_toolbar);
		spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		m_toolbar->addWidget(spacer);

		// Signal
		connect(this, &entity_widget::set_enabled, this, &entity_widget::on_set_enabled);

		connect(this, static_cast<void (entity_widget::*)()>(&entity_widget::create), this, static_cast<void (entity_widget::*)()>(&entity_widget::on_create));
		connect(this, static_cast<void (entity_widget::*)(const entity::ptr)>(&entity_widget::create), this, static_cast<void (entity_widget::*)(const entity::ptr)>(&entity_widget::on_create));

		connect(this, static_cast<void (entity_widget::*)()>(&entity_widget::destroy), this, static_cast<void (entity_widget::*)()>(&entity_widget::on_destroy));
		connect(this, static_cast<void (entity_widget::*)(const entity::ptr)>(&entity_widget::destroy), this, static_cast<void (entity_widget::*)(const entity::ptr)>(&entity_widget::on_destroy));

		connect(this, static_cast<void (entity_widget::*)()>(&entity_widget::init), this, static_cast<void (entity_widget::*)()>(&entity_widget::on_init));
		connect(this, static_cast<void (entity_widget::*)(const entity::ptr)>(&entity_widget::init), this, static_cast<void (entity_widget::*)(const entity::ptr)>(&entity_widget::on_init));

		connect(this, static_cast<void (entity_widget::*)()>(&entity_widget::update), this, static_cast<void (entity_widget::*)()>(&entity_widget::on_update));
		connect(this, static_cast<void (entity_widget::*)(const entity::ptr)>(&entity_widget::update), this, static_cast<void (entity_widget::*)(const entity::ptr)>(&entity_widget::on_update));

		connect(this, static_cast<void (entity_widget::*)()>(&entity_widget::shutdown), this, static_cast<void (entity_widget::*)()>(&entity_widget::on_shutdown));
		connect(this, static_cast<void (entity_widget::*)(const entity::ptr)>(&entity_widget::shutdown), this, static_cast<void (entity_widget::*)(const entity::ptr)>(&entity_widget::on_shutdown));

		connect(this, static_cast<void (entity_widget::*)()>(&entity_widget::add), this, static_cast<void (entity_widget::*)()>(&entity_widget::on_add));
		connect(this, static_cast<void (entity_widget::*)(const entity::ptr)>(&entity_widget::add), this, static_cast<void (entity_widget::*)(const entity::ptr)>(&entity_widget::on_add));

		connect(this, static_cast<void (entity_widget::*)()>(&entity_widget::remove), this, static_cast<void (entity_widget::*)()>(&entity_widget::on_remove));
		connect(this, static_cast<void (entity_widget::*)(const entity::ptr)>(&entity_widget::remove), this, static_cast<void (entity_widget::*)(const entity::ptr)>(&entity_widget::on_remove));

		connect(this, static_cast<void (entity_widget::*)()>(&entity_widget::clear), this, static_cast<void (entity_widget::*)()>(&entity_widget::on_clear));
		connect(this, static_cast<void (entity_widget::*)(const entity::ptr)>(&entity_widget::clear), this, static_cast<void (entity_widget::*)(const entity::ptr)>(&entity_widget::on_clear));
	}

	entity_widget::entity_widget(const entity::ptr entity, const QString& title, const int fa, const callback_type type /*= callback_type::none*/, QWidget* parent /*= 0*/) : entity_widget(title, fa, type, parent)
	{
		set_entity(entity);
	}

	// Utility
	void entity_widget::popout()
	{
		setFloating(!isFloating());
		auto button = get_button(fa::clone);
		if (button)
			button->setAttribute(Qt::WA_UnderMouse, false);
	}

	QAction* entity_widget::make_action(const char* text, const char* tooltip /*= nullptr*/)
	{
		auto action = new QAction(text, this);

		if (tooltip)
			action->setToolTip(tooltip);

		return action;
	}

	QAction* entity_widget::make_action(const char* text, const char* icon, const char* tooltip /*= nullptr*/)
	{
		auto action = new QAction(text, this);
		action->setIcon(QIcon(icon));
		
		if (tooltip)
			action->setToolTip(tooltip);

		return action;
	}

	QAction* entity_widget::make_action(const char* text, const int font, const char* tooltip /*= nullptr*/, const size_t size /*= 14*/)
	{
		auto action = new QAction(text, this);
		const auto icon = icon_provider::get_icon(size, font);
		action->setIcon(icon);
		
		if (tooltip)
			action->setToolTip(tooltip);

		return action;
	}

	font_button* entity_widget::make_button(const char* tooltip, const int font)
	{
		auto button = new font_button(font, this);
		button->setObjectName("menubar");
		button->setToolTip(tooltip);

		return button;
	}

	QMenu* entity_widget::make_menu(const char* text, const int font, const size_t size /*= 14*/)
	{
		auto menu = new QMenu(text, this);
		const auto icon = icon_provider::get_icon(size, font);
		menu->setIcon(icon);

		return menu;
	}

	// Event
	void entity_widget::closeEvent(QCloseEvent* event)
	{
		if (m_trigger)
			m_trigger->setChecked(false);
	}

	bool entity_widget::eventFilter(QObject* object, QEvent* event)
	{
		if (event->type() == QEvent::ContextMenu)
		{
			QContextMenuEvent* menu_event = static_cast<QContextMenuEvent *>(event);
			return menu_event->reason() == QContextMenuEvent::Mouse;
		}
		
		return QObject::eventFilter(object, event);
	}

	// Slot
	void entity_widget::on_set_enabled(const int font)
	{
		auto button = get_button(font);
		if (button)
			button->setEnabled(true);
	}

	void entity_widget::on_top_level_changed(bool topLevel)
	{
		if (topLevel)
		{
			const auto endpoint = m_entity->get<endpoint_component>();
			m_title->setText(QString("%1 - %2").arg(m_title_text).arg(endpoint->get_name().c_str()));
		}
		else
		{
			m_title->setText(m_title_text);
		}
	}

	// Has
	bool entity_widget::has_button(const int font) const
	{
		assert(thread_info::main());

		return m_buttons.find(font) != m_buttons.end();
	}

	// Set
	void entity_widget::set_modified(const bool value /*= true*/) const
	{
		const auto window = main_window::get();
		window->set_modified(value);
	}

	// Get
	const font_button* entity_widget::get_button(const int font) const
	{
		assert(thread_info::main());

		const auto it = m_buttons.find(font);
		return (it != m_buttons.end()) ? it->second : nullptr;
	}
}
