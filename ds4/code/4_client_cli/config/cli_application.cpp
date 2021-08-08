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

#include "component/error_component.h"
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

#include "config/cli_application.h"
#include "config/toml.h"
#include "entity/entity.h"
#include "factory/client_factory.h"
#include "http/http.h"
#include "http/http_socket.h"
#include "object/download.h"
#include "object/upload.h"
#include "security/checksum/xxhash.h"
#include "security/filter/hex.h"
#include "thread/thread_info.h"
#include "utility/value.h"
#include "utility/io/file_util.h"

namespace eja
{
	// Constructor
	cli_application::cli_application(int& argc, char* argv[]) : application(argc, argv)
	{
		assert(thread_info::main());
	}

	// Interface
	void cli_application::init()
	{
		assert(thread_info::main());

		application::init();

		for (const auto& entity : m_clients)
		{
			// Callback
			const auto callback = entity->get<callback_service_component>();
			callback->add(callback::error | callback::add, [&](const entity::ptr& entity) { error_add(entity); });
			callback->start();

			if (entity->enabled())
			{
				// Service
				const auto client_service = entity->get<client_service_component>();
				client_service->start();
			}
		}
	}

	void cli_application::shutdown()
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

	void cli_application::clear()
	{
		assert(thread_info::main());

		m_clients.clear();

		application::clear();
	}

	// Utility
	void cli_application::error_add(const entity::ptr& entity) const
	{
		const auto error = entity->get<error_component>();
		std::cerr << error->get_text() << std::endl;
	}

	// Read
	void cli_application::read()
	{
		assert(thread_info::main());

		clear();

		// Root
		const auto root_node = cpptoml::parse_file(m_path.string());

		// Global
		const auto global_node = root_node->get_table(toml::global);
		if (global_node)
			read_global(root_node);

		// Clients
		const auto client_nodes = root_node->get_table_array(toml::client);

		if (client_nodes)
		{
			for (const auto& client_node : *client_nodes)
			{
				const auto entity = client_factory::create_client();

				read_client(client_node, entity);
				read_group(client_node, entity);
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
	}

	// Read
	void cli_application::read_global(const std::shared_ptr<cpptoml::table>& table)
	{
		assert(thread_info::main());

		const auto global_node = table->get_table(toml::global);

		if (global_node)
		{
			// Tabs
			read_ex<size_t>(global_node, toml::max_tabs, [&](const size_t value) { set_tabs(clamp(ui::min_tabs, value, ui::max_tabs, ui::num_tabs)); });

			// Transfer
			read_ex<size_t>(global_node, toml::download_speed, [&](const size_t value) { m_download_throttle.set_max_size(clamp(download::min_throttle_size, (mb_to_b(value) >> 3), mb_to_b(download::max_throttle_size))); });
			read_ex<size_t>(global_node, toml::upload_speed, [&](const size_t value) { m_upload_throttle.set_max_size(clamp(upload::min_throttle_size, (mb_to_b(value) >> 3), mb_to_b(upload::max_throttle_size))); });
		}
	}

	void cli_application::read_client(const std::shared_ptr<cpptoml::table>& table, const entity::ptr& entity)
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

	void cli_application::read_group(const std::shared_ptr<cpptoml::table>& table, const entity::ptr& entity)
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

				// List
				group_list->push_back(e);
			}
		}
	}

	void cli_application::read_option(const std::shared_ptr<cpptoml::table>& table, const entity::ptr& entity)
	{
		assert(thread_info::main());

		const auto option_node = table->get_table(toml::option);

		if (option_node)
		{
			// Client
			const auto client = entity->get<client_component>();
			read<std::string>(option_node, toml::hash, [&](const std::string& value) { client->set_hash(value); });
			read<std::string>(option_node, toml::salt, [&](const std::string& value) { client->set_salt(value); });
			read_ex<size_t>(option_node, toml::max_uploads, [&](const size_t value) { client->set_uploads(clamp(upload::min_threads, value, upload::max_threads, upload::num_threads)); });
		}
	}

	void cli_application::read_routers(const std::shared_ptr<cpptoml::table>& table, const entity::ptr& entity)
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

	void cli_application::read_session(const std::shared_ptr<cpptoml::table>& table, const entity::ptr& entity)
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

	void cli_application::read_shares(const std::shared_ptr<cpptoml::table>& table, const entity::ptr& entity)
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

	void cli_application::read_verified(const std::shared_ptr<cpptoml::table>& table, const entity::ptr& entity)
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

	// Get
	size_t cli_application::get_max_size() const
	{
		return has_tabs() ? m_tabs : ui::num_tabs;
	}
}
