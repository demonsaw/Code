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
#include <list>
#include <thread>

#include "command/browse_command.h"
#include "command/chat_command.h"
#include "command/client_command.h"
#include "command/download_command.h"
#include "command/group_command.h"
#include "command/handshake_command.h"
#include "command/join_command.h"
#include "command/quit_command.h"
#include "command/search_command.h"
#include "command/file_command.h"
#include "command/socket_command.h"
#include "command/tunnel_command.h"
#include "command/upload_command.h"

#include "component/endpoint_component.h"
#include "component/error_component.h"
#include "component/header_component.h"
#include "component/socket_component.h"
#include "component/state_component.h"
#include "component/tunnel_component.h"
#include "component/version_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/group/group_component.h"
#include "component/group/group_security_component.h"
#include "component/io/download_component.h"
#include "component/message/message_acceptor_component.h"
#include "component/message/message_network_component.h"
#include "component/session/session_component.h"
#include "component/status/status_component.h"
#include "component/time/timeout_component.h"

#include "data/service_data.h"
#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "http/http_acceptor.h"
#include "http/http_request.h"
#include "http/http_response.h"
#include "http/http_socket.h"

#include "message/message_packer.h"
#include "message/message_unpacker.h"

#include "message/request/request_message.h"
#include "message/request/browse_request_message.h"
#include "message/request/chat_request_message.h"
#include "message/request/client_request_message.h"
#include "message/request/handshake_request_message.h"
#include "message/request/join_request_message.h"
#include "message/request/quit_request_message.h"
#include "message/request/socket_request_message.h"
#include "message/request/tunnel_request_message.h"

#include "message/response/response_message.h"
#include "message/response/browse_response_message.h"
#include "message/response/chat_response_message.h"
#include "message/response/client_response_message.h"
#include "message/response/handshake_response_message.h"
#include "message/response/join_response_message.h"
#include "message/response/quit_response_message.h"
#include "message/response/socket_response_message.h"
#include "message/response/tunnel_response_message.h"

#include "security/security.h"
#include "security/cipher/cipher.h"
#include "security/filter/hex.h"
#include "system/type.h"
#include "thread/thread_info.h"
#include "utility/value.h"

namespace eja
{
	// Operator
	message_acceptor_component& message_acceptor_component::operator=(const message_acceptor_component& comp)
	{
		if (this != &comp)
			acceptor_service_component::operator=(comp);

		return *this;
	}

	// Interface
	void message_acceptor_component::update()
	{
		assert(thread_info::main());

		acceptor_service_component::update();

		// Thread-safe
		m_group_list = copy_if_enabled<group_security_list_component>();
		m_transfer_list = copy_if_enabled<transfer_list_component>();

		set_status();
	}

	void message_acceptor_component::clear()
	{
		assert(thread_info::main());

		acceptor_service_component::clear();

		// Client
		const auto owner = get_entity();
		const auto group = owner->get<group_component>();
		group->clear();

		// Sessions
		const auto session_map = owner->get<session_map_component>();
		{
			thread_lock(session_map);
			session_map->clear();
		}

		// Groups
		const auto group_map = owner->get<group_map_component>();
		{
			thread_lock(group_map);
			group_map->clear();
		}

		// Thread-safe
		m_group_list.reset();
		m_transfer_list.reset();

		// Callback
		owner->call(callback_type::session, callback_action::clear);
		owner->call(callback_type::client, callback_action::clear);
		owner->call(callback_type::group, callback_action::clear);
		owner->call(callback_type::transfer, callback_action::clear);
		owner->call(callback_type::download, callback_action::clear);
		//owner->call(callback_type::error, callback_action::clear);
	}

	// Utility
	void message_acceptor_component::start()
	{
		if (valid())
			return;

		const auto owner = get_entity();
		const auto state = owner->get<state_component>();

		if (state->set(eja::state::idle, eja::state::none))
		{
			acceptor_service_component::start();

			if (thread_info::main())
				update();

			// Group
			async_group();

			// Transfer
			async_ping();

			// Timeout
			async_timeout();

			state->set(eja::state::running);
		}
	}

