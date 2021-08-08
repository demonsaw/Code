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
#include <QDesktopServices>
#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QStyleOption>
#include <QUrl>
#include <QVBoxLayout>

#include "dialog/about_dialog.h"
#include "font/font_button.h"
#include "font/font_awesome.h"
#include "resource/resource.h"
#include "utility/value.h"
#include "widget/help_widget.h"
#include "window/main_window.h"

namespace eja
{
	help_widget::help_widget(QPushButton* button, QWidget* parent /* = nullptr */) : m_activator_button(button), QWidget(parent)
	{
		// Ratio
		const auto ratio = resource::get_ratio();

		// Font
		QFont awesome(software::font_awesome);
		awesome.setPixelSize(resource::get_large_icon_size());

		auto font = QApplication::font();
		font.setFamily(font_family::main);
		font.setPixelSize(resource::get_font_size());
		setFont(font);

		// Title Font
		QFont title_font = QApplication::font();
		title_font.setFamily(font_family::main);
		title_font.setPixelSize(resource::get_label_font_size());

		// Drop Shadow
		QGraphicsDropShadowEffect* drop_shadow = new QGraphicsDropShadowEffect(this);
		drop_shadow->setXOffset(0);
		drop_shadow->setYOffset(2 * ratio);
		drop_shadow->setBlurRadius(8 * ratio);
		drop_shadow->setColor(QColor(200, 200, 200, 180));
		setGraphicsEffect(drop_shadow);

		// Logo Icon
		m_about_icon = new QLabel(this);
		QPixmap about(resource::logo_rgb);
		about = about.scaledToWidth(resource::get_large_icon_size(), Qt::TransformationMode::SmoothTransformation);
		m_about_icon->setPixmap(about);
		m_about_icon->setObjectName("about_menu_icon");

		// Logo Title
		m_about_title = new QLabel(this);
		m_about_title->setText("About");
		m_about_title->setFont(title_font);
		m_about_title->setObjectName("about_menu_title");

		// Logo Caption
		m_about_caption = new QLabel(this);
		m_about_caption->setText(software::motto);
		m_about_caption->setFont(font);
		m_about_caption->setObjectName("about_menu_caption");

		// Site
		m_site_icon = new QLabel(this);
		m_site_icon->setFont(awesome);
		m_site_icon->setText(QString(fa::info_circle));
		m_site_icon->setObjectName("about_menu_icon");

		m_site_title = new QLabel(this);
		m_site_title->setText("Website");
		m_site_title->setFont(title_font);
		m_site_title->setObjectName("about_menu_title");

		m_site_caption = new QLabel(this);
		m_site_caption->setFont(font);
		m_site_caption->setText("Learn more at www.demonsaw.com");
		m_site_caption->setObjectName("about_menu_caption");

		// Wiki
		m_wiki_icon = new QLabel(this);
		m_wiki_icon->setFont(awesome);
		m_wiki_icon->setText(QString(fa::question_circle));
		m_wiki_icon->setObjectName("about_menu_icon");

		m_wiki_title = new QLabel(this);
		m_wiki_title->setText("Help");
		m_wiki_title->setFont(title_font);
		m_wiki_title->setObjectName("about_menu_title");

		m_wiki_caption = new QLabel(this);
		m_wiki_caption->setFont(font);
		m_wiki_caption->setText("Read the docs at www.titan.wiki");
		m_wiki_caption->setObjectName("about_menu_caption");

		// Container
		m_about_container = make_container(m_about_icon, m_about_title, m_about_caption);
		m_site_container = make_container(m_site_icon, m_site_title, m_site_caption);
		m_wiki_container = make_container(m_wiki_icon, m_wiki_title, m_wiki_caption);

		m_about_container->setObjectName("about_menu_container_top");

		// Layout
		m_layout = resource::get_vbox_layout(this);
		m_layout->addWidget(m_about_container);
		m_layout->addWidget(m_site_container);
		m_layout->addWidget(m_wiki_container);

		setObjectName("about_menu");
		setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		setMinimumHeight(192 * ratio);
		setMinimumWidth(280 * ratio);
		setLayout(m_layout);

		hide();
	}

	void help_widget::focusOutEvent(QFocusEvent* event)
	{
		if (!m_activator_button->hasFocus())
			toggle();
	}

	font_button* help_widget::make_button(const int icon, const char* tooltip)
	{
		auto button = new font_button(icon, this);
		button->setToolTip(tooltip);

		return button;
	}

	QWidget* help_widget::make_container(QLabel* icon, QLabel* title, QLabel* caption)
	{
		const auto hlayout = resource::get_hbox_layout();
		const auto padding = resource::get_icon_size() / 2;
		hlayout->setContentsMargins(padding, padding, padding, padding);
		hlayout->setSpacing(padding);

		auto vlayout = resource::get_vbox_layout();
		vlayout->addWidget(icon, 0, Qt::AlignCenter);
		vlayout->addWidget(new QLabel("", this), 0, Qt::AlignCenter);
		hlayout->addLayout(vlayout);

		vlayout = resource::get_vbox_layout();
		vlayout->addWidget(title, 0, Qt::AlignLeft | Qt::AlignVCenter);
		vlayout->addWidget(caption, 0, Qt::AlignLeft | Qt::AlignVCenter);
		hlayout->addLayout(vlayout);

		hlayout->addStretch(1);

		QWidget* container = new QWidget(this);
		container->setLayout(hlayout);
		container->setObjectName("about_menu_container");
		container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		return container;
	}

	void help_widget::mouseReleaseEvent(QMouseEvent* event)
	{
		if (event->button() == Qt::RightButton)
			return;

		if (m_about_container->underMouse())
		{
			const auto window = main_window::get();
			window->setFocus();

			about_dialog dlg(this);
			dlg.exec();
		}
		else if (m_site_container->underMouse())
		{
			const auto window = main_window::get();
			window->setFocus();

			QDesktopServices::openUrl(QUrl(software::website));
		}
		else if (m_wiki_container->underMouse())
		{
			const auto window = main_window::get();
			window->setFocus();

			QDesktopServices::openUrl(QUrl(software::help));
		}
	}

	void help_widget::paintEvent(QPaintEvent* event)
	{
		QStyleOption opt;
		opt.init(this);
		QPainter p(this);
		style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
	}

	void help_widget::parent_resized()
	{
		resize();
	}

	void help_widget::resize()
	{
		QRect geometry = parentWidget()->geometry();
		move(m_activator_button->pos().x(), m_activator_button->pos().y() + m_activator_button->height() + (resource::get_icon_padding() * 1.5));
	}

	void help_widget::toggle()
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
