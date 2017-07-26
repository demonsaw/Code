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

#include <thread>
#include <boost/format.hpp>

#include "server_component.h"
#include "server_machine_component.h"
#include "command/server/request/server_handshake_request_command.h"
#include "command/server/request/server_quit_request_command.h"
#include "entity/entity.h"
#include "utility/default_value.h"

namespace eja
{
	// Interface
	void server_machine_component::init()
	{
		machine_component::init();

		const auto owner = get_entity();
		const auto self = shared_from_this();
		std::thread thread([self, owner]() { self->main(); });
		thread.detach();
	}

	void server_machine_component::shutdown()
	{
		machine_component::shutdown();

		close();
	}

	void server_machine_component::restart()
	{
		ignore(true);

		machine_component::restart();

		close();
	}

	void server_machine_component::stop()
	{
		ignore(true);

		machine_component::stop();

		close();
	}
	
	// Callback
	bool server_machine_component::on_start()
	{
		const auto owner = get_entity();
		const auto server = owner->get<server_component>();
		if (!server)
			return set_error();

		if (server->valid())
		{
			try
			{
				close();

				const auto option = owner->get<server_option_component>();
				if (!option)
					return set_error();

				m_socket->set_timeout(option->get_socket_timeout()); 
				m_socket->open(server->get_address(), server->get_port());

				set_status(status::pending); 
				set_state(machine_state::handshake);
			}
			catch (std::exception& e)
			{
				if (!stopping())
				{
					set_status(status::error);
					set_state(machine_state::sleep, default_timeout::server::idle);
					log(e);
				}
			}
			catch (...)
			{
				if (!stopping())
				{
					set_status(status::error);
					set_state(machine_state::sleep, default_timeout::server::idle);
					log();
				}
			}
		}
		else
		{
			set_status(status::warning); 
			set_state(machine_state::sleep, default_timeout::server::idle);

			if (!server->has_address())
			{
				const auto str = boost::str(boost::format("Invalid IP address: %s") % server->get_address());
				log(str);
			}
			else if (asio_port::invalid(server->get_port()))
			{
				const auto str = boost::str(boost::format("Invalid port: %u") % server->get_port());
				log(str);
			}
			else
			{
				log("Invalid server config");
			}
		}

		return machine_component::on_start();
	}

	bool server_machine_component::on_restart()
	{
		close();

		ignore(false);

		return machine_component::on_restart();
	}

	bool server_machine_component::on_stop()
	{
		close();

		ignore(false);

		set_status(status::none);

		return machine_component::on_stop();
	}

	bool server_machine_component::on_handshake()
	{
		try
		{
			const auto owner = get_entity();
			const auto request_command = server_handshake_request_command::create(owner, m_socket);
			const auto request_status = request_command->execute(owner);

			if (request_status.is_ok())
			{
				set_status(status::success); 
				set_state(machine_state::quit);
			}
			else
			{
				if (!stopping())
				{
					set_status(status::error);
					set_state(machine_state::restart, default_timeout::server::idle);
					log(request_status);
				}
			}
		}
		catch (std::exception& e)
		{
			if (!stopping())
			{
				set_status(status::error);
				set_state(machine_state::restart, default_timeout::server::idle);
				log(e);
			}
		}
		catch (...)
		{
			if (!stopping())
			{
				set_status(status::error);
				set_state(machine_state::restart, default_timeout::server::idle);
				log();
			}
		}

		return machine_component::on_handshake();
	}

	bool server_machine_component::on_quit()
	{
		try
		{
			const auto owner = get_entity();
			const auto request_command = server_quit_request_command::create(owner, m_socket);
			const auto request_status = request_command->execute(owner);

			if (request_status.is_ok())
			{
				set_status(status::success); 
				set_state(machine_state::restart, default_timeout::server::idle);
			}
			else
			{
				if (!stopping())
				{
					set_status(status::error);
					set_state(machine_state::restart, default_timeout::server::idle);
					log(request_status);
				}
			}
		}
		catch (std::exception& e)
		{
			if (!stopping())
			{
				set_status(status::error);
				set_state(machine_state::restart, default_timeout::server::idle);
				log(e);
			}
		}
		catch (...)
		{
			if (!stopping())
			{
				set_status(status::error);
				set_state(machine_state::restart, default_timeout::server::idle);
				log();
			}
		}

		close();

		return machine_component::on_quit();
	}

	// Utility
	void server_machine_component::close()
	{
		try
		{
			m_socket->close();

			if (has_entity())
			{
				const auto owner = get_entity();
				if (owner)
				{
					const auto session = owner->get<session_component>();
					if (session)
						session->clear();
				}
			}
		}
		catch (std::exception& e)
		{
			log(e);
		}
		catch (...)
		{
			log();
		}
	}
}
