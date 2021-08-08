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

#include <cassert>

#include <QApplication>
#include <QHBoxLayout>
#include <QHelpEvent>
#include <QIcon>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSplitter>
#include <QStyle>
#include <QTabBar>
#include <QTimer>
#include <QToolBar>
#include <QToolTip>

#include "component/chat_component.h"
#include "component/room_component.h"
#include "component/callback/callback.h"
#include "component/callback/callback_service_component.h"
#include "component/client/client_component.h"
#include "component/client/client_service_component.h"
#include "component/search/search_component.h"
#include "component/search/search_option_component.h"
#include "component/status/status_component.h"
#include "component/transfer/download_component.h"
#include "component/transfer/upload_component.h"

#include "controller/pm_controller.h"
#include "controller/room_controller.h"
#include "controller/search_controller.h"
#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "factory/client_factory.h"
#include "font/font_button.h"
#include "font/font_awesome.h"
#include "resource/icon_provider.h"
#include "resource/resource.h"
#include "security/filter/base.h"
#include "system/type.h"
#include "thread/thread_info.h"
#include "utility/value.h"

#include "widget/browse_widget.h"
#include "widget/chat_widget.h"
#include "widget/client_widget.h"
#include "widget/docked_tab_widget.h"
#include "widget/download_widget.h"
#include "widget/error_widget.h"
#include "widget/finished_widget.h"
#include "widget/group_widget.h"
#include "widget/help_widget.h"
#include "widget/menu_widget.h"
#include "widget/router_widget.h"
#include "widget/share_widget.h"
#include "widget/statusbar_widget.h"
#include "widget/upload_widget.h"
#include "Widget/option/option_widget.h"

#include "window/main_window.h"
#include "window/client_window.h"

namespace eja
{
	// Constructor
	client_window::client_window(const entity::ptr& entity, QWidget* parent /*= nullptr*/, Qt::WindowFlags flags /*= 0*/) : entity_window(entity, parent, flags)
	{
		// Ratio
		const auto ratio = resource::get_ratio();

		// Window
		setDockOptions(QMainWindow::AnimatedDocks | QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks);
		setMinimumWidth(s_min_width * ratio);
		setToolTipDuration(0);

		// Icon
		m_demonsaw_icon = QIcon(resource::logo_rgb);

		// Buttons
		QFont font(font_family::font_awesome);
		font.setPixelSize(resource::get_large_icon_size());

		m_refresh_button = make_button("Refresh", fa::refresh);
		m_refresh_button->setObjectName("refresh");
		m_refresh_button->setFont(font);

		m_hamburger_button = make_button("Menu", fa::bars);
		m_hamburger_button->setObjectName("hamburger");
		m_hamburger_button->setFixedHeight(resource::get_line_edit_height());
		m_hamburger_button->setFont(font);

		m_logo_button = new QPushButton(this);
		m_logo_button->setObjectName("search_logo");
		m_logo_button->setFixedHeight(resource::get_line_edit_height());
		m_logo_button->setText(software::name);
		m_logo_button->setIconSize(QSize(resource::get_icon_size(), resource::get_icon_size()));
		m_logo_button->setIcon(m_demonsaw_icon);

		// Search
		m_search_box = new QLineEdit(this);
		m_search_box->setPlaceholderText("Search");
		m_search_box->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_search_box->installEventFilter(parent);
		m_search_box->setFixedHeight(resource::get_line_edit_height());
		m_search_box->setEnabled(m_entity->enabled());

		m_search_bar = addToolBar("");
		m_search_bar->setObjectName("search_menu");
		m_search_bar->setToolTipDuration(0);
		m_search_bar->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextOnly);
		m_search_bar->setFont(QFont(software::font_awesome));
		m_search_bar->setFloatable(false);
		m_search_bar->setMovable(false);
		m_search_bar->installEventFilter(this);

		const auto hlayout = resource::get_hbox_layout();
		hlayout->addWidget(m_refresh_button);
		hlayout->addWidget(m_logo_button);
		hlayout->addWidget(m_search_box, 1);
		hlayout->addWidget(m_hamburger_button);

