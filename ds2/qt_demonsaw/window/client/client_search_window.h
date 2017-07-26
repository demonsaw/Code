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

#ifndef _EJA_CLIENT_SEARCH_WINDOW_
#define _EJA_CLIENT_SEARCH_WINDOW_

#include "model/client/client_search_model.h"
#include "table/client/client_search_table.h"
#include "widget/chat_text_edit.h"
#include "window/widget_window.h"

class QComboBox;
class QToolButton;

namespace eja
{
	class client_search_window final : public widget_window
	{
	private:
		bool m_enabled = true;
		QComboBox* m_filter = nullptr;
		chat_text_edit* m_keyword = nullptr;

		client_search_table* m_search_table = nullptr;
		client_search_model* m_search_model = nullptr;

	private:
		// Interface
		virtual void create(QWidget* parent = 0) override;
		virtual void layout(QWidget* parent = 0) override;
		virtual void signal(QWidget* parent = 0) override;

		// Event
		virtual void showEvent(QShowEvent* event) override;
		bool eventFilter(QObject* obj, QEvent* e);

	private slots:
		virtual void on_add() override;
		virtual void on_remove() override;
		virtual void on_clear() override;
		
		void on_click();
		void on_text_changed();
		void on_activated(int index);

	public:
		client_search_window(const entity::ptr entity, QWidget* parent = 0);
		virtual ~client_search_window() override { }

		// Interface
		virtual void update() override { m_search_table->update(); }
	};
}

#endif
