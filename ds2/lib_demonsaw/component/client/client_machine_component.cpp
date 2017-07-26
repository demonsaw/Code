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

#include "client_component.h"
#include "client_machine_component.h"
#include "client_option_component.h"
#include "component/tunnel_component.h"
#include "component/group/group_component.h"
#include "component/group/group_option_component.h"
#include "component/group/group_security_component.h"
#include "component/group/group_status_component.h"
#include "component/router/router_component.h"
#include "command/client/request/client_group_request_command.h"
#include "command/client/request/client_handshake_request_command.h"
#include "command/client/request/client_join_request_command.h"
#include "command/client/request/client_ping_request_command.h"
#include "command/client/request/client_quit_request_command.h"
#include "command/client/request/client_search_request_command.h"
#include "command/client/request/client_tunnel_request_command.h"
#include "command/client/response/client_browse_response_command.h"
#include "command/client/response/client_chat_response_command.h"
#include "command/client/response/client_ping_response_command.h"
#include "command/client/response/client_search_response_command.h"
#include "command/client/response/client_transfer_response_command.h"
#include "entity/entity.h"
#include "json/json_data.h"
#include "json/json_header.h"
#include "json/json_message.h"
#include "json/json_packet.h"
#include "utility/default_value.h"
#include "utility/io/folder_util.h"

namespace eja
{
	// Interface
	void client_machine_component::init()
	{
		machine_component::init();

		const auto owner = get_entity();
		const auto self = shared_from_this();
		std::thread thread([self, owner]() { self->main(); });
		thread.detach();
	}

	void client_machine_component::shutdown()
	{
		machine_component::shutdown();

		close();
	}

	void client_machine_component::restart()
	{
		ignore(true);

		machine_component::restart();

		const auto owner = get_entity();
		const auto group_vector = owner->get<group_entity_vector_component>();
		if (group_vector)
		{
			for (const auto& entity : group_vector->copy())
			{
				const auto status = entity->get<group_status_component>();
				if (status)
					status->set_status(eja::status::none);
			}
		}
		
		close();
	}

	void client_machine_component::stop()
	{
		ignore(true);

		machine_component::stop();

		// Clear the client list
		const auto owner = get_entity();
		const auto client_vector = owner->get<client_entity_vector_component>();
		if (client_vector)
			client_vector->clear();

		close();
	}

	bool client_machine_component::on_start()
	{
		set_status(status::pending);

		const auto owner = get_entity();

		try
		{						
			// Router
			const auto router = owner->get<router_component>();
			if (!router)
				return set_error();

			if (router->valid())
			{
				close();

				const auto option = owner->get<client_option_component>();
				if (!option)
					return set_error();

				//m_socket->set_keep_alive(true);
				m_socket->set_timeout(option->get_socket_timeout());
				m_socket->open(router->get_address(), router->get_port());

				set_status(status::info);
			}
			else
			{
				set_status(status::warning);
				set_state(machine_state::sleep, default_timeout::error);

				if (!router->has_address())
				{
					const auto str = boost::str(boost::format("Invalid IP address: %s") % router->get_address());
					log(str);
				}
				else if (asio_port::invalid(router->get_port()))
				{
					const auto str = boost::str(boost::format("Invalid port: %u") % router->get_port());
					log(str);
				}
				else
				{
					log("Invalid router config");
				}

				return machine_component::on_start();
			}

			// Group			
			const auto security = owner->get<client_security_component>();
			if (!security)
				return set_error();

			// Kill all previous transfers if we change groups (SECURITY)
			if (security->modified())
			{
				// Function
				call(function_type::transfer, function_action::clear, owner);
				call(function_type::download, function_action::clear, owner);
				call(function_type::upload, function_action::clear, owner);
				call(function_type::finished, function_action::clear, owner);
			}

			security->update();

			if (security->valid())
			{
				set_state(machine_state::handshake);
			}
			else
			{
				set_status(status::warning);
				set_state(machine_state::sleep, default_timeout::error);
			}
		}
		catch (std::exception& e)
		{
			// Clear the client list
			const auto client_vector = owner->get<client_entity_vector_component>();
			if (client_vector)
				client_vector->clear();

			set_status(status::error);
			set_state(machine_state::sleep, default_timeout::error);
			log(e);
		}
		catch (...)
		{
			// Clear the client list
			const auto client_vector = owner->get<client_entity_vector_component>();
			if (client_vector)
				client_vector->clear();

			set_status(status::error);
			set_state(machine_state::sleep, default_timeout::error);
			log();
		}

		return machine_component::on_start();
	}

