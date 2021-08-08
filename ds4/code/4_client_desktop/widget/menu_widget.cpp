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

#include <QDesktopServices>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QStyleOption>
#include <QToolBar>
#include <QUrl>
#include <QVBoxLayout>

#include "component/client/client_service_component.h"

#include "dialog/about_dialog.h"
#include "factory/client_factory.h"
#include "font/font_button.h"
#include "font/font_awesome.h"
#include "resource/resource.h"
#include "utility/value.h"
#include "widget/menu_widget.h"
#include "window/main_window.h"

namespace eja
{
	menu_widget::menu_widget(const entity::ptr& entity, font_button* button, QWidget* parent /* = nullptr */) : m_activator_button(button), entity_widget(entity, parent)
	{
		// Ratio
		const auto ratio = resource::get_ratio();

		// Font
		auto font = QApplication::font();
		font.setFamily(font_family::main);
		setFont(font);

		// Drop Shadow
		QGraphicsDropShadowEffect* drop_shadow = new QGraphicsDropShadowEffect(this);
		drop_shadow->setXOffset(0);
		drop_shadow->setYOffset(2 * ratio);
		drop_shadow->setBlurRadius(8 * ratio);
		drop_shadow->setColor(QColor(200, 200, 200, 180));
		setGraphicsEffect(drop_shadow);

		// Button
		m_browse_button = make_button(fa::folder_open);
		m_chat_button = make_button(fa::comment);
		m_client_button = make_button(fa::users);
		m_download_button = make_button(fa::arrow_down);
		m_errors_button = make_button(fa::exclamation_triangle);
		m_finished_button = make_button(fa::check);
		m_option_button = make_button(fa::cog);
		m_router_button = make_button(fa::server);
		m_search_button = make_button(fa::search);
		m_security_button = make_button(fa::shield);
		m_share_button = make_button(fa::share_alt);
		m_upload_button = make_button(fa::arrow_up);

		// Label
		m_browse_label = make_label("Browse");
		m_chat_label = make_label("Chat");
		m_client_label = make_label("Users");
		m_download_label = make_label("Downloads");
		m_errors_label = make_label("Errors");
		m_finished_label = make_label("Finished");
		m_router_label = make_label("Routers");
		m_search_label = make_label("Search");
		m_security_label = make_label("Group");
		m_option_label = make_label("Options");
		m_share_label = make_label("Share");
		m_upload_label = make_label("Uploads");

		// Container
		m_browse_container = make_container(m_browse_button, m_browse_label);
		m_chat_container = make_container(m_chat_button, m_chat_label);
		m_client_container = make_container(m_client_button, m_client_label);
		m_download_container = make_container(m_download_button, m_download_label);
		m_errors_container = make_container(m_errors_button, m_errors_label, "hamburger_container_bottom");
		m_finished_container = make_container(m_finished_button, m_finished_label);
		m_router_container = make_container(m_router_button, m_router_label, "hamburger_container_bottom");
		m_search_container = make_container(m_search_button, m_search_label);
		m_security_container = make_container(m_security_button, m_security_label);
		m_option_container = make_container(m_option_button, m_option_label, "hamburger_container_bottom");
		m_share_container = make_container(m_share_button, m_share_label);
		m_upload_container = make_container(m_upload_button, m_upload_label);

		// Footer
		m_about_button = new QPushButton(this);
		m_about_button->setIconSize(QSize(resource::get_large_icon_size(), resource::get_large_icon_size()));
		m_about_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
		m_about_button->setObjectName("hamburger_logo");
		m_about_button->setToolTipDuration(0);
		m_about_button->setToolTip("About");

		m_site_button = make_button_2(fa::info_circle, "Website", "hamburger_logo_website");
		m_wiki_button = make_button_2(fa::question_circle, "Help", "hamburger_logo_website");
		m_save_button = make_button_2(fa::save, "Save", "hamburger_logo_exit");

		QWidget* footer = new QWidget(this);
		m_logo_layout = new QHBoxLayout(footer);
		m_logo_toolbar = new QToolBar(this);
		m_url = new QUrl(software::help);

		m_logo_toolbar->addWidget(m_about_button);
		m_logo_toolbar->addWidget(m_site_button);
		m_logo_toolbar->addWidget(m_wiki_button);
		m_logo_toolbar->addWidget(m_save_button);
		m_logo_toolbar->setObjectName("hamburger_toolbar");
		m_logo_toolbar->setContentsMargins(0, 0, 0, 0);

		m_logo_layout->addWidget(m_logo_toolbar, 0);
		m_logo_layout->setContentsMargins(0, 0, 0, 0);
		m_logo_layout->setSpacing(0);

		footer->setObjectName("hamburger_footer");
		footer->setLayout(m_logo_layout);

		// Layout
		m_layout = new QGridLayout(this);
		m_layout->setHorizontalSpacing(0);
		m_layout->setContentsMargins(0, 0, 0, 0);

		m_layout->addWidget(m_chat_container, 0, 0, Qt::AlignCenter);
		m_layout->addWidget(m_browse_container, 0, 1, Qt::AlignCenter);
		m_layout->addWidget(m_search_container, 0, 2, Qt::AlignCenter);

		m_layout->addWidget(m_client_container, 1, 0, Qt::AlignCenter);
		m_layout->addWidget(m_share_container, 1, 1, Qt::AlignCenter);
		m_layout->addWidget(m_security_container, 1, 2, Qt::AlignCenter);

		m_layout->addWidget(m_download_container, 2, 0, Qt::AlignCenter);
		m_layout->addWidget(m_upload_container, 2, 1, Qt::AlignCenter);
		m_layout->addWidget(m_finished_container, 2, 2, Qt::AlignCenter);

		m_layout->addWidget(m_router_container, 3, 0, Qt::AlignCenter);
		m_layout->addWidget(m_option_container, 3, 1, Qt::AlignCenter);
		m_layout->addWidget(m_errors_container, 3, 2, Qt::AlignCenter);

		m_layout->addWidget(footer, 4, 0, 1, 3);

		// Connect
		signal();

		setObjectName("hamburger_menu");
		setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		setMinimumHeight(s_min_height * ratio);
		setMinimumWidth(s_min_width * ratio);
		setLayout(m_layout);

		hide();
	}

