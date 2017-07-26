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

#include <QDockWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QShowEvent>
#include <QTabBar>
#include <QTimer>
#include <QToolBar>
#include <QToolTip>

#include "component/ui_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/client/client_service_component.h"
#include "component/message/message_service_component.h"
#include "component/session/session_component.h"
#include "component/transfer/transfer_service_component.h"

#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "entity/entity_widget.h"
#include "font/font_button.h"
#include "font/font_trigger.h"
#include "font/font_awesome.h"
#include "pane/client/client_status_pane.h"
#include "system/type.h"
#include "utility/value.h"
#include "widget/error_widget.h"
#include "widget/client/client_browse_widget.h"
#include "widget/client/client_chat_widget.h"
#include "widget/client/client_client_widget.h"
#include "widget/client/client_download_widget.h"
#include "widget/client/client_group_widget.h"
#include "widget/client/client_help_widget.h"
#include "widget/client/client_message_widget.h"
#include "widget/client/client_option_widget.h"
#include "widget/client/client_queue_widget.h"
#include "widget/client/client_search_widget.h"
#include "widget/client/client_share_widget.h"
#include "widget/client/client_transfer_widget.h"
#include "widget/client/client_upload_widget.h"
#include "window/main_window.h"
#include "window/client/client_window.h"