	bool client_machine_component::on_restart()
	{
		close();

		ignore(false);

		return machine_component::on_restart();
	}

	bool client_machine_component::on_stop()
	{
		close();

		ignore(false);

		set_status(status::none);

		return machine_component::on_stop();
	}

	bool client_machine_component::on_handshake()
	{
		try
		{
			const auto owner = get_entity();
			const auto request_command = client_handshake_request_command<router_type::message>::create(owner, m_socket);
			const auto request_status = request_command->execute();

			if (request_status.is_ok())
			{				
				set_state(machine_state::join);
			}
			else
			{
				set_status(status::error); 
				set_state(machine_state::restart, default_timeout::error);
				log(request_status);
			}
		}
		catch (std::exception& e)
		{
			set_status(status::error); 
			set_state(machine_state::restart, default_timeout::error);
			log(e);
		}
		catch (...)
		{
			set_status(status::error); 
			set_state(machine_state::restart, default_timeout::error);
			log();
		}

		return machine_component::on_handshake();
	}

	bool client_machine_component::on_join()
	{
		try
		{
			const auto owner = get_entity();
			const auto request_command = client_join_request_command::create(owner, m_socket);
			const auto request_status = request_command->execute();

			if (request_status.is_ok())
			{
				// Only get the group the 1st time
				const auto client_vector = owner->get<client_entity_vector_component>();
				if (!client_vector)
					return set_error();

				set_state(!client_vector->empty() ? machine_state::tunnel : machine_state::group);
			}
			else
			{
				set_status(status::error);
				set_state(machine_state::restart, default_timeout::error);
				log(request_status);
			}
		}
		catch (std::exception& e)
		{
			set_status(status::error); 
			set_state(machine_state::restart, default_timeout::error);
			log(e);
		}
		catch (...)
		{
			set_status(status::error); 
			set_state(machine_state::restart, default_timeout::error);
			log();
		}

		return machine_component::on_join();
	}

	bool client_machine_component::on_group()
	{
		try
		{
			const auto owner = get_entity();			
			const auto request_command = client_group_request_command::create(owner, m_socket);
			const auto request_status = request_command->execute();

			if (request_status.is_ok())
			{
				set_state(machine_state::tunnel);
			}
			else
			{
				set_status(status::error);
				set_state(machine_state::restart, default_timeout::error);
				log(request_status);
			}
		}
		catch (std::exception& e)
		{
			set_status(status::error); 
			set_state(machine_state::restart, default_timeout::error);
			log(e);
		}
		catch (...)
		{
			set_status(status::error); 
			set_state(machine_state::restart, default_timeout::error);
			log();
		}

		return machine_component::on_group();
	}

	bool client_machine_component::on_tunnel()
	{
		try
		{
			const auto owner = get_entity();
			const auto request_command = client_tunnel_request_command::create(owner, m_socket);
			const auto request_status = request_command->execute();

			if (request_status.is_ok())
			{
				set_state(machine_state::run);
			}
			else
			{
				set_status(status::error);
				set_state(machine_state::restart, default_timeout::error);
				log(request_status);
			}
		}
		catch (std::exception& e)
		{
			set_status(status::error); 
			set_state(machine_state::restart, default_timeout::error);
			log(e);
		}
		catch (...)
		{
			set_status(status::error); 
			set_state(machine_state::restart, default_timeout::error);
			log();
		}

		return machine_component::on_tunnel();
	}

