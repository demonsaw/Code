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
#include <QHBoxLayout>
#include <QToolBar>

#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "entity/entity.h"
#include "entity/entity_table_view.h"
#include "font/font_awesome.h"
#include "model/router/router_download_model.h"
#include "widget/router/router_download_widget.h"

namespace eja
{
	// Constructor
	router_download_widget::router_download_widget(const entity::ptr entity, QWidget* parent /*= 0*/) : entity_widget(entity, "Downloads", fa::arrowdown, callback_type::download, parent)
	{
		assert(thread_info::main());

		// Menu
		add_button("Update", fa::bolt, [&]() { m_table->update(); });
		add_button("Popout", fa::clone, [&]() { popout(); });
		add_button("Close", fa::close, [&]() { close(); });

		// Layout
		auto hlayout = new QHBoxLayout;
		hlayout->setSpacing(0);
		hlayout->setMargin(0);

		hlayout->addWidget(m_icon);
		hlayout->addWidget(m_title);
		hlayout->addStretch(1);
		hlayout->addWidget(m_toolbar);

		m_titlebar = new QWidget(this);
		m_titlebar->installEventFilter(this);
		m_titlebar->setObjectName("menubar");
		m_titlebar->setLayout(hlayout);
		setTitleBarWidget(m_titlebar);

		// Table
		m_table = new entity_table_view(m_entity, this);
		m_model = new router_download_model(m_entity, { "File", "Queue", "Buffer" }, { 96, 64, 64 }, this);
		init(m_table, m_model);

		// Callback
		add_callback(callback_action::add, [&](const entity::ptr entity) { m_model->add(entity); });
		add_callback(callback_action::remove, [&](const entity::ptr entity) { m_model->remove(entity); });
		add_callback(callback_action::update, [&](const entity::ptr entity) { m_model->update(); });
		add_callback(callback_action::clear, [&](const entity::ptr entity) { m_model->clear(); });

		// Widget
		setWidget(m_table);
	}
}
