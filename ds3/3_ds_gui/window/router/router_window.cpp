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
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QShowEvent>
#include <QTabBar>
#include <QTimer>
#include <QToolBar>
#include <QToolTip>

#include "component/ui_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/message/message_acceptor_component.h"
#include "entity/entity.h"
#include "entity/entity.h"
#include "font/font_trigger.h"
#include "font/font_awesome.h"
#include "resource/resource.h"
#include "system/type.h"
#include "utility/value.h"
#include "entity/entity_widget.h"
#include "widget/error_widget.h"
#include "widget/router/router_client_widget.h"
#include "widget/router/router_download_widget.h"
#include "widget/router/router_group_widget.h"
#include "widget/router/router_message_widget.h"
#include "widget/router/router_option_widget.h"
#include "widget/router/router_session_widget.h"
#include "widget/router/router_transfer_widget.h"
#include "window/router/router_window.h"

namespace eja
{
	// Constructor
	router_window::router_window(const entity::ptr entity, QWidget* parent /*= 0*/, Qt::WindowFlags flags /*= 0*/) : entity_window(entity, callback_type::none, parent, flags)
	{
		// Window
		setDockOptions(dockOptions() | QMainWindow::AnimatedDocks | QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks /*| QMainWindow::GroupedDragging*/);
		setToolTipDuration(0);

		m_client = new router_client_widget(m_entity, this);
		m_client->setMaximumWidth(s_min_width);
		m_client->setMinimumWidth(s_min_width);
		m_client->setMinimumHeight(s_min_height);
		m_client->setAllowedAreas(Qt::TopDockWidgetArea);
		addDockWidget(Qt::TopDockWidgetArea, m_client);

		m_download = new router_download_widget(m_entity, this);
		m_download->setMinimumWidth(s_min_width);
		m_download->setMinimumHeight(s_min_height);
		m_download->setAllowedAreas(Qt::TopDockWidgetArea);
		addDockWidget(Qt::BottomDockWidgetArea, m_download);

		m_error = new error_widget(m_entity, this);
		m_error->setMaximumHeight(s_min_height);
		m_error->setMinimumWidth(s_min_width);
		m_error->setMinimumHeight(s_min_height);
		m_error->setAllowedAreas(Qt::TopDockWidgetArea);
		addDockWidget(Qt::TopDockWidgetArea, m_error);

		m_group = new router_group_widget(m_entity, this);
		m_group->setMinimumWidth(s_min_width);
		m_group->setMinimumHeight(s_min_height);
		m_group->setAllowedAreas(Qt::TopDockWidgetArea);
		addDockWidget(Qt::TopDockWidgetArea, m_group);

		/*m_help = new help_widget(m_entity, this);
		m_help->setMaximumWidth(s_min_width);
		m_help->setMinimumWidth(s_min_width);
		m_help->setMinimumHeight(s_min_height);
		m_help->setAllowedAreas(Qt::TopDockWidgetArea);
		addDockWidget(Qt::TopDockWidgetArea, m_help);*/

		m_option = new router_option_widget(m_entity, this);
		m_option->setMinimumWidth(s_min_width);
		m_option->setMinimumHeight(s_min_height);
		m_option->setAllowedAreas(Qt::TopDockWidgetArea);
		addDockWidget(Qt::TopDockWidgetArea, m_option);

		m_session = new router_session_widget(m_entity, this);
		m_session->setMinimumWidth(s_min_width);
		m_session->setMinimumHeight(s_min_height);
		m_session->setAllowedAreas(Qt::TopDockWidgetArea);
		addDockWidget(Qt::TopDockWidgetArea, m_session);

		m_transfer = new router_transfer_widget(m_entity, this);
		m_transfer->setMinimumWidth(s_min_width);
		m_transfer->setMinimumHeight(s_min_height);
		m_transfer->setAllowedAreas(Qt::TopDockWidgetArea);
		addDockWidget(Qt::BottomDockWidgetArea, m_transfer);

		// Action
		m_client_button = make_trigger("Clients", fa::user);
		m_download_button = make_trigger("Downloads", fa::arrowdown);
		m_error_button = make_trigger("Errors", fa::exclamation);
		m_group_button = make_trigger("Groups", fa::users);
		//m_help_button = make_trigger("Help", fa::question);
		m_option_button = make_trigger("Options", fa::cog);
		m_session_button = make_trigger("Sessions", fa::key);
		m_transfer_button = make_trigger("Transfer Routers", fa::exchange);

		// Slot
		connect(m_client, &QDockWidget::dockLocationChanged, this, &router_window::on_dock_location_changed);
		connect(m_download, &QDockWidget::dockLocationChanged, this, &router_window::on_dock_location_changed); 
		connect(m_error, &QDockWidget::dockLocationChanged, this, &router_window::on_dock_location_changed);
		connect(m_group, &QDockWidget::dockLocationChanged, this, &router_window::on_dock_location_changed);
		//connect(m_help, &QDockWidget::dockLocationChanged, this, &router_window::on_dock_location_changed);
		connect(m_option, &QDockWidget::dockLocationChanged, this, &router_window::on_dock_location_changed);		
		connect(m_session, &QDockWidget::dockLocationChanged, this, &router_window::on_dock_location_changed);
		connect(m_transfer, &QDockWidget::dockLocationChanged, this, &router_window::on_dock_location_changed);

		// View
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

		/*connect(m_help_button, &font_trigger::clicked, this, [&](bool checked)
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
		});*/

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

		connect(m_session_button, &font_trigger::clicked, this, [&](bool checked)
		{
			m_session->setVisible(checked);
			if (checked)
			{
				QTimer::singleShot(0, [&]() { m_session->raise(); });
				m_session_button->setToolTip("Hide Sessions");
			}
			else
			{
				m_session_button->setToolTip("Show Sessions");
			}
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

		// Menu
		m_toolbar->addWidget(m_client_button);
		init_menu(m_client_button, m_client, &ui_component::has_client, &ui_component::set_client);

		m_toolbar->addWidget(m_session_button);
		init_menu(m_session_button, m_session, &ui_component::has_session, &ui_component::set_session);

		m_toolbar->addWidget(m_group_button);
		init_menu(m_group_button, m_group, &ui_component::has_group, &ui_component::set_group);

		m_toolbar->addWidget(m_download_button);
		init_menu(m_download_button, m_download, &ui_component::has_download, &ui_component::set_download);

		m_toolbar->addWidget(m_transfer_button);
		init_menu(m_transfer_button, m_transfer, &ui_component::has_transfer, &ui_component::set_transfer);

		m_toolbar->addWidget(m_option_button);
		init_menu(m_option_button, m_option, &ui_component::has_option, &ui_component::set_option);

		/*m_toolbar->addWidget(m_help_button);
		init_menu(m_help_button, m_help, &ui_component::has_help, &ui_component::set_help);*/

		m_toolbar->addWidget(m_error_button);
		init_menu(m_error_button, m_error, &ui_component::has_error, &ui_component::set_error);

		add_about();

		// Layout
		splitDockWidget(m_client, m_session, Qt::Horizontal);
		splitDockWidget(m_session, m_error, Qt::Vertical);

		//tabifyDockWidget(m_client, m_help);
		tabifyDockWidget(m_session, m_group);
		tabifyDockWidget(m_group, m_download);
		tabifyDockWidget(m_download, m_transfer);
		tabifyDockWidget(m_transfer, m_option);

		// Select 1st tab
		auto tabbars = findChildren<QTabBar*>();
		for (auto& tabbar : tabbars)
		{
			tabbar->installEventFilter(this);
			tabbar->setCurrentIndex(0);
		}
	}

	// Utility
	void router_window::init_menu(font_trigger* button, entity_widget* window, font_trigger::has_function has, font_trigger::set_function set)
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

	// Event
	bool router_window::eventFilter(QObject* object, QEvent* event)
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
					case fa::cog:
					{
						QToolTip::showText(helpEvent->globalPos(), "Show Options");
						return true;
					}
					case fa::key:
					{
						QToolTip::showText(helpEvent->globalPos(), "Show Sessions");
						return true;
					}
					case fa::exchange:
					{
						QToolTip::showText(helpEvent->globalPos(), "Show Transfer Routers");
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

	void router_window::showEvent(QShowEvent* event)
	{
		if (!visibleRegion().isEmpty())
		{
			m_client->setMaximumWidth(s_max_width);
			//m_help->setMaximumWidth(s_max_width);

			m_error->setMaximumHeight(s_max_height);
		}		
	}
}
