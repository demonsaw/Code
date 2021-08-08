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

#include <QAbstractItemView>
#include <QAction>
#include <QApplication>
#include <QColor>
#include <QContextMenuEvent>
#include <QEvent>
#include <QFont>
#include <QHeaderView>
#include <QKeyEvent>
#include <QLabel>
#include <QSortFilterProxyModel>
#include <QString>
#include <QToolBar>
#include <QToolTip>

#include "entity/entity.h"
#include "entity/entity_model.h"
#include "entity/entity_dock_widget.h"
#include "font/font_button.h"
#include "font/font_awesome.h"
#include "resource/icon_provider.h"
#include "thread/thread_info.h"
#include "utility/value.h"

namespace eja
{
	// Constructor
	entity_dock_widget::entity_dock_widget(const QString& title, const int fa, QWidget* parent /*= nullptr*/) : QDockWidget(parent), m_fa(fa)
	{
		// Font
		auto font = QApplication::font();
		font.setFamily(font_family::main);
		font.setPixelSize(resource::get_label_font_size());
		setFont(font);

		// Title
		m_title_text = title;
		m_title = new QLabel(title, this);
		m_title->setFont(font);

		const auto icon_title = QString(fa);
		setWindowTitle(title);

		// Icon Font
		auto icon_font = QFont(software::font_awesome);
		icon_font.setPixelSize(resource::get_large_font_size());

		// Icon
		m_icon = new QLabel(this);
		m_icon->setText(icon_title);
		m_icon->setObjectName("menubar_icon");
		m_icon->setFont(icon_font);
		m_icon->setToolTip(title);

		// Menu
		m_toolbar = new QToolBar(this);
		m_toolbar->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextOnly);
		m_toolbar->setFont(QFont(software::font_awesome));

		// Spacer
		QWidget* spacer = new QWidget(m_toolbar);
		spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		m_toolbar->addWidget(spacer);

		// Object Name for State and Geometry
		setObjectName(title.toLower() + "_dock_widget");

		// Signal
		connect(this, static_cast<void (entity_dock_widget::*)()>(&entity_dock_widget::create), this, static_cast<void (entity_dock_widget::*)()>(&entity_dock_widget::on_create));
		connect(this, static_cast<void (entity_dock_widget::*)(const entity::ptr entity)>(&entity_dock_widget::create), this, static_cast<void (entity_dock_widget::*)(const entity::ptr entity)>(&entity_dock_widget::on_create));

		connect(this, static_cast<void (entity_dock_widget::*)()>(&entity_dock_widget::destroy), this, static_cast<void (entity_dock_widget::*)()>(&entity_dock_widget::on_destroy));
		connect(this, static_cast<void (entity_dock_widget::*)(const entity::ptr entity)>(&entity_dock_widget::destroy), this, static_cast<void (entity_dock_widget::*)(const entity::ptr entity)>(&entity_dock_widget::on_destroy));

		connect(this, static_cast<void (entity_dock_widget::*)()>(&entity_dock_widget::add), this, static_cast<void (entity_dock_widget::*)()>(&entity_dock_widget::on_add));
		connect(this, static_cast<void (entity_dock_widget::*)(const entity::ptr entity)>(&entity_dock_widget::add), this, static_cast<void (entity_dock_widget::*)(const entity::ptr entity)>(&entity_dock_widget::on_add));

		connect(this, static_cast<void (entity_dock_widget::*)()>(&entity_dock_widget::remove), this, static_cast<void (entity_dock_widget::*)()>(&entity_dock_widget::on_remove));
		connect(this, static_cast<void (entity_dock_widget::*)(const entity::ptr entity)>(&entity_dock_widget::remove), this, static_cast<void (entity_dock_widget::*)(const entity::ptr entity)>(&entity_dock_widget::on_remove));

		connect(this, static_cast<void (entity_dock_widget::*)()>(&entity_dock_widget::update), this, static_cast<void (entity_dock_widget::*)()>(&entity_dock_widget::on_update));
		connect(this, static_cast<void (entity_dock_widget::*)(const entity::ptr entity)>(&entity_dock_widget::update), this, static_cast<void (entity_dock_widget::*)(const entity::ptr entity)>(&entity_dock_widget::on_update));

