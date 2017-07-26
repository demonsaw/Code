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

#ifdef _MSC_VER
#pragma warning(disable: 4005 4075)
#endif

#include <cassert>
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>

#include "config/application.h"
#include "entity/entity.h"

#include "component/endpoint_component.h"
#include "component/header_component.h"
#include "component/ui_component.h"
#include "component/version_component.h"

#include "component/client/client_acceptor_component.h"
#include "component/client/client_io_component.h"
#include "component/client/client_network_component.h"
#include "component/client/client_option_component.h"
#include "component/client/client_service_component.h"
#include "component/group/group_security_component.h"

#include "component/io/file_component.h"
#include "component/io/folder_component.h"
#include "component/io/share_component.h"

#include "component/message/message_acceptor_component.h"
#include "component/message/message_network_component.h"
#include "component/message/message_service_component.h"
#include "component/session/session_security_component.h"
#include "component/status/status_component.h"
#include "component/transfer/transfer_service_component.h"

#include "config/toml.h"
#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "system/type.h"
#include "utility/value.h"

namespace eja
{
	// Constructor
	application::application(int argc, char* argv[])
	{
		assert(argc && argv);

		m_path = (argc > 1) ? argv[1] : demonsaw::config;
	}
	
	// Interface
	void application::init()
	{		
		// Routers
		for (const auto& entity : m_routers)
		{
			// Service
			if (entity->enabled())
			{
				const auto acceptor = entity->get<message_acceptor_component>();
				if (acceptor->invalid())
					acceptor->start();
			}
		}

		// Clients
		if (m_clients_enabled)
		{
			for (const auto& entity : m_clients)
			{
				// Service
				if (entity->enabled())
				{
					const auto service = entity->get<client_service_component>();
					if (service->invalid())
						service->start();
				}
			}
		}		
	}

	void application::shutdown()
	{
		// Clients
		if (m_clients_enabled)
		{
			for (const auto& entity : m_clients)
			{
				// Service
				if (entity->enabled())
				{
					const auto service = entity->get<client_service_component>();
					if (service->valid())
						service->async_quit();
				}
			}
		}		

		// Routers
		for (const auto& entity : m_routers)
		{
			// Service
			if (entity->enabled())
			{
				const auto acceptor = entity->get<message_acceptor_component>();
				if (acceptor->valid())
					acceptor->stop();
			}
		}

		save();

		clear();
	}

	void application::clear()
	{
		if (m_clients_enabled)
			m_clients->clear();

		m_routers->clear();
	}

	// Utility
	void application::load(const char* psz)
	{
		clear();

		const auto root_node = cpptoml::parse_file(psz);

		// Header
		const auto header_node = root_node->get_table(toml::demonsaw);
		if (!header_node)
		{
			const auto str = boost::str(boost::format("Unable to read config file %s.") % psz);
			throw std::runtime_error(str);
		}

		/*const auto version = header_node->get_as<int64_t>(toml::version);
		if (version && (static_cast<size_t>(*version) < version::toml))
		{
			const auto str = boost::str(boost::format("Unable to read config file. Expecting version %u.") % version::toml);
			throw std::runtime_error(str);
		}*/
		
		if (header_node->contains(toml::index))
		{
			const auto index = header_node->get_as<int64_t>(toml::index);
			set_index(*index);
		}

		if (header_node->contains(toml::theme))
		{
			const auto theme = header_node->get_as<std::string>(toml::theme);
			set_theme(*theme);
		}		

		// Options
		if (m_clients_enabled)
		{
			const auto option_node = root_node->get_table(toml::option);
			if (option_node)
			{
				if (option_node->contains(toml::geometry))
				{
					const auto value = option_node->get_as<std::string>(toml::geometry);
					if (value)
						m_geometry = *value;
				}

				if (option_node->contains(toml::state))
				{
					const auto value = option_node->get_as<std::string>(toml::state);
					if (value)
						m_state = *value;
				}
			}
		}		

		// Routers
		const auto router_nodes = root_node->get_table_array(toml::router);
		if (router_nodes)
		{			
			for (const auto& router_node : *router_nodes)
			{
				const auto entity = entity_factory::create_message();

				read_router(router_node, entity);
				read_router_options(router_node, entity);

				read_groups(router_node, entity);
				read_headers(router_node, entity);
				read_routers(router_node, entity);
				read_ui(router_node, entity);

				m_routers.push_back(entity);
			}
		}

		// Clients
		if (m_clients_enabled)
		{
			const auto clients = root_node->get_table_array(toml::client);
			if (clients)
			{
				for (const auto& client_node : *clients)
				{
					const auto entity = entity_factory::create_client();

					read_client(client_node, entity);
					read_client_io(client_node, entity);
					read_client_network(client_node, entity);
					read_client_options(client_node, entity);
					read_client_routers(client_node, entity);
					read_client_session(client_node, entity);

					read_groups(client_node, entity);
					read_headers(client_node, entity);
					read_ui(client_node, entity);

					m_clients.push_back(entity);
				}
			}
		}

		// Index
		if (m_index > m_clients->size())
			m_index = 0;
	}