		QWidget* widget = new QWidget(this);
		widget->setLayout(hlayout);
		m_search_bar->addWidget(widget);

		m_hamburger_widget = new menu_widget(m_entity, m_hamburger_button, this);
		m_about_widget = new help_widget(m_logo_button, this);

		// Statusbar
		m_statusbar = new statusbar_widget(m_entity, this);
		setStatusBar(m_statusbar);

		// Controller
		m_pm = new pm_controller(m_entity, this);
		m_room = new room_controller(m_entity, this);
		m_search = new search_controller(m_entity, this);

		// Left
		m_client = new client_widget(m_entity, this);
		m_client->setMinimumWidth(s_min_width * ratio);
		m_client->setMaximumWidth(s_min_width * ratio);
		m_client->setMinimumHeight(s_min_height * ratio);
		m_client->setAllowedAreas(Qt::AllDockWidgetAreas);
		addDockWidget(Qt::TopDockWidgetArea, m_client);

		// Right
		m_browse = new browse_widget(m_entity, this);
		m_browse->setMinimumWidth(s_min_width * ratio);
		m_browse->setMinimumHeight(s_min_height * ratio);
		m_browse->setAllowedAreas(Qt::AllDockWidgetAreas);
		addDockWidget(Qt::TopDockWidgetArea, m_browse);

		m_chat = new chat_widget(m_entity, this);
		m_chat->setMinimumWidth(s_min_width * ratio);
		m_chat->setMinimumHeight(s_min_height * ratio);
		m_chat->setAllowedAreas(Qt::AllDockWidgetAreas);
		addDockWidget(Qt::TopDockWidgetArea, m_chat);

		m_group = new group_widget(m_entity, this);
		m_group->setMinimumWidth(s_min_width * ratio);
		m_group->setMinimumHeight(s_min_height * ratio);
		m_group->setAllowedAreas(Qt::AllDockWidgetAreas);
		m_group->hide();
		addDockWidget(Qt::TopDockWidgetArea, m_group);

		m_option = new option_widget(m_entity, this);
		m_option->setMinimumWidth(s_min_width * ratio);
		m_option->setMinimumHeight(s_min_height * ratio);
		m_option->setAllowedAreas(Qt::AllDockWidgetAreas);
		m_option->hide();
		addDockWidget(Qt::TopDockWidgetArea, m_option);

		m_router = new router_widget(m_entity, this);
		m_router->setMinimumWidth(s_min_width * ratio);
		m_router->setMinimumHeight(s_min_height * ratio);
		m_router->setAllowedAreas(Qt::AllDockWidgetAreas);
		m_router->hide();
		addDockWidget(Qt::TopDockWidgetArea, m_router);

		m_share = new share_widget(m_entity, this);
		m_share->setMinimumWidth(s_min_width * ratio);
		m_share->setMinimumHeight(s_min_height * ratio);
		m_share->setAllowedAreas(Qt::AllDockWidgetAreas);
		addDockWidget(Qt::TopDockWidgetArea, m_share);

		// Bottom
		m_download = new download_widget(m_entity, this);
		m_download->setMinimumWidth(s_min_width * ratio);
		m_download->setMinimumHeight(s_min_height * ratio);
		m_download->setAllowedAreas(Qt::AllDockWidgetAreas);
		m_download->hide();
		addDockWidget(Qt::TopDockWidgetArea, m_download);

		m_error = new error_widget(m_entity, this);
		m_error->setMinimumWidth(s_min_width * ratio);
		m_error->setMinimumHeight(s_min_height * ratio);
		m_error->setAllowedAreas(Qt::AllDockWidgetAreas);
		m_error->hide();
		addDockWidget(Qt::TopDockWidgetArea, m_error);