namespace eja
{
	// Constructor
	client_window::client_window(const entity::ptr entity, QWidget* parent /*= 0*/, Qt::WindowFlags flags /*= 0*/) : entity_window(entity, callback_type::window, parent, flags)
	{
		// Window
		setDockOptions(dockOptions() | QMainWindow::AnimatedDocks | QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks /*| QMainWindow::GroupedDragging*/);
		setToolTipDuration(0);

		// Statusbar
		m_status = new client_status_pane(m_entity, this);
		setStatusBar(m_status);

		// Panes
		m_chat = new client_chat_widget(m_entity, this);
		m_chat->setMinimumWidth(s_min_width);
		m_chat->setMinimumHeight(s_min_height);
		m_chat->setAllowedAreas(Qt::TopDockWidgetArea);
		addDockWidget(Qt::TopDockWidgetArea, m_chat);

		m_client = new client_client_widget(m_entity, this);
		m_client->setMaximumWidth(s_min_width);
		m_client->setMinimumWidth(s_min_width);
		m_client->setMinimumHeight(s_min_height);
		m_client->setAllowedAreas(Qt::TopDockWidgetArea);
		addDockWidget(Qt::TopDockWidgetArea, m_client);

		m_download = new client_download_widget(m_entity, this);
		m_download->setMaximumHeight(s_min_height);
		m_download->setMinimumWidth(s_min_width);
		m_download->setMinimumHeight(s_min_height);
		m_download->setAllowedAreas(Qt::TopDockWidgetArea);
		addDockWidget(Qt::TopDockWidgetArea, m_download);

		m_error = new error_widget(m_entity, this);
		m_error->setMaximumHeight(s_min_height);
		m_error->setMinimumWidth(s_min_width);
		m_error->setMinimumHeight(s_min_height);
		m_error->setAllowedAreas(Qt::TopDockWidgetArea);
		addDockWidget(Qt::TopDockWidgetArea, m_error);

		m_group = new client_group_widget(m_entity, this);
		m_group->setMinimumWidth(s_min_width);
		m_group->setMinimumHeight(s_min_height);
		m_group->setAllowedAreas(Qt::TopDockWidgetArea);
		addDockWidget(Qt::TopDockWidgetArea, m_group);

		m_help = new client_help_widget(m_entity, this);
		m_help->setMaximumWidth(s_min_width);
		m_help->setMinimumWidth(s_min_width);
		m_help->setMinimumHeight(s_min_height);
		m_help->setAllowedAreas(Qt::TopDockWidgetArea);
		addDockWidget(Qt::TopDockWidgetArea, m_help);

		m_message = new client_message_widget(m_entity, this);
		m_message->setMinimumWidth(s_min_width);
		m_message->setMinimumHeight(s_min_height);
		m_message->setAllowedAreas(Qt::TopDockWidgetArea);
		addDockWidget(Qt::TopDockWidgetArea, m_message);

		m_option = new client_option_widget(m_entity, this);
		m_option->setMinimumWidth(s_min_width);
		m_option->setMinimumHeight(s_min_height);
		m_option->setAllowedAreas(Qt::TopDockWidgetArea);
		addDockWidget(Qt::TopDockWidgetArea, m_option);

		m_queue = new client_queue_widget(m_entity, this);
		m_queue->setMaximumHeight(s_min_height);
		m_queue->setMinimumWidth(s_min_width);
		m_queue->setMinimumHeight(s_min_height);
		m_queue->setAllowedAreas(Qt::TopDockWidgetArea);
		addDockWidget(Qt::TopDockWidgetArea, m_queue);

		m_browse = new client_browse_widget(m_entity, this);
		m_browse->setMinimumWidth(s_min_width);
		m_browse->setMinimumHeight(s_min_height);
		m_browse->setAllowedAreas(Qt::TopDockWidgetArea);
		addDockWidget(Qt::TopDockWidgetArea, m_browse);

		m_share = new client_share_widget(m_entity, this);
		m_share->setMinimumWidth(s_min_width);
		m_share->setMinimumHeight(s_min_height);
		m_share->setAllowedAreas(Qt::TopDockWidgetArea);
		addDockWidget(Qt::TopDockWidgetArea, m_share);

		m_transfer = new client_transfer_widget(m_entity, this);
		m_transfer->setMinimumWidth(s_min_width);
		m_transfer->setMinimumHeight(s_min_height);
		m_transfer->setAllowedAreas(Qt::TopDockWidgetArea);
		addDockWidget(Qt::BottomDockWidgetArea, m_transfer);

		m_upload = new client_upload_widget(m_entity, this);
		m_upload->setMaximumHeight(s_min_height);
		m_upload->setMinimumWidth(s_min_width);
		m_upload->setMinimumHeight(s_min_height);
		m_upload->setAllowedAreas(Qt::TopDockWidgetArea);
		addDockWidget(Qt::BottomDockWidgetArea, m_upload);

		// Action
		m_browse_button = make_trigger("Browse", fa::sitemap);
		m_chat_button = make_trigger("Chat", fa::comment);
		m_client_button = make_trigger("Clients", fa::user);
		m_download_button = make_trigger("Downloads", fa::arrowdown);
		m_error_button = make_trigger("Errors", fa::exclamation);
		m_group_button = make_trigger("Group", fa::users);
		m_help_button = make_trigger("Help", fa::question);
		m_message_button = make_trigger("Message Routers", fa::server);
		m_option_button = make_trigger("Options", fa::cog);
		m_queue_button = make_trigger("Queue", fa::hourglassstart);
		m_search_button = make_button("Show Search", fa::search);
		m_share_button = make_trigger("Shares", fa::sharealt);
		m_status_button = make_trigger("Statusbar", fa::desktop);
		m_transfer_button = make_trigger("Transfer Routers", fa::exchange);
		m_upload_button = make_trigger("Uploads", fa::arrowup);

		// Slot
		connect(m_browse, &QDockWidget::dockLocationChanged, this, &client_window::on_dock_location_changed);
		connect(m_chat, &QDockWidget::dockLocationChanged, this, &client_window::on_dock_location_changed);
		connect(m_client, &QDockWidget::dockLocationChanged, this, &client_window::on_dock_location_changed);
		connect(m_download, &QDockWidget::dockLocationChanged, this, &client_window::on_dock_location_changed);
		connect(m_error, &QDockWidget::dockLocationChanged, this, &client_window::on_dock_location_changed);
		connect(m_group, &QDockWidget::dockLocationChanged, this, &client_window::on_dock_location_changed);
		connect(m_help, &QDockWidget::dockLocationChanged, this, &client_window::on_dock_location_changed);
		connect(m_message, &QDockWidget::dockLocationChanged, this, &client_window::on_dock_location_changed);
		connect(m_option, &QDockWidget::dockLocationChanged, this, &client_window::on_dock_location_changed);
		connect(m_queue, &QDockWidget::dockLocationChanged, this, &client_window::on_dock_location_changed);
		connect(m_share, &QDockWidget::dockLocationChanged, this, &client_window::on_dock_location_changed);
		connect(m_transfer, &QDockWidget::dockLocationChanged, this, &client_window::on_dock_location_changed);
		connect(m_upload, &QDockWidget::dockLocationChanged, this, &client_window::on_dock_location_changed);

		// View
		connect(m_browse_button, &font_trigger::clicked, this, [&](bool checked)
		{ 
			m_browse->setVisible(checked); 
			if (checked)
			{
				QTimer::singleShot(0, [&]() { m_browse->raise(); });
				m_browse_button->setToolTip("Hide Browse");
			}
			else
			{
				m_browse_button->setToolTip("Show Browse");
			}
		});
		
		connect(m_chat_button, &font_trigger::clicked, this, [&](bool checked)
		{
			m_chat->setVisible(checked);
			if (checked)
			{
				QTimer::singleShot(0, [&]() { m_chat->raise(); });
				m_chat_button->setToolTip("Hide Chat");
			}
			else
			{
				m_chat_button->setToolTip("Show Chat");
			}
		});

		connect(m_client_button, &font_trigger::clicked, this, [&](bool checked) 
		{ 
			m_client->setVisible(checked); 
			if (checked)
			{
				QTimer::singleShot(0, [&]() { m_client->raise(); });
				m_client_button->setToolTip("Hide Clients");
			}
			else
			{
				m_client_button->setToolTip("Show Clients");
			}
		});
		
		connect(m_download_button, &font_trigger::clicked, this, [&](bool checked) 
		{ 
			m_download->setVisible(checked); 
			if (checked)
			{
				QTimer::singleShot(0, [&]() { m_download->raise(); });
				m_download_button->setToolTip("Hide Downloads");
			}
			else
			{
				m_download_button->setToolTip("Show Downloads");
			}
		});

		connect(m_error_button, &font_trigger::clicked, this, [&](bool checked) 
		{ 
			m_error->setVisible(checked);  
			if (checked)
			{
				QTimer::singleShot(0, [&]() { m_error->raise(); });
				m_error_button->setToolTip("Hide Errors");
			}
			else
			{
				m_error_button->setToolTip("Show Errors");
			}
		});

		connect(m_group_button, &font_trigger::clicked, this, [&](bool checked) 
		{ 
			m_group->setVisible(checked); 
			if (checked)
			{
				QTimer::singleShot(0, [&]() { m_group->raise(); });
				m_group_button->setToolTip("Hide Groups");
			}
			else
			{
				m_group_button->setToolTip("Show Groups");
			}
		});

		connect(m_help_button, &font_trigger::clicked, this, [&](bool checked)
		{
			m_help->setVisible(checked);
			if (checked)
			{
				QTimer::singleShot(0, [&]() { m_help->raise(); });
				m_help_button->setToolTip("Hide Help");
			}
			else
			{
				m_help_button->setToolTip("Show Help");
			}
		});

		connect(m_message_button, &font_trigger::clicked, this, [&](bool checked) 
		{ 
			m_message->setVisible(checked); 
			if (checked)
			{
				QTimer::singleShot(0, [&]() { m_message->raise(); });
				m_message_button->setToolTip("Hide Message Routers");
			}
			else
			{
				m_message_button->setToolTip("Show Message Routers");
			}
		});

		connect(m_option_button, &font_trigger::clicked, this, [&](bool checked) 
		{ 
			m_option->setVisible(checked); 
			if (checked)
			{
				QTimer::singleShot(0, [&]() { m_option->raise(); });
				m_option_button->setToolTip("Hide Options");
			}
			else
			{
				m_option_button->setToolTip("Show Options");
			}
		});

		connect(m_queue_button, &font_trigger::clicked, this, [&](bool checked)
		{
			m_queue->setVisible(checked);
			if (checked)
			{
				QTimer::singleShot(0, [&]() { m_queue->raise(); });
				m_queue_button->setToolTip("Hide Queue");
			}
			else
			{
				m_queue_button->setToolTip("Show Queue");
			}
		});

		connect(m_search_button, &font_trigger::clicked, this, &client_window::on_search);

		connect(m_share_button, &font_trigger::clicked, this, [&](bool checked) 
		{ 
			m_share->setVisible(checked); 
			if (checked)
			{
				QTimer::singleShot(0, [&]() { m_share->raise(); });
				m_share_button->setToolTip("Hide Shares");
			}
			else
			{
				m_share_button->setToolTip("Show Shares");
			}
		});

		connect(m_status_button, &font_trigger::clicked, this, [&](bool checked)
		{
			m_status->setVisible(checked);
			if (checked)
				m_status_button->setToolTip("Hide Statusbar");
			else
				m_status_button->setToolTip("Show Statusbar");
		});

		connect(m_transfer_button, &font_trigger::clicked, this, [&](bool checked) 
		{ 
			m_transfer->setVisible(checked); 
			if (checked)
			{
				QTimer::singleShot(0, [&]() { m_transfer->raise(); });
				m_transfer_button->setToolTip("Hide Transfer Routers");
			}
			else
			{
				m_transfer_button->setToolTip("Show Transfer Routers");
			}
		});

		connect(m_upload_button, &font_trigger::clicked, this, [&](bool checked) 
		{ 
			m_upload->setVisible(checked); 
			if (checked)
			{
				QTimer::singleShot(0, [&]() { m_upload->raise(); });
				m_upload_button->setToolTip("Hide Uploads");
			}
			else
			{
				m_upload_button->setToolTip("Show Uploads");
			}
		});

		// Callback		
		add_callback(callback_action::add, [&](const entity::ptr entity) { add(entity); });
		add_callback(callback_action::remove, [&](const entity::ptr entity) { remove(entity); });
		add_callback(callback_action::update, [&](const entity::ptr entity) { update(entity); });

		// Menu		
		m_toolbar->addWidget(m_search_button);

		m_toolbar->addSeparator();

		m_toolbar->addWidget(m_client_button);
		init_menu(m_client_button, m_client, &ui_component::has_client, &ui_component::set_client);

		m_toolbar->addWidget(m_chat_button);
		init_menu(m_chat_button, m_chat, &ui_component::has_chat, &ui_component::set_chat);

		m_toolbar->addWidget(m_browse_button);
		init_menu(m_browse_button, m_browse, &ui_component::has_browse, &ui_component::set_browse);

		m_toolbar->addWidget(m_group_button);
		init_menu(m_group_button, m_group, &ui_component::has_group, &ui_component::set_group);

		m_toolbar->addWidget(m_queue_button);
		init_menu(m_queue_button, m_queue, &ui_component::has_queue, &ui_component::set_queue);

		m_toolbar->addWidget(m_download_button);
		init_menu(m_download_button, m_download, &ui_component::has_download, &ui_component::set_download);

		m_toolbar->addWidget(m_upload_button);
		init_menu(m_upload_button, m_upload, &ui_component::has_upload, &ui_component::set_upload);

		m_toolbar->addWidget(m_message_button);
		init_menu(m_message_button, m_message, &ui_component::has_message, &ui_component::set_message);

		m_toolbar->addWidget(m_transfer_button);
		init_menu(m_transfer_button, m_transfer, &ui_component::has_transfer, &ui_component::set_transfer);

		m_toolbar->addWidget(m_share_button);
		init_menu(m_share_button, m_share, &ui_component::has_share, &ui_component::set_share);

		m_toolbar->addWidget(m_option_button);
		init_menu(m_option_button, m_option, &ui_component::has_option, &ui_component::set_option);

		m_toolbar->addWidget(m_error_button);
		init_menu(m_error_button, m_error, &ui_component::has_error, &ui_component::set_error);

		m_toolbar->addWidget(m_help_button);
		init_menu(m_help_button, m_help, &ui_component::has_help, &ui_component::set_help);

		m_toolbar->addSeparator();

		m_toolbar->addWidget(m_status_button);
		init_menu(m_status_button, m_status, &ui_component::has_status, &ui_component::set_status);

		add_about();

		// Layout
		splitDockWidget(m_client, m_chat, Qt::Horizontal);
		splitDockWidget(m_error, m_queue, Qt::Horizontal);
		splitDockWidget(m_chat, m_queue, Qt::Vertical);
		splitDockWidget(m_chat, m_help, Qt::Horizontal);

		tabifyDockWidget(m_chat, m_browse);
		tabifyDockWidget(m_browse, m_group);
		tabifyDockWidget(m_group, m_message);
		tabifyDockWidget(m_message, m_transfer);
		tabifyDockWidget(m_transfer, m_share);
		tabifyDockWidget(m_share, m_option);

		tabifyDockWidget(m_queue, m_download);
		tabifyDockWidget(m_download, m_upload);
		tabifyDockWidget(m_upload, m_error);

		// Select 1st tab
		auto tabbars = findChildren<QTabBar*>();
		for (auto& tabbar : tabbars)
		{
			tabbar->installEventFilter(this);
			tabbar->setCurrentIndex(0);
		}
	}

