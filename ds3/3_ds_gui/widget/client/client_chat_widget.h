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

#ifndef _EJA_CLIENT_CHAT_WIDGET_H_
#define _EJA_CLIENT_CHAT_WIDGET_H_

#include <map>
#include <memory>
#include <string>

#include "entity/entity_widget.h"
#include "thread/thread_safe.h"

class QAction;
class QPoint;
class QPushButton;

namespace eja
{
	class client_chat_model;
	class client_pm_widget;
	class entity;
	class entity_table_view;	
	class text_edit_widget;

	make_thread_safe_type(chat_widget_map, std::map<std::string, client_pm_widget*>);

	class client_chat_widget final : public entity_widget
	{
	private:
		enum mute_state { none, muted = 1, unmuted = 2 };

		// Action
		QAction* m_chat_action = nullptr;
		QAction* m_mute_action = nullptr;
		QAction* m_unmute_action = nullptr;
		QAction* m_remove_action = nullptr;
		QAction* m_update_action = nullptr;

		text_edit_widget* m_text = nullptr;
		QPushButton* m_enter_button = nullptr;

		entity_table_view* m_table = nullptr;
		client_chat_model* m_model = nullptr;

		bool m_enabled = true;
		chat_widget_map m_map;

	private:
		// Interface
		virtual void on_create(const std::shared_ptr<entity> entity) override;
		virtual void on_destroy(const std::shared_ptr<entity> entity) override;
		
		virtual void on_add() override;
		virtual void on_add(const std::shared_ptr<entity> entity) override;
		virtual void on_update() override;

		// Utility
		void on_chat();
		void on_mute(const bool mute);

		// Event
		virtual void showEvent(QShowEvent* event) override;

		// Has
		bool has_parent() const;

		// Get
		size_t get_mute();

	private slots:
		void show_menu(const QPoint& point);
		void on_double_click(const QModelIndex& index);
		void on_resize();
		void on_text_changed();		
		void on_visibility_changed(bool visible);

		void on_mute() { on_mute(true); }
		void on_unmute() { on_mute(false); }

	public:
		client_chat_widget(const std::shared_ptr<entity> entity, QWidget* parent = 0);
	};
}

#endif
