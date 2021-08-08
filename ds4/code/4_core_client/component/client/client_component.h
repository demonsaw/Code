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

#ifndef _EJA_CLIENT_COMPONENT_H_
#define _EJA_CLIENT_COMPONENT_H_

#include <deque>
#include <string>
#include <boost/filesystem.hpp>

#include "component/name_component.h"
#include "object/client.h"
#include "system/type.h"

namespace eja
{
	class client_list_component : public thread_safe_ex<component, std::deque<std::shared_ptr<entity>>>
	{
		make_factory(client_list_component);

		enum bits : size_t { none, expanded = bit(1) };

	public:
		client_list_component() { }

		// Interface
		virtual void clear() override;

		// Is
		bool is_expanded() const { return is_mode(bits::expanded); }

		// Set
		void set_expanded(const bool value) { set_mode(bits::expanded, value); }
	};

	class client_component final : public name_component
	{
		make_factory(client_component);

		enum bits : size_t { none, verified = bit(1), chat_alert = bit(2), chat_beep = bit(3), chat_notify = bit(4), join = bit(5), pm_alert = bit(6), pm_beep = bit(7), pm_notify = bit(8),
			motd = bit(9), mute = bit(10), poll = bit(11), room_alert = bit(12), room_beep = bit(13), room_notify = bit(14), share = bit(15), troll = bit(16), tunnel = bit(17), update = bit(18),
			chat_tray = bit(19), pm_tray = bit(20), room_tray = bit(21) };

	private:
		boost::filesystem::path m_path;
		std::string m_datestamp = "%m/%d/%G %I:%M:%S %p";
		std::string m_timestamp = "%I:%M:%S %p";

		std::string m_hash;
		std::string m_salt;

		std::string m_geometry;
		std::string m_state;

		// TODO: Constants should be defined centrally (not in value.cpp anymore)
		//
		//
		size_t m_buffer_size = 16;
		size_t m_chunk_size = 768 << 10;
		size_t m_downloads = 0;
		size_t m_uploads = 0;
		u64 m_size = 0;

	public:
		client_component();

		// Has
		bool has_path() const { return !m_path.empty(); }
		bool has_datestamp() const { return !m_datestamp.empty(); }
		bool has_timestamp() const { return !m_timestamp.empty(); }

		bool has_hash() const { return !m_hash.empty(); }
		bool has_salt() const { return !m_salt.empty(); }
		bool has_size() const { return m_size > 0; }

		bool has_buffer_size() const { return m_buffer_size > 0; }
		bool has_chunk_size() const { return m_chunk_size > 0; }
		bool has_downloads() const { return m_downloads > 0; }
		bool has_uploads() const { return m_uploads > 0; }

		bool has_geometry() const { return !m_geometry.empty(); }
		bool has_state() const { return !m_state.empty(); }

		// Is
		bool is_join() const { return is_mode(bits::join); }
		bool is_motd() const { return is_mode(bits::motd); }
		bool is_mute() const { return is_mode(bits::mute); }
		bool is_poll() const { return is_mode(bits::poll); }
		bool is_share() const { return is_mode(bits::share); }
		bool is_troll() const { return is_mode(bits::troll); }
		bool is_tunnel() const { return is_mode(bits::tunnel); }
		bool is_update() const { return is_mode(bits::update); }
		bool is_verified() const { return is_mode(bits::verified); }

		bool is_chat_alert() const { return is_mode(bits::chat_alert); }
		bool is_chat_beep() const { return is_mode(bits::chat_beep); }
		bool is_chat_notify() const { return is_mode(bits::chat_notify); }
		bool is_chat_tray() const { return is_mode(bits::chat_tray); }

		bool is_pm_alert() const { return is_mode(bits::pm_alert); }
		bool is_pm_beep() const { return is_mode(bits::pm_beep); }
		bool is_pm_notify() const { return is_mode(bits::pm_notify); }
		bool is_pm_tray() const { return is_mode(bits::pm_tray); }

