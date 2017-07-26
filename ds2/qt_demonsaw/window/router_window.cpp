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

#include <QHBoxLayout>
#include <QSplitter>

#include "router_window.h"
#include "component/error/error_component.h"
#include "component/server/server_component.h"
#include "resource/resource.h"
#include "pane/router/router_network_pane.h"
#include "pane/router/router_performance_pane.h"
#include "window/main_window.h"

namespace eja
{
	// Constructor
	router_window::router_window(const entity::ptr entity, QWidget* parent /*= 0*/, QWidget* statusbar /*= 0*/, entity_action* action /*= nullptr*/) : entity_window(entity, parent, action)
	{
		create_tabs();
		create_dashboard();
		create_activity();

		create_actions();
		create_toolbars();
		create_statusbar(statusbar);
		create_layouts();
		create_slots();
	}

	// Interface
	void router_window::update()
	{
		entity_window::update();

		//m_session_table->update();
		m_client_table->update();
		m_group_table->update();
		m_server_table->update();
		m_transfer_table->update();
	}

	void router_window::post_create()
	{
		// Model
		m_server_model->post_create();
	}

	// Utility
	void router_window::create_tabs()
	{
		entity_window::create_tabs();

		// Session
		/*m_session_table = new router_session_table(this);
		m_session_model = new router_session_model(m_value, { "Name", "Idle" }, { 224, 72 }, this);
		init(m_session_table, m_session_model);*/

		// Client
		m_client_table = new router_client_table(this);
		m_client_model = new router_client_model(m_value, { "Name", "Version", "Session", "Group", "Idle" }, { 128, 72, 224, 224, 72 }, this);
		init(m_client_table, m_client_model);

		// Group
		m_group_table = new router_group_table(this);
		m_group_model = new router_group_model(m_value, { "Name", "Clients" }, { 224, 72 }, this);
		init(m_group_table, m_group_model);

		// Router
		m_server_table = new router_server_table(this);
		m_server_model = new router_server_model(m_value, { "Name", "Version", "Address", "Port", "Passphrase", }, { 128, 72, 224, 72, 72 }, this);
		init(m_server_table, m_server_model);

		// Transfer
		m_transfer_table = new router_transfer_table(this);
		m_transfer_model = new router_transfer_model(m_value, { "Name", "Chunks", "Idle" }, { 224, 64, 72 }, this);
		init(m_transfer_table, m_transfer_model);

		// Add
		//m_main_tab->addTab(m_session_table, QIcon(resource::tab::session), "Session");
		m_main_tab->addTab(m_client_table, QIcon(resource::tab::client), "Client");
		m_main_tab->addTab(m_server_table, QIcon(resource::tab::router), "Router");
		m_main_tab->addTab(m_group_table, QIcon(resource::tab::group), "Group");
		m_main_tab->addTab(m_transfer_table, QIcon(resource::tab::transfer), "Transfer");
	}

	void router_window::create_dashboard()
	{
		entity_window::create_dashboard();

		// router_network_pane
		router_network_pane* network = new router_network_pane(m_value, m_toolbox);
		network->setProperty(entity_pane::action, QVariant::fromValue(m_action));
		m_toolbox->addItem(network, QIcon(resource::tool::network), "Network");

		// Performance
		router_performance_pane* performance = new router_performance_pane(m_value, m_toolbox);
		m_toolbox->addItem(performance, QIcon(resource::tab::lightning), "Performance");
	}

	void router_window::create_activity()
	{
		entity_window::create_activity();

		// Error
		m_error_table = new error_table(this);
		m_error_model = new error_model(m_value, { "Time", "Name", "Message" }, { 72, 128, 72 }, this);
		init(m_error_table, m_error_model);

		// Error
		m_activity_tab->addTab(m_error_table, QIcon(resource::tab::none), "Error");
	}

	void router_window::create_toolbars()
	{
		entity_window::create_toolbars();

		// Session
		/*QToolBar* stoolbar = m_session_table->get_toolbar();
		m_main_toolbar->addWidget(stoolbar);*/

		// Client
		QToolBar* ctoolbar = m_client_table->get_toolbar();
		m_main_toolbar->addWidget(ctoolbar);

		// Router
		QToolBar* rtoolbar = m_server_table->get_toolbar();
		m_main_toolbar->addWidget(rtoolbar);

		// Group
		QToolBar* gtoolbar = m_group_table->get_toolbar();
		m_main_toolbar->addWidget(gtoolbar);

		// Transfer
		QToolBar* ttoolbar = m_transfer_table->get_toolbar();
		m_main_toolbar->addWidget(ttoolbar);

		// Error
		QToolBar* etoolbar = m_error_table->get_toolbar();
		m_activity_toolbar->addWidget(etoolbar);
	}

	void router_window::create_statusbar(QWidget* parent /*= 0*/)
	{
		// Info
		QImage images[] =
		{
			QImage(),
			QImage(),
			QImage(),
			QImage(),
			QImage(),
			QImage(),
			QImage(resource::tab::client),
			QImage(resource::tab::router),
			QImage(resource::tab::group),
			QImage(resource::tab::transfer),
			QImage(resource::tab::none)			
		};

		QString tooltips[] = { "", "", "", "", "", "", "Client", "Router", "Group", "Transfer", "Error" };

		const auto size = sizeof(images) / sizeof(QImage);
		m_statusbar = new status_window(size, parent);

		for (size_t i = (size - 5); i < size; ++i)
		{
			const auto pane = m_statusbar->get_pane(i);
			pane->set_tooltip(tooltips[i]);
			pane->set_image(images[i]);
			pane->set_text("0");
		}
	}

	void router_window::create_layouts()
	{
		// Layout
		QWidget* widget1 = new QWidget(this);
		widget1->setObjectName("tab");		

		QVBoxLayout* vlayout1 = new QVBoxLayout(widget1);
		vlayout1->setSpacing(0);
		vlayout1->setMargin(0);
		
		vlayout1->addSpacing(4);
		vlayout1->addWidget(m_main_tab);
		widget1->setLayout(vlayout1);

		QVBoxLayout* vlayout2 = new QVBoxLayout(m_activity_tab);
		vlayout2->setSpacing(0);
		vlayout2->setMargin(0);

		m_activity_widget->setObjectName("tab");
		m_activity_widget->setLayout(vlayout2);
		vlayout2->addSpacing(4);
		vlayout2->addWidget(m_activity_tab);

		// Main
		QSplitter* splitter = new QSplitter(Qt::Vertical);
		splitter->setChildrenCollapsible(false);
		splitter->addWidget(widget1);
		splitter->setStretchFactor(0, 60);
		splitter->addWidget(m_activity_widget);
		splitter->setStretchFactor(1, 27);
		splitter->setHandleWidth(4);

		// Dashboard
		QHBoxLayout* hlayout = new QHBoxLayout;
		hlayout->setSpacing(0);
		hlayout->setMargin(0);

		hlayout->addWidget(m_toolbox);
		hlayout->addWidget(splitter);

		// Layout
		QVBoxLayout* vlayout = new QVBoxLayout(this);
		vlayout->setSpacing(0);
		vlayout->setMargin(0);
		
		vlayout->addSpacing(4);
		vlayout->addLayout(hlayout);

		setLayout(vlayout);
	}
}
