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

#ifndef _EJA_CLIENT_SEARCH_WIDGET_H_
#define _EJA_CLIENT_SEARCH_WIDGET_H_

#include "entity/entity_widget.h"

class QAction;
class QLabel;
class QPushButton;

namespace eja
{
	class client_search_model;
	class entity;
	class entity_table_view;
	class text_edit_widget;

	class client_search_widget final : public entity_widget
	{
		Q_OBJECT

	private:
		QAction* m_download_action = nullptr;
		QAction* m_download_to_action = nullptr;
		QAction* m_remove_action = nullptr;
		QAction* m_update_action = nullptr;

		QLabel* m_total = nullptr;
		text_edit_widget* m_text = nullptr;
		QPushButton* m_enter_button = nullptr;

		entity_table_view* m_table = nullptr;
		client_search_model* m_model = nullptr;

		bool m_enabled = true;

	private:
		// Interface
		virtual void on_add(const std::shared_ptr<entity> entity) override;
		virtual void on_clear() override;
		virtual void on_update() override;

		// Event
		virtual void closeEvent(QCloseEvent* event) override;
		virtual void showEvent(QShowEvent* event) override;

		// Has
		bool has_file() const;

	private slots:
		void show_menu(const QPoint& point);

		void on_download();
		void on_download_to();
		void on_search();
		void on_text_changed();
		void on_visibility_changed(bool visible);

	public:
		client_search_widget(const std::shared_ptr<entity> entity, QWidget* parent = 0) : client_search_widget(entity, "", parent) { }
		client_search_widget(const std::shared_ptr<entity> entity, const QString& text, QWidget* parent = 0);

		// Utility
		void search() { emit on_search(); }
	};
}

#endif
