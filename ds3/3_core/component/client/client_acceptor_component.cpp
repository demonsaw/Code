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

#include "command/browse_command.h"
#include "command/chat_command.h"
#include "command/download_command.h"
#include "command/search_command.h"
#include "command/file_command.h"
#include "command/tunnel_command.h"
#include "command/upload_command.h"

#include "component/endpoint_component.h"
#include "component/header_component.h"
#include "component/state_component.h"
#include "component/client/client_acceptor_component.h"
#include "component/client/client_network_component.h"
#include "component/client/client_service_component.h"
#include "component/group/group_component.h"
#include "component/group/group_security_component.h"
#include "component/session/session_component.h"

#include "entity/entity.h"
#include "http/http_acceptor.h"
#include "http/http_request.h"
#include "http/http_response.h"
#include "http/http_socket.h"

#include "message/message_unpacker.h"
#include "message/request/request_message.h"
#include "message/request/browse_request_message.h"
#include "message/request/chat_request_message.h"
#include "message/request/search_request_message.h"
#include "message/request/file_request_message.h"

#include "message/response/response_message.h"
#include "message/response/browse_response_message.h"
#include "message/response/chat_response_message.h"
#include "message/response/search_response_message.h"
#include "message/response/file_response_message.h"

#include "system/type.h"
#include "thread/thread_info.h"
#include "utility/value.h"
#include "utility/router/router_util.h"

namespace eja
{
	// Operator
	client_acceptor_component& client_acceptor_component::operator=(const client_acceptor_component& comp)
	{
		if (this != &comp)
			acceptor_service_component::operator=(comp);

		return *this;
	}

	// Interface
	void client_acceptor_component::clear()
	{
		acceptor_service_component::clear();

		// Sessions
		const auto owner = get_entity();
		const auto session_map = owner->get<session_map_component>();
		{
			thread_lock(session_map);
			session_map->clear();
		}
	}

	// Utility
	void client_acceptor_component::start()
	{
		if (valid())
			return;

		const auto owner = get_entity();
		const auto state = owner->get<state_component>();

		if (state->has(eja::state::idle))
		{
			// Server
			const auto network = owner->get<client_network_component>();

			if (network->get_mode() == network_mode::server)
			{
				const auto endpoint = owner->get<endpoint_component>();
				if (endpoint->has_endpoint())
				{
					acceptor_service_component::start();

					async_open();
				}
				else
				{
					// Status
					set_status(owner, status::error);

					// Log
					owner->log("Invalid client listener address and port");
				}
			}
		}		
	}

	void client_acceptor_component::restart()
	{
		if (invalid())
			start();
		else
			acceptor_service_component::restart();
	}

	void client_acceptor_component::stop()
	{
		if (invalid())
			return;

		const auto owner = get_entity();
		const auto state = owner->get<state_component>();

		if (state->has(eja::state::idle))
		{
			acceptor_service_component::stop();

			// Status
			set_status(owner, status::none);
		}		
	}

	void client_acceptor_component::close()
	{				
		// Tunnels
		const auto owner = get_entity();
		const auto socket_set = owner->get<socket_set_component>();
		{
			thread_lock(socket_set);
			for (const auto& socket : *socket_set)
				socket->close();

			socket_set->clear();
		}
	}

	bool client_acceptor_component::has_redirect(const http_request::ptr request)
	{
		return request->is_get() && (!request->has_body() && !request->has_param());
	}

	// Open
	void client_acceptor_component::async_open()
	{
		const auto self = shared_from_this();
		post([self]() { self->open(); });
	}

	void client_acceptor_component::open()
	{
		const auto owner = get_entity();
		const auto self = shared_from_this();

		try
		{
			// Open			
			const auto endpoint = owner->get<endpoint_component>();

			const auto acceptor = get_acceptor();			
			acceptor->open(endpoint->get_address(), endpoint->get_port());

			// Accept			
			acceptor->accept([self, owner, acceptor](const boost::system::error_code& error, const http_socket::ptr socket)
			{
				try
				{
					if (!error)
					{
						self->add_socket(owner, socket);

						socket->set_timeout(milliseconds::tunnel);

						self->read_request(socket);
					}
				}
				catch (const std::exception& ex)
				{ 
					if (self->valid())
						owner->log(ex);
				}
			});
		}
		catch (const std::exception& ex)
		{
			wait(milliseconds::open, [self]() { self->async_open(); });

			if (valid())
			{
				set_status(owner, status::error);

				owner->log(ex);
			}
		}
	}

	// Request
	void client_acceptor_component::read_request(const http_socket::ptr socket)
	{
		// Read
		const auto owner = get_entity();
		const auto self = shared_from_this();

		socket->read<http_request>([self, owner, socket](const boost::system::error_code& error, const http_request::ptr request)
		{
			try
			{
				if (!error)
				{
					self->read_request(socket, request);
				}
				else
				{
					self->remove_socket(owner, socket);
				}
			}
			catch (const std::exception& ex)
			{
				self->remove_socket(owner, socket);

				if (self->valid())
					owner->log(ex);
			}
		});
	}	