	// Utility
	void client_window::init_menu(font_trigger* button, QWidget* window, font_trigger::has_function has, font_trigger::set_function set)
	{
		// Action		
		button->set_functions(has, set);
		const auto checked = button->has_value();
		button->setChecked(checked);

		// Tooltip
		button->setToolTip(checked ? ("Hide " + button->toolTip()) : ("Show " + button->toolTip()));

		// Window
		window->setVisible(checked);
	}

	void client_window::init_menu(font_trigger* button, entity_widget* window, font_trigger::has_function has, font_trigger::set_function set)
	{
		// Action		
		button->set_functions(has, set);
		const auto checked = button->has_value();
		button->setChecked(checked);

		// Tooltip
		button->setToolTip(checked ? ("Hide " + button->toolTip()) : ("Show " + button->toolTip()));

		// Window
		window->setVisible(checked);
		window->set_trigger(button);
	}

	void client_window::on_add(const std::shared_ptr<entity> entity)
	{
		if (entity->has<client_service_component>())
		{
			assert(false);
			/*const auto service = entity->get<client_service_component>();
			service->async_start();*/
		}
		else if (entity->has<message_service_component>())
		{
			assert(false);
			/*const auto service = entity->get<message_service_component>();
			service->async_start();*/
		}
		else if (entity->has<transfer_service_component>())
		{
			assert(false);
			/*const auto service = entity->get<transfer_service_component>();
			service->async_start();*/
		}
	}

