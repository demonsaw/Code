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

#ifndef _EJA_QT_APPLICATION_H_
#define _EJA_QT_APPLICATION_H_

#include <memory>
#include <boost/filesystem.hpp>

#include <QApplication>

#include "config/application.h"
#include "entity/entity.h"
#include "http/http_throttle.h"
#include "object/mode.h"
#include "system/type.h"

class QSharedMemory;

namespace eja
{
	class qt_application final : public QApplication, public application, public mode<size_t>
	{
		enum bits : size_t { none, auto_open = bit(1), statusbar = bit(2), user_colors = bit(3), timestamps = bit(4), system_tray = bit(5), all = (auto_open | statusbar | user_colors | timestamps | system_tray) };

	private:
		entity_list m_clients;
		http_download_throttle m_download_throttle;
		http_upload_throttle m_upload_throttle;

		QSharedMemory* m_shared_memory = nullptr;
		boost::filesystem::path m_theme;
		std::string m_geometry;
		std::string m_state;

		size_t m_active = 0;
		size_t m_font_size = 0;
		size_t m_tabs = 0;

	private:
		// Utility
		void error_add(const std::shared_ptr<entity>& entity) const;

		// Read
		using application::read;
		void read(const boost::filesystem::path& path);
		void read_global(const std::shared_ptr<cpptoml::table>& table);
		void read_client(const std::shared_ptr<cpptoml::table>& table, const std::shared_ptr<entity>& entity);
		void read_group(const std::shared_ptr<cpptoml::table>& table, const std::shared_ptr<entity>& entity);
		//void read_headers(const std::shared_ptr<cpptoml::table>& table, const std::shared_ptr<entity>& entity);
		void read_option(const std::shared_ptr<cpptoml::table>& table, const std::shared_ptr<entity>& entity);
		void read_routers(const std::shared_ptr<cpptoml::table>& table, const std::shared_ptr<entity>& entity);
		void read_session(const std::shared_ptr<cpptoml::table>& table, const std::shared_ptr<entity>& entity);
		void read_shares(const std::shared_ptr<cpptoml::table>& table, const std::shared_ptr<entity>& entity);
		void read_verified(const std::shared_ptr<cpptoml::table>& table, const std::shared_ptr<entity>& entity);

		// Write
		using application::write;
		void write(const boost::filesystem::path& path);
		void write_global(const std::shared_ptr<cpptoml::table>& table);
		void write_client(const std::shared_ptr<cpptoml::table>& table, const std::shared_ptr<entity>& entity);
		void write_group(const std::shared_ptr<cpptoml::table>& table, const std::shared_ptr<entity>& entity);
		//void write_headers(const std::shared_ptr<cpptoml::table>& table, const std::shared_ptr<entity>& entity;
		void write_option(const std::shared_ptr<cpptoml::table>& table, const std::shared_ptr<entity>& entity);
		void write_routers(const std::shared_ptr<cpptoml::table>& table, const std::shared_ptr<entity>& entity);
		void write_session(const std::shared_ptr<cpptoml::table>& table, const std::shared_ptr<entity>& entity);
		void write_shares(const std::shared_ptr<cpptoml::table>& table, const std::shared_ptr<entity>& entity);
		void write_verified(const std::shared_ptr<cpptoml::table>& table, const std::shared_ptr<entity>& entity);

		// Get
		boost::filesystem::path get_backup_path() const;

	public:
		qt_application(int& argc, char* argv[]);

		// Interface
		virtual void init() override;
		virtual void shutdown() override;
		virtual void clear() override;

		// Utility
		virtual bool empty() const override { return application::empty() || !has_clients(); }
		bool exists();
		void backup();

		// Read
		virtual void read() override;

		// Write
		virtual void write() override;

		// Has
		bool has_clients() const { return !m_clients.empty(); }
		bool has_font_size() const { return m_font_size > 0; }
		bool has_geometry() const { return !m_geometry.empty(); }
		bool has_state() const { return !m_state.empty(); }
		bool has_tabs() const { return m_tabs > 0; }
		bool has_theme() const { return !m_theme.empty(); }

		// /Is
		bool is_auto_open() const { return is_mode(bits::auto_open); }
		bool is_statusbar() const { return is_mode(bits::statusbar); }
		bool is_system_tray() const { return is_mode(bits::system_tray); }
		bool is_timestamps() const { return is_mode(bits::timestamps); }
		bool is_user_colors() const { return is_mode(bits::user_colors); }

		// Set
		void set_active(const size_t active) { m_active = active; }
		void set_geometry(const std::string& geometry) { m_geometry = geometry; }
		void set_state(const std::string& state) { m_state = state; }
		void set_tabs(const size_t tabs) { m_tabs = tabs; }

		void set_download_throttle() { m_download_throttle.set_max_size(); }
		void set_upload_throttle() { m_upload_throttle.set_max_size(); }

		void set_font_size() { m_font_size = 0; }
		void set_font_size(const size_t font_size) { m_font_size = font_size; }

		void set_theme();
		void set_theme(const std::wstring& theme);
		void set_theme(const std::string& theme);

		void set_auto_open(const bool value) { set_mode(bits::auto_open, value); }
		void set_statusbar(const bool value) { set_mode(bits::statusbar, value); }
		void set_system_tray(const bool value) { set_mode(bits::system_tray, value); }
		void set_timestamps(const bool value) { set_mode(bits::timestamps, value); }
		void set_user_colors(const bool value) { set_mode(bits::user_colors, value); }

		// Scale
		void scale();

		// Get
		const entity_list& get_clients() const { return m_clients; }
		entity_list& get_clients() { return m_clients; }

		size_t get_active() const { return m_active; }
		size_t get_font_size() const { return m_font_size; }
		size_t get_max_size() const;
		size_t get_size() const { return m_clients->size(); }

		const http_download_throttle& get_download_throttle() const { return m_download_throttle; }
		http_download_throttle& get_download_throttle() { return m_download_throttle; }

		const http_upload_throttle& get_upload_throttle() const { return m_upload_throttle; }
		http_upload_throttle& get_upload_throttle() { return m_upload_throttle; }

		const std::string& get_geometry() const { return m_geometry; }
		const std::string& get_state() const { return m_state; }
		const boost::filesystem::path& get_theme() const { return m_theme; }
	};
}

#endif
