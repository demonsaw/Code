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

#ifndef _EJA_DOWNLOAD_WIDGET_H_
#define _EJA_DOWNLOAD_WIDGET_H_

#include <vector>

#include "entity/entity_dock_widget.h"

class QAction;
class QPoint;

namespace eja
{
	class download_model;
	class entity;
	class entity_table_view_ex;

	class download_widget final : public entity_dock_widget
	{
		Q_OBJECT;

	private:
		// Action
		QAction* m_file_action = nullptr;
		QAction* m_folder_action = nullptr;
		QAction* m_remove_action = nullptr;
		QAction* m_refresh_action = nullptr;
		QAction* m_close_action = nullptr;

		QAction* m_pause_action = nullptr;
		QAction* m_resume_action = nullptr;
		QAction* m_finish_action = nullptr;

		QAction* m_move_up_action = nullptr;
		QAction* m_move_down_action = nullptr;
		QAction* m_move_top_action = nullptr;
		QAction* m_move_bottom_action = nullptr;

		entity_table_view_ex* m_table = nullptr;
		download_model* m_model = nullptr;

		std::vector<std::shared_ptr<entity>> m_add_list;
		std::vector<std::shared_ptr<entity>> m_remove_list;
		bool m_moved = false;

	private:
		// Interface
		virtual void on_add(const std::shared_ptr<entity> entity) override;
		virtual void on_remove() override;
		virtual void on_update() override;
		virtual void on_clear() override;

		// Utility
		void finish();
		void pause();
		void resume();

		void move_up();
		void move_down();
		void move_top();
		void move_bottom();

		// Is
		bool is_paused() const;

	private slots:
		void show_menu(const QPoint& point);
		void on_transfer();

		void on_open_file();
		void on_open_folder();

	public:
		download_widget(const std::shared_ptr<entity>& entity, QWidget* parent = nullptr);
	};
}

#endif
