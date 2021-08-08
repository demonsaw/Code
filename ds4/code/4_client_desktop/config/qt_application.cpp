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

#include <cassert>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <QFile>
#include <QFont>
#include <QFontDatabase>
#include <QIcon>
#include <QMessageBox>
#include <QSharedMemory>
#include <QStandardPaths>
#include <QStyleFactory>
#include <QSystemSemaphore>
#include <QTextCodec>

#include "component/error_component.h"
#include "component/pixmap_component.h"
#include "component/callback/callback.h"
#include "component/callback/callback_service_component.h"
#include "component/client/client_component.h"
#include "component/client/client_service_component.h"
#include "component/group/group_component.h"
#include "component/group/group_option_component.h"
#include "component/io/file_component.h"
#include "component/io/folder_component.h"
#include "component/io/share_component.h"
#include "component/router/router_component.h"
#include "component/session/session_component.h"
#include "component/session/session_option_component.h"
#include "component/verified/verified_option_component.h"

#include "config/qt_application.h"
#include "config/toml.h"
#include "entity/entity.h"
#include "factory/client_factory.h"
#include "http/http.h"
#include "http/http_socket.h"
#include "object/download.h"
#include "object/upload.h"
#include "proxy/unfocus_proxy.h"
#include "resource/resource.h"
#include "security/checksum/xxhash.h"
#include "security/filter/hex.h"
#include "thread/thread_info.h"
#include "utility/value.h"
#include "utility/io/file_util.h"

namespace eja
{
	// Constructor
	qt_application::qt_application(int& argc, char* argv[]) : QApplication(argc, argv), application(argc, argv), mode<size_t>(all)
	{
		assert(thread_info::main());

		// Font
		QFontDatabase::addApplicationFont(resource::font);
		QFontDatabase::addApplicationFont(resource::font_bold);
		QFontDatabase::addApplicationFont(resource::font_mono);
		QFontDatabase::addApplicationFont(resource::font_awesome);

#if !_WIN32 && !_MACX
		QFontDatabase::addApplicationFont(resource::font_emoji);
#endif
		// Locale
		QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

		// Meta
		qRegisterMetaType<entity::ptr>("entity::ptr");
		qRegisterMetaType<std::shared_ptr<entity>>("std::shared_ptr<entity>");

		// UI
		m_font_size = ui::num_font_size;

		setStyle(new unfocus_proxy);
		set_theme();
	}

	// Interface
	void qt_application::init()
	{
		assert(thread_info::main());

		application::init();

		for (const auto& entity : m_clients)
		{
			// Callback
			const auto callback = entity->get<callback_service_component>();
			DEBUG_ONLY(callback->add(callback::error | callback::add, [&](const entity::ptr& entity) { error_add(entity); });)
			callback->start();

			if (entity->enabled())
			{
				// Service
				const auto client_service = entity->get<client_service_component>();
				client_service->start();
			}
		}
	}

	void qt_application::shutdown()
	{
		assert(thread_info::main());

		for (const auto& entity : m_clients)
		{
			if (entity->enabled())
			{
				// Service
				const auto client_service = entity->get<client_service_component>();
				client_service->stop();
			}

			// NOTE: Give as much time as possible for the async_quit to finish
			//
			// Callback
			//const auto callback = entity->get<callback_service_component>();
			//callback->stop();
		}

		application::shutdown();
	}

	void qt_application::clear()
	{
		assert(thread_info::main());

		m_clients.clear();

		application::clear();
	}

