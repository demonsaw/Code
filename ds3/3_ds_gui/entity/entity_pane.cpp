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
#include <QEvent>
#include <QFont>
#include <QLabel>
#include <QMenu>
#include <QToolBar>
#include <QString>

#include "entity/entity.h"
#include "entity/entity_pane.h"
#include "font/font_awesome.h"
#include "resource/icon_provider.h"
#include "utility/value.h"

namespace eja
{
	// Constructor
	entity_pane::entity_pane(const QString& title, const int fa, const callback_type type /*= callback_type::none*/, QWidget* parent /*= 0*/) : entity_callback(type), QWidget(parent)
	{
		// Title
		m_title = new QLabel(title, this);
		m_title->setObjectName("pane");

		const auto icon_title = QString(fa);
		setWindowTitle(icon_title);

		// Icon
		m_icon = new QLabel(this);
		m_icon->setText(icon_title);
		m_icon->setObjectName("pane_icon");
		m_icon->setFont(QFont(demonsaw::font_awesome));

		// Menu
		m_toolbar = new QToolBar(this);
		m_toolbar->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextOnly);
		m_toolbar->setFont(QFont(demonsaw::font_awesome));
		m_toolbar->setObjectName("pane");

		// Spacer
		QWidget* spacer = new QWidget(m_toolbar);
		spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		m_toolbar->addWidget(spacer);

		// Signal
		connect(this, &entity_pane::set_enabled, this, &entity_pane::on_set_enabled);

		connect(this, static_cast<void (entity_pane::*)()>(&entity_pane::create), this, static_cast<void (entity_pane::*)()>(&entity_pane::on_create));
		connect(this, static_cast<void (entity_pane::*)(const entity::ptr)>(&entity_pane::create), this, static_cast<void (entity_pane::*)(const entity::ptr)>(&entity_pane::on_create));

		connect(this, static_cast<void (entity_pane::*)()>(&entity_pane::destroy), this, static_cast<void (entity_pane::*)()>(&entity_pane::on_destroy));
		connect(this, static_cast<void (entity_pane::*)(const entity::ptr)>(&entity_pane::destroy), this, static_cast<void (entity_pane::*)(const entity::ptr)>(&entity_pane::on_destroy));

		connect(this, static_cast<void (entity_pane::*)()>(&entity_pane::init), this, static_cast<void (entity_pane::*)()>(&entity_pane::on_init));
		connect(this, static_cast<void (entity_pane::*)(const entity::ptr)>(&entity_pane::init), this, static_cast<void (entity_pane::*)(const entity::ptr)>(&entity_pane::on_init));

		connect(this, static_cast<void (entity_pane::*)()>(&entity_pane::update), this, static_cast<void (entity_pane::*)()>(&entity_pane::on_update));
		connect(this, static_cast<void (entity_pane::*)(const entity::ptr)>(&entity_pane::update), this, static_cast<void (entity_pane::*)(const entity::ptr)>(&entity_pane::on_update));

		connect(this, static_cast<void (entity_pane::*)()>(&entity_pane::shutdown), this, static_cast<void (entity_pane::*)()>(&entity_pane::on_shutdown));
		connect(this, static_cast<void (entity_pane::*)(const entity::ptr)>(&entity_pane::shutdown), this, static_cast<void (entity_pane::*)(const entity::ptr)>(&entity_pane::on_shutdown));

		connect(this, static_cast<void (entity_pane::*)()>(&entity_pane::add), this, static_cast<void (entity_pane::*)()>(&entity_pane::on_add));
		connect(this, static_cast<void (entity_pane::*)(const entity::ptr)>(&entity_pane::add), this, static_cast<void (entity_pane::*)(const entity::ptr)>(&entity_pane::on_add));

		connect(this, static_cast<void (entity_pane::*)()>(&entity_pane::remove), this, static_cast<void (entity_pane::*)()>(&entity_pane::on_remove));
		connect(this, static_cast<void (entity_pane::*)(const entity::ptr)>(&entity_pane::remove), this, static_cast<void (entity_pane::*)(const entity::ptr)>(&entity_pane::on_remove));

		connect(this, static_cast<void (entity_pane::*)()>(&entity_pane::clear), this, static_cast<void (entity_pane::*)()>(&entity_pane::on_clear));
		connect(this, static_cast<void (entity_pane::*)(const entity::ptr)>(&entity_pane::clear), this, static_cast<void (entity_pane::*)(const entity::ptr)>(&entity_pane::on_clear));
	}

	entity_pane::entity_pane(const entity::ptr entity, const QString& title, const int fa, const callback_type type /*= callback_type::none*/, QWidget* parent /*= 0*/) : entity_pane(title, fa, type, parent)
	{ 
		set_entity(entity);
	}

	// Utility
	QAction* entity_pane::make_action(const char* text, const char* tooltip /*= nullptr*/)
	{
		auto action = new QAction(text, this);

		if (tooltip)
			action->setToolTip(tooltip);

		return action;
	}

	QAction* entity_pane::make_action(const char* text, const char* icon, const char* tooltip /*= nullptr*/)
	{
		auto action = new QAction(text, this);
		action->setIcon(QIcon(icon));
		action->setText(text);

		if (tooltip)
			action->setToolTip(tooltip);

		return action;
	}
	QAction* entity_pane::make_action(const char* text, const int font, const char* tooltip /*= nullptr*/, const size_t size /*= 14*/)
	{
		auto action = new QAction(text, this);
		const auto icon = icon_provider::get_icon(size, font);
		action->setIcon(icon);
		action->setText(text);
		
		if (tooltip)
			action->setToolTip(tooltip);

		return action;
	}

	font_button* entity_pane::make_button(const char* tooltip, const int font)
	{
		auto button = new font_button(font, this);
		button->setObjectName("pane");
		button->setToolTip(tooltip);

		return button;
	}

	QMenu* entity_pane::make_menu(const char* text, const int font, const size_t size /*= 14*/)
	{
		auto menu = new QMenu(text, this);
		const auto icon = icon_provider::get_icon(size, font);
		menu->setIcon(icon);

		return menu;
	}

	// Slot
	void entity_pane::on_set_enabled(const int font)
	{
		auto button = get_button(font);
		if (button)
			button->setEnabled(true);
	}

	// Get
	const font_button* entity_pane::get_button(const int font) const
	{
		assert(thread_info::main());

		const auto it = m_buttons.find(font);
		return (it != m_buttons.end()) ? it->second : nullptr;
	}
}