		m_finished = new finished_widget(m_entity, this);
		m_finished->setMinimumWidth(s_min_width * ratio);
		m_finished->setMinimumHeight(s_min_height * ratio);
		m_finished->setAllowedAreas(Qt::AllDockWidgetAreas);
		m_finished->hide();
		addDockWidget(Qt::TopDockWidgetArea, m_finished);

		m_upload = new upload_widget(m_entity, this);
		m_upload->setMinimumWidth(s_min_width * ratio);
		m_upload->setMinimumHeight(s_min_height * ratio);
		m_upload->setAllowedAreas(Qt::AllDockWidgetAreas);
		m_upload->hide();
		addDockWidget(Qt::TopDockWidgetArea, m_upload);

		// Timer
		m_blink_timer = new QTimer(this);
		m_spam_timer = new QTimer(this);
		m_spam_timer->setSingleShot(true);

		// Callback
		add_callback(callback::download | callback::create, [&]() { tabify(m_download); });
		add_callback(callback::finished | callback::create, [&]() { tabify(m_finished); });
		add_callback(callback::window | callback::update, [&]() { update(); });
		add_callback(callback::search | callback::clear, [&](const entity::ptr entity) { clear(entity); });
		add_callback(callback::window | callback::restart, [&]() { restart(); });

		// Slot
		connect(this, &client_window::resize, m_hamburger_widget, &menu_widget::parent_resized);
		connect(this, &client_window::resize, m_about_widget, &help_widget::parent_resized);
		connect(this, &client_window::raise, this, &client_window::on_raise);
		connect(this, &client_window::tabify, this, &client_window::on_tabify);

		connect(m_search_box, &QLineEdit::returnPressed, this, &client_window::on_enter);
		connect(m_hamburger_button, &font_button::clicked, m_hamburger_widget, &menu_widget::toggle);
		connect(m_logo_button, &font_button::clicked, m_about_widget, &help_widget::toggle);
		connect(m_refresh_button, &font_button::clicked, [&]() { restart(); });

		connect(m_browse, &QDockWidget::dockLocationChanged, this, [&](Qt::DockWidgetArea area) { style_tabbar(); });
		connect(m_chat, &QDockWidget::dockLocationChanged, this, [&](Qt::DockWidgetArea area) { style_tabbar(); });
		connect(m_client, &QDockWidget::dockLocationChanged, this, [&](Qt::DockWidgetArea area) { style_tabbar(); });
		connect(m_download, &QDockWidget::dockLocationChanged, this, [&](Qt::DockWidgetArea area) { style_tabbar(); });
		connect(m_error, &QDockWidget::dockLocationChanged, this, [&](Qt::DockWidgetArea area) { style_tabbar(); });
		connect(m_finished, &QDockWidget::dockLocationChanged, this, [&](Qt::DockWidgetArea area) { style_tabbar(); });
		connect(m_group, &QDockWidget::dockLocationChanged, this, [&](Qt::DockWidgetArea area) { style_tabbar(); });
		connect(m_option, &QDockWidget::dockLocationChanged, this, [&](Qt::DockWidgetArea area) { style_tabbar(); });
		connect(m_router, &QDockWidget::dockLocationChanged, this, [&](Qt::DockWidgetArea area) { style_tabbar(); });
		connect(m_share, &QDockWidget::dockLocationChanged, this, [&](Qt::DockWidgetArea area) { style_tabbar(); });
		connect(m_upload, &QDockWidget::dockLocationChanged, this, [&](Qt::DockWidgetArea area) { style_tabbar(); });

