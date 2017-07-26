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

#ifndef _EJA_ROUTER_WINDOW_
#define _EJA_ROUTER_WINDOW_

#include "entity/entity_action.h"
#include "entity/entity.h"
#include "entity/entity_window.h"
#include "model/error_model.h"
#include "model/router/router_client_model.h"
#include "model/router/router_group_model.h"
#include "model/router/router_server_model.h"
#include "model/router/router_session_model.h"
#include "model/router/router_transfer_model.h"
#include "table/error_table.h"
#include "table/router/router_client_table.h"
#include "table/router/router_group_table.h"
#include "table/router/router_server_table.h"
#include "table/router/router_session_table.h"
#include "table/router/router_transfer_table.h"

namespace eja
{
	enum class router_tab { /*session,*/ client, router, group, transfer };
	enum class router_statusbar { session = 5, client, router, group, transfer, error, max };

	class router_window final : public entity_window
	{
		Q_OBJECT

	private:		
		router_client_model* m_client_model = nullptr;
		router_group_model* m_group_model = nullptr;		
		router_server_model* m_server_model = nullptr;
		//router_session_model* m_session_model = nullptr; 
		router_transfer_model* m_transfer_model = nullptr;
		error_model* m_error_model = nullptr;

		router_client_table* m_client_table = nullptr;		 
		router_group_table* m_group_table = nullptr;
		router_server_table* m_server_table = nullptr;
		//router_session_table* m_session_table = nullptr;
		router_transfer_table* m_transfer_table = nullptr;
		error_table* m_error_table = nullptr;

	private:
		// Utility
		virtual void create_tabs() override;
		virtual void create_dashboard() override;
		virtual void create_activity() override;
		virtual void create_toolbars() override;
		virtual void create_statusbar(QWidget* statusbar = 0) override;
		virtual void create_layouts();

	public:
		router_window(const entity::ptr entity, QWidget* parent = 0, QWidget* statusbar_parent = 0, entity_action* action = nullptr);
		virtual ~router_window() override { }

		// Interface
		virtual void update() override;
		void post_create();

		// Accessor
		error_table* get_error_table() const { return m_error_table; }
	};
}

#endif
