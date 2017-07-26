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

#include <boost/format.hpp>

#include "client_component.h"
#include "client_idle_component.h"
#include "client_option_component.h"
#include "command/client/request/client_ping_request_command.h"
#include "command/client/request/client_quit_request_command.h"
#include "component/group/group_component.h"
#include "component/session_component.h"
#include "component/timer_component.h"
#include "component/client/client_component.h"
#include "component/client/client_machine_component.h"
#include "component/router/router_component.h"
#include "entity/entity.h"
#include "utility/default_value.h"

namespace eja
{
	// Interface
	void client_idle_component::shutdown()
	{
		idle_component::shutdown();

		close();
	}

	// Callback
	bool client_idle_component::on_run()
	{
		on_ping();

		return idle_component::on_run();
	}

	bool client_idle_component::on_stop()
	{
		close();

		return idle_component::on_stop();
	}

	void client_idle_component::on_ping()
	{
		close();

		// Machine
		const auto owner = get_entity();
		const auto machine = owner->get<client_machine_component>();
		if (!machine || !machine->success())
			return;

		// Option			
		const auto option = owner->get<client_option_component>();
		if (!option)
			return;

		try
		{
			// Delay
			set_delay(option->get_ping_timeout());

			// Router
			const auto router = owner->get<router_component>();
			if (!router)
				return;

			// Socket
			m_socket->set_timeout(option->get_socket_timeout());			
			m_socket->open(router->get_address(), router->get_port());

			const auto request_command = client_ping_request_command::create(owner, m_socket);
			const auto request_status = request_command->execute();
			const auto request_code = request_status.get_code();

			switch (request_code)
			{
				case http_code::ok:
				case http_code::not_implemented:
				{
					break;
				}
				default:
				{
					/*const auto client = owner->get<client_component>();
					if (client)
					{
						const auto str = boost::str(boost::format("Restarting \"%s\" due to code \"%s\"") % client->get_name() % request_status.get_code());
						log(str);
					}*/

					// Thread
					std::thread thread([this, owner, machine]()
					{
						const auto socket = http_socket::create();

						try
						{
							const auto router = owner->get<router_component>();
							if (router)
							{
								// Socket
								const auto option = owner->get<client_option_component>();
								if (option)
									socket->set_timeout(option->get_socket_timeout());

								socket->open(router->get_address(), router->get_port());

								// Quit
								const auto request_command = client_quit_request_command::create(owner, socket);
								request_command->execute();
							}
						}
						catch (...) {}

						socket->close();

						// Restart
						machine->restart();
					});

					thread.detach();

					break;
				}
			}
		}
		catch (std::exception& ex)
		{
			/*const auto client = owner->get<client_component>();
			if (client)
			{
				const auto str = boost::str(boost::format("Restarting \"%s\" due to \"%s\"") % client->get_name() % ex.what());
				log(str);
			}*/

			// Thread
			std::thread thread([this, owner, machine]()
			{
				const auto socket = http_socket::create();

				try
				{
					const auto router = owner->get<router_component>();
					if (router)
					{
						// Socket
						const auto option = owner->get<client_option_component>();
						if (option)
							socket->set_timeout(option->get_socket_timeout());

						socket->open(router->get_address(), router->get_port());

						// Quit
						const auto request_command = client_quit_request_command::create(owner, socket);
						request_command->execute();
					}
				}
				catch (...) {}

				socket->close();

				// Restart
				machine->restart();
			});

			thread.detach();
		}
		catch (...)
		{	
			/*const auto client = owner->get<client_component>();
			if (client)
			{
				const auto str = boost::str(boost::format("Restarting \"%s\"") % client->get_name());
				log(str);
			}*/

			// Thread
			std::thread thread([this, owner, machine]()
			{
				const auto socket = http_socket::create();

				try
				{
					const auto router = owner->get<router_component>();
					if (router)
					{
						// Socket
						const auto option = owner->get<client_option_component>();
						if (option)
							socket->set_timeout(option->get_socket_timeout());

						socket->open(router->get_address(), router->get_port());

						// Quit
						const auto request_command = client_quit_request_command::create(owner, socket);
						request_command->execute();
					}
				}
				catch (...) {}

				socket->close();

				// Restart
				machine->restart();
			});

			thread.detach();
		}

		close();
	}

	// Utility
	void client_idle_component::close()
	{
		try
		{
			m_socket->close();
		}
		catch (std::exception& /*ex*/)
		{
			//log(ex);
		}
		catch (...)
		{
			//log();
		}
	}
}
