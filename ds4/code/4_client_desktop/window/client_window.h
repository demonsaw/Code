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

#ifndef _EJA_CLIENT_WINDOW_H_
#define _EJA_CLIENT_WINDOW_H_

#include <map>
#include <memory>

#include <QIcon>

#include "entity/entity_window.h"
#include "system/type.h"
#include "widget/statusbar_widget.h"

class QLineEdit;
class QPushButton;
class QTimer;
class QToolBar;

namespace eja
{
	class docked_tab_widget;
	class entity_controller;
	class entity_dock_widget;
	class help_widget;
	class menu_widget;
	class font_button;

	class client_window final : public entity_window
	{
		Q_OBJECT

	private:
		// Controller
		entity_controller* m_pm = nullptr;
		entity_controller* m_room = nullptr;
		entity_controller* m_search = nullptr;		

		// Window
		entity_dock_widget* m_browse = nullptr;
		entity_dock_widget* m_chat = nullptr;
		entity_dock_widget* m_client = nullptr;
		entity_dock_widget* m_download = nullptr;
		entity_dock_widget* m_error = nullptr;
		entity_dock_widget* m_finished = nullptr;
		entity_dock_widget* m_group = nullptr;
		entity_dock_widget* m_option = nullptr;
		entity_dock_widget* m_router = nullptr;
		entity_dock_widget* m_share = nullptr;
		entity_dock_widget* m_upload = nullptr;

		// Search 
		QToolBar* m_search_bar = nullptr;
		QLineEdit* m_search_box = nullptr;
		help_widget* m_about_widget = nullptr;
		menu_widget* m_hamburger_widget = nullptr;

		// Timer		
		QTimer* m_blink_timer = nullptr;
		QTimer* m_spam_timer = nullptr;

		// Button
		font_button* m_hamburger_button = nullptr;
		font_button* m_refresh_button = nullptr;
		QPushButton* m_logo_button = nullptr;
		QString m_refresh_style;

		// Statusbar
		statusbar_widget* m_statusbar = nullptr;

		QIcon m_demonsaw_icon;
		std::map<QString, std::shared_ptr<entity>> m_map;
		bool m_blinking_refresh = false;
		bool m_blink_blue = true;
		bool m_enabled = true;

	private:
		// Interface
		virtual void on_create() override; 
		virtual void on_update() override;
		virtual void on_clear(const std::shared_ptr<entity> entity) override;

		// Utility		
		void search();		
		void start_blink();
		void stop_blink();
		bool verify();

		// Event
		virtual bool eventFilter(QObject* watched, QEvent* event);
		virtual void showEvent(QShowEvent* event) override;
		virtual void resizeEvent(QResizeEvent* event) override { emit resize(); }

		// Init
		void style_tabbar(std::shared_ptr<entity> entity = nullptr);		
		void style_tabbar(QTabBar* tabbar, std::shared_ptr<entity> entity = nullptr);
		void style_tabbar(QTabBar* tabbar, const int index, std::shared_ptr<entity> entity = nullptr);

	private slots:
		virtual void on_start() override;
		virtual void on_stop() override;
		virtual void on_restart() override;

		void on_enter();		
		void on_raise(entity_dock_widget* widget = nullptr);
		void on_tabify(entity_dock_widget* widget);
		void on_visibility_changed(bool visible);

	public:
		client_window(const std::shared_ptr<entity>& entity, QWidget* parent = nullptr, Qt::WindowFlags flags = 0);

		// Is
		bool is_chat_visible(const void* ptr) const;
		bool is_chat_visible() const;

		bool is_pm_visible(const void* ptr) const;
		bool is_pm_visible(const std::shared_ptr<entity>& entity) const;		
		bool is_pm_visible() const;

		bool is_room_visible(const void* ptr) const; 
		bool is_room_visible(const std::shared_ptr<entity>& entity) const;
		bool is_room_visible() const;

		// Utility
		void close(docked_tab_widget* tab); 		
		void dock(entity_dock_widget* widget);
		void undock(entity_dock_widget* widget);

		void restore();		
		void update_color();
		void update_theme();

		// Set
		void set_statusbar(const bool value);

		// Get
		std::shared_ptr<entity> get_pm_entity() const;
		std::shared_ptr<entity> get_room_entity() const;

	signals:
		void resize();
		void raise(entity_dock_widget* widget);
		void tabify(entity_dock_widget* widget);
	};
}

#endif
