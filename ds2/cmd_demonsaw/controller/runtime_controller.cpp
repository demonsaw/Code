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

#include <iostream>
#include <boost/format.hpp>
#include <boost/thread.hpp>

#include "command/client/request/client_quit_request_command.h"
#include "component/chat_idle_component.h"
#include "component/session_component.h"
#include "component/timeout_component.h"
#include "component/timer_component.h"
#include "component/client/client_component.h"
#include "component/client/client_idle_component.h"
#include "component/client/client_machine_component.h"
#include "component/client/client_option_component.h"
#include "component/error/error_component.h"
#include "component/group/group_component.h"
#include "component/router/router_component.h"
#include "component/router/router_idle_component.h"
#include "component/router/router_machine_component.h"
#include "component/router/router_option_component.h"
#include "component/server/server_component.h"
#include "component/server/server_machine_component.h"
#include "component/server/server_option_component.h"
#include "component/transfer/chunk_component.h"
#include "component/transfer/finished_component.h"
#include "component/transfer/transfer_component.h"
#include "component/transfer/transfer_idle_component.h"
#include "component/transfer/upload_component.h"
#include "component/transfer/upload_thread_component.h"

#include "controller/runtime_controller.h"
#include "security/checksum.h"
#include "security/hex.h"

namespace eja
{
	// Interface
	void runtime_controller::restart()
	{
		// Routers
		const auto& routers = m_config.get_routers();
		auto_lock_ref(routers);

		for (const auto& entity : routers)
		{
			const auto option = entity->get<router_option_component>();
			if (!option || option->disabled())
				continue;

			const auto machine = entity->get<router_machine_component>();
			if (machine)
				machine->restart();
		}

		// Clients
		const auto& clients = m_config.get_clients();
		auto_lock_ref(clients);

		for (const auto& entity : clients)
		{
			const auto option = entity->get<client_option_component>();
			if (!option || option->disabled())
				continue;

			const auto machine = entity->get<client_machine_component>();
			if (machine)
				machine->restart();
		}
	}

	void runtime_controller::refresh()
	{
		// Clients
		const auto& clients = m_config.get_clients();
		auto_lock_ref(clients);

		for (const auto& entity : clients)
		{
			const auto option = entity->get<client_option_component>();
			if (!option || option->disabled())
				continue;

			// Share
			const auto share = entity->get<share_idle_component>();
			if (share)
				share->update();
		}
	}

