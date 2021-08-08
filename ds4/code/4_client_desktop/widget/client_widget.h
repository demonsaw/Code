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

#ifndef _EJA_CLIENT_WIDGET_H_
#define _EJA_CLIENT_WIDGET_H_

#include "entity/entity_dock_widget.h"

class QAction;
class QPoint;

namespace eja
{
	class client_model;
	class entity;
	class entity_tree_view_ex;
	
	class client_widget final : public entity_dock_widget
	{
		Q_OBJECT;

	private:		
		QAction* m_join_action = nullptr;
		QAction* m_message_action = nullptr;
		QAction* m_mute_action = nullptr;
		QAction* m_unmute_action = nullptr;
		QAction* m_refresh_action = nullptr;
		QAction* m_clear_action = nullptr;
		QAction* m_close_action = nullptr;

		entity_tree_view_ex* m_tree = nullptr;
		client_model* m_model = nullptr;

	private:
		// Interface
		virtual void on_add(const std::shared_ptr<entity> entity) override;
		virtual void on_remove(const std::shared_ptr<entity> entity) override;
		virtual void on_update() override;
		virtual void on_clear() override;

		virtual void on_double_click(const QModelIndex& index) override;

		// Utility
		void on_join(); 
		void on_message();
		void on_mute(const bool value);

		// Event
		virtual bool eventFilter(QObject* object, QEvent* event) override;

		// Has
		bool has_client() const;
		bool has_message() const;
		bool has_room() const;

		// Is
		bool is_owner(const std::shared_ptr<entity>& entity) const;
		bool is_muted() const;

	private slots:		
		void show_menu(const QPoint& point);

	public:
		client_widget(const std::shared_ptr<entity>& entity, QWidget* parent = nullptr);
	};
}

#endif