	void application::save(const char* psz)
	{
		if (!m_saving_enabled)
			return;

		//  Root
		const auto root_node = cpptoml::make_table();

		// Header
		const auto header_node = cpptoml::make_table();
		write_ex(header_node, toml::version, version::toml);

		if (m_clients_enabled && (m_index > 0))
			write_ex(header_node, toml::index, m_index);

		if (m_clients_enabled && has_theme())
			write(header_node, toml::theme, m_theme);

		root_node->insert(toml::demonsaw, header_node);

		// Options
		if (m_clients_enabled)
		{
			const auto option_node = cpptoml::make_table();

			if (!m_geometry.empty())
				write(option_node, toml::geometry, m_geometry);

			if (!m_state.empty())
				write(option_node, toml::state, m_state);

			if (!option_node->empty())
				root_node->insert(toml::option, option_node);
		}		

		// Routers
		const auto router_nodes = cpptoml::make_table_array();

		for (const auto& entity : m_routers)
		{
			const auto router_node = cpptoml::make_table();

			write_router(router_node, entity);			
			write_router_options(router_node, entity);
			
			write_groups(router_node, entity);
			write_headers(router_node, entity);
			write_routers(router_node, entity);
			write_ui(router_node, entity);

			router_nodes->push_back(router_node);
		}

		root_node->insert(toml::router, router_nodes);

		// Clients
		if (m_clients_enabled)
		{
			const auto client_nodes = cpptoml::make_table_array();

			for (const auto& entity : m_clients)
			{
				const auto client_node = cpptoml::make_table();

				write_client(client_node, entity);
				write_client_io(client_node, entity);
				write_client_network(client_node, entity);
				write_client_options(client_node, entity);
				write_client_routers(client_node, entity);
				write_client_session(client_node, entity);

				write_groups(client_node, entity);
				write_headers(client_node, entity);
				write_ui(client_node, entity);

				client_nodes->push_back(client_node);
			}

			root_node->insert(toml::client, client_nodes);
		}		

		// Write
		std::ofstream fout(psz);
		cpptoml::toml_writer writer(fout);
		root_node->accept(writer);
	}

	// Read - Client
	void application::read_client(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity)
	{
		// Entity
		read<bool>(table, toml::enabled, [&](const bool value) { entity->set_enabled(value); });

		// Client
		const auto endpoint = entity->get<endpoint_component>();
		read<std::string>(table, toml::name, [&](const std::string& value) { endpoint->set_name(value); });
		read_ex<u32>(table, toml::color, [&](const u32 value) { endpoint->set_color(value); });
		read<std::string>(table, toml::address, [&](const std::string& value) { endpoint->set_address(value); });
		read_ex<u16>(table, toml::port, [&](const u16 value) { endpoint->set_port(value); });

		read<std::string>(table, toml::address_ext, [&](const std::string& value) { endpoint->set_address_ext(value); });
		read_ex<u16>(table, toml::port_ext, [&](const u16 value) { endpoint->set_port_ext(value); });
	}

	void application::read_client_io(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity)
	{		
		// Shares
		const auto file_node = table->get_array(toml::shares);

		if (file_node)
		{
			const auto share_list = entity->get<share_list_component>();

			for (const auto& it : *file_node)
			{
				const auto value = it->as<std::string>();
				const auto path = static_cast<std::string>(value->get());
				entity::ptr e;

				if (boost::filesystem::is_directory(path))
				{
					// Folder					
					e = entity_factory::create_folder(entity);
					const auto folder = e->get<folder_component>();
					folder->set_path(path);
				}
				else
				{
					// File
					e = entity_factory::create_file(entity);
					const auto file = e->get<file_component>();
					file->set_path(path);
					file->set_size();
				}

				share_list->push_back(e);
			}
		}

		// Save
		const auto io = entity->get<client_io_component>();
		read<std::string>(table, toml::path, [&](const std::string& value) { io->set_path(value); });

		// IO
		const auto io_node = table->get_table(toml::io);
		if (io_node)
		{			
			read<bool>(io_node, toml::remove, [&](const bool value) { io->set_remove(value); });
			read<std::string>(io_node, toml::hash, [&](const std::string& value) { io->set_hash(value); });
			read<std::string>(io_node, toml::salt, [&](const std::string& value) { io->set_salt(value); });
		}
	}