	bool client_machine_component::on_run()
	{
		try
		{
			bool loop = true;
			const auto owner = get_entity();
			set_status(status::success);

			// HACK: Do a reverse ping to determine that we are active
			ping();

			do
			{
				// Timeout
				m_socket->set_timeout(0);

				// Request
				http_request request;
				const size_t bytes = m_socket->read(request);
				if (!bytes || request.invalid())
				{
					if (!stopping())
					{
						/*const auto router = owner->get<router_component>();
						if (router)
						{
							const auto str = boost::str(boost::format("Reconnecting to \"%s\"") % router->get_address());
							log(str);
						}*/

						set_state(machine_state::restart);
					}						

					break;
				}

				const auto& body = request.get_body();
				const auto body_decoded = base64::decode(body);

				// Session
				const auto session = owner->get<session_component>();
				if (!session)
					return set_error();

				const auto body_decypted = session->decrypt(body_decoded);
				const auto request_packet = json_packet::create(body_decypted);

				// Header					
				const auto request_header = json_header::create(request_packet);
				if (request_header->invalid())
				{
					m_socket->write(http_code::bad_request);
					continue;
				}

				// Message
				const auto request_message = json_message::create(request_packet);
				if (request_message->invalid())
				{
					m_socket->write(http_code::bad_request);
					continue;
				}

				// Data
				const auto request_data = json_data::create(request_packet);
				if (request_data->invalid())
				{
					m_socket->write(http_code::bad_request);
					continue;
				}
				
				// Command
				switch (request_message->get_type())
				{
					case json_type::ping:
					{
						const auto command = client_ping_response_command::create(owner, m_socket);
						command->execute(request_message, request_data);
						break;
					}
					case json_type::search:
					{
						switch (request_message->get_action())
						{
							case json_action::request:
							{
								const auto command = client_search_request_command::create(owner, m_socket);
								command->execute(request_message, request_data);
								break;
							}
							case json_action::response:
							{
								const auto command = client_search_response_command::create(owner, m_socket);
								command->execute(request_message, request_data);
								break;
							}
						}
						
						break;
					}
					case json_type::browse:
					{
						const auto command = client_browse_response_command::create(owner, m_socket);
						command->execute(request_message, request_data);
						break;
					}
					case json_type::transfer:
					{
						const auto command = client_transfer_response_command::create(owner, m_socket);
						command->execute(request_message, request_data);
						break;
					}
					case json_type::chat:
					{
						const auto command = client_chat_response_command::create(owner, m_socket);
						command->execute(request_message, request_data);
						break;
					}
					default:
					{
						m_socket->write(http_code::not_implemented);
						break;
					}
				}
			}
			while (loop);
		}
		catch (std::exception& /*ex*/)
		{
			set_status(status::error); 
			set_state(machine_state::restart);
			//log(ex);
		}
		catch (...)
		{
			set_status(status::error); 
			set_state(machine_state::restart);
			//log();
		}

		return machine_component::on_run();
	}

	// Utility
	void client_machine_component::ping()
	{
		const auto owner = get_entity();
		const auto self = shared_from_this();
		std::thread thread([self, owner]()
		{
			try
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(default_timeout::error));

				const auto router = owner->get<router_component>();
				if (!router)
					return;

				const auto option = owner->get<client_option_component>();
				if (!option)
					return;

				// Socket
				const auto socket = http_socket::create();
				socket->set_timeout(option->get_socket_timeout());
				socket->open(router->get_address(), router->get_port());

				const auto request_command = client_ping_request_command::create(owner, socket);
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
						self->set_status(status::warning);

#if ANDROID
						self->restart();
#else
						const auto client = owner->get<client_component>();
						if (client)
						{
							const auto str = boost::str(boost::format("Failed to socket tunnel into \"%s\" (you won't be able to receive chats). Try using a non-standard port like 14480.") % router->get_address());
							self->log(str);
						}
#endif
						break;
					}
				}

				socket->close();
			}
			catch (std::exception& e)
			{
				self->set_status(status::warning);
				self->log(e);
			}
			catch (...)
			{
				self->set_status(status::warning);
				self->log();
			}
		});
		
		thread.detach();
	}

	void client_machine_component::close()
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
