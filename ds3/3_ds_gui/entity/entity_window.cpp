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

#include <QDockWidget>
#include <QTabBar>
#include <QToolBar>

#include "dialog/about_dialog.h"
#include "entity/entity.h"
#include "entity/entity_window.h"
#include "font/font_button.h"
#include "font/font_trigger.h"
#include "utility/value.h"
#include "widget/empty_widget.h"

namespace eja
{
	// Constructor
	entity_window::entity_window(const entity::ptr entity, const callback_type type /*= callback_type::none*/, QWidget* parent /*= 0*/, Qt::WindowFlags flags /*= 0*/) : entity_callback(entity, type), QMainWindow(parent, flags)
	{
		setTabPosition(Qt::DockWidgetArea::BottomDockWidgetArea, QTabWidget::TabPosition::South);

		// Menu
		m_toolbar = new QToolBar(this);
		m_toolbar->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextOnly);
		m_toolbar->setFont(QFont(demonsaw::font_awesome));
		m_toolbar->setObjectName("titlebar");

		// Spacer
		QWidget* spacer = new QWidget(m_toolbar);
		spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		m_toolbar->addWidget(spacer);

		// Signal
		connect(this, static_cast<void (entity_window::*)(const entity::ptr)>(&entity_window::create), this, static_cast<void (entity_window::*)(const entity::ptr)>(&entity_window::on_create));		
		connect(this, static_cast<void (entity_window::*)(const entity::ptr)>(&entity_window::destroy), this, static_cast<void (entity_window::*)(const entity::ptr)>(&entity_window::on_destroy));

		connect(this, static_cast<void (entity_window::*)(const entity::ptr)>(&entity_window::init), this, static_cast<void (entity_window::*)(const entity::ptr)>(&entity_window::on_init));
		connect(this, static_cast<void (entity_window::*)(const entity::ptr)>(&entity_window::update), this, static_cast<void (entity_window::*)(const entity::ptr)>(&entity_window::on_update));
		connect(this, static_cast<void (entity_window::*)(const entity::ptr)>(&entity_window::shutdown), this, static_cast<void (entity_window::*)(const entity::ptr)>(&entity_window::on_shutdown));

		connect(this, static_cast<void (entity_window::*)(const entity::ptr)>(&entity_window::add), this, static_cast<void (entity_window::*)(const entity::ptr)>(&entity_window::on_add));
		connect(this, static_cast<void (entity_window::*)(const entity::ptr)>(&entity_window::remove), this, static_cast<void (entity_window::*)(const entity::ptr)>(&entity_window::on_remove));		
		connect(this, static_cast<void (entity_window::*)(const entity::ptr)>(&entity_window::clear), this, static_cast<void (entity_window::*)(const entity::ptr)>(&entity_window::on_clear));

		connect(this, static_cast<void (entity_window::*)(const entity::ptr)>(&entity_window::start), this, static_cast<void (entity_window::*)(const entity::ptr)>(&entity_window::on_start));
		connect(this, static_cast<void (entity_window::*)(const entity::ptr)>(&entity_window::stop), this, static_cast<void (entity_window::*)(const entity::ptr)>(&entity_window::on_stop));
		connect(this, static_cast<void (entity_window::*)(const entity::ptr)>(&entity_window::restart), this, static_cast<void (entity_window::*)(const entity::ptr)>(&entity_window::on_restart));
		connect(this, static_cast<void (entity_window::*)(const entity::ptr)>(&entity_window::reset), this, static_cast<void (entity_window::*)(const entity::ptr)>(&entity_window::on_reset));
	}

	// Utility
	void entity_window::about()
	{
		about_dialog about(this);
		about.exec();
	}

	void entity_window::add_about()
	{
		// Button
		auto button = new QPushButton(this);
		button->setObjectName("about");
		button->setToolTip("About");

		// Toolbar
		m_toolbar->addWidget(button);

		// Signal
		connect(button, &QPushButton::clicked, this, &entity_window::about);
	}

	font_button* entity_window::make_button(const char* tooltip, const int font)
	{
		auto button = new font_button(font, this);
		button->setObjectName("titlebar");
		button->setToolTip(tooltip);

		return button;
	}

	font_trigger* entity_window::make_trigger(const char* tooltip, const int font)
	{
		auto trigger = new font_trigger(m_entity, font, this);
		trigger->setObjectName("titlebar");
		trigger->setFont(QFont(demonsaw::font_awesome));
		trigger->setText(QString(font));
		trigger->setToolTip(tooltip);
		trigger->setCheckable(true);

		return trigger;
	}

	// Slot
	void entity_window::on_dock_location_changed(Qt::DockWidgetArea area)
	{
		// HACK: Removes the ugly black line below the QTabBar
		const auto tabbars = findChildren<QTabBar*>();
		for (auto& tabbar : tabbars)
			tabbar->setDrawBase(false);
	}
}