	void application::read_client_message(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity)
	{
		// Entity
		read<bool>(table, toml::enabled, [&](const bool value) { entity->set_enabled(value); });

		// Router
		const auto endpoint = entity->get<endpoint_component>();
		read<std::string>(table, toml::name, [&](const std::string& value) { endpoint->set_name(value); });
		read_ex<u32>(table, toml::color, [&](const u32 value) { endpoint->set_color(value); });
		read<std::string>(table, toml::address, [&](const std::string& value) { endpoint->set_address(value); });
		read_ex<u16>(table, toml::port, [&](const u16 value) { endpoint->set_port(value); });

		// Service
		const auto service = entity->get<message_service_component>();
		read_ex<size_t>(table, toml::threads, [&](const size_t value) { service->set_threads(clamp(network::min_threads, value, network::max_threads)); });
	}

	void application::read_client_network(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity)
	{
		const auto network_node = table->get_table(toml::network);
		if (network_node)
		{
			// Network
			const auto network = entity->get<client_network_component>();
			read_ex<network_mode>(network_node, toml::mode, [&](const network_mode value) { network->set_mode(clamp(network_mode::none, value, network_mode::max)); });
			read<std::string>(network_node, toml::redirect, [&](const std::string& value) { network->set_redirect(value); });
			read_ex<network_trust>(network_node, toml::trust, [&](const network_trust value) { network->set_trust(clamp(network_trust::min, value, network_trust::max)); });

			read_ex<size_t>(network_node, toml::chunk, [&](const size_t value) { network->set_chunk(clamp(network::min_chunk, value, network::max_chunk)); });
			read_ex<size_t>(network_node, toml::downloads, [&](const size_t value) { network->set_downloads(clamp(network::min_downloads, value, network::max_downloads)); });
			read_ex<size_t>(network_node, toml::interval, [&](const size_t value) { network->set_interval(clamp(static_cast<size_t>(sec_to_ms(1)), value, static_cast<size_t>(sec_to_ms(60)))); });
			read_ex<size_t>(network_node, toml::retries, [&](const size_t value) { network->set_retries(clamp(network::min_retries, value, network::max_retries)); });
			read_ex<size_t>(network_node, toml::threads, [&](const size_t value) { network->set_threads(clamp(network::min_threads, value, network::max_threads)); });
			read_ex<size_t>(network_node, toml::sockets, [&](const size_t value) { network->set_sockets(clamp(network::min_sockets, value, network::max_sockets)); });
			read_ex<size_t>(network_node, toml::timeout, [&](const size_t value) { network->set_timeout(clamp(network::min_timeout, value, network::max_timeout)); });
			read_ex<size_t>(network_node, toml::uploads, [&](const size_t value) { network->set_uploads(clamp(network::min_uploads, value, network::max_uploads)); });
			read_ex<size_t>(network_node, toml::buffer, [&](const size_t value) { network->set_window(clamp(network::min_window, value, network::max_window)); });

			// Notification
			read<bool>(network_node, toml::motd, [&](const bool value) { network->set_motd(value); });
			read<bool>(network_node, toml::version, [&](const bool value) { network->set_version(value); });

			// Visibility
			read<bool>(network_node, toml::browse, [&](const bool value) { network->set_browse(value); });
			read<bool>(network_node, toml::chat, [&](const bool value) { network->set_chat(value); });
			read<bool>(network_node, toml::pm, [&](const bool value) { network->set_pm(value); });
			read<bool>(network_node, toml::search, [&](const bool value) { network->set_search(value); });
			read<bool>(network_node, toml::upload, [&](const bool value) { network->set_upload(value); });
		}
	}

	void application::read_client_options(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity)
	{		
		// Service
		const auto service = entity->get<client_service_component>();
		read_ex<size_t>(table, toml::threads, [&](const size_t value) { service->set_threads(clamp(network::min_threads, value, network::max_threads)); });

		// Acceptor
		const auto acceptor = entity->get<client_acceptor_component>();
		read_ex<size_t>(table, toml::threads, [&](const size_t value) { acceptor->set_threads(clamp(network::min_threads, value, network::max_threads)); });

		// Options
		const auto option_node = table->get_table(toml::option);
		if (option_node)
		{			
			const auto option = entity->get<client_option_component>();
			read<bool>(option_node, toml::beep, [&](const bool value) { option->set_beep(value); });
			read<bool>(option_node, toml::flash, [&](const bool value) { option->set_flash(value); });
			read<bool>(option_node, toml::vibrate, [&](const bool value) { option->set_vibrate(value); });
			read<std::string>(option_node, toml::timestamp, [&](const std::string& value) { option->set_timestamp(value); });
		}
	}	

