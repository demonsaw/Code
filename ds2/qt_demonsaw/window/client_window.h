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

#ifndef _EJA_CLIENT_WINDOW_
#define _EJA_CLIENT_WINDOW_

#include "entity/entity_action.h"
#include "entity/entity.h"
#include "entity/entity_window.h"
#include "model/client/client_browse_model.h"
#include "model/client/client_download_model.h"
#include "model/client/client_finished_model.h"
#include "model/client/client_group_model.h"
#include "model/client/client_transfer_model.h"
#include "model/client/client_share_model.h"
#include "model/client/client_upload_model.h"
#include "model/error_model.h"
#include "table/client/client_download_table.h"
#include "table/client/client_finished_table.h"
#include "table/client/client_group_table.h"
#include "table/client/client_transfer_table.h"
#include "table/client/client_upload_table.h"
#include "table/error_table.h"
#include "tree/client/client_browse_tree.h"
#include "tree/client/client_share_tree.h"
#include "window/client/client_chat_window.h"
#include "window/client/client_message_window.h"
#include "window/client/client_search_window.h"

namespace eja
{	
	enum class client_tab { browse, search, chat, message, group, share };
	enum class client_statusbar { browse, search, chat, message, group, share, download, upload, queue, finished, error, max };

	class client_window final : public entity_window
	{
		Q_OBJECT

	private:
		// Model
		client_browse_tree* m_browse_tree = nullptr;
		client_browse_model* m_browse_model = nullptr; 

		client_search_window* m_search_window = nullptr;
		
		client_chat_window* m_chat_window = nullptr;

		client_message_window* m_message_window = nullptr;

		client_group_table* m_group_table = nullptr; 
		client_group_model* m_group_model = nullptr;
		
		client_share_tree* m_share_tree = nullptr;
		client_share_model* m_share_model = nullptr;

		client_download_table* m_download_table = nullptr;
		client_download_model* m_download_model = nullptr;
		
		client_upload_table* m_upload_table = nullptr;
		client_upload_model* m_upload_model = nullptr;

		client_transfer_table* m_transfer_table = nullptr;
		client_transfer_model* m_transfer_model = nullptr;
		
		client_finished_table* m_finished_table = nullptr;
		client_finished_model* m_finished_model = nullptr;

		error_table* m_error_table = nullptr;
		error_model* m_error_model = nullptr;				

	private:
		// Utility
		virtual void create_tabs() override;
		virtual void create_dashboard() override;
		virtual void create_activity() override;		
		virtual void create_toolbars() override;
		virtual void create_statusbar(QWidget* parent = 0) override;
		virtual void create_layouts();

	public:
		client_window(const entity::ptr entity, QWidget* parent = 0, QWidget* statusbar = 0, entity_action* action = nullptr);
		virtual ~client_window() override { }

		// Interface
		virtual void update() override;
		void post_create();

		// Accessor
		error_table* get_error_table() const { return m_error_table; }
	};
}

#endif
