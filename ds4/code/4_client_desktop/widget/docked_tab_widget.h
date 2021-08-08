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

#ifndef _EJA_DOCKED_TAB_WIDGET_H_
#define _EJA_DOCKED_TAB_WIDGET_H_

#include <QFont>
#include <QPaintEvent>
#include <QString>

#include "entity/entity_widget.h"
#include "font/font_awesome.h"
#include "system/type.h"

class QAction;
class QPoint;
class QWidget;

namespace eja
{
	class entity_dock_widget;

	class docked_tab_widget final : public entity_widget
	{
		Q_OBJECT;

	private:
		int m_icon;
		QFont m_font;
		QString m_text;
		QString m_elided_text;
		QAction* m_close_action = nullptr;

	private:
		// Utility
		void on_close();

		// Event
		virtual bool eventFilter(QObject* object, QEvent* event) override;
		virtual void paintEvent(QPaintEvent *event) override;

	private slots:
		// Menu
		void show_menu(const QPoint& point);

	public:
		docked_tab_widget(const QString& text, QWidget* parent = nullptr);
		docked_tab_widget(const int icon, const QString& text, QWidget* parent = nullptr) : docked_tab_widget(text, parent) { m_icon = icon; }
		docked_tab_widget(const std::shared_ptr<entity>& entity, const int icon, QString& text, QWidget* parent = nullptr) : docked_tab_widget(icon, text, parent) { set_entity(entity); }

		// Has
		bool has_entity() const { return m_entity != nullptr; }

		// Set
		void set_entity(std::shared_ptr<entity> entity) { m_entity = entity; }
		void set_icon(const int icon) { m_icon = icon; }
		void set_text(const QString& text);

		// Get
		int get_icon() const { return m_icon; }
		const QString& get_text() const { return m_text; }
	};
}

#endif
