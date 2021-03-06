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

#include <QLabel>
#include <QToolBar>
#include <QTableView>
#include <QVBoxLayout>

#include "component/callback/callback_type.h"
#include "entity/entity.h"
#include "font/font_awesome.h"
#include "widget/empty_widget.h"

namespace eja
{
	// Constructor
	empty_widget::empty_widget(const entity::ptr entity, const QString& title, const int fa, QWidget* parent /*= 0*/) : entity_widget(entity, title, fa, callback_type::none, parent)
	{
		assert(thread_info::main());

		// Menu
		add_button("Popout", fa::clone, [&]() { popout(); });
		add_button("Close", fa::close, [&]() { close(); });

		auto hlayout = new QHBoxLayout;
		hlayout->setSpacing(0);
		hlayout->setMargin(0);

		hlayout->addSpacing(4);
		hlayout->addWidget(m_title);
		hlayout->addStretch(1);
		hlayout->addWidget(m_toolbar);

		m_titlebar = new QWidget(this);
		m_titlebar->installEventFilter(this);
		m_titlebar->setObjectName("menubar");
		m_titlebar->setLayout(hlayout);
		setTitleBarWidget(m_titlebar);

		setWidget(new QTableView);
	}
}
