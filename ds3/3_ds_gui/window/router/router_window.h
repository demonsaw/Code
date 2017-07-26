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

#ifndef _EJA_ROUTER_WINDOW_H_
#define _EJA_ROUTER_WINDOW_H_

#include <memory>

#include "entity/entity_window.h"
#include "font/font_trigger.h"

namespace eja
{
	class router_window final : public entity_window
	{
		Q_OBJECT

	private:
		// Action
		font_trigger* m_client_button = nullptr;
		font_trigger* m_download_button = nullptr;
		font_trigger* m_error_button = nullptr;
		font_trigger* m_group_button = nullptr;
		//font_trigger* m_help_button = nullptr;
		font_trigger* m_option_button = nullptr;
		font_trigger* m_session_button = nullptr;
		font_trigger* m_transfer_button = nullptr;

		// Window		
		entity_widget* m_client = nullptr;
		entity_widget* m_download = nullptr;
		entity_widget* m_error = nullptr; 
		entity_widget* m_group = nullptr;
		//entity_widget* m_help = nullptr;
		entity_widget* m_option = nullptr;
		entity_widget* m_session = nullptr;
		entity_widget* m_transfer = nullptr;

	private:
		// Interface
		void init_menu(font_trigger* button, entity_widget* window, font_trigger::has_function has, font_trigger::set_function set);

		// Event
		virtual bool eventFilter(QObject* object, QEvent* event) override;
		virtual void showEvent(QShowEvent* event) override;

	public:
		router_window(const std::shared_ptr<entity> entity, QWidget* parent = 0, Qt::WindowFlags flags = 0);
	};
}

#endif
