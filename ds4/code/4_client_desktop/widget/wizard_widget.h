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

#ifndef _EJA_WIZARD_WIDGET_H_
#define _EJA_WIZARD_WIDGET_H_

#include <QCheckBox>
#include <QColor>
#include <QLineEdit>
#include <QPixmap>
#include <QTextEdit>
#include <QWidget>

#include "system/type.h"

class QEvent;
class QLabel;
class QPaintEvent;
class QPushButton;

namespace eja
{
	class wizard_widget final : public QWidget
	{
		Q_OBJECT;

	private:
		QPixmap m_background;

		// Color
		QColor m_color;
		bool m_user_color = true;

		QLabel* m_info = nullptr;
		bool m_user_modified = false;

		// Checkbox
		QCheckBox* m_start_checkbox = nullptr;

		// Field
		QLineEdit* m_address = nullptr;
		QLineEdit* m_name = nullptr;
		QLineEdit* m_port = nullptr;
		QTextEdit* m_password = nullptr;

		// Button
		QPushButton* m_start_button = nullptr;
		QPushButton* m_name_color = nullptr;
		QPushButton* m_random = nullptr;

	private:
		// Event
		virtual bool eventFilter(QObject* watched, QEvent* event) override;

		// Paint
		virtual void paintEvent(QPaintEvent* event) override;

		// Get
		QColor get_font_color(const QColor& background);
		QString get_random_name() const;

		// Set
		void set_color();
		void set_random_color();

	signals:
		void done();

	public:
		wizard_widget(QWidget* parent = nullptr);
		void activate();

		// Utility
		bool enabled() const { return m_start_checkbox->isChecked(); }
		bool disabled() const { return !enabled(); }

		// Get
		u32 get_color() const { return m_color.rgba(); }
		std::string get_name() const { return m_name->text().simplified().toStdString(); }
		std::string get_address() const { return m_address->text().simplified().toStdString(); }		
		std::string get_password() const { return m_password->toPlainText().toStdString(); }
		u16 get_port() const { return m_port->text().toUShort(); }
	};
}

#endif
