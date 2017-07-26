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

#ifndef _EJA_CLIENT_FINISHED_TABLE_
#define _EJA_CLIENT_FINISHED_TABLE_

#include <QWidget>

#include "model/client/client_finished_model.h"
#include "window/table_window.h"

class QAction;

namespace eja
{
	class client_finished_table final : public table_window
	{
	private:
		QAction* m_file_action = nullptr;
		QAction* m_folder_action = nullptr;
		QAction* m_resume_action = nullptr;
		QAction* m_remove_action = nullptr;
		QAction* m_clear_action = nullptr;

	private:
		// Interface
		virtual void create(QWidget* parent = 0) override;
		virtual void layout(QWidget* parent = 0) override;
		virtual void signal(QWidget* parent = 0) override;

		// Utility
		bool resumable() const { return resumable(selectionModel()->selectedIndexes()); }
		bool resumable(const QModelIndexList& selected) const;

	private slots:
		virtual void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) override;				
		void show_menu(const QPoint& point);
		
		virtual void double_click() override { open_file(); }
		void open_file();
		void open_folder();

		void resume();

	public:
		client_finished_table(QWidget* parent = 0);
		virtual ~client_finished_table() override { }

		// Accessor
		virtual int get_sort_column() const override { return client_finished_model::column::file; }
	};
}

#endif
