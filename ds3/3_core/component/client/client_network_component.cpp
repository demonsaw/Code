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

#include "component/client/client_network_component.h"
#include "utility/value.h"

namespace eja
{
	// Constructor
	client_network_component::client_network_component()
	{
		m_mode = network::mode;
		m_trust = network::trust;

		m_chunk = network::num_chunk_size;
		m_downloads = network::num_downloads;
		m_interval = network::interval;
		m_retries = network::num_retries;
		m_threads = network::min_threads;
		m_sockets = network::num_sockets;
		m_timeout = network::num_timeout;
		m_uploads = network::num_uploads;
		m_window = network::num_window;

		m_motd = network::motd;
		m_version = network::version;

		m_browse = network::browse;
		m_chat = network::chat;
		m_pm = network::pm;
		m_search = network::search;
		m_upload = network::upload;
	}

	// Operator
	client_network_component& client_network_component::operator=(const client_network_component& comp)
	{
		if (this != &comp)
		{
			component::operator=(comp);
			
			m_mode = comp.m_mode;
			m_redirect = comp.m_redirect;
			m_trust = comp.m_trust;

			m_chunk = comp.m_chunk;
			m_downloads = comp.m_downloads;
			m_interval = comp.m_interval;
			m_retries = comp.m_retries;			
			m_sockets = comp.m_sockets;
			m_threads = comp.m_threads;
			m_timeout = comp.m_timeout;
			m_uploads = comp.m_uploads;
			m_window = comp.m_window;

			m_motd = comp.m_motd;
			m_share = comp.m_share;
			m_version = comp.m_version;

			m_browse = comp.m_browse;
			m_chat = comp.m_chat;
			m_pm = comp.m_pm;
			m_search = comp.m_search;			
			m_upload = comp.m_upload;
		}

		return *this;
	}

	// Interface
	void client_network_component::clear()
	{
		component::clear();

		m_redirect.clear();
		m_mode = network_mode::none;
		m_trust = network_trust::none;		

		m_chunk = 0;
		m_downloads = 0;		
		m_interval = 0; 
		m_retries = 0;		
		m_sockets = 0;
		m_threads = 0;
		m_timeout = 0;
		m_uploads = 0;
		m_window = 0;

		m_motd = false;
		m_share = false;
		m_version = false;

		m_browse = false;
		m_chat = false;
		m_pm = false;
		m_search = false;		
		m_upload = false;
	}
}