	void client_acceptor_component::read_request(const http_socket::ptr socket, const http_request::ptr request)
	{
		// Redirect
		if (has_redirect(request))
			return write_response(socket);

		const auto owner = get_entity();

		try
		{
			// Entity
			const auto router = get_entity(request);
			if (!router)
				return write_ok(socket);

			// Request			
			auto request_body = request->get_body();
			if (request_body.empty())
				return write_not_found(socket);

			// Header			
			const auto header_map = owner->get<header_map_component>();

			// Response
			const auto response = http_response::create(http_code::ok);
			response->set_headers(*header_map);

			// Group			
			const auto group = router->get<group_component>();
			if (group->valid())
				request_body = group->decrypt(request_body);

			// Session			
			const auto session = router->get<session_component>();
			request_body = session->decrypt(request_body);

			// Message
			message_unpacker unpacker;
			unpacker.unpack(request_body.data(), request_body.size());
			message_packer packer;

			for (const auto& message : unpacker)
			{
				// Client Id
				const auto data_request = std::static_pointer_cast<data_message>(message);
				if (unlikely(!data_request->has_client()))
					continue;

				// Type
				switch (message->get_type())
				{
					case message_type::browse_request:
					{
						if (router_util::unmuted(owner, data_request->get_client()))
						{
							client_browse_command cmd(owner);
							const auto request = std::static_pointer_cast<browse_request_message>(message);
							const auto response = cmd.execute(router, request);
							if (response)
								packer.push_back(response);
						}

						break;
					}
					case message_type::browse_response:
					{
						client_browse_command cmd(owner);
						const auto response = std::static_pointer_cast<browse_response_message>(message);
						cmd.execute(router, response);

						break;
					}
					case message_type::chat_request:
					{
						if (router_util::unmuted(owner, data_request->get_client()))
						{
							client_chat_command cmd(owner);
							const auto request = std::static_pointer_cast<chat_request_message>(message);
							const auto response = cmd.execute(router, request);
							if (response)
								packer.push_back(response);
						}

						break;
					}
					case message_type::file_request:
					{
						if (router_util::unmuted(owner, data_request->get_client()))
						{
							client_file_command cmd(owner);
							const auto request = std::static_pointer_cast<file_request_message>(message);
							const auto response = cmd.execute(router, request);
							if (response)
								packer.push_back(response);
						}

						break;
					}
					case message_type::file_response:
					{
						client_file_command cmd(owner);
						const auto response = std::static_pointer_cast<file_response_message>(message);
						cmd.execute(router, response);

						break;
					}
					case message_type::search_request:
					{
						if (router_util::unmuted(owner, data_request->get_client()))
						{
							client_search_command cmd(owner);
							const auto request = std::static_pointer_cast<search_request_message>(message);
							const auto response = cmd.execute(router, request);
							if (response)
								packer.push_back(response);
						}

						break;
					}
					case message_type::search_response:
					{
						client_search_command cmd(owner);
						const auto response = std::static_pointer_cast<search_response_message>(message);
						cmd.execute(router, response);

						break;
					}
					default:
					{
						assert(false);

						return write_not_implemented(socket);
					}
				}
			}

			// Response		
			if (!packer.empty())
			{
				// Session
				/*const*/ auto response_body = packer.pack();
				response_body = session->encrypt(response_body);

				// Group
				if (group->valid())
					response_body = group->encrypt(response_body);

				response->set_body(std::move(response_body));
			}

			write_response(socket, response, async_state::read);
		}
		catch (const std::exception& ex)
		{
			write_ok(socket);

			if (valid())
				owner->log(ex);
		}
	}

	// Response	
	void client_acceptor_component::write_response(const http_socket::ptr socket)
	{
		// Response
		const auto response = http_response::create();

		// Headers
		const auto owner = get_entity();
		const auto header_map = owner->get<header_map_component>();
		response->set_headers(*header_map);

		// Redirect		
		const auto network = owner->get<client_network_component>();
		if (network->has_redirect())
		{
			response->set_status(http_code::see_other);
			response->set_header(http_header::location, network->get_redirect());
		}
		else
		{
			response->set_status(http_code::ok);
		}

		// Write
		write_response(socket, response, async_state::close);
	}

	void client_acceptor_component::write_response(const http_socket::ptr socket, const http_code code, const async_state state)
	{
		// Response
		const auto response = http_response::create(code);

		// Headers
		const auto owner = get_entity();
		const auto header_map = owner->get<header_map_component>();
		response->set_headers(*header_map);

		// Write
		write_response(socket, response, state);
	}

	void client_acceptor_component::write_response(const http_socket::ptr socket, const http_response::ptr response, const async_state state)
	{
		// Write
		const auto owner = get_entity();
		const auto self = shared_from_this();

		socket->write<http_response>(response, [self, owner, socket, response, state](const boost::system::error_code& error, const http_response::ptr response)
		{
			try
			{
				if (!error)
				{
					switch (state)
					{
						case async_state::read:
						{
							self->read_request(socket);
							
							break;
						}
						case async_state::close:
						{
							self->remove_socket(owner, socket);

							break;
						}
						default:
						{
							assert(false);
							
							break;
						}
					}
				}
				else
				{
					self->remove_socket(owner, socket);
				}
			}
			catch (const std::exception& ex)
			{
				self->remove_socket(owner, socket);

				if (self->valid())
					owner->log(ex);
			}
		});
	}

	// Set
	void client_acceptor_component::set_status()
	{
		if (invalid())
			return;

		const auto owner = get_entity();
		if (!owner)
			return;

		const auto service = owner->get<client_service_component>();
		service->set_status();
	}

	// Get
	entity::ptr client_acceptor_component::get_entity(const http_request::ptr request) const
	{
		// Existing?
		if (request->has_session())
		{
			const auto owner = get_entity();
			if (!owner)
				return nullptr;

			const auto session_map = owner->get<session_map_component>();

			thread_lock(session_map);
			const auto it = session_map->find(request->get_session());
			if (it != session_map->end())
				return it->second;
		}

		return nullptr;
	}
}
