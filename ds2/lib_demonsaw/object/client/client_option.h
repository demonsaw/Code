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

#ifndef _EJA_CLIENT_OPTION_
#define _EJA_CLIENT_OPTION_

#include <memory>
#include <set>
#include <string>

#include "object/option.h"
#include "object/io/folder.h"
#include "system/type.h"
#include "utility/default_value.h"
#include "utility/io/folder_util.h"

namespace eja
{
	class client_option : public option
	{
	protected:
		// Transfer
		std::string m_download_path;
		bool m_partial = default_client::partial;

		size_t m_max_searches = default_client::num_searches;
		size_t m_max_downloads = default_client::num_downloads;
		size_t m_max_uploads = default_client::num_uploads;		
		size_t m_max_retries = default_client::num_retries;

		// Priority		
		size_t m_priority_size = default_client::num_priority_size;
		std::string m_priority_ext;

		// Timeout
		size_t m_download_timeout = default_timeout::client::num_download;
		size_t m_upload_timeout = default_timeout::client::num_upload;
		size_t m_ping_timeout = default_timeout::client::num_ping;

		// Threads
		size_t m_download_threads = default_client::num_download_threads;
		size_t m_upload_threads = default_client::num_upload_threads;
		size_t m_thread_size = default_client::num_thread_size;

		// Chat
		bool m_chat_audio = default_chat::audio; 
		bool m_chat_visual = default_chat::visual;
		std::string m_chat_sound = default_chat::sound;
		std::string m_chat_timestamp = default_chat::timestamp;
		size_t m_chat_volume = default_chat::num_volume;
		size_t m_chat_history = default_chat::num_history;

		// Message
		bool m_message_audio = default_message::audio;
		bool m_message_visual = default_message::visual;
		std::string m_message_sound = default_message::sound;
		std::string m_message_timestamp = default_message::timestamp;
		size_t m_message_volume = default_message::num_volume;

		// Feature
		bool m_browse = default_client::action::browse; 
		bool m_search = default_client::action::search;
		bool m_transfer = default_client::action::transfer;
		bool m_chat = default_client::action::chat;
		bool m_message = default_client::action::message;
		bool m_motd = default_client::action::motd;

		// MOTD
		bool m_show_motd = true;

	public:
		using ptr = std::shared_ptr<client_option>;

	public:
		client_option() { }
		client_option(const std::string& id) : option(id) { }
		client_option(const char* id) : option(id) { }
		virtual ~client_option() override { }

		// Interface
		virtual void clear() override;

		// Utility
		bool has_priority_size() const { return m_priority_size > 0; }
		bool has_priority_ext() const { return !m_priority_ext.empty(); }

		bool has_download_path() const { return !m_download_path.empty(); }		
		bool has_partial() const { return m_partial; }

		bool has_chat_audio() const { return m_chat_audio; }
		bool has_chat_visual() const { return m_chat_visual; }		
		bool has_chat_sound() const { return !m_chat_sound.empty(); }

		bool has_message_audio() const { return m_message_audio; }
		bool has_message_visual() const { return m_message_visual; }
		bool has_message_sound() const { return !m_message_sound.empty(); }

		bool has_thread_size() const { return m_thread_size > 0; }

		// Mutator		
		void set_download_path(const std::string& download_path) { m_download_path = download_path; }
		void set_download_path(const char* download_path) { m_download_path = download_path; }
		void set_download_path() { m_download_path.clear(); }

		void set_partial(const bool partial) { m_partial = partial; }
		void set_max_searches(const size_t max_searches) { m_max_searches = max_searches; }
		void set_max_downloads(const size_t max_downloads) { m_max_downloads = max_downloads; }
		void set_max_uploads(const size_t max_uploads) { m_max_uploads = max_uploads; }
		void set_max_retries(const size_t max_retries) { m_max_retries = max_retries; }		

		void set_priority_size(const size_t priority_size) { m_priority_size = priority_size; }
		void set_priority_ext(const std::string& ext) { m_priority_ext = ext; }
		void set_priority_ext() { m_priority_ext.clear(); }

		void set_download_timeout(const size_t download_timeout) { m_download_timeout = download_timeout; }
		void set_upload_timeout(const size_t upload_timeout) { m_upload_timeout = upload_timeout; }
		void set_ping_timeout(const size_t ping_timeout) { m_ping_timeout = ping_timeout; }