		// Menu
		connect(m_hamburger_widget, &menu_widget::on_browse, this, [&]() { raise(m_browse); });
		connect(m_hamburger_widget, &menu_widget::on_chat, this, [&]() { raise(m_chat); });
		connect(m_hamburger_widget, &menu_widget::on_client, this, [&]() { raise(m_client); });
		connect(m_hamburger_widget, &menu_widget::on_download, this, [&]() { raise(m_download); });
		connect(m_hamburger_widget, &menu_widget::on_error, this, [&]() { raise(m_error); });
		connect(m_hamburger_widget, &menu_widget::on_finished, this, [&]() { raise(m_finished); });
		connect(m_hamburger_widget, &menu_widget::on_group, this, [&]() { raise(m_group); });
		connect(m_hamburger_widget, &menu_widget::on_option, this, [&]() { raise(m_option); });
		connect(m_hamburger_widget, &menu_widget::on_router, this, [&]() { raise(m_router); });
		connect(m_hamburger_widget, &menu_widget::on_search, this, [&]() { create(); });
		connect(m_hamburger_widget, &menu_widget::on_share, this, [&]() { raise(m_share); });
		connect(m_hamburger_widget, &menu_widget::on_upload, this, [&]() { raise(m_upload); });

		// Visible
		connect(m_client, &QDockWidget::visibilityChanged, this, &client_window::on_visibility_changed);
		connect(m_download, &QDockWidget::visibilityChanged, this, &client_window::on_visibility_changed);
		connect(m_error, &QDockWidget::visibilityChanged, this, &client_window::on_visibility_changed);
		connect(m_finished, &QDockWidget::visibilityChanged, this, &client_window::on_visibility_changed);
		connect(m_group, &QDockWidget::visibilityChanged, this, &client_window::on_visibility_changed);
		connect(m_option, &QDockWidget::visibilityChanged, this, &client_window::on_visibility_changed);
		connect(m_router, &QDockWidget::visibilityChanged, this, &client_window::on_visibility_changed);
		connect(m_share, &QDockWidget::visibilityChanged, this, &client_window::on_visibility_changed);
		connect(m_upload, &QDockWidget::visibilityChanged, this, &client_window::on_visibility_changed);

		connect(m_chat, &QDockWidget::visibilityChanged, this, [](bool visible)
		{
			if (visible)
			{
				const auto window = main_window::get();
				window->clear_chat();
			}
		});

		connect(m_blink_timer, &QTimer::timeout, this, [&]
		{
			if (m_blink_blue)
			{
				m_refresh_button->setStyleSheet(m_refresh_style);
				m_refresh_button->setObjectName("refresh_active");

				const auto style = m_refresh_button->style();
				style->unpolish(m_refresh_button);
				style->polish(m_refresh_button);
				m_blink_blue = false;
			}
			else
			{
				m_refresh_button->setStyleSheet("");
				m_refresh_button->setObjectName("refresh");

				const auto style = m_refresh_button->style();
				style->unpolish(m_refresh_button);
				style->polish(m_refresh_button);
				m_blink_blue = true;
			}
		});

		connect(m_spam_timer, &QTimer::timeout, this, [&]
		{
			m_enabled = true;
		});

		// Layout
		const auto client = m_entity->get<client_component>();
		if (!client->has_state())
		{
			// Client
			m_client->setMaximumWidth(s_min_width);

			splitDockWidget(m_client, m_chat, Qt::Horizontal);
			splitDockWidget(m_chat, m_download, Qt::Vertical);

			tabifyDockWidget(m_chat, m_browse);
			tabifyDockWidget(m_browse, m_share);
			tabifyDockWidget(m_share, m_group);
			tabifyDockWidget(m_group, m_router);
			tabifyDockWidget(m_router, m_option);

			tabifyDockWidget(m_download, m_upload);
			tabifyDockWidget(m_upload, m_finished);
			tabifyDockWidget(m_finished, m_error);
		}

		installEventFilter(this);

		// HACK
		emit on_activated(m_chat);