	void menu_widget::about() const
	{
		const auto window = main_window::get();
		window->setFocus();

		about_dialog dlg(const_cast<menu_widget*>(this));
		dlg.exec();
	}

	void menu_widget::site() const
	{
		const auto window = main_window::get();
		window->setFocus();

		QDesktopServices::openUrl(QUrl(software::website));
	}

	void menu_widget::help() const
	{
		const auto window = main_window::get();
		window->setFocus();

		QDesktopServices::openUrl(QUrl(software::help));
	}

	void menu_widget::save() const
	{
		const auto window = main_window::get();
		window->setFocus();

		window->save();
	}

	void menu_widget::focusOutEvent(QFocusEvent* event)
	{
		if (!m_activator_button->hasFocus())
			toggle();
	}

	font_button* menu_widget::make_button(const int icon, const char* tooltip /*= nullptr*/, const char* name /*= "hamburger_button" */)
	{
		auto button = new font_button(icon, resource::get_icon_size() * 1.75, this);
		button->setObjectName(name);
		button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		if (tooltip)
		{
			button->setToolTip(tooltip);
			button->setToolTipDuration(0);
		}

		return button;
	}

	font_button* menu_widget::make_button_2(const int icon, const char* tooltip /*= nullptr*/, const char* name /*= "hamburger_button" */)
	{
		auto button = new font_button(icon, resource::get_large_icon_size(), this);
		button->setObjectName(name);
		button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		if (tooltip)
		{
			button->setToolTip(tooltip);
			button->setToolTipDuration(0);
		}

		return button;
	}

