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

#ifndef _EJA_ROUTER_SESSION_WIDGET_H_
#define _EJA_ROUTER_SESSION_WIDGET_H_

#include "entity/entity_widget.h"

class QAction;
class QPoint;

namespace eja
{
	class entity;
	class entity_table_view;
	class router_session_model;

	class router_session_widget final : public entity_widget
	{
	private:
		// Action
		QAction* m_update_action = nullptr;

		// Table
		entity_table_view* m_table = nullptr;
		router_session_model* m_model = nullptr;

	private:
		// Interface
		virtual void on_update() override;

	private slots:
		void show_menu(const QPoint& point);

	public:
		router_session_widget(const std::shared_ptr<entity> entity, QWidget* parent = 0);
	};
}

#endif
