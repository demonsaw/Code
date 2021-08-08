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

#ifndef _EJA_STATUSBAR_WIDGET_H_
#define _EJA_STATUSBAR_WIDGET_H_

#include <QIcon>
#include <QStatusBar>

#include "entity/entity_callback.h"
#include "pane/statusbar_pane.h"

class QLabel;
class QPushButton;
class QTimer;

namespace eja
{
	class entity;

	class statusbar_widget final : public QStatusBar, public entity_callback
	{
		Q_OBJECT;

		enum pane : size_t { client, browse, chat, queued, download, upload, finished, router, max };

	private:
		statusbar_pane** m_pane = nullptr;

		QIcon m_active;
		QIcon m_inactive;		

		QPushButton* m_activity = nullptr;
		QLabel* m_author = nullptr;
		QTimer* m_timer = nullptr;

	private slots:
		void on_add();
		void on_remove() { on_add(); }
		void on_update();
		void on_clear();

		void on_about();
		void on_timeout();

	signals:
		void add();
		void remove();
		void update();
		void clear();

	public:
		explicit statusbar_widget(const std::shared_ptr<entity>& entity, QWidget* parent = nullptr);
		virtual ~statusbar_widget() override;

		// Get
		statusbar_pane* get_pane(const size_t index) const { return (index < pane::max) ? m_pane[index] : nullptr; }
	};
}

#endif