	bool runtime_controller::load(const char* path)
	{
		try
		{
			return m_config.read(path);
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
		catch (...)
		{
			std::cerr << default_error::unknown << std::endl;
		}

		return false;
	}

	void runtime_controller::init()
	{
		try
		{
			{
				// Routers
				const auto& routers = m_config.get_routers();
				auto_lock_ref(routers);

				for (const auto& entity : routers)
				{
					// Servers
					const auto server_vector = entity->get<server_entity_vector_component>();
					auto_lock_ptr(server_vector);

					for (const auto& entity : *server_vector)
					{
						const auto option = entity->get<server_option_component>();
						if (option->enabled())
						{
							const auto machine = entity->get<server_machine_component>();
							machine->start();
						}
					}

					// Router
					const auto option = entity->get<router_option_component>();

					if (option->enabled())
					{
						const auto machine = entity->get<router_machine_component>();
						machine->set_threads(option->get_thread_pool());
						machine->start();

						const auto idle = entity->get<router_idle_component>();
						idle->start();
					}
				}
			}

			{
				// Clients
				const auto& clients = m_config.get_clients();
				auto_lock_ref(clients);

				for (const auto& entity : clients)
				{
					// Callback
					const auto function_map_list = entity->get<function_map_list_component>();
					assert(function_map_list && "m_entity->get<function_map_list_component>()");

					const auto transfer_function_list = function_map_list->get(function_type::transfer);
					const auto transfer_function = function::create([this](const function_action action, const entity::ptr entity) { on_transfer_callback(action, entity); });
					transfer_function_list->add(transfer_function);

					const auto upload_function_list = function_map_list->get(function_type::upload);
					const auto upload_function = function::create([this](const function_action action, const entity::ptr entity) { on_upload_callback(action, entity); });
					upload_function_list->add(upload_function);

					// Client
					const auto option = entity->get<client_option_component>();

					if (option->enabled())
					{
						const auto machine = entity->get<client_machine_component>();
						machine->start();

						const auto idle = entity->get<client_idle_component>();
						idle->start();

						const auto transfer = entity->get<transfer_idle_component>();
						transfer->start();
					}
				}
			}
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
		catch (...)
		{
			std::cerr << default_error::unknown << std::endl;
		}
	}

	void runtime_controller::shutdown()
	{
		try
		{
			// Clients
			boost::thread_group threads;
			auto& clients = m_config.get_clients();
			auto_lock_ref(clients);

			for (const auto& entity : clients)
			{
				threads.create_thread([entity]()
				{
					const auto service = std::make_shared<boost::asio::io_service>();
					const auto timer = std::make_shared<boost::asio::deadline_timer>(*service);
					timer->expires_from_now(boost::posix_time::milliseconds(default_timeout::client::shutdown));

					service->post([entity, timer]()
					{
						const auto machine = entity->get<client_machine_component>();
						if (machine)
						{
							if (machine->success())
							{
								const auto socket = http_socket::create();

								try
								{
									const auto router = entity->get<router_component>();
									if (router)
									{
										// Socket
										const auto option = entity->get<client_option_component>();
										if (option)
											socket->set_timeout(option->get_socket_timeout());

										socket->open(router->get_address(), router->get_port());

										// Quit
										const auto request_command = client_quit_request_command::create(entity, socket);
										request_command->execute();
									}
								}
								catch (...) {}

								socket->close();
							}
						}

						timer->cancel();
					});

					service->run();
				});
			}

			threads.join_all();

			clients.shutdown();

			// Routers
			auto& routers = m_config.get_routers(); 
			routers.shutdown();
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
		catch (...)
		{
			std::cerr << default_error::unknown << std::endl;
		}
	}

	// Transfer
	void runtime_controller::on_transfer_callback(const function_action action, const entity::ptr entity)
	{
		switch (action)
		{
			case function_action::add:
			{
				on_transfer_add(entity);
				break;
			}
			case function_action::remove:
			{
				on_transfer_remove(entity);
				break;
			}
			default:
			{
				assert(false);
			}
		}
	}

	void runtime_controller::on_transfer_add(const entity::ptr entity)
	{
		// Verify
		const auto owner = entity->get_entity();
		const auto transfer_vector = owner->get<transfer_entity_vector_component>();
		if (!transfer_vector || transfer_vector->has(entity))
			return;

		// Time
		if (!entity->has<timer_component>())
			entity->add<timer_component>();

		if (!entity->has<timeout_component>())
			entity->add<timeout_component>();

		// Error
		if (!entity->has<error_component>())
			entity->add<error_component>();

		// Chunk
		if (!entity->has<chunk_component>())
			entity->add<chunk_component>();

		transfer_vector->add(entity);
	}

	void runtime_controller::on_transfer_remove(const entity::ptr entity)
	{
		// Verify
		const auto owner = entity->get_entity();
		const auto transfer_vector = owner->get<transfer_entity_vector_component>();
		if (!transfer_vector)
			return;

		transfer_vector->remove(entity);
	}

	// Upload
	void runtime_controller::on_upload_callback(const function_action action, const entity::ptr entity)
	{
		switch (action)
		{
			case function_action::add:
			{
				on_upload_add(entity);
				break;
			}
			case function_action::remove:
			{
				on_upload_remove(entity);
				break;
			}
			default:
			{
				assert(false);
			}
		}
	}

	void runtime_controller::on_upload_add(const entity::ptr entity)
	{
		// Verify
		const auto owner = entity->get_entity();
		const auto upload_vector = owner->get<upload_entity_vector_component>();
		if (!upload_vector || upload_vector->has(entity))
			return;

		// Timer
		const auto timer = entity->get<timer_component>();
		if (!timer)
			return;

		timer->start();

		// Timeout
		const auto timeout = entity->get<timeout_component>();
		if (!timeout)
			return;

		timeout->start();

		// Thread
		const auto option = owner->get<client_option_component>();
		const auto threads = option ? option->get_upload_threads() : 1;
		const auto thread = upload_thread_component::create(threads);
		entity->add(thread);
		thread->start();

		upload_vector->add(entity);
	}

	void runtime_controller::on_upload_remove(const entity::ptr entity)
	{		
		// Verify
		const auto owner = entity->get_entity();
		const auto upload_vector = owner->get<upload_entity_vector_component>();
		if (!upload_vector)
			return;

		entity->shutdown();

		upload_vector->remove(entity);
	}

	// Utility
	void runtime_controller::routers()
	{
		const auto& routers = m_config.get_routers();
		if (routers.empty())
			return;

		try
		{
			endl();
			std::cout << "Routers" << std::endl;
			dash(80);

			const auto& routers = m_config.get_routers();
			auto_lock_ref(routers);

			for (const auto& entity : routers)
			{
				const auto router = entity->get<router_component>();
				if (!router)
					continue;

				const auto machine = entity->get<router_machine_component>();
				if (!machine)
					continue;

				const auto client_map = entity->get<client_entity_map_component>();
				if (!client_map)
					continue;

				const auto group_map = entity->get<group_entity_map_component>();
				if (!group_map)
					continue;

				std::cout << boost::format("+ %-22s %-22s %-5u %5s %8u %5u %5u\n") % router->get_name() % router->get_address() % router->get_port() % router->get_version() % machine->get_status() % client_map->size() % group_map->size();

				// Servers
				const auto server_vector = entity->get<server_entity_vector_component>();
				if (!server_vector)
					continue;

				auto_lock_ptr(server_vector);

				for (const auto& entity : *server_vector)
				{
					const auto server = entity->get<server_component>();
					if (!server)
						continue;

					const auto machine = entity->get<server_machine_component>();
					if (!machine)
						continue;

					std::cout << boost::format("  %-22s %-22s %-5u %5s %8u\n") % server->get_name() % server->get_address() % server->get_port() % server->get_version() % machine->get_status();
				}
			}

			endl();
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
		catch (...)
		{
			std::cerr << default_error::unknown << std::endl;
		}
	}

	void runtime_controller::clients()
	{
		try
		{
			endl();
			std::cout << "Clients" << std::endl;
			dash(80);

			// Clients
			const auto& clients = m_config.get_clients();

			if (!clients.empty())
			{				
				auto_lock_ref(clients);

				for (const auto& entity : clients)
				{
					const auto client = entity->get<client_component>();
					if (!client)
						continue;

					const auto machine = entity->get<client_machine_component>();
					if (!machine)
						continue;

					const auto upload = entity->get<upload_entity_vector_component>();
					if (!upload)
						continue;

					const auto finished = entity->get<finished_entity_vector_component>();
					if (!finished)
						continue;

					std::cout << boost::format("+ %-57s %8u %5u %5u\n") % client->get_name() % machine->get_status() % upload->size() % finished->size();

					// Shares
					const auto share = entity->get<share_idle_component>();
					if (!share)
						continue;

					const auto& folders = share->get_folders();
					auto_lock_ref(folders);

					for (const auto& folder : folders)
						std::cout << "   " << folder->get_path() << std::endl;
				}
			}

			endl();
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
		catch (...)
		{
			std::cerr << default_error::unknown << std::endl;
		}
	}

	void runtime_controller::transfers()
	{
		try
		{
			endl();
			std::cout << "Transfers" << std::endl;
			dash(80);

			const auto& routers = m_config.get_routers();
			auto_lock_ref(routers);

			for (const auto& entity : routers)
			{
				const auto router = entity->get<router_component>();
				if (!router)
					continue;

				const auto chunk_map = entity->get<chunk_entity_map_component>();
				if (!chunk_map)
					continue;

				std::cout << boost::format("+ %-72s %5u\n") % router->get_name() % chunk_map->size();
			}

			endl();
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
		catch (...)
		{
			std::cerr << default_error::unknown << std::endl;
		}
	}
}