	void application::read_client_routers(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity)
	{
		// Message Routers
		const auto message_nodes = table->get_table_array(toml::message);
		if (message_nodes)
		{
			bool enabled = false;			
			const auto message_list = entity->get<message_list_component>();

			for (const auto& message_node : *message_nodes)
			{
				// Entity
				const auto e = entity_factory::create_client_message(entity);

				read_client_message(message_node, e);
				read_groups(message_node, e);

				// Only 1 router
				if (e->enabled())
				{
					e->set_enabled(!enabled);
					enabled = true;
				}

				// List				
				message_list->push_back(e);
			}
		}

		// Transfer Routers
		const auto transfer_nodes = table->get_table_array(toml::transfer);
		if (transfer_nodes)
		{
			const auto transfer_list = entity->get<transfer_list_component>();

			for (const auto& transfer_node : *transfer_nodes)
			{
				// Entity
				const auto e = entity_factory::create_client_transfer(entity);

				read_client_transfer(transfer_node, e);
				read_groups(transfer_node, e);

				// List				
				transfer_list->push_back(e);
			}
		}
	}

	void application::read_client_session(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity)
	{
		// Session
		const auto session_node = table->get_table(toml::session);
		if (session_node)
		{
			const auto session = entity->get<session_security_component>();
			read<std::string>(session_node, toml::algorithm, [&](const std::string& value) { session->set_algorithm(value); });
			read_ex<size_t>(session_node, toml::prime_size, [&](const size_t value) { session->set_prime_size(value); });
			read<std::string>(session_node, toml::cipher, [&](const std::string& value) { session->set_cipher(value); });
			read_ex<size_t>(session_node, toml::key_size, [&](const size_t value) { session->set_key_size(value); });
			read<std::string>(session_node, toml::hash, [&](const std::string& value) { session->set_hash(value); });			
			read_ex<size_t>(session_node, toml::iterations, [&](const size_t value) { session->set_iterations(value); });
			read<std::string>(session_node, toml::salt, [&](const std::string& value) { session->set_salt(value); });
		}
	}

	void application::read_client_transfer(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity)
	{
		// Entity
		read<bool>(table, toml::enabled, [&](const bool value) { entity->set_enabled(value); });

		// Router
		const auto endpoint = entity->get<endpoint_component>();
		read<std::string>(table, toml::name, [&](const std::string& value) { endpoint->set_name(value); });
		read_ex<u32>(table, toml::color, [&](const u32 value) { endpoint->set_color(value); });
		read<std::string>(table, toml::address, [&](const std::string& value) { endpoint->set_address(value); });
		read_ex<u16>(table, toml::port, [&](const u16 value) { endpoint->set_port(value); });

		// Service
		const auto service = entity->get<transfer_service_component>();
		read_ex<size_t>(table, toml::threads, [&](const size_t value) { service->set_threads(clamp(network::min_threads, value, network::max_threads)); });
	}

	// Read - Router
	void application::read_router(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity)
	{
		// Entity
		read<bool>(table, toml::enabled, [&](const bool value) { entity->set_enabled(value); });

		// Router
		const auto endpoint = entity->get<endpoint_component>();
		read<std::string>(table, toml::name, [&](const std::string& value) { endpoint->set_name(value); });
		read_ex<u32>(table, toml::color, [&](const u32 value) { endpoint->set_color(value); });
		read<std::string>(table, toml::address, [&](const std::string& value) { endpoint->set_address(value); });
		read_ex<u16>(table, toml::port, [&](const u16 value) { endpoint->set_port(value); });		
	}

	void application::read_headers(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity)
	{
		const auto header_nodes = table->get_table(toml::header);
		if (header_nodes)
		{
			for (const auto& header_node : *header_nodes)
			{
				const auto value = header_node.second->as<std::string>();
				if (value)
				{
					const auto pair = std::make_pair(header_node.first, static_cast<std::string>(value->get()));
					const auto header_map = entity->get<header_map_component>();
					header_map->insert(pair);
				}
			}
		}
	}

	void application::read_router_options(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity)
	{
		// Service
		const auto acceptor = entity->get<message_acceptor_component>();
		read_ex<size_t>(table, toml::threads, [&](const size_t value) { acceptor->set_threads(value); });

		// Options
		const auto network_node = table->get_table(toml::network);
		if (network_node)
		{
			const auto network = entity->get<message_network_component>();

			// Network
			read<std::string>(network_node, toml::motd, [&](const std::string& value) { network->set_motd(value); });
			read<std::string>(network_node, toml::redirect, [&](const std::string& value) { network->set_redirect(value); });			
			read_ex<size_t>(network_node, toml::buffer, [&](const size_t value) { network->set_buffer(value); });

			// Type
			read<bool>(network_node, toml::message, [&](const bool value) { network->set_message(value); });
			read<bool>(network_node, toml::transfer, [&](const bool value) { network->set_transfer(value); });
			read<bool>(network_node, toml::proxy, [&](const bool value) { network->set_proxy(value); });
			read<bool>(network_node, toml::open, [&](const bool value) { network->set_open(value); });
		}
	}

