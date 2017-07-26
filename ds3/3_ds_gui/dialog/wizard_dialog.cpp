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

#include <QApplication>
#include <QEvent>
#include <QFile>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

#include "dialog/wizard_dialog.h"
#include "utility/value.h"

namespace eja
{
	wizard_dialog::wizard_dialog(QWidget* parent /*= 0*/) : QDialog(parent)
	{
		setObjectName("wizard");
		setFixedSize(QSize(480, 320));

		// Widget
		m_stack = new QStackedWidget(this);
		m_stack->setObjectName("wizard");

		// Welcome
		m_welcome = new wizard_welcome_pane(this);		
		m_stack->addWidget(m_welcome);

		// Client
		m_client = new wizard_client_pane(this);
		m_stack->addWidget(m_client);

		// Router
		m_router = new wizard_router_pane(this);
		m_stack->addWidget(m_router);

		// Layout
		auto vlayout = new QVBoxLayout;
		vlayout->setContentsMargins(0, 0, 0, 0);
		vlayout->setSpacing(0);
		vlayout->setMargin(16);

		vlayout->addWidget(m_stack);
		setLayout(vlayout);
	}

	// Utility
	void wizard_dialog::back()
	{
		switch (m_stack->currentIndex())
		{
			case 1:
			case 2:
			{
				m_stack->setCurrentIndex(0);
				break;
			}
			default:
			{
				assert(false);
				break;
			}
		}
	}

	void wizard_dialog::next()
	{
		switch (m_stack->currentIndex())
		{
			case 0:
			{
				if (m_welcome->is_client())
					m_stack->setCurrentIndex(1);
				else if (m_welcome->is_router())
					m_stack->setCurrentIndex(2);

				break;
			}
			default:
			{
				done();
				break;
			}
		}		
	}

	void wizard_dialog::quick()
	{
		m_status = wizard_status::quick;
		close();
	}

	void wizard_dialog::done()
	{
		m_status = wizard_status::done;
		close();
	}
}
