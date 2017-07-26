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

#include <QBrush>
#include <QColor>
#include <QIcon>
#include <QPainter>
#include <QPixmap>

#include "entity/entity.h"
#include "entity/entity_action.h"
#include "resource/icon_provider.h"

namespace eja
{
	// Constructor
	entity_action::entity_action(const entity::ptr entity, QWidget* parent /*= 0*/) : entity_type(entity), QAction(parent) { }
	entity_action::entity_action(const entity::ptr entity, const QString& text, QWidget* parent /*= 0*/) : entity_type(entity), QAction(text, parent) { }
	entity_action::entity_action(const entity::ptr entity, const QIcon& icon, const QString& text, QWidget* parent /*= 0*/) : entity_type(entity), QAction(icon, text, parent) { }

	// Set
	void entity_action::set_color()
	{
		const auto icon = icon_provider::get_logo(32, m_entity);
		setIcon(icon);
	}

	void entity_action::set_color(const int red, const int green, const int blue)
	{
		const auto icon = icon_provider::get_logo(32, red, green, blue);
		setIcon(icon);
	}

	void entity_action::set_color(const u32 rgba)
	{
		QColor color;
		color.setRgba(rgba);
		set_color(color);
	}

	void entity_action::set_text(const char* text)
	{ 
		setText(text); 
		setIconText(text);
		
		if (m_action)
		{
			m_action->setText(text);
			m_action->setIconText(text);
		}		
	}

	void entity_action::set_text(const std::string& text)
	{
		const auto qtext = QString::fromStdString(text);

		setText(qtext);
		setIconText(qtext);

		if (m_action)
		{
			m_action->setText(qtext);
			m_action->setIconText(qtext);
		}
	}
	
	void entity_action::set_text(const QString& text)
	{ 
		setText(text);
		setIconText(text);

		if (m_action)
		{
			m_action->setText(text);
			m_action->setIconText(text);
		}
	}
}