	// Read - Misc
	void application::read_groups(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity)
	{
		const auto group_nodes = table->get_table_array(toml::group);
		if (group_nodes)
		{
			const auto security_list = entity->get<group_security_list_component>();

			for (const auto& group_node : *group_nodes)
			{
				// Entity
				const auto e = entity_factory::create_group(entity);
				read<bool>(group_node, toml::enabled, [&](const bool value) { e->set_enabled(value); });

				// Security
				const auto security = group_security_component::create();
				read<std::string>(group_node, toml::entropy, [&](const std::string& value) { security->set_entropy(value); });
				read<double>(group_node, toml::percent, [&](const double value) { security->set_percent(value); });
				read<std::string>(group_node, toml::cipher, [&](const std::string& value) { security->set_cipher(value); });
				read_ex<size_t>(group_node, toml::key_size, [&](const size_t value) { security->set_key_size(value); });
				read<std::string>(group_node, toml::hash, [&](const std::string& value) { security->set_hash(value); });
				read_ex<size_t>(group_node, toml::iterations, [&](const size_t value) { security->set_iterations(value); });
				read<std::string>(group_node, toml::salt, [&](const std::string& value) { security->set_salt(value); });
				e->add(security);

				// Status
				e->add<status_component>();

				// List				
				security_list->push_back(e);
			}
		}
	}

	void application::read_routers(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity)
	{
		// Transfer Routers
		const auto transfer_nodes = table->get_table_array(toml::transfer);
		if (transfer_nodes)
		{
			const auto transfer_list = entity->get<transfer_list_component>();

			for (const auto& transfer_node : *transfer_nodes)
			{
				// Entity
				const auto e = entity_factory::create_message_transfer(entity);
				read_router(transfer_node, e);

				// List				
				transfer_list->push_back(e);
			}
		}
	}

	void application::read_ui(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity)
	{
		const auto ui_node = table->get_table(toml::ui);
		if (ui_node)
		{
			const auto ui = entity->get<ui_component>();
			read<bool>(ui_node, toml::browse, [&](const bool value) { ui->set_browse(value); });
			read<bool>(ui_node, toml::chat, [&](const bool value) { ui->set_chat(value); });
			read<bool>(ui_node, toml::client, [&](const bool value) { ui->set_client(value); });			
			read<bool>(ui_node, toml::download, [&](const bool value) { ui->set_download(value); });
			read<bool>(ui_node, toml::error, [&](const bool value) { ui->set_error(value); });
			read<bool>(ui_node, toml::group, [&](const bool value) { ui->set_group(value); });
			read<bool>(ui_node, toml::help, [&](const bool value) { ui->set_help(value); });
			read<bool>(ui_node, toml::message, [&](const bool value) { ui->set_message(value); });
			read<bool>(ui_node, toml::option, [&](const bool value) { ui->set_option(value); });
			read<bool>(ui_node, toml::queue, [&](const bool value) { ui->set_queue(value); });
			read<bool>(ui_node, toml::session, [&](const bool value) { ui->set_session(value); });
			read<bool>(ui_node, toml::share, [&](const bool value) { ui->set_share(value); });
			read<bool>(ui_node, toml::status, [&](const bool value) { ui->set_status(value); });
			read<bool>(ui_node, toml::transfer, [&](const bool value) { ui->set_transfer(value); });
			read<bool>(ui_node, toml::upload, [&](const bool value) { ui->set_upload(value); });
		}
	}

	// Write - Client
	void application::write_client(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity)
	{
		// Entity		
		write(table, toml::enabled, entity->enabled(), [&]() { return !entity->enabled(); });

		// Client
		const auto endpoint = entity->get<endpoint_component>();
		write(table, toml::name, endpoint->get_name(), [&]() { return endpoint->has_name(); });
		write_ex(table, toml::color, endpoint->get_color(), [&]() { return endpoint->has_color(); });
		write(table, toml::address, endpoint->get_address(), [&]() { return endpoint->has_address(); });
		write_ex(table, toml::port, endpoint->get_port(), [&]() { return endpoint->has_port(); });

		write(table, toml::address_ext, endpoint->get_address_ext(), [&]() { return endpoint->has_address_ext(); });
		write_ex(table, toml::port_ext, endpoint->get_port_ext(), [&]() { return endpoint->has_port_ext(); });
	}

