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

#ifndef _EJA_ENTITY_ACTION_H_
#define _EJA_ENTITY_ACTION_H_

#include <memory>
#include <string>

#include <QAction>
#include <QString>

#include "entity/entity_type.h"
#include "font/font_awesome.h"
#include "system/type.h"

class QColor;
class QIcon;

namespace eja
{
	class entity;

	class entity_action : public QAction, public entity_type
	{
	private:
		// HACK: We have to store this because Qt is stupid
		QAction* m_action = nullptr;

	public:
		entity_action(const std::shared_ptr<entity> entity, QWidget* parent = 0);
		entity_action(const std::shared_ptr<entity> entity, const QString& text, QWidget* parent = 0);
		entity_action(const std::shared_ptr<entity> entity, const QIcon& icon, const QString& text, QWidget* parent = 0);

		// Set
		void set_action(QAction* action) { m_action = action; }

		void set_color();
		void set_color(const QColor& color) { set_color(color.red(), color.green(), color.blue()); }
		void set_color(int red, int green, int blue);
		void set_color(const u32 rgba);

		void set_text(const char* text);
		void set_text(const std::string& text);
		void set_text(const QString& text);

		// Get
		QAction* get_action() const { return m_action; }
	};
}

#endif