	// Utility
	bool qt_application::exists()
	{
		if (is_system_tray())
		{
			// Single instance
			QSystemSemaphore sema("demonsaw.lock", 1);
			sema.acquire();

#ifndef Q_OS_WIN32
			// On linux/unix shared memory is not freed upon crash. So if there is any trash from previous instance, clean it
			QSharedMemory nix_fix_shmem("demonsaw.shared");
			if (nix_fix_shmem.attach())
				nix_fix_shmem.detach();
#endif
			bool running = false;
			m_shared_memory = new QSharedMemory("demonsaw.shared", this);

			if (m_shared_memory->attach())
				running = true;
			else
				m_shared_memory->create(1);

			sema.release();

			if (running)
			{
				const auto text = "Demonsaw is already running.\r\nDo you really want to start another instance?";
				const auto result = QMessageBox::question(nullptr, software::name, text, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
				if (result == QMessageBox::No)
					return true;
			}
		}

		return false;
	}

	void qt_application::scale()
	{
		assert(thread_info::main());

		// Get Font Size
		m_font_size = get_font_size();

		// Resource
		resource::init(m_font_size);

		// Font
		QFont font = QFont(font_family::main);
		font.setPixelSize(resource::get_font_size());
		setFont(font);
	}

	void qt_application::error_add(const entity::ptr& entity) const
	{
		const auto error = entity->get<error_component>();
		std::cerr << error->get_text() << std::endl;
	}

	// Read
	void qt_application::read()
	{
		assert(thread_info::main());

		if (m_path.empty())
			return;

		if (file_util::exists(m_path) && !file_util::empty(m_path))
			read(m_path);

		// Backup
		if (empty())
		{
			const auto path = get_backup_path();
			if (file_util::exists(path) && !file_util::empty(path))
				read(path);
		}
	}

	void qt_application::read(const boost::filesystem::path& path)
	{
		assert(thread_info::main());

		clear();

		// Tabs
		const auto root_node = cpptoml::parse_file(path.string());
		const auto global_node = root_node->get_table(toml::global);
		if (global_node)
			read_ex<size_t>(global_node, toml::max_tabs, [&](const size_t value) { set_tabs(clamp(ui::min_tabs, value, ui::max_tabs, ui::num_tabs)); });

		// Clients
		const auto client_nodes = root_node->get_table_array(toml::client);

		if (client_nodes)
		{
			for (const auto& client_node : *client_nodes)
			{
				const auto entity = client_factory::create_client();

				read_client(client_node, entity);
				read_group(client_node, entity);
				//read_headers(client_node, entity);
				read_option(client_node, entity);
				read_routers(client_node, entity);
				read_session(client_node, entity);
				read_shares(client_node, entity);
				read_verified(client_node, entity);

				m_clients.push_back(entity);

				// Max
				if (m_clients->size() >= get_max_size())
					break;
			}
		}

		// Global
		read_global(root_node);

		// Active
		if (m_active >= m_clients.size())
			m_active = 0;
	}

	// Read
	void qt_application::read_global(const std::shared_ptr<cpptoml::table>& table)
	{
		assert(thread_info::main());

		const auto global_node = table->get_table(toml::global);
		if (global_node)
		{
			read_ex<bool>(global_node, toml::auto_open, [&](const bool value) { set_auto_open(value); });
			read_ex<bool>(global_node, toml::statusbar, [&](const bool value) { set_statusbar(value); });
			read_ex<bool>(global_node, toml::system_tray, [&](const bool value) { set_system_tray(value); });
			read_ex<bool>(global_node, toml::timestamps, [&](const bool value) { set_timestamps(value); });
			read_ex<bool>(global_node, toml::user_colors, [&](const bool value) { set_user_colors(value); });

			read_ex<size_t>(global_node, toml::active_tab, [&](const size_t value) { set_active(value); });
			read_ex<size_t>(global_node, toml::font_size, [&](const size_t value) { set_font_size(clamp(ui::min_font_size, value, ui::max_font_size, ui::num_font_size)); });
#if _WSTRING
			read<std::string>(global_node, toml::theme, [&](const std::string& value) { set_theme(utf::to_utf16(value)); });
#else
			read<std::string>(global_node, toml::theme, [&](const std::string& value) { set_theme(value); });
#endif
			// Transfer
			read_ex<size_t>(global_node, toml::download_speed, [&](const size_t value) { m_download_throttle.set_max_size(clamp(download::min_throttle_size, (mb_to_b(value) >> 3), mb_to_b(download::max_throttle_size))); });
			read_ex<size_t>(global_node, toml::upload_speed, [&](const size_t value) { m_upload_throttle.set_max_size(clamp(upload::min_throttle_size, (mb_to_b(value) >> 3), mb_to_b(upload::max_throttle_size))); });

			// Geometry
			string_deque geometries;
			const auto geometry_node = global_node->get_array(toml::qt_geometry);
			if (geometry_node)
			{
				for (const auto& it : *geometry_node)
				{
					const auto value = it->as<std::string>();
					const auto geometry = static_cast<std::string>(value->get());
					geometries.push_back(geometry);
				}

				if (!geometries.empty())
					m_geometry = geometries.front();
			}

			// State
			string_deque states;
			const auto state_node = global_node->get_array(toml::qt_state);
			if (state_node)
			{
				for (const auto& it : *state_node)
				{
					const auto value = it->as<std::string>();
					const auto state = static_cast<std::string>(value->get());
					states.push_back(state);
				}

				if (!states.empty())
					m_state = states.front();
			}

			// Client
			size_t i = 1;
			for (const auto& e : m_clients)
			{
				const auto client = e->get<client_component>();

				if (i < geometries.size())
					client->set_geometry(geometries[i]);

				if (i < states.size())
					client->set_state(states[i]);

				++i;
			}
		}
	}

	void qt_application::read_client(const std::shared_ptr<cpptoml::table>& table, const entity::ptr& entity)
	{
		assert(thread_info::main());

		// Entity
		read<bool>(table, toml::enabled, [&](const bool value) { entity->set_enabled(value); });

		// Service
		size_t threads = client_service_component::num_threads;
		read_ex<size_t>(table, toml::threads, [&](const size_t value) { threads = value; });

		const auto client_service = entity->get<client_service_component>();
		client_service->set_threads(clamp(client_service_component::min_threads, threads, client_service_component::max_threads, client_service_component::num_threads));

		const auto router_service = entity->get<router_service_component>();
		router_service->set_threads(clamp(router_service_component::min_threads, threads, router_service_component::max_threads, router_service_component::num_threads));

		// Client
		const auto client = entity->get<client_component>();
		read<std::string>(table, toml::name, [&](const std::string& value) { client->set_name((!value.empty() && (value.size() <= io::max_name_size)) ? value : client::name); });
		read<std::string>(table, toml::color, [&](const std::string& value)
		{
			if (value.size() <= io::max_color_size)
			{
				client->set_color(std::stoul(value, nullptr, 16));
				client->set_alpha(0xFF);
			}
		});
	}

	void qt_application::read_group(const std::shared_ptr<cpptoml::table>& table, const entity::ptr& entity)
	{
		assert(thread_info::main());

		const auto group_nodes = table->get_table_array(toml::group);
		if (group_nodes)
		{
			const auto group_list = entity->get<group_list_component>();

			for (const auto& group_node : *group_nodes)
			{
				// Entity
				const auto e = client_factory::create_group_option(entity);
				read<bool>(group_node, toml::enabled, [&](const bool value) { e->set_enabled(value); });

				// Option
				const auto option = e->get<group_option_component>();
				read<std::string>(group_node, toml::entropy, [&](const std::string& value) { option->set_entropy(value); });
				read<double>(group_node, toml::percent, [&](const double value) { option->set_percent(value); });

				read<std::string>(group_node, toml::cipher, [&](const std::string& value) { option->set_cipher(value); });
				read_ex<size_t>(group_node, toml::key_size, [&](const size_t value) { option->set_key_size(value); });

				read<std::string>(group_node, toml::hash, [&](const std::string& value) { option->set_hash(value); });
				read_ex<size_t>(group_node, toml::iterations, [&](const size_t value) { option->set_iterations(value); });
				read<std::string>(group_node, toml::salt, [&](const std::string& value) { option->set_salt(value); });

				// Components
				e->add<pixmap_component>();

				// List
				group_list->push_back(e);
			}
		}
	}

	void qt_application::read_option(const std::shared_ptr<cpptoml::table>& table, const entity::ptr& entity)
	{
		assert(thread_info::main());

		const auto option_node = table->get_table(toml::option);
		if (option_node)
		{
			// Client
			const auto client = entity->get<client_component>();
			read_ex<size_t>(option_node, toml::mode, [&](const size_t value) { client->set_mode(value); });
			read<std::string>(option_node, toml::datestamp, [&](const std::string& value) { client->set_datestamp(value); });
			read<std::string>(option_node, toml::timestamp, [&](const std::string& value) { client->set_timestamp(value); });

			read<std::string>(option_node, toml::hash, [&](const std::string& value) { client->set_hash(value); });
			read<std::string>(option_node, toml::salt, [&](const std::string& value) { client->set_salt(value); });

			read_ex<size_t>(option_node, toml::buffer_size, [&](const size_t value) { client->set_buffer_size(clamp(network::min_buffer_size, value, network::max_buffer_size, network::num_buffer_size)); });
			read_ex<size_t>(option_node, toml::chunk_size, [&](const size_t value) { client->set_chunk_size(clamp(network::min_chunk_size, value, network::max_chunk_size, network::num_chunk_size)); });
			read_ex<size_t>(option_node, toml::max_downloads, [&](const size_t value) { client->set_downloads(clamp(download::min_threads, value, download::max_threads, download::num_threads)); });
			read_ex<size_t>(option_node, toml::max_uploads, [&](const size_t value) { client->set_uploads(clamp(upload::min_threads, value, upload::max_threads, upload::num_threads)); });
#if _WSTRING
			read<std::string>(option_node, toml::path, [&](const std::string& value) { client->set_path(utf::to_utf16(value)); });
#else
			read<std::string>(option_node, toml::path, [&](const std::string& value) { client->set_path(value); });
#endif
		}
	}

	void qt_application::read_routers(const std::shared_ptr<cpptoml::table>& table, const entity::ptr& entity)
	{
		assert(thread_info::main());

		bool enabled = false;
		const auto router_list = entity->get<router_list_component>();
		const auto router_nodes = table->get_table_array(toml::router);

		if (router_nodes)
		{
			for (const auto& router_node : *router_nodes)
			{
				// Entity
				const auto e = client_factory::create_router(entity);
				e->disable();

				read<bool>(router_node, toml::enabled, [&](const bool value) { e->set_enabled(!enabled ? (enabled = value) : false); });

				// Router
				const auto router = e->get<router_component>();
				read<std::string>(router_node, toml::name, [&](const std::string& value) { router->set_name(value); });
				read<std::string>(router_node, toml::address, [&](const std::string& value) { router->set_address(boost::trim_copy(value)); });
				read_ex<u16>(router_node, toml::port, [&](const u16 value) { router->set_port(value); });
				read<std::string>(router_node, toml::password, [&](const std::string& value) { router->set_password((value.size() <= io::max_passphrase_size) ? value : ""); });

				// List
				router_list->push_back(e);

				if (e->enabled())
					router_list->set_entity(e);
			}
		}

		// Default
		if (!router_list->empty())
		{
			// Enable the first router
			if (!enabled)
			{
				router_list->set_entity();
				const auto e = router_list->front();
				e->enable();
			}
		}
		else
		{
			// Entity
			const auto e = client_factory::create_router(entity);

			// Component
			const auto router = e->get<router_component>();
			router->set_address(router::address);
			router->set_port(router::port);

			const auto router_name = boost::str(boost::format("%s 1") % router::name);
			router->set_name(router_name);

			router_list->push_back(e);
			router_list->set_entity();
		}
	}

	void qt_application::read_session(const std::shared_ptr<cpptoml::table>& table, const entity::ptr& entity)
	{
		assert(thread_info::main());

		const auto session_node = table->get_table(toml::session);
		if (session_node)
		{
			const auto option = entity->get<session_option_component>();
			read<std::string>(session_node, toml::algorithm, [&](const std::string& value) { option->set_algorithm(value); });
			read_ex<size_t>(session_node, toml::prime_size, [&](const size_t value) { option->set_prime_size(value); });
			read<std::string>(session_node, toml::cipher, [&](const std::string& value) { option->set_cipher(value); });
			read_ex<size_t>(session_node, toml::key_size, [&](const size_t value) { option->set_key_size(value); });
			read<std::string>(session_node, toml::hash, [&](const std::string& value) { option->set_hash(value); });
			read_ex<size_t>(session_node, toml::iterations, [&](const size_t value) { option->set_iterations(value); });
			read<std::string>(session_node, toml::salt, [&](const std::string& value) { option->set_salt(value); });
		}
	}

	void qt_application::read_shares(const std::shared_ptr<cpptoml::table>& table, const entity::ptr& entity)
	{
		assert(thread_info::main());

		const auto share_list = entity->get<share_list_component>();
		const auto share_nodes = table->get_table_array(toml::share);

		if (share_nodes)
		{
			for (const auto& share_node : *share_nodes)
			{
				// Entity
				entity::ptr e;
				boost::filesystem::path path;
#if _WSTRING
				read<std::string>(share_node, toml::path, [&](const std::string& value) { path = utf::to_utf16(value); });
#else
				read<std::string>(share_node, toml::path, [&](const std::string& value) { path = value; });
#endif
				if (boost::filesystem::is_directory(path))
				{
					// Folder
					e = client_factory::create_folder(entity);
					read<bool>(share_node, toml::enabled, [&](const bool value) { e->set_enabled(value); });

					const auto folder = e->get<folder_component>();
					folder->set_path(path);
					folder->set_time();
				}
				else
				{
					// File
					e = client_factory::create_file(entity);
					read<bool>(share_node, toml::enabled, [&](const bool value) { e->set_enabled(value); });

					const auto file = e->get<file_component>();
					file->set_path(path);
					file->set_size();
					file->set_time();
				}

				// List
				share_list->push_back(e);
			}
		}
	}

	void qt_application::read_verified(const std::shared_ptr<cpptoml::table>& table, const entity::ptr& entity)
	{
		assert(thread_info::main());

		const auto verified_node = table->get_table(toml::verified);
		if (verified_node)
		{
			const auto verified_option = entity->get<verified_option_component>();
			read<std::string>(verified_node, toml::hash, [&](const std::string& value) { verified_option->set_hash(value); });
			read_ex<size_t>(verified_node, toml::iterations, [&](const size_t value) { verified_option->set_iterations(value); });
			read<std::string>(verified_node, toml::key, [&](const std::string& value) { verified_option->set_key(value); });
			read_ex<size_t>(verified_node, toml::key_size, [&](const size_t value) { verified_option->set_key_size(bits_to_bytes(value)); });
			read<std::string>(verified_node, toml::salt, [&](const std::string& value) { verified_option->set_salt(value); });
		}
	}

	// Write
	void qt_application::write()
	{
		assert(thread_info::main());

		write(m_path.string());
	}

	void qt_application::backup()
	{
		// Backup
		if (file_util::exists(m_path) && !file_util::empty(m_path))
		{
			const auto path = get_backup_path();
			write(path);
		}
	}

	void qt_application::write(const boost::filesystem::path& path)
	{
		assert(thread_info::main());

		const auto root_node = cpptoml::make_table();

		// Clients
		if (!m_clients->empty())
		{
			const auto client_nodes = cpptoml::make_table_array();

			for (const auto& entity : m_clients)
			{
				const auto client_node = cpptoml::make_table();

				write_client(client_node, entity);
				write_group(client_node, entity);
				//write_headers(client_node, entity);
				write_option(client_node, entity);
				write_routers(client_node, entity);
				write_session(client_node, entity);
				write_shares(client_node, entity);
				write_verified(client_node, entity);

				client_nodes->push_back(client_node);
			}

			root_node->insert(toml::client, client_nodes);

			// Global
			write_global(root_node);
		}

		// Write
		std::ofstream fout(path.string());
		cpptoml::toml_writer writer(fout);
		root_node->accept(writer);
		fout.close();
	}

	void qt_application::write_global(const std::shared_ptr<cpptoml::table>& table)
	{
		assert(thread_info::main());

		const auto global_node = cpptoml::make_table();

		write_ex(global_node, toml::auto_open, is_auto_open());
		write_ex(global_node, toml::statusbar, is_statusbar());
		write_ex(global_node, toml::system_tray, is_system_tray());
		write_ex(global_node, toml::timestamps, is_timestamps());
		write_ex(global_node, toml::user_colors, is_user_colors());

		write_ex(global_node, toml::active_tab, m_active);
		write_ex(global_node, toml::font_size, m_font_size);
		write_ex(global_node, toml::max_tabs, m_tabs, [&]() { return has_tabs(); });

#if _WSTRING
		write(global_node, toml::theme, utf::to_utf8(m_theme.wstring()));
#else
		write(global_node, toml::theme, m_theme.string());
#endif
		// Transfer
		write_ex(global_node, toml::download_speed, b_to_mb(m_download_throttle.get_max_size() << 3));
		write_ex(global_node, toml::upload_speed, b_to_mb(m_upload_throttle.get_max_size() << 3));

		// Geometry
		const auto geometry_node = cpptoml::make_array();
		geometry_node->push_back(m_geometry);

		for (const auto& e : m_clients)
		{
			const auto client = e->get<client_component>();
			geometry_node->push_back(client->get_geometry());
		}

		global_node->insert(toml::qt_geometry, geometry_node);

		// State
		const auto state_node = cpptoml::make_array();
		state_node->push_back(m_state);

		for (const auto& e : m_clients)
		{
			const auto client = e->get<client_component>();
			state_node->push_back(client->get_state());
		}

		global_node->insert(toml::qt_state, state_node);

		table->insert(toml::global, global_node);
	}

	void qt_application::write_client(const std::shared_ptr<cpptoml::table>& table, const entity::ptr& entity)
	{
		assert(thread_info::main());

		// Entity
		write(table, toml::enabled, entity->enabled());

		// Service
		const auto client_service = entity->get<client_service_component>();
		write_ex(table, toml::threads, client_service->get_threads());

		// Client
		const auto client = entity->get<client_component>();
		write(table, toml::name, client->get_name());

		auto color = client->get_color();
		BOOST_ENDIAN_LITTLE_ONLY(color = boost::endian::endian_reverse(color));
		write(table, toml::color, hex::encode(color));
	}

	void qt_application::write_group(const std::shared_ptr<cpptoml::table>& table, const entity::ptr& entity)
	{
		assert(thread_info::main());

		const auto group_list = entity->get<group_list_component>();
		if (!group_list->empty())
		{
			const auto group_nodes = cpptoml::make_table_array();

			for (const auto& e : *group_list)
			{
				// Entity
				const auto group_node = cpptoml::make_table();
				write(group_node, toml::enabled, e->enabled());

				// Group
				const auto security = e->get<group_option_component>();
				write(group_node, toml::entropy, security->get_entropy());
				write(group_node, toml::percent, security->get_percent());

				write(group_node, toml::cipher, security->get_cipher());
				write_ex(group_node, toml::key_size, security->get_key_size());

				write(group_node, toml::hash, security->get_hash());
				write_ex(group_node, toml::iterations, security->get_iterations());
				write(group_node, toml::salt, security->get_salt());

				group_nodes->push_back(group_node);

			}

			table->insert(toml::group, group_nodes);
		}
	}

	void qt_application::write_option(const std::shared_ptr<cpptoml::table>& table, const entity::ptr& entity)
	{
		assert(thread_info::main());

		const auto option_node = cpptoml::make_table();

		// Client
		const auto client = entity->get<client_component>();
		write_ex(option_node, toml::mode, client->get_mode());
		write(option_node, toml::datestamp, client->get_datestamp());
		write(option_node, toml::timestamp, client->get_timestamp());

		write(option_node, toml::hash, client->get_hash());
		write(option_node, toml::salt, client->get_salt());

		write_ex(option_node, toml::buffer_size, client->get_buffer_size());
		write_ex(option_node, toml::chunk_size, client->get_chunk_size());
		write_ex(option_node, toml::max_downloads, client->get_downloads());
		write_ex(option_node, toml::max_uploads, client->get_uploads());
#if _WSTRING
		write(option_node, toml::path, utf::to_utf8(client->get_path().wstring()));
#else
		write(option_node, toml::path, client->get_path().string());
#endif
		table->insert(toml::option, option_node);
	}

	void qt_application::write_routers(const std::shared_ptr<cpptoml::table>& table, const entity::ptr& entity)
	{
		assert(thread_info::main());

		const auto router_list = entity->get<router_list_component>();
		if (!router_list->empty())
		{
			const auto router_nodes = cpptoml::make_table_array();

			for (const auto& e : *router_list)
			{
				// Entity
				const auto router_node = cpptoml::make_table();
				write(router_node, toml::enabled, e->enabled());

				// Router
				const auto router = e->get<router_component>();
				write(router_node, toml::name, router->get_name());
				write(router_node, toml::address, router->get_address());
				write_ex(router_node, toml::port, router->get_port());
				write(router_node, toml::password, router->get_password());

				router_nodes->push_back(router_node);

			}

			table->insert(toml::router, router_nodes);
		}
	}

	void qt_application::write_session(const std::shared_ptr<cpptoml::table>& table, const entity::ptr& entity)
	{
		assert(thread_info::main());

		const auto session_node = cpptoml::make_table();

		const auto option = entity->get<session_option_component>();
		write(session_node, toml::algorithm, option->get_algorithm());
		write_ex(session_node, toml::prime_size, option->get_prime_size());
		write(session_node, toml::cipher, option->get_cipher());
		write_ex(session_node, toml::key_size, option->get_key_size());
		write(session_node, toml::hash, option->get_hash());
		write_ex(session_node, toml::iterations, option->get_iterations());
		write(session_node, toml::salt, option->get_salt());

		table->insert(toml::session, session_node);
	}

	void qt_application::write_shares(const std::shared_ptr<cpptoml::table>& table, const entity::ptr& entity)
	{
		assert(thread_info::main());

		const auto share_list = entity->get<share_list_component>();
		if (!share_list->empty())
		{
			const auto share_nodes = cpptoml::make_table_array();

			for (const auto& e : *share_list)
			{
				// Entity
				const auto share_node = cpptoml::make_table();
				write(share_node, toml::enabled, e->enabled());

				if (e->has<folder_component>())
				{
					const auto f = e->get<folder_component>();
#if _WSTRING
					write(share_node, toml::path, utf::to_utf8(f->get_data().wstring()));
#else
					write(share_node, toml::path, f->get_data().string());
#endif
				}
				else if (e->has<file_component>())
				{
					const auto f = e->get<file_component>();
#if _WSTRING
					write(share_node, toml::path, utf::to_utf8(f->get_data().wstring()));
#else
					write(share_node, toml::path, f->get_data().string());
#endif
				}

				share_nodes->push_back(share_node);
			}

			table->insert(toml::share, share_nodes);
		}
	}

	void qt_application::write_verified(const std::shared_ptr<cpptoml::table>& table, const entity::ptr& entity)
	{
		assert(thread_info::main());

		const auto verified_option = entity->get<verified_option_component>();
		if (verified_option->valid())
		{
			const auto verified_node = cpptoml::make_table();

			write(verified_node, toml::hash, verified_option->get_hash());
			write_ex(verified_node, toml::iterations, verified_option->get_iterations());
			write(verified_node, toml::key, verified_option->get_key());
			write_ex(verified_node, toml::key_size, bytes_to_bits(verified_option->get_key_size()));
			write(verified_node, toml::salt, verified_option->get_salt());

			table->insert(toml::verified, verified_node);
		}
	}

	// Set
	void qt_application::set_theme()
	{
		assert(thread_info::main());

		m_theme.clear();

		// Style
		QFile file(resource::style);
		file.open(QFile::ReadOnly);
		QString style = QLatin1String(file.readAll());
		qApp->setStyleSheet(style);
	}

	void qt_application::set_theme(const std::wstring& theme)
	{
		assert(thread_info::main());

		if (!theme.empty())
		{
			m_theme = theme;

			QFile file(QString::fromStdWString(theme));

			if (file.open(QFile::ReadOnly))
			{
				QString style = QLatin1String(file.readAll());
				qApp->setStyleSheet(style);
			}
			else
			{
				set_theme();
			}
		}
		else if (has_theme())
		{
			set_theme();
		}
	}

	void qt_application::set_theme(const std::string& theme)
	{
		assert(thread_info::main());

		if (!theme.empty())
		{
			m_theme = theme;

			QFile file(QString::fromStdString(theme));

			if (file.open(QFile::ReadOnly))
			{
				QString style = QLatin1String(file.readAll());
				qApp->setStyleSheet(style);
			}
			else
			{
				set_theme();
			}
		}
		else if (has_theme())
		{
			set_theme();
		}
	}

	// Get
	boost::filesystem::path qt_application::get_backup_path() const
	{
		auto path = m_path.parent_path();
		path += boost::str(boost::format("%s.bak") % m_path.filename().string());

		return path;
	}

	size_t qt_application::get_max_size() const
	{
		return has_tabs() ? m_tabs : ui::num_tabs;
	}
}