	void application::write_client_io(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity)
	{		
		// Shares
		const auto share_list = entity->get<share_list_component>();

		if (!share_list->empty())
		{
			const auto share_node = cpptoml::make_array();

			for (const auto& e : *share_list)
			{
				if (e->has<folder_component>())
				{
					const auto f = e->get<folder_component>();
					const auto& path = f->get_path();
					share_node->push_back(path);
				}
				else if (e->has<file_component>())
				{
					const auto f = e->get<file_component>();
					const auto& path = f->get_path();
					share_node->push_back(path);
				}
			}

			table->insert(toml::shares, share_node);
		}

		// Save
		const auto io = entity->get<client_io_component>();
		write(table, toml::path, io->get_path(), [&]() { return io->has_path(); });

		// IO
		const auto io_node = cpptoml::make_table();		
		write(io_node, toml::remove, io->has_remove(), [&]() { return io->has_remove() != io::remove; });
		write(io_node, toml::hash, io->get_hash(), [&]() { return io->get_hash() != io::hash; });
		write(io_node, toml::salt, io->get_salt(), [&]() { return io->has_salt(); });

		if (!io_node->empty())
			table->insert(toml::io, io_node);
	}

	void application::write_client_message(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity)
	{
		// Entity		
		write(table, toml::enabled, entity->enabled(), [&]() { return !entity->enabled(); });

		// Router				
		const auto endpoint = entity->get<endpoint_component>();
		write(table, toml::name, endpoint->get_name(), [&]() { return endpoint->has_name(); });
		write_ex(table, toml::color, endpoint->get_color(), [&]() { return endpoint->has_color(); });
		write(table, toml::address, endpoint->get_address(), [&]() { return endpoint->has_address(); });
		write_ex(table, toml::port, endpoint->get_port(), [&]() { return endpoint->has_port(); });

		// Service
		const auto service = entity->get<message_service_component>();
		write_ex(table, toml::threads, service->get_threads(), [&]() { return service->get_threads() != network::num_threads; });
	}

	void application::write_client_network(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity)
	{
		const auto network_node = cpptoml::make_table();
		const auto network = entity->get<client_network_component>();

		// Network
		write_ex(network_node, toml::mode, network->get_mode(), [&]() { return network->get_mode() != network::mode; });
		write(network_node, toml::redirect, network->get_redirect(), [&]() { return network->has_redirect(); });
		write_ex(network_node, toml::trust, network->get_trust(), [&]() { return network->get_trust() != network::trust; });

		write_ex(network_node, toml::chunk, network->get_chunk(), [&]() { return network->get_chunk() != network::num_chunk; });
		write_ex(network_node, toml::downloads, network->get_downloads(), [&]() { return network->get_downloads() != network::num_downloads; });
		write_ex(network_node, toml::interval, network->get_interval(), [&]() { return network->get_interval() != network::interval; });
		write_ex(network_node, toml::retries, network->get_retries(), [&]() { return network->get_retries() != network::num_retries; });
		write_ex(network_node, toml::threads, network->get_threads(), [&]() { return network->get_threads() != network::min_threads; });
		write_ex(network_node, toml::sockets, network->get_sockets(), [&]() { return network->get_sockets() != network::num_sockets; });
		write_ex(network_node, toml::timeout, network->get_timeout(), [&]() { return network->get_timeout() != network::num_timeout; });
		write_ex(network_node, toml::uploads, network->get_uploads(), [&]() { return network->get_uploads() != network::num_uploads; });
		write_ex(network_node, toml::buffer, network->get_window(), [&]() { return network->get_window() != network::num_window; });

		// Notification
		write(network_node, toml::motd, network->has_motd(), [&]() { return network->has_motd() != network::motd; });
		write(network_node, toml::version, network->has_version(), [&]() { return network->has_version() != network::version; });

		// Visibility
		write(network_node, toml::browse, network->has_browse(), [&]() { return network->has_browse() != network::browse; });
		write(network_node, toml::chat, network->has_chat(), [&]() { return network->has_chat() != network::chat; });
		write(network_node, toml::pm, network->has_pm(), [&]() { return network->has_pm() != network::pm; });
		write(network_node, toml::search, network->has_search(), [&]() { return network->has_search() != network::search; });
		write(network_node, toml::upload, network->has_upload(), [&]() { return network->has_upload() != network::upload; });

		if (!network_node->empty())
			table->insert(toml::network, network_node);
	}

	void application::write_client_options(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity)
	{
		// Service
		const auto service = entity->get<client_service_component>();
		write_ex(table, toml::threads, service->get_threads(), [&]() { return service->get_threads() != network::num_threads; });

		// Options
		const auto option_node = cpptoml::make_table();
		const auto option = entity->get<client_option_component>();
		write(option_node, toml::beep, option->has_beep(), [&]() { return option->has_beep() != client::beep; });
		write(option_node, toml::flash, option->has_flash(), [&]() { return option->has_flash() != client::flash; });
		write(option_node, toml::vibrate, option->has_vibrate(), [&]() { return option->has_vibrate() != client::vibrate; });
		write(option_node, toml::timestamp, option->get_timestamp(), [&]() { return option->get_timestamp() != client::timestamp; });

		if (!option_node->empty())
			table->insert(toml::option, option_node);
	}

