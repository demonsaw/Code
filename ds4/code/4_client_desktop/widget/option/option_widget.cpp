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

#include <QAction>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QPushButton>
#include <QScrollArea>
#include <QSizePolicy>
#include <QStyle>
#include <QVBoxLayout>

#include "component/callback/callback.h"
#include "component/client/client_component.h"

#include "entity/entity.h"
#include "font/font_awesome.h"
#include "resource/resource.h"
#include "thread/thread_info.h"
#include "widget/option/general_option_widget.h"
#include "widget/option/system_option_widget.h"
#include "widget/option/option_widget.h"
#include "widget/option/router_option_widget.h"
#include "widget/option/transfer_option_widget.h"
#include "window/client_window.h"
#include "window/main_window.h"

namespace eja
{
	// Constructor
	option_widget::option_widget(const entity::ptr& entity, QWidget* parent /*= nullptr*/) : entity_dock_widget(entity, "Options", fa::cog, parent)
	{
		assert(thread_info::main());

		setContextMenuPolicy(Qt::CustomContextMenu);

		// Action
		m_close_action = make_action(" Close", fa::close);

		// Menu
		add_button("Close", fa::close, [&]() { close(); });

		// Icon
		auto icon_font = QFont(software::font_awesome);
		icon_font.setPixelSize(resource::get_large_font_size());
		m_icon->setFont(icon_font);

		// Layout
		auto hlayout = get_hbox_layout();
		hlayout->addWidget(m_icon);
		hlayout->addWidget(m_title);
		hlayout->addStretch(1);
		hlayout->addWidget(m_toolbar);

		m_titlebar = new QWidget(this);
		m_titlebar->setObjectName("menubar");
		m_titlebar->setLayout(hlayout);
		setTitleBarWidget(m_titlebar);

		m_general = new general_option_widget(m_entity, this);
		m_system = new system_option_widget(m_entity, this);
		m_router = new router_option_widget(m_entity, this);
		m_transfer = new transfer_option_widget(m_entity, this);

		m_layout = get_vbox_layout();
		m_layout->setContentsMargins(40, 14, 0, 0);

		section();
		m_layout->addWidget(m_general);
		m_layout->addWidget(m_system);
		m_layout->addWidget(m_router);
		m_layout->addWidget(m_transfer);
		signal();

		set_button_style();

		m_layout->addStretch(1);

		// Hide
		m_system->hide();
		m_router->hide();
		m_transfer->hide();
 
		const auto widget = new QWidget(this);
		widget->setLayout(m_layout);
		widget->setObjectName("pane_background");

		const auto scroll_area = new QScrollArea(this);
		scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		scroll_area->setWidget(widget);
		scroll_area->setObjectName("pane_background");
		setWidget(scroll_area);

		// Event
		installEventFilter(this);

		// Signal
		connect(this, &option_widget::customContextMenuRequested, this, &option_widget::show_menu);
		connect(m_help, &QPushButton::clicked, this, &option_widget::help);
		connect(m_reset, &QPushButton::clicked, this, &option_widget::reset);
		connect(m_close_action, &QAction::triggered, [&]() { close(); });
	}

	// Interface
	void option_widget::on_update()
	{
		// Callback
		m_entity->async_call(callback::window | callback::update);
	}

	// Utility
	void option_widget::help()
	{
		if (m_general->isVisible())
			m_general->help();
		else if (m_system->isVisible())
			m_system->help();
		else if (m_router->isVisible())
			m_router->help();
		else if (m_transfer->isVisible())
			m_transfer->help();
		else
			assert(false);
	}

	void option_widget::reset()
	{
		if (m_general->isVisible())
			m_general->reset();
		else if (m_system->isVisible())
			m_system->reset();
		else if (m_router->isVisible())
			m_router->reset();
		else if (m_transfer->isVisible())
			m_transfer->reset();
		else
			assert(false);
	}

