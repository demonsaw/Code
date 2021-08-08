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
#include <QFont>
#include <QIcon>

#include "entity/entity.h"
#include "entity/entity_widget.h"
#include "resource/icon_provider.h"
#include "resource/resource.h"

namespace eja
{
	// Constructor
	entity_widget::entity_widget(QWidget* parent /*= nullptr*/) : QWidget(parent)
	{
		// Font
		auto font = QApplication::font();
		font.setFamily(font_family::main);
		setFont(font);

		// Signal
		connect(this, static_cast<void (entity_widget::*)()>(&entity_widget::create), this, static_cast<void (entity_widget::*)()>(&entity_widget::on_create));
		connect(this, static_cast<void (entity_widget::*)(const entity::ptr entity)>(&entity_widget::create), this, static_cast<void (entity_widget::*)(const entity::ptr entity)>(&entity_widget::on_create));

		connect(this, static_cast<void (entity_widget::*)()>(&entity_widget::destroy), this, static_cast<void (entity_widget::*)()>(&entity_widget::on_destroy));
		connect(this, static_cast<void (entity_widget::*)(const entity::ptr entity)>(&entity_widget::destroy), this, static_cast<void (entity_widget::*)(const entity::ptr entity)>(&entity_widget::on_destroy));

		connect(this, static_cast<void (entity_widget::*)()>(&entity_widget::add), this, static_cast<void (entity_widget::*)()>(&entity_widget::on_add));
		connect(this, static_cast<void (entity_widget::*)(const entity::ptr entity)>(&entity_widget::add), this, static_cast<void (entity_widget::*)(const entity::ptr entity)>(&entity_widget::on_add));

		connect(this, static_cast<void (entity_widget::*)()>(&entity_widget::remove), this, static_cast<void (entity_widget::*)()>(&entity_widget::on_remove));
		connect(this, static_cast<void (entity_widget::*)(const entity::ptr entity)>(&entity_widget::remove), this, static_cast<void (entity_widget::*)(const entity::ptr entity)>(&entity_widget::on_remove));

		connect(this, static_cast<void (entity_widget::*)()>(&entity_widget::update), this, static_cast<void (entity_widget::*)()>(&entity_widget::on_update));
		connect(this, static_cast<void (entity_widget::*)(const entity::ptr entity)>(&entity_widget::update), this, static_cast<void (entity_widget::*)(const entity::ptr entity)>(&entity_widget::on_update));

		connect(this, static_cast<void (entity_widget::*)()>(&entity_widget::clear), this, static_cast<void (entity_widget::*)()>(&entity_widget::on_clear));
		connect(this, static_cast<void (entity_widget::*)(const entity::ptr entity)>(&entity_widget::clear), this, static_cast<void (entity_widget::*)(const entity::ptr entity)>(&entity_widget::on_clear));

		connect(this, static_cast<void (entity_widget::*)(const QModelIndex& index)>(&entity_widget::click), this, static_cast<void (entity_widget::*)(const QModelIndex& index)>(&entity_widget::on_click));
		connect(this, static_cast<void (entity_widget::*)(const QModelIndex& index)>(&entity_widget::double_click), this, static_cast<void (entity_widget::*)(const QModelIndex& index)>(&entity_widget::on_double_click));
		connect(this, static_cast<void (entity_widget::*)(const QModelIndex& index)>(&entity_widget::execute), this, static_cast<void (entity_widget::*)(const QModelIndex& index)>(&entity_widget::on_execute));
	}

	entity_widget::entity_widget(const entity::ptr& entity, QWidget* parent /*= nullptr*/) : entity_widget(parent)
	{ 
		set_entity(entity);
	}

	// Utility
	QAction* entity_widget::make_action(const char* text, const char* icon, const char* tooltip /*= nullptr*/)
	{
		auto action = new QAction(text, this);
		action->setIcon(QIcon(icon));
		action->setText(text);

		if (tooltip)
			action->setToolTip(tooltip);

		return action;
	}

	QAction* entity_widget::make_action(const char* text, const int font)
	{
		auto action = new QAction(text, this);
		const auto icon = icon_provider::get_icon(resource::get_icon_size(), font);
		action->setIcon(icon);
		action->setText(text);

		return action;
	}
}