	void application::write_client_routers(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity)
	{
		// Message Routers				
		const auto message_list = entity->get<message_list_component>();
		if (!message_list->empty())
		{
			const auto message_nodes = cpptoml::make_table_array();

			for (const auto& e : *message_list)
			{
				const auto message_node = cpptoml::make_table();
				write_client_message(message_node, e);
				write_groups(message_node, e);

				message_nodes->push_back(message_node);
			}

			table->insert(toml::message, message_nodes);
		}

		// Transfer Routers
		const auto transfer_list = entity->get<transfer_list_component>();
		if (!transfer_list->empty())
		{
			const auto transfer_nodes = cpptoml::make_table_array();

			for (const auto& e : *transfer_list)
			{
				if (e->get_state() != entity_state::temporary)
				{
					const auto transfer_node = cpptoml::make_table();
					write_client_transfer(transfer_node, e);
					write_groups(transfer_node, e);

					transfer_nodes->push_back(transfer_node);
				}
			}

			table->insert(toml::transfer, transfer_nodes);
		}
	}

	void application::write_client_session(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity)
	{
		// Session
		const auto session_node = cpptoml::make_table();
		const auto session = entity->get<session_security_component>();
		write(session_node, toml::algorithm, session->get_algorithm(), [&]() { return session->get_algorithm() != session::algorithm; });		
		write_ex(session_node, toml::prime_size, session->get_prime_size(), [&]() { return session->get_prime_size() != session::prime_size; });
		write(session_node, toml::cipher, session->get_cipher(), [&]() { return session->get_cipher() != session::cipher; });
		write_ex(session_node, toml::key_size, session->get_key_size(), [&]() { return session->get_key_size() != session::key_size; });
		write(session_node, toml::hash, session->get_hash(), [&]() { return session->get_hash() != session::hash; });
		write_ex(session_node, toml::iterations, session->get_iterations(), [&]() { return session->get_iterations() != session::iterations; });
		write(session_node, toml::salt, session->get_salt(), [&]() { return session->has_salt(); });

		if (!session_node->empty())
			table->insert(toml::session, session_node);
	}

	void application::write_client_transfer(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity)
	{
		// Entity		
		write(table, toml::enabled, entity->enabled(), [&]() { return !entity->enabled(); });

		// Router				
		const auto endpoint = entity->get<endpoint_component>();
		write(table, toml::name, endpoint->get_name(), [&]() { return endpoint->has_name(); });
		write_ex(table, toml::color, endpoint->get_color(), [&]() { return endpoint->has_color(); });
		write(table, toml::address, endpoint->get_address(), [&]() { return endpoint->has_address(); });
		write_ex(table, toml::port, endpoint->get_port(), [&]() { return endpoint->has_port(); });

		// Service
		const auto service = entity->get<transfer_service_component>();
		write_ex(table, toml::threads, service->get_threads(), [&]() { return service->get_threads() != network::min_threads; });
	}

	// Write - Router
	void application::write_router(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity)
	{
		// Entity		
		write(table, toml::enabled, entity->enabled(), [&]() { return !entity->enabled(); });

		// Router				
		const auto endpoint = entity->get<endpoint_component>();
		write(table, toml::name, endpoint->get_name(), [&]() { return endpoint->has_name(); });
		write_ex(table, toml::color, endpoint->get_color(), [&]() { return endpoint->has_color(); });
		write(table, toml::address, endpoint->get_address(), [&]() { return endpoint->has_address(); });
		write_ex(table, toml::port, endpoint->get_port(), [&]() { return endpoint->has_port(); });
	}

	void application::write_headers(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity)
	{
		// Header
		const auto header_map = entity->get<header_map_component>();
		if (!header_map->empty())
		{
			const auto header_node = cpptoml::make_table();

			for (const auto& pair : *header_map)
				write(header_node, pair.first.c_str(), pair.second);

			if (!header_node->empty())
				table->insert(toml::header, header_node);
		}
	}

