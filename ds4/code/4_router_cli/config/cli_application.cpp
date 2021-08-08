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
#include <boost/algorithm/string.hpp>

#include "component/error_component.h"
#include "component/room_component.h"
#include "component/verified_component.h"
#include "component/callback/callback.h"
#include "component/callback/callback_service_component.h"
#include "component/group/group_component.h"
#include "component/group/group_option_component.h"
#include "component/router/router_component.h"
#include "component/router/router_acceptor_component.h"

#include "config/cli_application.h"
#include "config/toml.h"
#include "factory/router_factory.h"
#include "http/http_socket.h"
#include "security/checksum/xxhash.h"
#include "security/filter/base.h"
#include "system/type.h"
#include "thread/thread_info.h"
#include "utility/value.h"

namespace eja
{
	// Constructor
	cli_application::cli_application(int& argc, char* argv[]) : application(argc, argv)
	{
	}

	// Interface
	void cli_application::init()
	{
		assert(thread_info::main());

		application::init();

		for (const auto& entity : m_routers)
		{
			// Callback
			const auto callback = entity->get<callback_service_component>();
			callback->start();

			// DEBUG
			//callback->add(callback::error | callback::add, [&](const entity::ptr& entity) { error_add(entity); });

			if (entity->enabled())
			{
				// Service
				const auto acceptor = entity->get<router_acceptor_component>();
				acceptor->start();
			}
		}
	}

	void cli_application::shutdown()
	{
		assert(thread_info::main());

		for (const auto& entity : m_routers)
		{
			if (entity->enabled())
			{
				// Service
				const auto acceptor = entity->get<router_acceptor_component>();
				acceptor->stop();
			}

			// Callback
			const auto callback = entity->get<callback_service_component>();
			callback->stop();
		}

		application::shutdown();
	}

	void cli_application::clear()
	{
		assert(thread_info::main());

		m_routers.clear();

		application::clear();
	}

	// Utility
	/*void cli_application::error_add(const entity::ptr& entity) const
	{
		const auto error = entity->get<error_component>();
		std::cerr << error->get_text() << std::endl;
	}*/

	// Read
	void cli_application::read()
	{
		assert(thread_info::main());

		clear();

		const auto root_node = cpptoml::parse_file(m_path.string());
		const auto router_nodes = root_node->get_table_array(toml::router);

		if (router_nodes)
		{
			for (const auto& router_node : *router_nodes)
			{
				const auto e = router_factory::create_router();

				read_router(router_node, e);
				//read_headers(router_node, e);
				read_option(router_node, e);
				read_rooms(router_node, e);
				read_routers(router_node, e);
				read_verifieds(router_node, e);

				m_routers.push_back(e);
			}
		}
	}

	void cli_application::read_router(const std::shared_ptr<cpptoml::table>& table, const entity::ptr& entity)
	{
		assert(thread_info::main());

		// Entity
		read<bool>(table, toml::enabled, [&](const bool value) { entity->set_enabled(value); });

		// Service
		const auto acceptor = entity->get<router_acceptor_component>();
		read_ex<size_t>(table, toml::threads, [&](const size_t value) { acceptor->set_threads(value); });

		// Router
		const auto router = entity->get<router_component>();
		read<std::string>(table, toml::name, [&](const std::string& value) { router->set_name(value); });
		read<std::string>(table, toml::address, [&](const std::string& value) { router->set_address(boost::trim_copy(value)); });
		read_ex<u16>(table, toml::port, [&](const u16 value) { router->set_port(value); });
		read<std::string>(table, toml::password, [&](const std::string& value) { router->set_password((value.size() <= io::max_passphrase_size) ? value : ""); });
	}

	void cli_application::read_option(const std::shared_ptr<cpptoml::table>& table, const entity::ptr& entity)
	{
		assert(thread_info::main());

		const auto option_node = table->get_table(toml::option);
		if (option_node)
		{
			// Router
			const auto router = entity->get<router_component>();
			read_ex<size_t>(option_node, toml::buffer_size, [&](const size_t value) { router->set_buffer_size(value); });
			read<std::string>(option_node, toml::motd, [&](const std::string& value) { router->set_motd(value); });
			read<std::string>(option_node, toml::redirect, [&](const std::string& value) { router->set_redirect(value); });
		}
	}

