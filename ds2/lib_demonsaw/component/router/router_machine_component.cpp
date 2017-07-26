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
#include <thread>
#include <boost/format.hpp>

#include "router_component.h"
#include "router_machine_component.h"
#include "command/router/response/router_browse_response_command.h"
#include "command/router/response/router_chat_response_command.h"
#include "command/router/response/router_download_response_command.h"
#include "command/router/response/router_transfer_response_command.h"
#include "command/router/response/router_group_response_command.h"
#include "command/router/response/router_handshake_response_command.h"
#include "command/router/response/router_info_response_command.h"
#include "command/router/response/router_join_response_command.h"
#include "command/router/response/router_ping_response_command.h"
#include "command/router/response/router_quit_response_command.h"
#include "command/router/response/router_search_response_command.h"
#include "command/router/response/router_tunnel_response_command.h"
#include "command/router/response/router_upload_response_command.h"
#include "component/server/server_component.h"
#include "component/session_component.h"
#include "component/timer_component.h"
#include "entity/entity.h"
#include "json/json_data.h"
#include "json/json_header.h"
#include "json/json_message.h"
#include "json/json_packet.h"
#include "security/base.h"
#include "security/pbkdf.h"
#include "utility/default_value.h"

#include "http/http_acceptor.h"
#include "http/http_timer.h"
#include "thread/thread_info.h"

namespace eja
{
	// Constructor
	router_machine_component::router_machine_component()
	{
		m_threads = default_router::thread_pool;
	}

	// Operator
	router_machine_component& router_machine_component::operator=(const router_machine_component& comp)
	{
		if (this != &comp)
			acceptor_service_component::operator=(comp);

		return *this;
	}

	// Interface
	void router_machine_component::shutdown()
	{
		assert(thread_info::main());

		acceptor_service_component::shutdown();

		// Callback
		m_functions.clear();
	}

	void router_machine_component::clear()
	{
		assert(thread_info::main());

		acceptor_service_component::clear();

		// Callback
		m_functions.clear();
	}

	void router_machine_component::start()
	{
		assert(thread_info::main());

		acceptor_service_component::start();

		main();
	}

	void router_machine_component::stop()
	{
		assert(thread_info::main());

		acceptor_service_component::stop();

		// Status
		set_status(status::none);

		try
		{
			if (has_entity())
			{
				// Entity
				const auto owner = get_entity();
				const auto entity_map = owner->get<entity_map_component>();
				if (entity_map)
					entity_map->clear();

				// Session
				const auto session_map = owner->get<session_entity_map_component>();
				if (session_map)
					session_map->clear();

				// Client
				const auto client_map = owner->get<client_entity_map_component>();
				if (client_map)
					client_map->clear();

				// Group
				const auto group_map = owner->get<group_entity_map_component>();
				if (group_map)
					group_map->clear();

				// Chunk
				const auto chunk_map = owner->get<chunk_entity_map_component>();
				if (chunk_map)
					chunk_map->clear();
			}
		}
		catch (const std::exception& ex)
		{
			log(ex);
		}
		catch (...)
		{
			log();
		}
	}

	// Utility	
	void router_machine_component::main()
	{
		const auto owner = get_entity();

		try
		{
			// Status
			set_status(status::pending);

			// Open
			const auto router = owner->get<router_component>();
			m_acceptor->open(router->get_address(), router->get_port());			
			set_status(status::info);

			// Accept
			const auto self = shared_from_this();
			const auto service = get_service();
			m_acceptor->accept([self, service](const http_socket::ptr socket) { self->on_accept(service, socket); } );
			set_status(status::success);
		}
		catch (const std::exception& ex)
		{
			set_status(status::error);
			log(ex);

			on_main();
		}
	}

