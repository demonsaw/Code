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

#include "entity_window.h"

namespace eja
{
	// Utility
	void entity_window::create_tabs()
	{
		// Tab
		m_main_tab = new QTabWidget(this);
		m_main_tab->setMinimumHeight(160);
		m_main_tab->setTabPosition(QTabWidget::North);
		m_main_tab->setFocusPolicy(Qt::FocusPolicy::NoFocus);
	}

	void entity_window::create_dashboard()
	{
		// Toolbox
		m_toolbox = new QToolBox(this);
		m_toolbox->setFixedWidth(256);
		m_toolbox->setFocusPolicy(Qt::FocusPolicy::NoFocus);
	}

	void entity_window::create_activity()
	{
		// Tab
		m_activity_tab = new QTabWidget(this);
		m_activity_tab->setMinimumHeight(160);
		m_activity_tab->setTabPosition(QTabWidget::North);
		m_activity_tab->setFocusPolicy(Qt::FocusPolicy::NoFocus);
		m_activity_widget = new QWidget(this);
	}

	void entity_window::create_actions()
	{
		m_dashboard_action = new QAction(tr("&Dashboard"), this);
		m_dashboard_action->setCheckable(true);
		m_dashboard_action->setChecked(true);

		m_activity_action = new QAction(tr("&Activity"), this);
		m_activity_action->setCheckable(true);
		m_activity_action->setChecked(true);
	}

	void entity_window::create_toolbars()
	{
		// Main
		m_main_toolbar = new QStackedWidget(this);
		m_main_tab->setCornerWidget(m_main_toolbar, Qt::TopRightCorner);

		// Detail
		m_activity_toolbar = new QStackedWidget(this);
		m_activity_tab->setCornerWidget(m_activity_toolbar, Qt::TopRightCorner);
	}

	void entity_window::create_slots()
	{
		// Action
		connect(m_dashboard_action, &QAction::toggled, this, &entity_window::show_options);
		connect(m_activity_action, &QAction::toggled, this, &entity_window::show_details);

		// Tab
		connect(m_main_tab, &QTabWidget::currentChanged, this, &entity_window::main_tab_changed);
		connect(m_activity_tab, &QTabWidget::currentChanged, this, &entity_window::activity_tab_changed);
	}
}