		// Select 1st Tab
		auto tabbars = findChildren<QTabBar*>();
		for (auto& tabbar : tabbars)
		{
			tabbar->installEventFilter(this);
			tabbar->setCurrentIndex(0);
			tabbar->setDrawBase(false);
			style_tabbar(tabbar);
		}
	}

	// Interface
	void client_window::on_create()
	{
		if (!m_search->is_visible())
		{
			// Create
			const auto entity = client_factory::create_search(m_entity);
			const auto client_service = m_entity->get<client_service_component>();
			client_service->async_search(entity);
		}

		// Cursor
		m_search_box->setFocus();
		m_search_box->selectAll();
	}

	void client_window::on_update()
	{
		assert(thread_info::main());

		const auto client_service = m_entity->get<client_service_component>();
		if (client_service->valid())
		{
			const auto client = m_entity->get<client_component>();
			m_refresh_style = QString("QPushButton {background: rgba(%1, %2, %3, %4);}").arg(client->get_red()).arg(client->get_green()).arg(client->get_blue()).arg(client->get_alpha());
			start_blink();
		}
	}

	void client_window::on_clear(const entity::ptr entity)
	{
		assert(thread_info::main());

		m_search_box->clear();
		m_search_box->setFocus();
	}

	// Utility
	void client_window::start_blink()
	{
		if (!m_blinking_refresh)
		{
			m_blinking_refresh = true;
			m_blink_timer->start(500);
		}
	}

	void client_window::stop_blink()
	{
		if (m_blinking_refresh)
		{
			m_blink_timer->stop();

			m_refresh_button->setStyleSheet("");
			m_refresh_button->setObjectName("refresh");

			const auto style = m_refresh_button->style();
			style->unpolish(m_refresh_button);
			style->polish(m_refresh_button);

			m_blinking_refresh = false;
			m_blink_blue = true;
		}
	}

	void client_window::style_tabbar(entity::ptr entity /*= nullptr*/)
	{
		auto tabbars = findChildren<QTabBar*>();
		for (auto& tabbar : tabbars)
		{
			tabbar->removeEventFilter(this);
			tabbar->installEventFilter(this);
			tabbar->setDrawBase(false);

			style_tabbar(tabbar, entity);
		}
	}

	void client_window::style_tabbar(QTabBar* tabbar, entity::ptr entity /*= nullptr*/)
	{
		for (int i = 0; i < tabbar->count(); ++i)
			style_tabbar(tabbar, i, entity);
	}

	void client_window::style_tabbar(QTabBar* tabbar, const int index, entity::ptr entity /*= nullptr*/)
	{
		const auto text = tabbar->tabText(index);
		const auto widget = new docked_tab_widget(text, tabbar);
		widget->setToolTip(text);

		// HACK
		tabbar->setStyleSheet(QString("QTabBar::tab { width:%1px; }").arg(resource::get_row_height() * 5));

		const auto tab = qobject_cast<docked_tab_widget*>(tabbar->tabButton(index, QTabBar::LeftSide));

		if (tab)
		{
			const auto icon = tab->get_icon();
			widget->set_entity(tab->get_entity());

			switch (icon)
			{
				case fa::envelope:
				case fa::hash_tag:
				{
					// break;
				}
				default:
				{
					widget->set_icon(icon);
				}
			}

		}
		else
		{
			if (entity)
			{
				const auto it = m_map.find(text);
				if (it != m_map.end())
					entity = it->second;

				if (entity->has<room_component>())
				{
					widget->set_entity(entity);
					widget->set_icon(fa::hash_tag);
				}
				else if (entity->has<chat_list_component>())
				{
					widget->set_entity(entity);
					widget->set_icon(fa::envelope);
				}
				else if (text == "Search")
				{
					widget->set_entity(entity);
					widget->set_icon(fa::search);
				}
				else
				{
					if (text == "Browse")
						widget->set_icon(fa::folder_open);
					else if (text == "Chat")
						widget->set_icon(fa::comment);
					else if (text == "Users")
						widget->set_icon(fa::users);
					else if (text == "Downloads")
						widget->set_icon(fa::arrow_down);
					else if (text == "Errors")
						widget->set_icon(fa::exclamation_triangle);
					else if (text == "Finished")
						widget->set_icon(fa::check);
					else if (text == "Options")
						widget->set_icon(fa::cog);
					else if (text == "Routers")
						widget->set_icon(fa::server);
					else if (text == "Group")
						widget->set_icon(fa::shield);
					else if (text == "Share")
						widget->set_icon(fa::share_alt);
					else if (text == "Uploads")
						widget->set_icon(fa::arrow_up);
					else
						assert(false);
				}
			}
			else
			{
				if (text == "Browse")
					widget->set_icon(fa::folder_open);
				else if (text == "Chat")
					widget->set_icon(fa::comment);
				else if (text == "Users")
					widget->set_icon(fa::users);
				else if (text == "Downloads")
					widget->set_icon(fa::arrow_down);
				else if (text == "Errors")
					widget->set_icon(fa::exclamation_triangle);
				else if (text == "Finished")
					widget->set_icon(fa::check);
				else if (text == "Options")
					widget->set_icon(fa::cog);
				else if (text == "Routers")
					widget->set_icon(fa::server);
				else if (text == "Group")
					widget->set_icon(fa::shield);
				else if (text == "Search")
					widget->set_icon(fa::search);
				else if (text == "Share")
					widget->set_icon(fa::share_alt);
				else if (text == "Uploads")
					widget->set_icon(fa::arrow_up);
				else
					assert(false);
			}
		}

		tabbar->setTabButton(index, QTabBar::LeftSide, widget);
	}

	// Utility
	void client_window::dock(entity_dock_widget* widget)
	{
		assert(thread_info::main());

		// Map
		if (widget->has_entity())
		{
			switch (widget->get_fa())
			{
				case fa::hash_tag:
				case fa::envelope:
				{
					const auto entity = widget->get_entity();
					const auto pair = std::make_pair(widget->windowTitle(), entity);
					const auto result = m_map.insert(pair);

					if (!result.second)
					{
						// Replace
						const auto it = result.first;
						it->second = entity;
					}
					break;
				}
			}
		}

		// Signal
		const auto entity = widget->get_entity();
		connect(widget, &QDockWidget::dockLocationChanged, this, [=](Qt::DockWidgetArea area) { style_tabbar(entity); });

		connect(widget, &QDockWidget::visibilityChanged, this, [=](bool visible)
		{
			switch (widget->get_fa())
			{
				case fa::hash_tag:
				{
					if (visible)
					{
						const auto window = main_window::get();
						window->clear_room(entity);
						m_search_box->clear();
					}

					break;
				}
				case fa::envelope:
				{
					if (visible)
					{
						const auto window = main_window::get();
						window->clear_pm(entity);
						m_search_box->clear();
					}

					break;
				}
				case fa::search:
				{
					if (visible)
					{
						const auto search_option = entity->get<search_option_component>();
						const auto qtext = QString::fromStdString(search_option->get_text());
						m_search_box->setText(qtext);

						m_search_box->setFocus();
						m_search_box->selectAll();
					}

					break;
				}
			}
		});

		// TODO: Place the new search, chat, pm windows in the proper place
		//
		//
		addDockWidget(Qt::TopDockWidgetArea, widget);
		tabifyDockWidget(m_chat, widget);

		if (!widget->isVisible())
		{
			widget->setVisible(true);
			style_tabbar(entity);
		}
	}

	void client_window::undock(entity_dock_widget* widget)
	{
		assert(thread_info::main());

		// Map
		const auto& text = widget->windowTitle();
		m_map.erase(text);
	}

	void client_window::close(docked_tab_widget* tab)
	{
		const auto icon = tab->get_icon();

		switch (icon)
		{
			case fa::arrow_down:
			{
				m_download->close();
				break;
			}
			case fa::arrow_up:
			{
				m_upload->close();
				break;
			}
			case fa::exclamation_triangle:
			{
				m_error->close();
				break;
			}
			case fa::check:
			{
				m_finished->close();
				break;
			}
			case fa::cog:
			{
				m_option->close();
				break;
			}
			case fa::comment:
			{
				m_chat->close();
				break;
			}
			case fa::envelope:
			{
				const auto entity = tab->get_entity();
				const auto widget = entity ? m_pm->get_widget(entity) : m_pm->get_visible_widget();
				if (widget)
					widget->close();

				break;
			}
			case fa::folder_open:
			{
				m_browse->close();
				break;
			}
			case fa::hash_tag:
			{
				const auto entity = tab->get_entity();
				const auto widget = entity ? m_room->get_widget(entity) : m_room->get_visible_widget();
				if (widget)
					widget->close();

				break;
			}
			case fa::search:
			{
				const auto entity = tab->get_entity();
				const auto widget = entity ? m_search->get_widget(entity) : m_search->get_visible_widget();
				if (widget)
					widget->close();

				break;
			}
			case fa::server:
			{
				m_router->close();
				break;
			}
			case fa::share_alt:
			{
				m_share->close();
				break;
			}
			case fa::shield:
			{
				m_group->close();
				break;
			}
			case fa::users:
			{
				m_client->close();
				break;
			}
		}
	}

	void client_window::restore()
	{
		assert(thread_info::main());

		// Geometry
		const auto client = m_entity->get<client_component>();

		if (client->has_geometry())
		{
			const auto& geometry = client->get_geometry();
			const auto data = base64::decode(geometry);
			QByteArray byteArray(data.c_str(), data.length());
			restoreGeometry(byteArray);
		}

		// State
		if (client->has_state())
		{
			// State
			const auto& state = client->get_state();
			const auto data = base64::decode(state);
			QByteArray byteArray(data.c_str(), data.length());
			restoreState(byteArray);
		}

		style_tabbar();
	}

	void client_window::update_color()
	{
		// Callback
		m_entity->async_call(callback::pm | callback::update);
		m_entity->async_call(callback::room | callback::update);
	}

	void client_window::update_theme()
	{
		const auto option = static_cast<option_widget*>(m_option);
		option->set_button_style();
	}

	bool client_window::verify()
	{
		const auto client = m_entity->get<client_component>();
		if (client->is_update())
		{
			const auto download_list = m_entity->get<download_list_component>();
			if (!download_list->empty())
			{
				const QString message = "You're downloading files.  Do you really want to disconnect?";
				const auto result = QMessageBox::question(this, software::name, message, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
				return (result == QMessageBox::Yes);
			}

			const auto upload_list = m_entity->get<upload_list_component>();
			if (!upload_list->empty())
			{
				const QString message = "You're uploading files.  Do you really want to quit?";
				const auto result = QMessageBox::question(this, software::name, message, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
				return (result == QMessageBox::Yes);
			}
		}

		return true;
	}

	// Is
	bool client_window::is_chat_visible(const void* ptr) const
	{
		return m_chat->is_visible(ptr);
	}

	bool client_window::is_chat_visible() const
	{
		return m_chat->is_visible();
	}

	bool client_window::is_pm_visible(const void* ptr) const
	{
		return m_pm->is_visible(ptr);
	}

	bool client_window::is_pm_visible(const entity::ptr& entity) const
	{
		return m_pm->is_visible(entity);
	}

	bool client_window::is_pm_visible() const
	{
		return m_pm->is_visible();
	}

	bool client_window::is_room_visible(const void* ptr) const
	{
		return m_room->is_visible(ptr);
	}

	bool client_window::is_room_visible(const entity::ptr& entity) const
	{
		return m_room->is_visible(entity);
	}

	bool client_window::is_room_visible() const
	{
		return m_room->is_visible();
	}

	// Slot
	void client_window::on_start()
	{
		m_search_box->setEnabled(true);

		stop_blink();

		// Service
		const auto client_service = m_entity->get<client_service_component>();
		if (client_service->invalid())
		{
			m_entity->enable();

			client_service->start();
		}
	}

	void client_window::on_stop()
	{
		m_search_box->clear();
		m_search_box->setDisabled(true);

		stop_blink();

		// Service
		const auto client_service = m_entity->get<client_service_component>();
		if (client_service->valid())
		{
			client_service->stop();

			m_entity->disable();
		}

		m_map.clear();
	}

	void client_window::on_restart()
	{
		// Spam
		if (!m_enabled)
			return;

		stop_blink();

		// Service
		const auto client_service = m_entity->get<client_service_component>();
		if (client_service->valid())
		{
			if (verify())
			{
				client_service->restart();

				// Spam
				m_enabled = false;
				m_spam_timer->start(ui::enter);
			}
		}
	}

	void client_window::on_enter()
	{
		// Spam
		if (!m_enabled)
			return;

		// Clear
		const auto window = m_search->get_visible_widget();
		if (window)
			window->clear();

		// Valid
		const auto qtext = m_search_box->text().simplified();
		const auto qlength = static_cast<size_t>(qtext.length() - qtext.count(' '));

		if (qlength >= search::min_text)
		{
			// Command
			auto text = qtext.toStdString();
			auto entity = m_search->get_visible_entity();
			if (entity)
			{
				const auto search_option = entity->get<search_option_component>();
				search_option->set_text(text);
			}
			else
			{
				entity = client_factory::create_search(m_entity, text);
			}

			const auto client_service = m_entity->get<client_service_component>();
			client_service->async_search(entity);

			// Cursor
			m_search_box->setFocus();
			m_search_box->selectAll();

			// Spam
			m_enabled = false;
			m_spam_timer->start(ui::enter);
		}
	}

	void client_window::on_raise(entity_dock_widget* widget)
	{
		if (!widget->isVisible())
		{
			widget->setVisible(true);
			style_tabbar();

			widget->raise();

			if (m_chat == widget)
				static_cast<chat_widget*>(m_chat)->auto_scroll();
		}
		else
		{
			widget->raise();
		}
	}

	void client_window::on_tabify(entity_dock_widget* widget)
	{
		if (!widget->isVisible())
		{
			widget->setVisible(true);
			style_tabbar();
		}
	}

	void client_window::on_visibility_changed(bool visible)
	{
		if (visible)
			m_search_box->clear();
	}

	// Event
	bool client_window::eventFilter(QObject* object, QEvent* event)
	{
		if (event->type() == QEvent::Wheel)
			return true;

		if (event->type() == QEvent::ContextMenu)
			return true;

		return QObject::eventFilter(object, event);
	}

	void client_window::showEvent(QShowEvent* event)
	{
		if (!visibleRegion().isEmpty())
		{
			// Client
			const auto ratio = resource::get_ratio();
			m_client->setMinimumWidth(s_min_width * ratio);
			m_client->setMaximumWidth(s_min_width * ratio * 2);

			// Search
			const auto entity = m_search->get_visible_entity();
			if (entity)
			{
				const auto search_option = entity->get<search_option_component>();
				const auto qtext = QString::fromStdString(search_option->get_text());
				m_search_box->setText(qtext);

				m_search_box->setFocus();
				m_search_box->selectAll();
			}
		}
	}

	// Set
	void client_window::set_statusbar(const bool value)
	{
		m_statusbar->setVisible(value);
	}

	// Get
	entity::ptr client_window::get_pm_entity() const
	{
		return m_pm->get_visible_entity();
	}

	entity::ptr client_window::get_room_entity() const
	{
		return m_room->get_visible_entity();
	}
}

// NOTE: The following code will iterate over all tabs and execute on any tabs visible, active or not
/*
const auto tabbars = window->findChildren<QTabBar*>();
for (const auto& tabbar : tabbars)
{
	int count = tabbar->count();
	for (int i = 0; i < count; i++)
	{
		QVariant data = tabbar->tabData(i);
		QVariant::DataPtr dataPtr = data.data_ptr();
		const auto ptr = dataPtr.data.ptr;

		if (window->is_chat_visible(ptr))
		{
			clear_chat();
		}
		else if (window->is_pm_visible(ptr))
		{
			const auto entity = window->get_pm_entity();
			if (entity)
				clear_pm(entity);
		}
		else if (window->is_room_visible(ptr))
		{
			const auto entity = window->get_room_entity();
			if (entity)
				clear_room(entity);
		}
	}
}
*/