	QWidget* menu_widget::make_container(font_button* button, QLabel* label, const char* name /*= "hamburger_container"*/)
	{
		// Ratio
		const auto ratio = resource::get_ratio();

		const auto widget = new QWidget(this);
		const auto layout = resource::get_vbox_layout(widget);
		layout->addWidget(button, Qt::AlignHCenter);
		layout->addWidget(label, Qt::AlignHCenter);
		layout->setAlignment(Qt::AlignCenter);
		layout->setContentsMargins(0, resource::get_icon_size(), 0, resource::get_icon_size());

		widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		widget->setLayout(layout);
		widget->setObjectName(name);
		widget->setFixedSize((s_min_width * ratio) / 3, (s_min_width * ratio) / 3);

		return widget;
	}

	QLabel* menu_widget::make_label(const char* text)
	{
		auto label = new QLabel(this);
		label->setText(text);
		label->setObjectName("hamburger_label");
		label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
		label->setAlignment(Qt::AlignHCenter);

		QFont font(font_family::main);
		font.setPixelSize(resource::get_large_font_size() * 0.95);
		label->setFont(font);

		return label;
	}

	void menu_widget::paintEvent(QPaintEvent* event)
	{
		QStyleOption opt;
		opt.init(this);
		QPainter p(this);
		style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
	}

	void menu_widget::parent_resized()
	{
		resize();
	}

	void menu_widget::resize()
	{
		QRect geometry = parentWidget()->geometry();
		move(geometry.width() - width() - (s_right_margin * resource::get_ratio()), m_activator_button->pos().y() + m_activator_button->height() + (resource::get_icon_padding() * 1.5));
	}

	void menu_widget::mouseReleaseEvent(QMouseEvent* event)
	{
		if (m_browse_container->underMouse())
			emit on_browse();
		else if (m_chat_container->underMouse())
			emit on_chat();
		else if (m_client_container->underMouse())
			emit on_client();
		else if (m_download_container->underMouse())
			emit on_download();
		else if (m_errors_container->underMouse())
			emit on_error();
		else if (m_finished_container->underMouse())
			emit on_finished();
		else if (m_option_container->underMouse())
			emit on_option();
		else if (m_router_container->underMouse())
			emit on_router();
		else if (m_search_container->underMouse())
			emit on_search();
		else if (m_security_container->underMouse())
			emit on_group();
		else if (m_share_container->underMouse())
			emit on_share();
		else if (m_upload_container->underMouse())
			emit on_upload();

		const auto window = main_window::get();
		window->setFocus();
	}

	void menu_widget::signal()
	{
		connect(m_browse_button, &font_button::clicked, this, [&] { emit on_browse(); });
		connect(m_chat_button, &font_button::clicked, this, [&] { emit on_chat(); });
		connect(m_client_button, &font_button::clicked, this, [&] { emit on_client(); });
		connect(m_download_button, &font_button::clicked, this, [&] { emit on_download(); });
		connect(m_errors_button, &font_button::clicked, this, [&] { emit on_error(); });
		connect(m_finished_button, &font_button::clicked, this, [&] { emit on_finished(); });
		connect(m_option_button, &font_button::clicked, this, [&] { emit on_option(); });
		connect(m_router_button, &font_button::clicked, this, [&] { emit on_router(); });
		connect(m_search_button, &font_button::clicked, this, [&] { emit on_search(); });
		connect(m_security_button, &font_button::clicked, this, [&] { emit on_group(); });
		connect(m_share_button, &font_button::clicked, this, [&] { emit on_share(); });
		connect(m_upload_button, &font_button::clicked, this, [&] { emit on_upload(); });

		connect(m_about_button, &QPushButton::clicked, this, &menu_widget::about);
		connect(m_wiki_button, &font_button::clicked, this, &menu_widget::help);
		connect(m_site_button, &font_button::clicked, this, &menu_widget::site);
		connect(m_save_button, &font_button::clicked, this, &menu_widget::save);
	}

	void menu_widget::toggle()
	{
		if (isVisible())
		{
			hide();
		}
		else
		{
			resize();
			show();
			raise();
			setFocus(Qt::FocusReason::PopupFocusReason);
		}
	}
}
