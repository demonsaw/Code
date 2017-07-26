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

#include "client_option.h"

namespace eja
{
	// Interface
	void client_option::clear()
	{
		option::clear();

		// Transfer
		m_download_path.clear();
		m_partial = default_client::partial;

		m_max_searches = default_client::num_searches;
		m_max_downloads = default_client::num_downloads;
		m_max_uploads = default_client::num_uploads;		
		m_max_retries = default_client::num_retries;

		// Priority
		m_priority_size = default_client::num_priority_size;
		m_priority_ext.clear();

		// Uploads
		m_max_uploads = default_client::num_uploads;
		m_max_retries = default_client::num_retries;

		// Timeout
		m_download_timeout = default_timeout::client::num_download;
		m_upload_timeout = default_timeout::client::num_upload;
		m_ping_timeout = default_timeout::client::num_ping;

		// Threads
		m_download_threads = default_client::num_download_threads;
		m_upload_threads = default_client::num_upload_threads;
		m_thread_size = default_client::num_thread_size;

		// Chat
		m_chat_audio = default_chat::audio; 
		m_chat_visual = default_chat::visual;
		m_chat_sound = default_chat::sound;
		m_chat_timestamp = default_chat::timestamp;
		m_chat_volume = default_chat::num_volume;
		m_chat_history = default_chat::num_history;

		// Message
		m_message_audio = default_message::audio;
		m_message_visual = default_message::visual;
		m_message_sound = default_message::sound;
		m_message_timestamp = default_message::timestamp;
		m_message_volume = default_message::num_volume;

		// API
		m_search = default_client::action::search;
		m_browse = default_client::action::browse;
		m_transfer = default_client::action::transfer;
		m_chat = default_client::action::chat;
		m_message = default_client::action::message;
		m_motd = default_client::action::motd;
	}
}
