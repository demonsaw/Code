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

#ifndef _EJA_FONT_TRIGGER_H_
#define _EJA_FONT_TRIGGER_H_

#include <QPushButton>
#include <QColor>

#include "entity/entity_type.h"

class QString;

namespace eja
{
	class entity;
	class ui_component;

	class font_trigger final : public QPushButton, public entity_type
	{
	public:
		typedef bool(ui_component::*has_function)() const;
		typedef void(ui_component::*set_function)(const bool);

	private:
		int m_font;
		has_function m_has = nullptr;
		set_function m_set = nullptr;

		// Utility
		void on_action(bool value);

	public:
		font_trigger(const std::shared_ptr<entity> entity, const int font, QWidget* parent = 0);
		font_trigger(const std::shared_ptr<entity> entity, const int font, const QString& text, QWidget* parent = 0);

		// Has
		bool has_font() const { return m_font != 0; }
		bool has_functions() const { return (m_has != nullptr) && (m_set != nullptr); }
		bool has_value() const;

		// Set
		void set_font(const int font) { m_font = font; }
		void set_functions(has_function has, set_function set);
		void set_value(const bool& value);

		// get
		int get_font() const { return m_font; }
	};
}

#endif
