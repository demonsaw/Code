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

#include <memory>

#include "entity/entity_window.h"
#include "font/font_trigger.h"

namespace eja
{
	class client_status_pane;
	class font_button;

	class client_window final : public entity_window
	{
		Q_OBJECT

	private:
		// Action
		font_trigger* m_browse_button = nullptr;
		font_trigger* m_chat_button = nullptr;
		font_trigger* m_client_button = nullptr;
		font_trigger* m_download_button = nullptr;
		font_trigger* m_error_button = nullptr;
		font_trigger* m_group_button = nullptr;
		font_trigger* m_help_button = nullptr;
		font_trigger* m_message_button = nullptr;
		font_trigger* m_option_button = nullptr;
		font_trigger* m_queue_button = nullptr;
		font_button* m_search_button = nullptr;
		font_trigger* m_share_button = nullptr;
		font_trigger* m_transfer_button = nullptr;
		font_trigger* m_upload_button = nullptr;

		// Window
		entity_widget* m_browse = nullptr;
		entity_widget* m_chat = nullptr;
		entity_widget* m_client = nullptr;
		entity_widget* m_download = nullptr;
		entity_widget* m_error = nullptr;
		entity_widget* m_group = nullptr;
		entity_widget* m_help = nullptr;
		entity_widget* m_message = nullptr;
		entity_widget* m_option = nullptr;
		entity_widget* m_queue = nullptr; 
		entity_widget* m_share = nullptr;
		entity_widget* m_transfer = nullptr;
		entity_widget* m_upload = nullptr;

		// Statusbar
		font_trigger* m_status_button = nullptr;
		client_status_pane* m_status = nullptr;

	private:
		// Interface
		virtual void on_add(const std::shared_ptr<entity> entity) override;
		virtual void on_remove(const std::shared_ptr<entity> entity) override;
		virtual void on_update(const std::shared_ptr<entity> entity) override;

		void init_menu(font_trigger* button, QWidget* window, font_trigger::has_function has, font_trigger::set_function set);
		void init_menu(font_trigger* button, entity_widget* window, font_trigger::has_function has, font_trigger::set_function set);

		// Utility
		void on_search();

		// Event
		virtual bool eventFilter(QObject* object, QEvent* event) override;
		virtual void showEvent(QShowEvent* event) override;

	public:
		client_window(const std::shared_ptr<entity> entity, QWidget* parent = 0, Qt::WindowFlags flags = 0);

		// Utility
		void on_click(const int image) const;
	};
}

#endif
