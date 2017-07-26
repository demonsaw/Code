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

#ifndef _EJA_CLIENT_MESSAGE_WINDOW_
#define _EJA_CLIENT_MESSAGE_WINDOW_

#include <QModelIndex>
#include <QWidget>

#include "entity/entity.h"
#include "model/client/client_client_model.h"
#include "model/client/client_message_model.h"
#include "table/client/client_client_table.h"
#include "table/client/client_message_table.h"
#include "widget/chat_text_edit.h"
#include "window/widget_window.h"
#include "window/window.h"

class QToolButton;

namespace eja
{
	//  [entity]
	//  -> chat_component
	//  -> client_component
	//  -> color_component
	//  -> time_component

	class client_message_window final : public widget_window
	{
		Q_OBJECT

	private:
		size_t m_size = 0; 
		bool m_enabled = true;
		entity::ptr m_selected_entity = nullptr;

		chat_text_edit* m_text = nullptr;
		client_message_table* m_message_table = nullptr;
		client_message_model* m_message_model = nullptr;	

		client_client_table* m_client_table = nullptr;
		client_client_model* m_client_model = nullptr;

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

		virtual void on_clear(const entity::ptr entity) override;
		virtual void on_refresh(const entity::ptr entity) override;

		void on_click();		
		void on_list_clicked(const QModelIndex& index);
		void on_tab_clicked(int index); 
		void on_text_changed();

	signals:
		void alert();

	public:
		client_message_window(const entity::ptr entity, QWidget* parent = 0);
		virtual ~client_message_window() override { }

		// Using
		using widget_window::init;
		using widget_window::add;
		using widget_window::remove;

		// Interface
		virtual void init() override;
		virtual void update() override;

		virtual void refresh() override;

		// Callback
		virtual void add(const entity::ptr entity) override;
		virtual void remove(const entity::ptr entity) override;
		virtual void clear(const entity::ptr entity) override;		

		// Utility
		bool selected() const;
		void post_create();

		// Accessor
		entity::ptr get_selected() const;
		size_t get_selected_count() const;

		entity::ptr get_entity() const { return get_entity(m_client_table->currentIndex()); }
		entity::ptr get_entity(const QModelIndex& index) const;
	};
}

#endif
