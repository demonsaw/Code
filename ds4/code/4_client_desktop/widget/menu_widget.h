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

#ifndef _EJA_MENU_WIDGET_H_
#define _EJA_MENU_WIDGET_H_

#include <QWidget>

#include "entity/entity_widget.h"

class QLabel;
class QFocusEvent;
class QGridLayout;
class QHBoxLayout;
class QPaintEvent;
class QPushButton;
class QMouseEvent;
class QToolBar;
class QUrl;

namespace eja
{
	class font_button;

	class menu_widget final : public entity_widget
	{
		Q_OBJECT

	private:
		QGridLayout* m_layout = nullptr;

		// Logo
		QHBoxLayout* m_logo_layout = nullptr;
		QToolBar* m_logo_toolbar = nullptr;
		font_button* m_activator_button = nullptr;
		QUrl* m_url = nullptr;

		// Button
		font_button* m_chat_button = nullptr;
		font_button* m_client_button = nullptr;
		font_button* m_browse_button = nullptr;
		font_button* m_download_button = nullptr;
		font_button* m_errors_button = nullptr;
		font_button* m_finished_button = nullptr;
		font_button* m_option_button = nullptr;
		font_button* m_router_button = nullptr;
		font_button* m_search_button = nullptr;
		font_button* m_security_button = nullptr;
		font_button* m_share_button = nullptr;
		font_button* m_upload_button = nullptr;

		QPushButton* m_about_button = nullptr;
		font_button* m_site_button = nullptr;
		font_button* m_wiki_button = nullptr;
		font_button* m_save_button = nullptr;

		// Label
		QLabel* m_browse_label = nullptr;
		QLabel* m_chat_label = nullptr;
		QLabel* m_client_label = nullptr;
		QLabel* m_download_label = nullptr;
		QLabel* m_errors_label = nullptr;
		QLabel* m_finished_label = nullptr;
		QLabel* m_option_label = nullptr;
		QLabel* m_router_label = nullptr;
		QLabel* m_search_label = nullptr;
		QLabel* m_security_label = nullptr;
		QLabel* m_share_label = nullptr;
		QLabel* m_upload_label = nullptr;

		// Container
		QWidget* m_browse_container = nullptr;
		QWidget* m_chat_container = nullptr;
		QWidget* m_client_container = nullptr;
		QWidget* m_download_container = nullptr;
		QWidget* m_errors_container = nullptr;
		QWidget* m_finished_container = nullptr;
		QWidget* m_option_container = nullptr;
		QWidget* m_router_container = nullptr;
		QWidget* m_search_container = nullptr;
		QWidget* m_security_container = nullptr;
		QWidget* m_share_container = nullptr;
		QWidget* m_upload_container = nullptr;

	private:
		font_button* make_button(const int icon, const char* tooltip = nullptr, const char* name = "hamburger_button");
		font_button* make_button_2(const int icon, const char* tooltip = nullptr, const char* name = "hamburger_button");
		QLabel* make_label(const char* text);
		QWidget* make_container(font_button* button, QLabel* label, const char* name = "hamburger_container");

		// Event
		virtual void focusOutEvent(QFocusEvent* event) override;
		virtual void paintEvent(QPaintEvent* event) override;
		virtual void mouseReleaseEvent(QMouseEvent* event) override;

		void resize();
		void signal();

	private slots:
		void about() const;
		void site() const;
		void help() const;
		void save() const;

	public:
		menu_widget(const std::shared_ptr<entity>& entity, font_button* button, QWidget* parent = nullptr);
		static const size_t s_min_height = 416;
		static const size_t s_min_width = 282;
		static const size_t s_right_margin = 8;

	public slots:
		void toggle();
		void parent_resized();

	signals:
		void on_browse();
		void on_chat();
		void on_client();
		void on_download();
		void on_error();
		void on_finished();
		void on_group();
		void on_option();
		void on_router();
		void on_search();
		void on_share();
		void on_upload();
	};
}

#endif
