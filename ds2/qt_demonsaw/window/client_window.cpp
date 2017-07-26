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

#include "client_window.h"
#include "component/client/client_component.h"
#include "component/error/error_component.h"
#include "resource/resource.h"
#include "pane/client/client_communication_pane.h"
#include "pane/client/client_performance_pane.h"
#include "pane/client/client_network_pane.h"
#include "pane/client/client_security_pane.h"
#include "window/main_window.h"

namespace eja
{
	// Constructor
	client_window::client_window(const entity::ptr entity, QWidget* parent /*= 0*/, QWidget* statusbar /*= 0*/, entity_action* action /*= nullptr*/) : entity_window(entity, parent, action)
	{
		create_statusbar(statusbar);

		create_tabs();
		create_dashboard();
		create_activity();

		create_actions();
		create_toolbars();		
		create_layouts();
		create_slots();
	}

	// Interface
	void client_window::update()
	{
		entity_window::update();

		m_browse_tree->update();
		m_search_window->update();
		m_chat_window->update();
		m_message_window->update();
		m_group_table->update();
		m_share_tree->update();
	}

	// Utility
	void client_window::create_tabs()
	{
		entity_window::create_tabs();

		// Browse
		m_browse_tree = new client_browse_tree(this);
		m_browse_model = new client_browse_model(m_value, { "File", "Size", "Type" }, { 448, 72, 72 }, this);
		init(m_browse_tree, m_browse_model);

		// Search
		m_search_window = new client_search_window(m_value, this);

		// Chat
		m_chat_window = new client_chat_window(m_value, this);

		// Message
		m_message_window = new client_message_window(m_value, this);

		// Group
		m_group_table = new client_group_table(this);
		m_group_model = new client_group_model(m_value, { "", "File", "Size", "Entropy", "Cipher", "Bits", "Hash", "Iterations", "Salt" }, { 37, 283, 72, 72, 72, 72, 96, 72, 72 }, this);
		init(m_group_table, m_group_model);

		// Share
		m_share_tree = new client_share_tree(this);
		m_share_model = new client_share_model(m_value, { "File", "Size", "Type" }, { 480, 72, 72 }, this);
		init(m_share_tree, m_share_model);

		// Add
		m_main_tab->addTab(m_browse_tree, QIcon(resource::tab::browse), "Browse"); 		
		m_main_tab->addTab(m_search_window, QIcon(resource::tab::search), "Search");
		m_main_tab->addTab(m_chat_window, QIcon(resource::tab::chat), "Chat");
		m_main_tab->addTab(m_message_window, QIcon(resource::tab::message), "Message");
		m_main_tab->addTab(m_group_table, QIcon(resource::tab::group), "Group");
		m_main_tab->addTab(m_share_tree, QIcon(resource::tab::share), "Share");
	}

	void client_window::create_dashboard()
	{
		entity_window::create_dashboard();
				
		// Network
		client_network_pane* network = new client_network_pane(m_value, m_toolbox);
		network->setProperty(entity_pane::action, QVariant::fromValue(m_action));
		network->setFocusPolicy(Qt::FocusPolicy::NoFocus);
		m_toolbox->addItem(network, QIcon(resource::tool::network), "Network");

		// Security
		client_security_pane* security = new client_security_pane(m_value, m_toolbox);
		security->setFocusPolicy(Qt::FocusPolicy::NoFocus);
		m_toolbox->addItem(security, QIcon(resource::tool::security), "Security");

		// Communication
		client_communication_pane* communication = new client_communication_pane(m_value, m_toolbox);
		communication->setFocusPolicy(Qt::FocusPolicy::NoFocus);
		m_toolbox->addItem(communication, QIcon(resource::tab::phone), "Communication");

		// Performance
		client_performance_pane* performance = new client_performance_pane(m_value, m_toolbox);		
		performance->setFocusPolicy(Qt::FocusPolicy::NoFocus);
		m_toolbox->addItem(performance, QIcon(resource::tab::lightning), "Performance");
	}