	// Sections
	void option_widget::section()
	{
		// Ratio
		const auto ratio = resource::get_ratio();

		// Font
		QFont awesome(software::font_awesome);
		awesome.setPixelSize(resource::get_icon_size());

		// Name
		const auto hlayout = resource::get_hbox_layout();
		hlayout->setSpacing(resource::get_horizontal_spacing());

		const auto client = m_entity->get<client_component>();
		m_name_label = new QLabel(QString::fromStdString(client->get_name()), this);		
		hlayout->addWidget(m_name_label);

		// Buttons
		m_help = new QPushButton(QString(fa::question), this);
		m_help->setFixedSize(QSize(resource::get_fa_button_width(), resource::get_fa_button_height()));
		m_help->setToolTip("Help");
		m_help->setFont(awesome);

		m_reset = new QPushButton(QString(fa::undo), this);
		m_reset->setFixedSize(QSize(resource::get_fa_button_width(), resource::get_fa_button_height()));
		m_reset->setToolTip("Restore Defaults");
		m_reset->setFont(awesome);

		hlayout->addStretch(1);
		hlayout->addWidget(m_reset);
		hlayout->addWidget(m_help);
		m_layout->addLayout(hlayout);
		
		// Sections
		m_general_button = new QPushButton("General", this);		
		m_general_button->setCheckable(true);
		m_general_button->setFixedSize(137 * ratio, 40 * ratio);
		m_general_button->setChecked(true);

		m_router_button = new QPushButton("Router", this);
		m_router_button->setCheckable(true);
		m_router_button->setFixedSize(137 * ratio, 40 * ratio);

		m_system_button = new QPushButton("System", this);
		m_system_button->setCheckable(true);
		m_system_button->setFixedSize(138 * ratio, 40 * ratio);

		m_transfer_button = new QPushButton("Transfer", this);
		m_transfer_button->setCheckable(true);
		m_transfer_button->setFixedSize(137 * ratio, 40 * ratio);

		auto sections = get_hbox_layout();
		sections->setSpacing(0);
		sections->setAlignment(Qt::AlignLeft);
		sections->addWidget(m_general_button);		
		sections->addWidget(m_router_button);
		sections->addWidget(m_system_button);
		sections->addWidget(m_transfer_button);

		// Section Container
		auto section_container = new QWidget(this);
		section_container->setLayout(sections);
		section_container->setObjectName("option_bar");

		// Layout		
		m_layout->addSpacing(12 * ratio);
		m_layout->addWidget(section_container);
		m_layout->addSpacing(24 * ratio);
	}

	void option_widget::on_section(QPushButton* button)
	{
		const auto is_general = (button == m_general_button);		
		m_general_button->setChecked(is_general);
		m_general->setVisible(is_general);

		const auto is_router = (button == m_router_button);
		m_router_button->setChecked(is_router);
		m_router->setVisible(is_router);

		const auto is_system = (button == m_system_button);
		m_system_button->setChecked(is_system);
		m_system->setVisible(is_system);

		const auto is_transfer = (button == m_transfer_button);
		m_transfer_button->setChecked(is_transfer);
		m_transfer->setVisible(is_transfer);
	}

	void option_widget::set_button_style()
	{
		// Get Section Button Background Color, Hover Color, & Border Color
		QWidget empty;
		empty.setObjectName("option_button");
		empty.style()->polish(&empty);

		m_button_background = empty.palette().background().color();
		m_button_border = empty.palette().foreground().color();
		empty.setObjectName("option_button_hover");
		empty.style()->polish(&empty);
		m_button_hover = empty.palette().background().color();

		// Set Color
		const auto client = m_entity->get<client_component>();
		QColor color(client->get_color());
		update_color(color);
	}

	void option_widget::signal()
	{
		// Section
		connect(m_general_button, &QPushButton::clicked, this, [&] { on_section(m_general_button); });
		connect(m_system_button, &QPushButton::clicked, this, [&] { on_section(m_system_button); });
		connect(m_router_button, &QPushButton::clicked, this, [&] { on_section(m_router_button); });
		connect(m_transfer_button, &QPushButton::clicked, this, [&] { on_section(m_transfer_button); });

		// Client
		connect(m_general, &general_option_widget::name_changed, this, [&] 
		{
			auto comp = m_entity->get<client_component>();
			auto name = QString::fromStdString(comp->get_name());
			m_name_label->setText(name);

			update();
		});

		connect(m_general, &general_option_widget::color_changed, this, [=](QColor& color) 
		{
			update_color(color); 
			update();
		});

		// Router
		connect(m_router, &router_option_widget::prime_changed, this, [&] { update(); });
		connect(m_router, &router_option_widget::cipher_changed, this, [&] { update(); });
		connect(m_router, &router_option_widget::key_changed, this, [&] { update(); });
		connect(m_router, &router_option_widget::hash_changed, this, [&] { update(); });
		connect(m_router, &router_option_widget::iterations_changed, this, [&] { update(); });
		connect(m_router, &router_option_widget::salt_changed, this, [&] { update(); });

		// System
		connect(m_system, &system_option_widget::hash_changed, this, [&] { update(); });
		connect(m_system, &system_option_widget::salt_changed, this, [&] { update(); });

		// Transfer
		connect(m_transfer, &transfer_option_widget::max_downloads_changed, this, [&] { update(); });
		connect(m_transfer, &transfer_option_widget::max_uploads_changed, this, [&] { update(); });
	}

