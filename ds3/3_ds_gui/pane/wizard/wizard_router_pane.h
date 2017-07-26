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

#ifndef _EJA_WIZARD_ROUTER_PANE_H_
#define _EJA_WIZARD_ROUTER_PANE_H_

#include <QColor>
#include <QLineEdit>
#include <QWidget>

#include "system/type.h"

class QLabel;
class QPushButton;

namespace eja
{
	class wizard_dialog;

	class wizard_router_pane final : public QWidget
	{
	private:
		wizard_dialog* m_dialog = nullptr;

		QColor m_color;
		bool m_user_color = true;
		bool m_user_modified = false;

		QLineEdit* m_name = nullptr;
		QPushButton* m_name_color = nullptr;
		QPushButton* m_name_default = nullptr;

		QLineEdit* m_address = nullptr;
		QLineEdit* m_port = nullptr;
		QLabel* m_info = nullptr;

		QPushButton* m_back = nullptr;
		QPushButton* m_done = nullptr;

	private:
		virtual void showEvent(QShowEvent* event) override;

		// Set
		void set_color();

	public:
		wizard_router_pane(QWidget* parent = 0);

		u32 get_color() const { return m_color.rgba(); }
		std::string get_name() const { return m_name->text().toStdString(); }
		std::string get_address() const { return m_address->text().toStdString(); }
		u16 get_port() const { return m_port->text().toUShort(); }
	};
}

#endif