	void message_acceptor_component::restart()
	{
		if (invalid())
			return;

		const auto owner = get_entity();
		const auto state = owner->get<state_component>();

		if (state->set(eja::state::idle, eja::state::running))
		{
			// Stop
			acceptor_service_component::stop();

			// Start
			acceptor_service_component::start();

			if (thread_info::main())
				update();

			// Group
			async_group();

			// Transfer
			async_ping();

			// Timeout
			async_timeout();

			state->set(eja::state::running);
		}
	}

	void message_acceptor_component::stop()
	{
		if (invalid())
			return;

		const auto owner = get_entity();
		const auto state = owner->get<state_component>();

		if (state->set(eja::state::idle, eja::state::running))
		{
			acceptor_service_component::stop();

			// Status
			set_status(owner, status::none);

			state->set(eja::state::none);
		}
	}

	void message_acceptor_component::close()
	{
		// Sockets
		const auto owner = get_entity();
		const auto socket_set = owner->get<socket_set_component>();
		{
			thread_lock(socket_set);
			for (const auto& socket : *socket_set)
				socket->close();

			socket_set->clear();
		}

		// Clients
		const auto client_map = owner->get<client_map_component>();
		{
			thread_lock(client_map);
			
			for (const auto& pair : *client_map)
			{
				const auto& entity = pair.second;

				// Tunnels
				const auto tunnel_list = entity->get<tunnel_list_component>();
				{
					thread_lock(tunnel_list);
					for (const auto& socket : *tunnel_list)
						socket->close();

					tunnel_list->clear();
				}
			}

			client_map->clear();
		}
	}

	void message_acceptor_component::close(const entity::ptr entity)
	{
		// Sockets
		const auto socket_set = entity->get<socket_set_component>();
		{
			thread_lock(socket_set);
			for (const auto& socket : *socket_set)
				socket->close();

			socket_set->clear();
		}
	}

	bool message_acceptor_component::has_redirect(const http_request::ptr request)
	{
		return request->is_get() && (!request->has_body() && !request->has_param());
	}

	// Group
	void message_acceptor_component::async_group()
	{
		const auto self = shared_from_this();
		post([self]() { self->group(); });
	}

	void message_acceptor_component::group()
	{
		// Status
		const auto owner = get_entity();
		set_status(owner, status::pending);

		try
		{			
			// Command
			entropy_command cmd(owner);
			const auto group_list = get_group_security_list();
			cmd.execute(group_list);

			// Callback
			owner->call(callback_type::group, callback_action::update);

			// Message
			async_open();
		}
		catch (const std::exception& ex)
		{
			set_status(owner, status::error);

			owner->log(ex);
		}
	}

	// Open
	void message_acceptor_component::async_open()
	{
		const auto self = shared_from_this();
		post([self]() { self->open(); });
	}