	void cli_application::read_rooms(const std::shared_ptr<cpptoml::table>& table, const entity::ptr& entity)
	{
		assert(thread_info::main());

		const auto room_list = entity->get<room_list_component>();
		room_list->clear();

		const auto room_nodes = table->get_table_array(toml::room);
		if (room_nodes)
		{
			for (const auto& room_node : *room_nodes)
			{
				// Entity
				const auto e = router_factory::create_room(entity);
				read<bool>(room_node, toml::enabled, [&](const bool value) { e->set_enabled(value); });

				// Room
				const auto room = e->get<room_component>();
				read<std::string>(room_node, toml::name, [&](const std::string& value) { room->set_name(value); });
				read<std::string>(room_node, toml::color, [&](const std::string& value)
				{
					room->set_color(std::stoul(value, nullptr, 16));
					room->set_alpha(0xFF);
				});

				const auto id = xxhash()(room->get_name());
				room->set_id(std::to_string(id));

				room_list->push_back(e);
			}
		}
	}

	void cli_application::read_routers(const std::shared_ptr<cpptoml::table>& table, const entity::ptr& entity)
	{
		assert(thread_info::main());

		const auto router_list = entity->get<router_list_component>();
		router_list->clear();

		const auto router_nodes = table->get_table_array(toml::router);
		if (router_nodes)
		{
			bool enabled = false;

			for (const auto& router_node : *router_nodes)
			{
				// Entity
				const auto e = router_factory::create_router(entity);
				read<bool>(router_node, toml::enabled, [&](const bool value) { e->set_enabled(value); });
				enabled |= e->enabled();

				// Router
				const auto router = e->get<router_component>();
				read<std::string>(router_node, toml::name, [&](const std::string& value) { router->set_name(value); });
				read<std::string>(router_node, toml::address, [&](const std::string& value) { router->set_address(boost::trim_copy(value)); });
				read_ex<u16>(router_node, toml::port, [&](const u16 value) { router->set_port(value); });
				read<std::string>(router_node, toml::password, [&](const std::string& value) { router->set_password((value.size() <= io::max_passphrase_size) ? value : ""); });

				router_list->push_back(e);
			}

			/*
			Router Rules
			#1) If you have no transfer routers you are a message & transfer router (default)
			#2) If you have transfer routers you are a message router only.
			#3) Unless you are listed in the transfer routers and then you are a message & transfer router
			#4) Every transfer router is also a message router (by default)
			#5) If all transfer routers go offline the message router will become a transfer router
			*/
			if (!router_list->empty() && enabled)
			{
				const auto router = entity->get<router_component>();
				router->set_transfer(false);

				const auto& endpoint = router->get_endpoint();
				const auto it = std::find_if(router_list->begin(), router_list->end(), [&](const entity::ptr& e)
				{
					if (e->enabled())
					{
						const auto r = e->get<router_component>();
						return endpoint == r->get_endpoint();
					}

					return false;
				});

				if (it != router_list->end())
					router->set_transfer(true);
			}
		}
	}

	void cli_application::read_verifieds(const std::shared_ptr<cpptoml::table>& table, const entity::ptr& entity)
	{
		assert(thread_info::main());

		const auto verified_map = entity->get<verified_map_component>();
		verified_map->clear();

		const auto verified_nodes = table->get_table(toml::verified);
		if (verified_nodes)
		{
			for (const auto& verified_node : *verified_nodes)
			{
				const auto node = verified_node.second->as<std::string>();
				if (node)
				{
					const auto& value = static_cast<std::string>(node->get());
					const auto pair = std::make_pair(verified_node.first, value);
					verified_map->insert(pair);
				}
			}
		}
	}
}