	void client_window::on_remove(const std::shared_ptr<entity> entity)
	{
		if (entity->has<client_service_component>())
		{
			assert(false);
			/*const auto service = entity->get<client_service_component>();
			service->async_stop();*/
		}
		else if (entity->has<message_service_component>())
		{
			assert(false);
			/*const auto service = entity->get<message_service_component>();
			service->async_stop();*/
		}
		else if (entity->has<transfer_service_component>())
		{
			const auto service = entity->get<transfer_service_component>();
			service->stop();
		}
	}

	void client_window::on_update(const std::shared_ptr<entity> entity)
	{
		if (entity->has<client_service_component>())
		{
			assert(false);
			/*const auto service = entity->get<client_service_component>();
			service->async_restart();*/
		}
		else if (entity->has<message_service_component>())
		{
			const auto service = entity->get<message_service_component>();
			service->async_restart();
		}
		else if (entity->has<transfer_service_component>())
		{
			assert(false);
			/*const auto service = entity->get<transfer_service_component>();
			service->async_restart();*/
		}
	}

	void client_window::on_search()
	{
		// Callback
		m_entity->call(callback_type::browse, callback_action::create);
	}

	// Event
	bool client_window::eventFilter(QObject* object, QEvent* event)
	{
		const QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);

		if (helpEvent->type() == QEvent::ToolTip)
		{
			const auto tabbar = qobject_cast<QTabBar*>(object);
			if (tabbar)
			{
				const auto index = tabbar->tabAt(helpEvent->pos());
				if (index >= 0)
				{
					const auto text = tabbar->tabText(index);
					const auto fa = static_cast<fa::icon>(text.at(0).unicode());

					switch (fa)
					{
						case fa::sitemap:
						{
							QToolTip::showText(helpEvent->globalPos(), "Show Browse");
							return true;
						}
						case fa::comment:
						{
							QToolTip::showText(helpEvent->globalPos(), "Show Chat");
							return true;
						}
						case fa::user:
						{
							QToolTip::showText(helpEvent->globalPos(), "Show Clients");
							return true;
						}
						case fa::arrowdown:
						{
							QToolTip::showText(helpEvent->globalPos(), "Show Downloads");
							return true;
						}
						case fa::exclamation:
						{
							QToolTip::showText(helpEvent->globalPos(), "Show Errors");
							return true;
						}
						case fa::users:
						{
							QToolTip::showText(helpEvent->globalPos(), "Show Groups");
							return true;
						}
						case fa::question:
						{
							QToolTip::showText(helpEvent->globalPos(), "Show Help");
							return true;
						}
						case fa::server:
						{
							QToolTip::showText(helpEvent->globalPos(), "Show Message Routers");
							return true;
						}
						case fa::cog:
						{
							QToolTip::showText(helpEvent->globalPos(), "Show Options");
							return true;
						}
						case fa::hourglassstart:
						{
							QToolTip::showText(helpEvent->globalPos(), "Show Queue");
							return true;
						}
						case fa::envelope:
						{
							QToolTip::showText(helpEvent->globalPos(), "Show Private Messages");
							return true;
						}
						case fa::search:
						{
							QToolTip::showText(helpEvent->globalPos(), "Show Search");
							return true;
						}
						case fa::sharealt:
						{
							QToolTip::showText(helpEvent->globalPos(), "Show Shares");
							return true;
						}
						case fa::exchange:
						{
							QToolTip::showText(helpEvent->globalPos(), "Show Transfer Routers");
							return true;
						}
						case fa::arrowup:
						{
							QToolTip::showText(helpEvent->globalPos(), "Show Uploads");
							return true;
						}
					}
				}				
			}			
		}
		else
			QToolTip::hideText();