	void application::write_router_options(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity)
	{
		// Service
		const auto acceptor = entity->get<message_acceptor_component>();
		write_ex(table, toml::threads, acceptor->get_threads(), [&]() { return acceptor->get_threads() != network::num_threads; });

		// Options
		const auto network_node = cpptoml::make_table();
		const auto network = entity->get<message_network_component>();

		// Network
		write(network_node, toml::motd, network->get_motd(), [&]() { return network->has_motd(); });
		write(network_node, toml::redirect, network->get_redirect(), [&]() { return network->has_redirect(); });
		write_ex(network_node, toml::buffer, network->get_buffer(), [&]() { return network->get_buffer() != network::num_buffer; });

		// Type
		write(network_node, toml::message, network->has_message(), [&]() { return network->has_message() != router::message; });
		write(network_node, toml::transfer, network->has_transfer(), [&]() { return network->has_transfer() != router::transfer; });
		write(network_node, toml::proxy, network->has_proxy(), [&]() { return network->has_proxy() != router::proxy; });
		write(network_node, toml::open, network->has_open(), [&]() { return network->has_open() != router::open; });

		if (!network_node->empty())
			table->insert(toml::network, network_node);
	}

	// Write - Misc
	void application::write_groups(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity)
	{
		const auto security_list = entity->get<group_security_list_component>();
		if (!security_list->empty())
		{
			const auto group_nodes = cpptoml::make_table_array();

			for (const auto& e : *security_list)
			{
				// Entity
				const auto group_node = cpptoml::make_table();
				write(group_node, toml::enabled, e->enabled(), [&]() { return !e->enabled(); });

				// Group
				const auto security = e->get<group_security_component>();
				write(group_node, toml::entropy, security->get_entropy(), [&]() { return security->has_entropy(); });
				write(group_node, toml::percent, security->get_percent(), [&]() { return security->get_percent() != group::percent; });
				write(group_node, toml::cipher, security->get_cipher(), [&]() { return security->get_cipher() != group::cipher; });
				write_ex(group_node, toml::key_size, security->get_key_size(), [&]() { return security->get_key_size() != group::key_size; });
				write(group_node, toml::hash, security->get_hash(), [&]() { return security->get_hash() != group::hash; });
				write_ex(group_node, toml::iterations, security->get_iterations(), [&]() { return security->get_iterations() != group::iterations; });
				write(group_node, toml::salt, security->get_salt(), [&]() { return security->has_salt(); });
				
				group_nodes->push_back(group_node);

			}

			if (!table->empty())
				table->insert(toml::group, group_nodes);
		}
	}

	void application::write_routers(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity)
	{
		// Transfer Routers
		const auto transfer_list = entity->get<transfer_list_component>();
		if (!transfer_list->empty())
		{
			const auto transfer_nodes = cpptoml::make_table_array();

			for (const auto& e : *transfer_list)
			{
				if (e->get_state() != entity_state::temporary)
				{
					const auto transfer_node = cpptoml::make_table();
					write_router(transfer_node, e);

					transfer_nodes->push_back(transfer_node);
				}				
			}

			table->insert(toml::transfer, transfer_nodes);
		}
	}

	void application::write_ui(const std::shared_ptr<cpptoml::table>& table, const entity::ptr entity)
	{
		const auto ui_node = cpptoml::make_table();
		const auto ui = entity->get<ui_component>();
		write(ui_node, toml::browse, ui->has_browse(), [&]() { return ui->has_browse() != ui::browse; });
		write(ui_node, toml::chat, ui->has_chat(), [&]() { return ui->has_chat() != ui::chat; });
		write(ui_node, toml::client, ui->has_client(), [&]() { return ui->has_client() != ui::client; });		
		write(ui_node, toml::download, ui->has_download(), [&]() { return ui->has_download() != ui::download; });
		write(ui_node, toml::error, ui->has_error(), [&]() { return ui->has_error() != ui::error; });
		write(ui_node, toml::group, ui->has_group(), [&]() { return ui->has_group() != ui::group; });
		write(ui_node, toml::help, ui->has_help(), [&]() { return ui->has_help() != ui::help; });
		write(ui_node, toml::message, ui->has_message(), [&]() { return ui->has_message() != ui::message; });
		write(ui_node, toml::option, ui->has_option(), [&]() { return ui->has_option() != ui::option; });
		write(ui_node, toml::queue, ui->has_queue(), [&]() { return ui->has_queue() != ui::queue; });
		write(ui_node, toml::session, ui->has_session(), [&]() { return ui->has_session() != ui::session; });
		write(ui_node, toml::share, ui->has_share(), [&]() { return ui->has_share() != ui::share; });
		write(ui_node, toml::status, ui->has_status(), [&]() { return ui->has_status() != ui::status; });
		write(ui_node, toml::transfer, ui->has_transfer(), [&]() { return ui->has_transfer() != ui::transfer; });
		write(ui_node, toml::upload, ui->has_upload(), [&]() { return ui->has_upload() != ui::upload; });

		if (!ui_node->empty())
			table->insert(toml::ui, ui_node);
	}
}