	void client_window::create_activity()
	{
		entity_window::create_activity();

		// Download
		m_download_table = new client_download_table(this);
		m_download_model = new client_download_model(m_value, { /*"Time",*/ "", "", "File", "Router", "Version", "Size", "Time", "Speed", "Progress" }, {/* 80,*/ 37, 37, 283, 128, 72, 72, 72, 72, 72 }, this);
		init(m_download_table, m_download_model);

		// Upload
		m_upload_table = new client_upload_table(this);
		m_upload_model = new client_upload_model(m_value, { /*"Time",*/ "", "", "File", "Router", "Version", "Size", "Time", "Speed", "Progress" }, { /*80,*/ 37, 37, 283, 128, 72, 72, 72, 72, 72 }, this);
		init(m_upload_table, m_upload_model);

		// Queued
		m_transfer_table = new client_transfer_table(this);
		m_transfer_model = new client_transfer_model(m_value, { /*"Time",*/ "", "", "File", "", "Size", "Time", "Speed", "Progress" }, { /*80,*/ 37, 37, 446, 37, 72, 72, 72, 72 }, this);
		init(m_transfer_table, m_transfer_model);

		// Finished
		m_finished_table = new client_finished_table(this);
		m_finished_model = new client_finished_model(m_value, { /*"Time",*/ "", "", "File", "", "Size", "Time", "Speed", "Progress" }, { /*80,*/ 37, 37, 446, 37, 72, 72, 72, 72 }, this);
		init(m_finished_table, m_finished_model);

		// Error
		m_error_table = new error_table(this);
		m_error_model = new error_model(m_value, { "Time", "Name", "Message" }, { 72, 128, 72 }, this);
		init(m_error_table, m_error_model);

		// Error		
		m_activity_tab->addTab(m_download_table, QIcon(resource::tab::download), "Download");
		m_activity_tab->addTab(m_upload_table, QIcon(resource::tab::upload), "Upload");
		m_activity_tab->addTab(m_transfer_table, QIcon(resource::tab::queued), "Queued");
		m_activity_tab->addTab(m_finished_table, QIcon(resource::tab::finish), "Finished");
		m_activity_tab->addTab(m_error_table, QIcon(resource::tab::none), "Error");
	}

	void client_window::create_toolbars()
	{
		entity_window::create_toolbars();

		// Browse
		QToolBar* btoolbar = m_browse_tree->get_toolbar();
		m_main_toolbar->addWidget(btoolbar);

		// Search
		QToolBar* stoolbar = m_search_window->get_toolbar();
		m_main_toolbar->addWidget(stoolbar);

		// Chat
		QToolBar* ctoolbar = m_chat_window->get_toolbar();
		m_main_toolbar->addWidget(ctoolbar);

		// Message
		QToolBar* mtoolbar = m_message_window->get_toolbar();
		m_main_toolbar->addWidget(mtoolbar);

		// Group
		QToolBar* gtoolbar = m_group_table->get_toolbar();
		m_main_toolbar->addWidget(gtoolbar);

		// Share
		QToolBar* htoolbar = m_share_tree->get_toolbar();
		m_main_toolbar->addWidget(htoolbar);

		// Download
		QToolBar* dtoolbar = m_download_table->get_toolbar();
		m_activity_toolbar->addWidget(dtoolbar);

		// Upload
		QToolBar* utoolbar = m_upload_table->get_toolbar();
		m_activity_toolbar->addWidget(utoolbar);

		// Queued
		QToolBar* qtoolbar = m_transfer_table->get_toolbar();
		m_activity_toolbar->addWidget(qtoolbar);

		// Finished
		QToolBar* ftoolbar = m_finished_table->get_toolbar();
		m_activity_toolbar->addWidget(ftoolbar);

		// Error
		QToolBar* etoolbar = m_error_table->get_toolbar();
		m_activity_toolbar->addWidget(etoolbar);
	}

	void client_window::create_statusbar(QWidget* parent /*= 0*/)
	{
		// Info
		QImage images[] =
		{
			QImage(resource::tab::browse),
			QImage(resource::tab::search),
			QImage(resource::tab::chat),
			QImage(resource::tab::message),
			QImage(resource::tab::group),
			QImage(resource::tab::share),
			QImage(resource::tab::download),
			QImage(resource::tab::upload),
			QImage(resource::tab::queued),
			QImage(resource::tab::finish),
			QImage(resource::tab::none)
		};

		QString tooltips[] = { "Browse", "Search", "Chat", "Message", "Group", "Share", "Download", "Upload", "Queued", "Finished", "Error" };

		const auto size = sizeof(images) / sizeof(QImage); 
		m_statusbar = new status_window(size, parent);

		for (size_t i = 0; i < size; ++i)
		{
			const auto pane = m_statusbar->get_pane(i);
			pane->set_tooltip(tooltips[i]);
			pane->set_image(images[i]);
			pane->set_text("0");
		}
	}

	void client_window::create_layouts()
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
		splitter->setStretchFactor(0, 5);
		splitter->addWidget(m_activity_widget);
		splitter->setStretchFactor(1, 4); 
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

	// HACK: to get tab signals working
	//
	void client_window::post_create()
	{
		// Window
		m_chat_window->post_create();
		m_message_window->post_create();

		// Model
		m_group_model->post_create();
		m_share_model->post_create();
	}
}