		bool is_room_alert() const { return is_mode(bits::room_alert); }
		bool is_room_beep() const { return is_mode(bits::room_beep); }
		bool is_room_notify() const { return is_mode(bits::room_notify); }
		bool is_room_tray() const { return is_mode(bits::room_tray); }

		// Set
		void set_path(const boost::filesystem::path& path) { m_path = path; }
		void set_path(const std::wstring& path) { m_path = path; }
		void set_path(const std::string& path) { m_path = path; }
		void set_path(const char* path) { m_path = path; }
		void set_datestamp(const std::string& datestamp) { m_datestamp = datestamp; }
		void set_timestamp(const std::string& timestamp) { m_timestamp = timestamp; }

		void set_hash(const std::string& hash) { m_hash = hash; }
		void set_salt(const std::string& salt) { m_salt = salt; }

		virtual void set_size() { m_size = 0; }
		void set_size(const u64 size) { m_size = size; }
		void add_size(const u64 size) { m_size += size; }
		void sub_size(const u64 size) { m_size -= size; }

		void set_buffer_size(const size_t buffer_size) { m_buffer_size = buffer_size; }
		void set_chunk_size(const size_t chunk_size) { m_chunk_size = chunk_size; }
		void set_downloads(const size_t downloads) { m_downloads = downloads; }
		void set_uploads(const size_t uploads) { m_uploads = uploads; }

		void set_geometry(const std::string& geometry) { m_geometry = geometry; }
		void set_state(const std::string& state) { m_state = state; }
		using mode::set_mode;
		virtual void set_mode(const size_t bits) override;

		void set_join(const bool value) { set_mode(bits::join, value); }
		void set_motd(const bool value) { set_mode(bits::motd, value); }
		void set_mute(const bool value) { set_mode(bits::mute, value); }
		void set_poll(const bool value) { set_mode(bits::poll, value); }
		void set_share(const bool value) { set_mode(bits::share, value); }
		void set_troll(const bool value) { set_mode(bits::troll, value); }
		void set_tunnel(const bool value) { set_mode(bits::tunnel, value); }
		void set_update(const bool value) { set_mode(bits::update, value); }
		void set_verified(const bool value) { set_mode(bits::verified, value); }

		void set_chat_alert(const bool value) { set_mode(bits::chat_alert, value); }
		void set_chat_beep(const bool value) { set_mode(bits::chat_beep, value); }
		void set_chat_notify(const bool value) { set_mode(bits::chat_notify, value); }
		void set_chat_tray(const bool value) { set_mode(bits::chat_tray, value); }

		void set_pm_alert(const bool value) { set_mode(bits::pm_alert, value); }
		void set_pm_beep(const bool value) { set_mode(bits::pm_beep, value); }
		void set_pm_notify(const bool value) { set_mode(bits::pm_notify, value); }
		void set_pm_tray(const bool value) { set_mode(bits::pm_tray, value); }

		void set_room_alert(const bool value) { set_mode(bits::room_alert, value); }
		void set_room_beep(const bool value) { set_mode(bits::room_beep, value); }
		void set_room_notify(const bool value) { set_mode(bits::room_notify, value); }
		void set_room_tray(const bool value) { set_mode(bits::room_tray, value); }

		// Get
		virtual size_t get_mode() const override;
		const boost::filesystem::path& get_path() const { return m_path; }
		const std::string& get_datestamp() const { return m_datestamp; }
		const std::string& get_timestamp() const { return m_timestamp; }

		const std::string& get_hash() const { return m_hash; }
		const std::string& get_salt() const { return m_salt; }
		u64 get_size() const { return m_size; }

		size_t get_buffer_size() const { return m_buffer_size; }
		size_t get_chunk_size() const { return m_chunk_size; }
		size_t get_downloads() const { return m_downloads; }
		size_t get_uploads() const { return m_uploads; }

		const std::string& get_geometry() const { return m_geometry; }
		const std::string& get_state() const { return m_state; }
	};
}

#endif
