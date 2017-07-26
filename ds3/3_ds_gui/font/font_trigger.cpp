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
#include <QString>

#include "component/ui_component.h"
#include "entity/entity.h"
#include "font/font_trigger.h"
#include "utility/value.h"

namespace eja
{
	// Constructor
	font_trigger::font_trigger(const entity::ptr entity, const int font, QWidget* parent /*= 0*/) : entity_type(entity), QPushButton(parent), m_font(font)
	{
		setFont(QFont(demonsaw::font_awesome));
		setText(QString(font));

		connect(this, &QPushButton::clicked, this, &font_trigger::set_value); 
		connect(this, &QPushButton::toggled, this, &font_trigger::set_value);
	}

	font_trigger::font_trigger(const entity::ptr entity, const int font, const QString& text, QWidget* parent /*= 0*/) : entity_type(entity), QPushButton(text, parent), m_font(font)
	{
		setFont(QFont(demonsaw::font_awesome));
		setText(QString(font));

		connect(this, &QPushButton::clicked, this, &font_trigger::set_value);
		connect(this, &QPushButton::toggled, this, &font_trigger::set_value);		
	}

	// Has
	bool font_trigger::has_value() const
	{ 
		if (m_has)
		{
			const auto ui = m_entity->get<ui_component>();
			return ((&(*ui))->*m_has)();
		}

		return false;
	}

	// Set
	void font_trigger::set_functions(has_function has, set_function set)
	{ 
		m_has = has; 
		m_set = set;
	}
	
	void font_trigger::set_value(const bool& value)
	{ 
		if (m_set)
		{
			const auto checked = isChecked();
			const auto ui = m_entity->get<ui_component>();
			((&(*ui))->*m_set)(checked);
		}
	}
}