	// Get
	QVBoxLayout* option_widget::get_vbox_layout() const
	{
		const auto layout = new QVBoxLayout;
		layout->setContentsMargins(0, 0, 0, 0);
		layout->setSpacing(0);
		layout->setMargin(0);

		return layout;
	}

	QHBoxLayout* option_widget::get_hbox_layout() const
	{
		const auto layout = new QHBoxLayout;
		layout->setContentsMargins(0, 0, 0, 0);
		layout->setSpacing(6);
		layout->setMargin(0);

		return layout;
	}

	// Slots
	void option_widget::update_color(const QColor& color)
	{
		const auto client = m_entity->get<client_component>();
		QColor font(client->is_dark() ? QColor(255, 255, 255) : QColor(35, 35, 35));

		const auto rgba = QString("rgba(%1, %2, %3, %4)").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha());
		const auto background = QString("rgba(%1, %2, %3, %4)").arg(m_button_background.red()).arg(m_button_background.green()).arg(m_button_background.blue()).arg(m_button_background.alpha());
		const auto border = QString("rgba(%1, %2, %3, %4)").arg(m_button_border.red()).arg(m_button_border.green()).arg(m_button_border.blue()).arg(m_button_border.alpha());
		const auto hover = QString("rgba(%1, %2, %3, %4)").arg(m_button_hover.red()).arg(m_button_hover.green()).arg(m_button_hover.blue()).arg(m_button_hover.alpha());

		const auto section_button = QString("QPushButton{ background: transparent; border: 0; border-top: 1px solid %3; border-bottom: 1px solid %3; margin: 0; font-size: %5px; padding: 6px 18px; } QPushButton::checked, QPushButton::checked:hover { background: %2; border-bottom: 4px solid %1; padding-bottom: 4px; } QPushButton::hover { background: %4; }").arg(rgba).arg(background).arg(border).arg(hover).arg(resource::get_large_font_size());		
		const auto name_style = QString("QLabel { color: %1;font-size: %2px; font-weight: bold; padding:0; }").arg(rgba).arg(resource::get_large_font_size());

		m_name_label->setStyleSheet(name_style);
		m_general_button->setStyleSheet(section_button);
		m_system_button->setStyleSheet(section_button);		
		m_router_button->setStyleSheet(section_button);
		m_transfer_button->setStyleSheet(section_button);

		set_style_color();
	}

	// Menu
	void option_widget::show_menu(const QPoint& pos)
	{
		assert(thread_info::main());

		QMenu menu;
		menu.setStyleSheet(QString("QMenu::item { height:%1px; width:%2px; }").arg(resource::get_row_height()).arg(resource::get_menu_width()));
		menu.addAction(m_close_action);
		menu.exec(mapToGlobal(pos));
	}

	// Set
	void option_widget::set_style_color()
	{
		const auto client = m_entity->get<client_component>();

		QColor font_color(client->get_font_color());
		const auto font_rgb = QString("rgba(%1, %2, %3)").arg(font_color.red()).arg(font_color.green()).arg(font_color.blue());
		const auto text_rgb = QString("rgba(%1, %2, %3, %4)").arg(client->get_red()).arg(client->get_green()).arg(client->get_blue()).arg(client->get_alpha());
		const auto style = QString("QPushButton {color: %2; background: %1; border-radius: 4px; border: 0; padding: 4px; margin: 1px 0;} QPushButton:hover, QPushButton:pressed { color: %1; background: %2; border: 1px solid %1; }").arg(text_rgb).arg(font_rgb);

		m_help->setStyleSheet(style);
		m_reset->setStyleSheet(style);
	}
}
