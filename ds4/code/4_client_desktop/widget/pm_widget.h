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

#ifndef _EJA_PM_WIDGET_H_
#define _EJA_PM_WIDGET_H_

#include <QTextCursor>

#include "entity/entity_dock_widget.h"

class QAction;
class QPoint;
class QPushButton;
class QTimer;

namespace eja
{
	class emoji_widget;
	class entity;
	class entity_table_view;
	class pm_model;
	class text_edit_widget;

	class pm_widget final : public entity_dock_widget
	{
	private:
		// Action
		QAction* m_clear_action = nullptr;
		QAction* m_mute_action = nullptr;
		QAction* m_unmute_action = nullptr;
		QAction* m_remove_action = nullptr;
		QAction* m_close_action = nullptr;

		QTimer* m_resize_timer = nullptr;

		text_edit_widget* m_text = nullptr;
		QPushButton* m_enter_button = nullptr;
		QString m_enter_style;

		QPushButton* m_emoji = nullptr;
		emoji_widget* m_emoji_menu = nullptr;
		QTextCursor m_cursor;

		entity_table_view* m_table = nullptr;
		pm_model* m_model = nullptr;

		bool m_enabled = true;
		bool m_emoji_enabled = false;
		bool m_init = true;

	private:
		// Interface
		virtual void on_add() override;
		virtual void on_add(const std::shared_ptr<entity> entity) override;
		virtual void on_update() override;
		virtual void on_update(const std::shared_ptr<entity> entity) override;
		virtual void on_clear() override;

		// Utility
		void resize();
		void style_icon();
		void on_mute(const bool value);
		void on_timeout();

		// Emoji
		void show_emoji(bool show);
		void size_emoji();

		// Event
		virtual void closeEvent(QCloseEvent* event) override;
		virtual bool eventFilter(QObject* object, QEvent* event) override;
		virtual void resizeEvent(QResizeEvent* event) override;

		// Has
		bool has_client() const;

		// Is
		bool is_muted() const;

	private slots:
		void on_resize();
		void show_menu(const QPoint& point);
		void on_text_changed();
		void on_visibility_changed(bool visible);

	public:
		pm_widget(const std::shared_ptr<entity>& entity, QWidget* parent = nullptr);

		// Utility
		void auto_scroll();
	};
}

#endif