	void message_acceptor_component::open()
	{
		const auto owner = get_entity();
		set_status(owner, status::info);

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

			// Status
			set_status(owner, status::success);
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

	// Ping
	void message_acceptor_component::async_ping()
	{
		const auto transfer_list = get_transfer_list();
		for (const auto& e : *transfer_list)
			async_ping(e);
	}	

	void message_acceptor_component::async_ping(const entity::ptr router)
	{
		const auto self = shared_from_this();
		post([self, router]() { self->ping(router); });
	}

	void message_acceptor_component::ping(const entity::ptr router)
	{
		// Status
		const auto status = router->get<status_component>();
		status->set_status(eja::status::pending);

		const auto service = get_service();
		const auto socket = http_socket::create(service);

		const auto owner = get_entity();
		const auto self = shared_from_this();
		const auto endpoint = router->get<endpoint_component>();

		socket->open(endpoint->get_address(), endpoint->get_port(), [self, owner, router, socket, status](const boost::system::error_code& error)
		{
			try
			{
				self->set_status(router, !error ? eja::status::success : eja::status::error);
			}
			catch (const std::exception& ex)
			{
				if (self->valid())
				{
					self->set_status(router, eja::status::error);

					owner->log(ex);
				}				
			}
		});
	}

	//  Timeout
	void message_acceptor_component::async_timeout()
	{
		const auto self = shared_from_this();

		wait(milliseconds::timeout, [self]()
		{ 
			if (self->valid())
				self->timeout();
		});
	}

	void message_acceptor_component::timeout()
	{
		try
		{
			// Ping
			async_ping();

			// Transfer
			const auto owner = get_entity();
			const auto download_map = owner->get<download_map_component>();
			{				
				thread_lock(download_map);

				for (auto it = download_map->begin(); it != download_map->end(); /*++it*/)
				{
					const auto& pair = *it;
					const auto& entity = pair.second;
					const auto timeout = entity->get<timeout_component>();
					
					if (timeout->expired(milliseconds::transfer))
						download_map->erase(it++);
					else
						++it;
				}
			}

			// Client
			std::list<entity::ptr> clients;
			const auto client_map = owner->get<client_map_component>();
			{
				thread_lock(client_map);

				for (auto it = client_map->begin(); it != client_map->end(); ++it)
				{
					const auto& pair = *it;
					const auto& entity = pair.second;
					const auto endpoint = entity->get<endpoint_component>();
					const auto timeout = entity->get<timeout_component>();

					if (timeout->expired(!endpoint->has_endpoint() ? milliseconds::client : milliseconds::server))
						clients.push_back(entity);
				}
			}

			router_quit_command cmd(owner);
			for (const auto& e : clients)
				cmd.clear(e);

			// Repeat
			async_timeout();
		}
		catch (const std::exception& ex)
		{
			log(ex);
		}
	}

	// Request
	void message_acceptor_component::read_request(const http_socket::ptr socket, const service_data::ptr data /*= nullptr*/)
	{
		// Read
		const auto owner = get_entity();
		const auto self = shared_from_this();

		socket->read<http_request>([self, owner, socket, data](const boost::system::error_code& error, const http_request::ptr request)
		{
			try
			{
				if (!error)
				{
					self->read_request(socket, request, data);
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

	void message_acceptor_component::read_request(const http_socket::ptr socket, const http_request::ptr request, const service_data::ptr data /*= nullptr*/)
	{		
		// Redirect
		if (has_redirect(request))
			return write_response(socket, data);
		
		entity::ptr client;
		const auto owner = get_entity();

		try
		{
			// Entity
			client = get_entity(request);

			if (!client)
			{
				if (request->has_session())
					return write_unauthorized(socket);

				client = entity_factory::create_message_client();
			}

			// Request			
			auto request_body = request->get_body();
			if (request_body.empty())
				return write_not_found(socket);

			// Group
			const auto group = owner->get<group_component>();
			if (group->valid())
				request_body = group->decrypt(request_body);

			// Session
			auto session = client->get<session_component>();
			request_body = session->decrypt(request_body);

			// Message
			message_unpacker unpacker;
			unpacker.unpack(request_body.data(), request_body.size());

			const auto network = owner->get<message_network_component>();
			auto state = async_state::read;
			message_packer packer;

			for (const auto& message : unpacker)
			{
				switch (message->get_type())
				{
					case message_type::browse_request:
					{
						if (!network->has_message())
							return write_not_implemented(socket);

						if (session->invalid())
							return write_unauthorized(socket);

						router_browse_command cmd(owner);
						const auto request = std::static_pointer_cast<browse_request_message>(message);
						const auto response = cmd.execute(client, request);
						if (response)
							packer.push_back(response);

						break;
					}
					case message_type::chat_request:
					{
						if (!network->has_message())
							return write_not_implemented(socket);

						if (session->invalid())
							return write_unauthorized(socket);

						router_chat_command cmd(owner);
						const auto request = std::static_pointer_cast<chat_request_message>(message);
						const auto response = cmd.execute(client, request);
						if (response)
							packer.push_back(response);

						break;
					}
					case message_type::client_request:
					{
						if (!network->has_message())
							return write_not_implemented(socket);

						if (session->invalid())
							return write_unauthorized(socket);

						router_client_command cmd(owner);
						const auto request = std::static_pointer_cast<client_request_message>(message);
						const auto response = cmd.execute(client, request);
						if (response)
							packer.push_back(response);

						break;
					}
					case message_type::download_request:
					{
						if (!network->has_transfer())
							return write_not_implemented(socket);

						if (session->invalid())
							return write_unauthorized(socket);

						router_download_command cmd(owner);
						const auto request = std::static_pointer_cast<download_request_message>(message);
						const auto response = cmd.execute(client, request);
						if (response)
							packer.push_back(response);

						break;
					}
					case message_type::file_request:
					{
						if (!network->has_message())
							return write_not_implemented(socket);

						if (session->invalid())
							return write_unauthorized(socket);

						router_file_command cmd(owner);
						const auto request = std::static_pointer_cast<file_request_message>(message);
						const auto response = cmd.execute(client, request);
						if (response)
							packer.push_back(response);

						break;
					}
					case message_type::handshake_request:
					{
						/*if (session->invalid())
						return write_unauthorized(socket);*/

						// Copy the cipher
						session = session->copy();

						// Version
						const auto request = std::static_pointer_cast<handshake_request_message>(message);
						
						// BETA CODE - REMOVE ME EVENTUALLY
						if (request->has_version())
						{							
							version_component version(request->get_version());
							if (version < version::min)
								return write_not_implemented(socket);
						}
						
						router_handshake_command cmd(owner);
						const auto response = cmd.execute(client, request);
						if (response)
							packer.push_back(response);

						break;
					}
					case message_type::join_request:
					{
						if (!network->has_message())
							return write_not_implemented(socket);

						if (session->invalid())
							return write_unauthorized(socket);

						// Public
						const auto request = std::static_pointer_cast<join_request_message>(message);
						if (!network->has_open() && !request->has_group())
							return write_not_implemented(socket);

						router_join_command cmd(owner);
						const auto response = cmd.execute(client, request);
						if (response)
							packer.push_back(response);

						break;
					}
					case message_type::quit_request:
					{
						if (session->invalid())
							return write_unauthorized(socket);

						router_quit_command cmd(owner);
						const auto request = std::static_pointer_cast<quit_request_message>(message);
						const auto response = cmd.execute(client, request);
						if (response)
							packer.push_back(response);

						break;
					}
					case message_type::search_request:
					{
						if (!network->has_message())
							return write_not_implemented(socket);

						if (session->invalid())
							return write_unauthorized(socket);

						router_search_command cmd(owner);
						const auto request = std::static_pointer_cast<search_request_message>(message);
						const auto callback = cmd.execute(client, request);
						if (callback)
							packer.push_back(callback);

						break;
					}
					case message_type::socket_request:
					{
						if (!network->has_message())
							return write_not_implemented(socket);

						if (session->invalid())
							return write_unauthorized(socket);

						router_socket_command cmd(owner);
						const auto request = std::static_pointer_cast<socket_request_message>(message);
						const auto response = cmd.execute(client, socket, request);
						if (response)
							packer.push_back(response);

						// Tunnel
						const auto endpoint = client->get<endpoint_component>();
						if (!endpoint->has_endpoint())
							state = async_state::none;

						break;
					}
					case message_type::tunnel_request:
					{
						if (!network->has_message())
							return write_not_implemented(socket);

						if (session->invalid())
							return write_unauthorized(socket);

						router_tunnel_command cmd(owner);
						const auto request = std::static_pointer_cast<tunnel_request_message>(message);
						const auto response = cmd.execute(client, socket, request);
						if (response)
							packer.push_back(response);

						break;
					}
					case message_type::upload_request:
					{
						if (!network->has_transfer())
							return write_not_implemented(socket);

						if (session->invalid())
							return write_unauthorized(socket);

						router_upload_command cmd(owner);
						const auto request = std::static_pointer_cast<upload_request_message>(message);
						const auto response = cmd.execute(client, request);
						if (response)
							packer.push_back(response);

						break;
					}
					default:
					{
						if (session->invalid())
							return write_unauthorized(socket);

						assert(false);
						return write_not_implemented(socket);
					}
				}
			}

			// Response
			const auto response = http_response::create(http_code::ok);

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

			const auto header_map = owner->get<header_map_component>();
			response->set_headers(*header_map);

			// Timer
			const auto timeout = client->get<timeout_component>();
			timeout->update();

			write_response(socket, response, state, data);
		}
		catch (const CryptoPP::Exception& /*ex*/)
		{
			// Command
			router_quit_command cmd(owner);
			cmd.clear(client);

			write_unauthorized(socket);
		}
		catch (const std::exception& ex)
		{
			write_internal_service_error(socket);

			if (valid())
				owner->log(ex);
		}
	}

	void message_acceptor_component::write_request(const entity::ptr client, const message_packer::ptr packer, const service_data::ptr data /*= nullptr*/)
	{
		const auto owner = get_entity();
		const auto self = shared_from_this();
		const auto endpoint = client->get<endpoint_component>();

		if (endpoint->has_endpoint())
		{
			auto socket = get_tunnel(client);

			if (socket && socket->valid())
			{
				return write_request(client, socket, packer, data);
			}
			else if (!socket)
			{
				socket = http_socket::create(get_service());
			}

			// Async			
			socket->open(endpoint->get_address(), endpoint->get_port(), [self, owner, client, data, socket, packer](const boost::system::error_code& error)
			{
				try
				{
					if (!error)
					{
						//socket->set_keep_alive(true);

						self->write_request(client, socket, packer, data);
					}
					else
					{
						// Retry
						if (data && data->has_retry(true))
							self->wait(milliseconds::open, [self, client, packer, data]() { self->write_request(client, packer, data); });
					}
				}
				catch (const std::exception& ex)
				{
					// Retry
					if (data && data->has_retry(true))
						self->wait(milliseconds::open, [self, client, packer, data]() { self->write_request(client, packer, data); });

					if (self->valid())
						owner->log(ex);
				}
			});
		}
		else
		{
			const auto socket = get_tunnel(client);

			if (socket)
			{
				write_request(client, socket, packer, data);
			}
			else
			{
				// Retry
				if (data && data->has_retry(true))
					self->wait(rand() % milliseconds::retry, [self, client, packer, data]() { self->write_request(client, packer, data); });
				else
					owner->log("Not enough tunnels...");
			}
		}
	}

	void message_acceptor_component::write_request(const entity::ptr client, const http_socket::ptr socket, const message_packer::ptr packer, const service_data::ptr data /*= nullptr*/)
	{
		const auto owner = get_entity();
		const auto self = shared_from_this();

		try
		{
			// Request
			/*const*/ auto request_body = packer->pack();

			// Session			
			const auto session = client->get<session_component>();
			if (session->invalid())
			{
				add_tunnel(client, socket);

				// Retry
				if (data && data->has_retry(true))
					self->wait(rand() % milliseconds::retry, [self, client, packer, data]() { self->write_request(client, packer, data); });

				// Log
				return owner->log("Invalid session");
			}

			request_body = session->encrypt(request_body);

			// Group			
			const auto group = owner->get<group_component>();
			if (group->valid())
				request_body = group->encrypt(request_body);

			const auto request = http_request::create(http::post);
			request->set_body(std::move(request_body));

			// Headers			
			const auto header_map = owner->get<header_map_component>();
			request->set_headers(*header_map);

			// Session
			request->set_session(session->get_id());

			const auto endpoint = client->get<endpoint_component>();
			if (endpoint->has_endpoint())
				request->set_host(endpoint->get_address());

			// Write			
			socket->write<http_request>(request, [self, owner, client, socket, request, packer, data](const boost::system::error_code& error, const http_request::ptr request)
			{
				try
				{
					if (!error)
					{
						self->read_response(client, socket, packer, data);
					}
					else
					{
						if (self->valid())
						{
							// Retry
							if (data && data->has_retry(true))
								self->wait(rand() % milliseconds::retry, [self, client, socket, packer, data]() { self->write_request(client, socket, packer, data); });
						}

					}
				}
				catch (const std::exception& ex)
				{
					// Retry
					if (data && data->has_retry(true))
						self->wait(rand() % milliseconds::retry, [self, client, socket, packer, data]() { self->write_request(client, socket, packer, data); });

					if (self->valid())
						owner->log(ex);
				}
			});
		}
		catch (const std::exception& ex)
		{
			// Retry
			if (data && data->has_retry(true))
				self->wait(rand() % milliseconds::retry, [self, client, socket, packer, data]() { self->write_request(client, socket, packer, data); });

			if (valid())
				owner->log(ex);
		}		
	}

	// Response
	void message_acceptor_component::read_response(const entity::ptr client, const http_socket::ptr socket, const message_packer::ptr packer, const service_data::ptr data /*= nullptr*/)
	{
		const auto owner = get_entity();
		const auto self = shared_from_this();

		// Read
		socket->read<http_response>([self, owner, client, socket, packer, data](const boost::system::error_code& error, const http_response::ptr response)
		{
			try
			{
				if (!error)
				{
					self->add_tunnel(client, socket);

					const auto& hstatus = response->get_status();
					if (hstatus.ok())
						self->read_response(client, response, data);
				}
				else
				{
					// Retry
					if (data && data->has_retry(true))
						self->wait(rand() % milliseconds::retry, [self, client, socket, packer, data]() { self->write_request(client, socket, packer, data); });
				}
			}
			catch (const std::exception& ex)
			{
				// Retry
				if (data && data->has_retry(true))
					self->wait(rand() % milliseconds::retry, [self, client, socket, packer, data]() { self->write_request(client, socket, packer, data); });

				if (self->valid())
					owner->log(ex);
			}
		});
	}

	void message_acceptor_component::read_response(const entity::ptr client, const http_response::ptr response, const service_data::ptr data /*= nullptr*/)
	{
		const auto owner = get_entity();

		try
		{
			// Response
			auto response_body = response->get_body();
			if (response_body.empty())
				return;

			// Group			
			const auto group = owner->get<group_component>();
			if (group->valid())
				response_body = group->decrypt(response_body);

			// Session		
			const auto session = client->get<session_component>();
			response_body = session->decrypt(response_body);

			// Unpack
			message_unpacker unpacker;
			unpacker.unpack(response_body.data(), response_body.size());
			if (unpacker.empty())
				return;

			// Message
			for (const auto& message : unpacker)
			{
				switch (message->get_type())
				{
					case message_type::browse_response:
					{						
						const auto response = std::static_pointer_cast<browse_response_message>(message);

						// Optimization
						if (response->has_data())
						{
							router_browse_command cmd(owner);
							cmd.execute(client, data->get_entity(), response);
						}						

						break;
					}
					case message_type::chat_response:
					{						
						const auto response = std::static_pointer_cast<chat_response_message>(message);

						// Optimization
						if (response->has_data())
						{
							router_chat_command cmd(owner);
							cmd.execute(client, data->get_entity(), response);
						}						

						break;
					}
					case message_type::file_response:
					{
						const auto response = std::static_pointer_cast<file_response_message>(message);

						// Optimization
						if (response->has_data())
						{
							router_file_command cmd(owner);
							cmd.execute(client, data->get_entity(), response);
						}

						break;
					}
					case message_type::search_response:
					{						
						const auto response = std::static_pointer_cast<search_response_message>(message);
						
						// Optimization
						if (response->has_data())
						{
							router_search_command cmd(owner);
							cmd.execute(client, data->get_entity(), response);
						}						

						break;
					}					
					default:
					{
						assert(false);
					}
				}
			}

			// Timeout
			const auto timeout = client->get<timeout_component>();
			timeout->update();
		}
		catch (const std::exception& ex)
		{
			if (valid())
				owner->log(ex);
		}
	}

	void message_acceptor_component::write_response(const http_socket::ptr socket, const service_data::ptr data /*= nullptr*/)
	{
		// Response
		const auto response = http_response::create();

		// Headers
		const auto owner = get_entity();
		const auto header_map = owner->get<header_map_component>();
		response->set_headers(*header_map);

		// Redirect
		const auto network = owner->get<message_network_component>();

		if (network->has_redirect())
		{
			response->set_status(http_code::see_other);
			response->set_header(http_header::location, network->get_redirect());
		}
		else
		{
			response->set_status(http_code::not_found);
		}

		// Write
		write_response(socket, response, async_state::close, data);
	}

	void message_acceptor_component::write_response(const http_socket::ptr socket, const http_code code, const async_state state, const service_data::ptr data /*= nullptr*/)
	{
		// Response
		const auto response = http_response::create(code);

		// Headers
		const auto owner = get_entity();
		const auto header_map = owner->get<header_map_component>();
		response->set_headers(*header_map);

		// Write
		write_response(socket, response, state, data);
	}

	void message_acceptor_component::write_response(const http_socket::ptr socket, const http_response::ptr response, const async_state state, const service_data::ptr data /*= nullptr*/)
	{
		// Write
		const auto owner = get_entity();
		const auto self = shared_from_this();

		socket->write<http_response>(response, [self, owner, data, socket, response, state](const boost::system::error_code& error, const http_response::ptr response)
		{
			try
			{
				if (!error)
				{
					self->set_status(owner, status::success);

					switch (state)
					{
						case async_state::read:
						{
							self->read_request(socket, data);

							break;
						}
						case async_state::close:
						{							
							self->remove_socket(owner, socket);

							break;
						}
						default:
						{
							// NOTE: Fall through for tunnels
							//

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
	void message_acceptor_component::set_status()
	{
		if (invalid())
			return;

		// Transfer	
		const auto transfer_list = get_transfer_list();
		if (transfer_list->empty())
			return;

		size_t values[static_cast<size_t>(eja::status::max)] = { 0 };

		for (const auto& entity : *transfer_list)
		{
			const auto status = entity->get<status_component>();
			const auto pos = static_cast<size_t>(status->get_status());
			++values[pos];
		}

		const auto owner = get_entity();
		const auto status = owner->get<status_component>();

		// Warning
		if ((values[static_cast<size_t>(eja::status::error)] + values[static_cast<size_t>(eja::status::warning)]) == transfer_list->size())
			return status->set_status(eja::status::warning);

		// Pending
		if (values[static_cast<size_t>(eja::status::pending)])
			return status->set_status(eja::status::pending);

		// Info
		if (values[static_cast<size_t>(eja::status::info)])
			return status->set_status(eja::status::info);

		// Success
		if (values[static_cast<size_t>(eja::status::success)])
			return status->set_status(eja::status::success);
	}

	// Get
	entity::ptr message_acceptor_component::get_entity(const http_request::ptr request) const
	{
		// Existing?
		if (request->has_session())
		{
			const auto owner = get_entity();
			const auto session_map = owner->get<session_map_component>();

			thread_lock(session_map);
			const auto it = session_map->find(request->get_session());
			if (it != session_map->end())
				return it->second;
		}

		return nullptr;
	}
}