		connect(this, static_cast<void (entity_dock_widget::*)()>(&entity_dock_widget::clear), this, static_cast<void (entity_dock_widget::*)()>(&entity_dock_widget::on_clear));
		connect(this, static_cast<void (entity_dock_widget::*)(const entity::ptr entity)>(&entity_dock_widget::clear), this, static_cast<void (entity_dock_widget::*)(const entity::ptr entity)>(&entity_dock_widget::on_clear));

		connect(this, static_cast<void (entity_dock_widget::*)(const QModelIndex& index)>(&entity_dock_widget::click), this, static_cast<void (entity_dock_widget::*)(const QModelIndex& index)>(&entity_dock_widget::on_click));
		connect(this, static_cast<void (entity_dock_widget::*)(const QModelIndex& index)>(&entity_dock_widget::double_click), this, static_cast<void (entity_dock_widget::*)(const QModelIndex& index)>(&entity_dock_widget::on_double_click));
		connect(this, static_cast<void (entity_dock_widget::*)(const QModelIndex& index)>(&entity_dock_widget::execute), this, static_cast<void (entity_dock_widget::*)(const QModelIndex& index)>(&entity_dock_widget::on_execute));
	}

	entity_dock_widget::entity_dock_widget(const entity::ptr& entity, const QString& title, const int fa, QWidget* parent /*= nullptr*/) : entity_dock_widget(title, fa, parent)
	{
		set_entity(entity);
	}

	// Interface
	void entity_dock_widget::init(QAbstractItemView* view, entity_model* model)
	{
		view->setModel(model);
	}

	void entity_dock_widget::init_ex(QAbstractItemView* view, entity_model* model)
	{
		QSortFilterProxyModel* proxy = new QSortFilterProxyModel(this);
		proxy->setSortCaseSensitivity(Qt::CaseInsensitive);
		proxy->setSortRole(Qt::UserRole);
		proxy->setSourceModel(model);
		view->setModel(proxy);
	}

	// Utility
	void entity_dock_widget::popout()
	{
		setFloating(!isFloating());
		auto button = get_button(fa::clone);
		if (button)
			button->setAttribute(Qt::WA_UnderMouse, false);
	}

	QAction* entity_dock_widget::make_action(const char* text, const char* icon, const char* tooltip /*= nullptr*/)
	{
		auto action = new QAction(text, this);
		action->setIcon(QIcon(icon));
		action->setText(text);

		if (tooltip)
			action->setToolTip(tooltip);

		return action;
	}

	QAction* entity_dock_widget::make_action(const char* text, const int fa)
	{
		auto action = new QAction(text, this);
		const auto icon = icon_provider::get_icon(resource::get_icon_size(), fa);
		action->setIcon(icon);
		action->setText(text);

		return action;
	}

	font_button* entity_dock_widget::make_button(const char* tooltip, const int icon)
	{
		auto button = new font_button(icon, this);
		button->setObjectName("menubar");
		button->setToolTip(tooltip);

		return button;
	}

	QMenu* entity_dock_widget::make_menu(const char* text, const int fa)
	{
		auto menu = new QMenu(text, this);
		const auto icon = icon_provider::get_icon(resource::get_icon_size(), fa);
		menu->setIcon(icon);

		return menu;
	}

	// Event
	bool entity_dock_widget::eventFilter(QObject* object, QEvent* event)
	{
		if (event->type() == QEvent::ContextMenu)
		{
			QContextMenuEvent* menu_event = static_cast<QContextMenuEvent *>(event);
			return menu_event->reason() == QContextMenuEvent::Mouse;
		}
		else if (event->type() == QEvent::KeyPress)
		{
			const auto ke = static_cast<QKeyEvent*>(event);

			switch (ke->key())
			{
				case Qt::Key_Escape:
				{
					close();
					break;
				}
			}
		}

		return QObject::eventFilter(object, event);
	}

	// Slot
	void entity_dock_widget::on_top_level_changed(bool topLevel)
	{
		if (topLevel)
		{
			//const auto endpoint = m_entity->get<endpoint_component>();
			//m_title->setText(QString("%1 - %2").arg(m_title_text).arg(endpoint->get_name().c_str()));
		}
		else
		{
			m_title->setText(m_title_text);
		}
	}

	// Has
	bool entity_dock_widget::has_button(const int font) const
	{
		assert(thread_info::main());

		return m_buttons.find(font) != m_buttons.end();
	}

	// Get
	const font_button* entity_dock_widget::get_button(const int font) const
	{
		assert(thread_info::main());

		const auto it = m_buttons.find(font);
		return (it != m_buttons.end()) ? it->second : nullptr;
	}
}