		return entity_window::eventFilter(object, event);
	}

	void client_window::showEvent(QShowEvent* event)
	{
		if (!visibleRegion().isEmpty())
		{
			m_client->setMaximumWidth(s_max_width);
			m_help->setMaximumWidth(s_max_width);

			m_download->setMaximumHeight(s_max_height);			
			m_error->setMaximumHeight(s_max_height);
			m_queue->setMaximumHeight(s_max_height);
			m_upload->setMaximumHeight(s_max_height);
		}		
	}

	void client_window::on_click(const int image) const
	{
		switch (image)
		{
			case fa::sitemap:
			{
				m_browse->setVisible(true);
				m_browse_button->setChecked(true);
				m_browse_button->setToolTip("Hide Browse");
				QTimer::singleShot(0, [&]() { m_browse->raise(); });
				break;
			}
			case fa::comment:
			{
				m_chat->setVisible(true);
				m_chat_button->setChecked(true);
				m_chat_button->setToolTip("Hide Chat");
				QTimer::singleShot(0, [&]() { m_chat->raise(); });
				break;
			}
			case fa::user:
			{
				m_client->setVisible(true);
				m_client_button->setChecked(true); 
				m_client_button->setToolTip("Hide Clients");
				QTimer::singleShot(0, [&]() { m_client->raise(); });
				break;
			}
			case fa::arrowdown:
			{
				m_download->setVisible(true);
				m_download_button->setChecked(true);
				m_download_button->setToolTip("Hide Downloads");
				QTimer::singleShot(0, [&]() { m_download->raise(); });
				break;
			}
			case fa::server:
			{
				m_message->setVisible(true);
				m_message_button->setChecked(true);
				m_message_button->setToolTip("Hide Message Routers");
				QTimer::singleShot(0, [&]() { m_message->raise(); });
				break;
			}
			case fa::hourglassstart:
			{
				m_queue->setVisible(true);
				m_queue_button->setChecked(true);
				m_queue_button->setToolTip("Hide Queue");
				QTimer::singleShot(0, [&]() { m_queue->raise(); });
				break;
			}
			case fa::exchange:
			{
				m_transfer->setVisible(true);
				m_transfer_button->setChecked(true);
				m_transfer_button->setToolTip("Hide Transfer Routers");
				QTimer::singleShot(0, [&]() { m_transfer->raise(); });
				break;
			}
			case fa::arrowup:
			{
				m_upload->setVisible(true);
				m_upload_button->setChecked(true);
				m_upload_button->setToolTip("Hide Uploads");
				QTimer::singleShot(0, [&]() { m_upload->raise(); });
				break;
			}
		}
	}
}
