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

#include "component/ui_component.h"
#include "utility/value.h"

namespace eja
{
	// Constructor
	ui_component::ui_component()
	{
		m_browse = ui::browse;
		m_chat = ui::chat;
		m_client = ui::client;
		m_download = ui::download; 
		m_error = ui::error;
		m_group = ui::group;
		m_help = ui::help;
		m_message = ui::message;
		m_option = ui::option;
		m_queue = ui::queue;
		m_search = ui::search;
		m_session = ui::session;
		m_share = ui::share;
		m_status = ui::status;
		m_transfer = ui::transfer;
		m_upload = ui::upload;
	}

	// Operator
	ui_component& ui_component::operator=(const ui_component& comp)
	{
		if (this != &comp)
		{
			component::operator=(comp);

			m_browse = comp.m_browse;
			m_chat = comp.m_chat;
			m_client = comp.m_client;
			m_download = comp.m_download; 
			m_error = comp.m_error;
			m_group = comp.m_group;
			m_help = comp.m_help;
			m_message = comp.m_message;
			m_option = comp.m_option;
			m_queue = comp.m_queue;
			m_search = comp.m_search;
			m_session = comp.m_session;
			m_share = comp.m_share;
			m_status = comp.m_status;
			m_transfer = comp.m_transfer;
			m_upload = comp.m_upload;
		}

		return *this;
	}

	// Interface
	void ui_component::clear()
	{
		component::clear();
		
		m_browse = false;
		m_chat = false;
		m_client = false;		
		m_download = false; 
		m_error = false;
		m_group = false;
		m_help = false;
		m_message = false;
		m_option = false;
		m_queue = false;
		m_search = false;
		m_session = false;
		m_share = false;
		m_status = false;
		m_transfer = false;
		m_upload = false;
	}
}