		void set_download_threads(const size_t download_threads) { m_download_threads = download_threads; }
		void set_upload_threads(const size_t upload_threads) { m_upload_threads = upload_threads; }
		void set_thread_size(const size_t thread_size) { m_thread_size = thread_size; }

		void set_chat_sound(const std::string& chat_sound) { m_chat_sound = chat_sound; }
		void set_chat_sound(const char* chat_sound) { m_chat_sound = chat_sound; }
		void set_chat_sound() { m_chat_sound.clear(); }

		void set_chat_timestamp(const std::string& chat_timestamp) { m_chat_timestamp = chat_timestamp; }
		void set_chat_timestamp(const char* chat_timestamp) { m_chat_timestamp = chat_timestamp; }
		void set_chat_timestamp() { m_chat_timestamp = default_chat::timestamp; }

		void set_chat_audio(const bool chat_audio) { m_chat_audio = chat_audio; }
		void set_chat_visual(const bool chat_visual) { m_chat_visual = chat_visual; }
		void set_chat_volume(const size_t chat_volume) { m_chat_volume = chat_volume; }
		void set_chat_history(const size_t chat_history) { m_chat_history = chat_history; }

		void set_message_sound(const std::string& message_sound) { m_message_sound = message_sound; }
		void set_message_sound(const char* message_sound) { m_message_sound = message_sound; }
		void set_message_sound() { m_message_sound.clear(); }

		void set_message_timestamp(const std::string& message_timestamp) { m_message_timestamp = message_timestamp; }
		void set_message_timestamp(const char* message_timestamp) { m_message_timestamp = message_timestamp; }
		void set_message_timestamp() { m_message_timestamp = default_message::timestamp; }

		void set_message_audio(const bool message_audio) { m_message_audio = message_audio; }
		void set_message_visual(const bool message_visual) { m_message_visual = message_visual; }
		void set_message_volume(const size_t message_volume) { m_message_volume = message_volume; }

		void set_browse(const bool value) { m_browse = value; }
		void set_search(const bool value) { m_search = value; }
		void set_transfer(const bool value) { m_transfer = value; }		
		void set_chat(const bool value) { m_chat = value; }
		void set_message(const bool value) { m_message = value; }
		void set_motd(const bool value) { m_motd = value; }

		void show_motd(const bool value) { m_show_motd = value; }

		// Accessor 
		const std::string& get_download_path() const { return m_download_path; }
		size_t get_max_searches() const { return m_max_searches; }
		size_t get_max_downloads() const { return m_max_downloads; }
		size_t get_max_uploads() const { return m_max_uploads; }		
		size_t get_max_retries() const { return m_max_retries; }

		size_t get_priority_size() const { return m_priority_size; }
		const std::string& get_priority_ext() const { return m_priority_ext; }

		size_t get_download_timeout() const { return m_download_timeout; }
		size_t get_upload_timeout() const { return m_upload_timeout; }
		size_t get_ping_timeout() const { return m_ping_timeout; }

		size_t get_download_threads() const { return m_download_threads; }
		size_t get_upload_threads() const { return m_upload_threads; }
		size_t get_thread_size() const { return m_thread_size; }

		const std::string& get_chat_sound() const { return m_chat_sound; }
		const std::string& get_chat_timestamp() const { return m_chat_timestamp; }
		const size_t get_chat_volume() const { return m_chat_volume; }
		const size_t get_chat_history() const { return m_chat_history; }

		const std::string& get_message_sound() const { return m_message_sound; }
		const std::string& get_message_timestamp() const { return m_message_timestamp; }
		const size_t get_message_volume() const { return m_message_volume; }

		bool get_browse() const { return m_browse; }
		bool get_search() const { return m_search; }
		bool get_transfer() const { return m_transfer; }		
		bool get_chat() const { return m_chat; }
		bool get_message() const { return m_message; }
		bool get_motd() const { return m_motd; }

		bool show_motd() const { return m_show_motd; }

		// Static
		static ptr create() { return std::make_shared<client_option>(); }
		static ptr create(const client_option::ptr client_option) { return std::make_shared<eja::client_option>(*client_option); }
		static ptr create(const std::string& id) { return std::make_shared<client_option>(id); }
		static ptr create(const char* id) { return std::make_shared<client_option>(id); }
	};
}

#endif