	// Utility
	void router_machine_component::on_accept(const io_service_ptr service, const http_socket::ptr socket)
	{
		bool close = true;

		try
		{
			assert(socket);

			{
				// Proxy
				thread_lock(m_sockets);
				m_sockets->insert(socket);
			}

			bool loop = false;			
			const auto owner = get_entity();
			const auto self = shared_from_this();

			do
			{
				// Timer
				socket->set_timeout(min_to_ms(3));
				//http_timer timer(service, [self, socket](const boost::system::error_code& error) { self->on_timer(socket, error); });
				//timer.start(min_to_ms(3));

				// Request
				http_request request;
				const size_t bytes = socket->read(request);

				// Timer
				socket->set_timeout(sec_to_ms(15));
				//timer.start(sec_to_ms(15));

				if (!bytes || request.invalid())
				{
					// Get
					if (request.is_get())
					{
						const auto option = owner->get<router_option_component>();
						if (option && option->has_redirect())
						{
							// HTTP 
							http_response response;
							response.set_status(http_code::see_other);
							response.set_location(option->get_redirect());
							socket->write(response);
						}
						else
						{
							socket->write(http_code::method_not_allowed);
						}
					}

					socket->close();
					break;
				}

				loop = request.has_keep_alive();
				const auto& body = request.get_body();
				const auto body_decoded = base64::decode(body);

				// Session
				if (!request.has_session())
				{					
					// Packet
					const auto security = owner->get<router_security_component>();
					if (!security)
					{
						socket->write(http_code::internal_server_error);
						break;
					}

					const auto body_decypted = security->decrypt(body_decoded);
					const auto request_packet = json_packet::create(body_decypted);

					// Header					
					const auto request_header = json_header::create(request_packet);
					if (request_header->invalid())
					{
						socket->write(http_code::bad_request);
						continue;
					}

					// Message
					const auto request_message = json_message::create(request_packet);
					if (request_message->invalid() || !request_message->is_handshake())
					{
						socket->write(http_code::bad_request);
						continue;
					}

					// Data
					auto request_data = json_data::create(request_packet);
					if (request_data->invalid())
					{
						socket->write(http_code::bad_request);
						continue;
					}

					// Command
					const auto command = router_handshake_response_command::create(owner, socket);
					command->execute(request_message, request_data);
					continue;
				}

				// Session Map				
				const auto session_map = owner->get<session_entity_map_component>();
				if (!session_map)
				{
					socket->write(http_code::internal_server_error);
					break;
				}

				// Entity
				const auto& session_id = request.get_session();
				const auto entity = session_map->get(session_id);
				if (!entity)
				{
					socket->write(http_code::unauthorized);
					continue;
				}

				// Session
				const auto session = entity->get<session_component>();
				if (!session)
				{
					socket->write(http_code::internal_server_error);
					break;
				}

				const auto body_decypted = session->decrypt(body_decoded);
				const auto request_packet = json_packet::create(body_decypted);

				// Header				
				const auto request_header = json_header::create(request_packet);
				if (request_header->invalid())
				{
					socket->write(http_code::bad_request);
					continue;
				}

				// Message
				const auto request_message = json_message::create(request_packet);
				if (request_message->invalid())
				{
					socket->write(http_code::bad_request);
					continue;
				}

				// Data
				const auto request_data = json_data::create(request_packet);
				if (request_data->invalid())
				{
					socket->write(http_code::bad_request);
					continue;
				}

				// Command
				switch (request_message->get_type())
				{
					case json_type::ping:
					{
						const auto command = router_ping_response_command::create(owner, socket);
						command->execute(entity, request_message, request_data);
						break;
					}
					case json_type::info:
					{
						const auto command = router_info_response_command::create(owner, socket);
						command->execute(entity, request_message, request_data);  
						break;
					}
					case json_type::handshake:
					{
						const auto command = router_handshake_response_command::create(owner, socket); 
						command->execute(entity, request_message, request_data);
						break;
					}
					case json_type::join:
					{
						const auto command = router_join_response_command::create(owner, socket);
						command->execute(entity, request_message, request_data);
						break;
					}
					case json_type::tunnel:
					{
						const auto option = owner->get<router_option_component>();
						if (option)
							socket->set_timeout(option->get_socket_timeout());

						const auto command = router_tunnel_response_command::create(owner, socket);
						command->execute(entity, request_message, request_data);
						loop = false;
						close = false;
						break;
					}
					case json_type::search:
					{
						const auto command = router_search_response_command::create(owner, socket);
						command->execute(entity, request_message, request_data);
						break;
					}
					case json_type::group:
					{
						const auto command = router_group_response_command::create(owner, socket);
						command->execute(entity, request_message, request_data);
						break;
					}
					case json_type::browse:
					{
						const auto command = router_browse_response_command::create(owner, socket);
						command->execute(entity, request_message, request_data);
						break;
					}
					case json_type::transfer:
					{
						const auto command = router_transfer_response_command::create(owner, socket);
						command->execute(entity, request_message, request_data);
						break;
					}
					case json_type::download:
					{
						const auto command = router_download_response_command::create(owner, socket);
						command->execute(entity, request_message, request_data);
						break;
					}
					case json_type::upload:
					{
						const auto command = router_upload_response_command::create(owner, socket);
						command->execute(entity, request_message, request_data);
						break;
					}
					case json_type::chat:
					{
						const auto command = router_chat_response_command::create(owner, socket);
						command->execute(entity, request_message, request_data);
						break;
					}
					case json_type::quit:
					{
						const auto command = router_quit_response_command::create(owner, socket);
						command->execute(entity, request_message, request_data);
						loop = false;						
						break;
					}
					default:
					{
						socket->write(http_code::not_found);
						loop = false;
						break;
					}
				}
			} while (loop);
		}
		catch (const std::exception& /*ex*/)
		{
			//log(ex);
		}
		catch (...)
		{
			//log();
		}

		// Close
		if (close)
			socket->close();

		// Proxy
		thread_lock(m_sockets);
		m_sockets->erase(socket);
	}

	void router_machine_component::on_main()
	{
		const auto self = shared_from_this();

		try
		{
			const auto timer = http_timer::create(m_service);
			timer->assign([self, timer](const boost::system::error_code& error)
			{
				try
				{
					if (error != boost::asio::error::operation_aborted)
						self->main();
				}
				catch (const std::exception& /*ex*/)
				{
					/*if (!self->m_service->stopped())
						self->log(ex);*/
				}
			});

			timer->start(default_timeout::error);
		}
		catch (const std::exception& /*ex*/)
		{
			/*if (!self->m_service->stopped())
				self->log(ex);*/
		}
	}

	// Set
	void router_machine_component::set_status(const eja::status status)
	{
		m_status = status;

		const auto owner = get_entity();

		auto_lock_ref(m_functions);
		for (const auto& function : m_functions)
			function->call(owner);
	}
}
