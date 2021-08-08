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

#include "command/clear_command.h"
#include "command/entropy_command.h"
#include "command/client_command.h"
#include "command/group_command.h"
#include "command/quit_command.h"
#include "command/router_command.h"
#include "command/troll_command.h"

#include "component/error_component.h"
#include "component/room_component.h"
#include "component/callback/callback.h"
#include "component/callback/callback_service_component.h"
#include "component/client/client_component.h"
#include "component/group/group_option_component.h"
#include "component/router/router_component.h"
#include "component/router/router_acceptor_component.h"
#include "component/service/service_state_component.h"
#include "component/service/socket_component.h"
#include "component/session/session_component.h"
#include "component/status/status_component.h"
#include "component/time/timeout_component.h"
#include "component/transfer/transfer_component.h"

#include "entity/entity_util.h"
#include "exception/not_implemented_exception.h"
#include "exception/version_exception.h"
#include "factory/router_factory.h"
#include "http/http_status.h"
#include "http/http_acceptor.h"
#include "http/http_request.h"
#include "http/http_response.h"
#include "http/http_socket.h"

#include "message/message_packer.h"
#include "message/message_unpacker.h"
#include "message/request/client_request_message.h"
#include "message/request/group_request_message.h"
#include "message/request/router_request_message.h"
#include "message/response/client_response_message.h"
#include "message/response/group_response_message.h"
#include "message/response/router_response_message.h"

#include "security/security.h"
#include "security/cipher/cipher.h"
#include "security/filter/hex.h"
#include "thread/thread_info.h"
#include "utility/value.h"

namespace eja
{
	// Static
	const size_t router_acceptor_component::num_threads = 128;

	// Interface
	void router_acceptor_component::clear()
	{
		assert(thread_info::main() && invalid());

		acceptor_service_component::clear();

		// Client
		const auto owner = get_owner();
		const auto entity_map = owner->get<entity_map_component>();
		{
			thread_lock(entity_map);
			entity_map->clear();
		}

		const auto session_map = owner->get<session_map_component>();
		{
			thread_lock(session_map);
			session_map->clear();
		}

		const auto group_map = owner->get<group_map_component>();
		{
			thread_lock(group_map);
			group_map->clear();
		}

		// Router
		const auto socket_set = owner->get<socket_set_component>();
		{
			thread_lock(socket_set);
			socket_set->clear();
		}
	}

	// Utility
	void router_acceptor_component::start()
	{
		if (invalid() && set_state(state::starting, state::none))
		{
			acceptor_service_component::start();

			// Open
			async_open();

			// Ping
			const auto owner = get_owner();
			const auto router_list = owner->get<router_list_component>();
			for (const auto& e : *router_list)
			{
				if (e->enabled())
				{
					const auto status = e->get<status_component>();
					status->set_pending();
				}
			}

			async_ping();

			// Timeout
			async_timeout();

			set_state(state::running);
		}
	}

	void router_acceptor_component::stop()
	{
		if (valid() && set_state(state::stopping, state::running))
		{
			acceptor_service_component::stop();

			const auto owner = get_owner();
			set_status(owner, status::none);

			set_state(state::none);
		}
	}

	void router_acceptor_component::close()
	{
		acceptor_service_component::close();

		// Clients
		const auto owner = get_owner();
		const auto entity_map = owner->get<entity_map_component>();
		{
			thread_lock(entity_map);

			for (const auto& pair : *entity_map)
			{
				const auto& e = pair.second;

				// Tunnels
				const auto socket_set = e->get<socket_set_component>();
				{
					thread_lock(socket_set);
					assert(socket_set->size() <= 1);

					for (const auto& socket : *socket_set)
						socket->close();

					socket_set->clear();
				}
			}

			// NOTE: Duplicate code in clear() for safety
			entity_map->clear();
		}
	}

	void router_acceptor_component::add_messages(const entity::ptr& entity, const message_list_component::ptr& message_list)
	{
		const auto message_ring = entity->get<message_ring_component>();
		{
			thread_lock(message_ring);

			// Discard the oldest messages that won't fit
			const auto count = std::min(message_ring->capacity() - message_ring->size(), message_list->size());
			std::copy(message_list->rbegin(), message_list->rbegin() + count, std::front_inserter(*message_ring));
		}
	}

	message_list_component::ptr router_acceptor_component::get_messages(const entity::ptr& entity, const size_t overage /*= 0*/)
	{
		message_list_component::ptr message_list;
		const auto message_ring = entity->get<message_ring_component>();
		{
			thread_lock(message_ring);

			if (!message_ring->empty())
			{
				message_list = message_list_component::create();
				std::copy(message_ring->begin(), message_ring->end(), std::back_inserter(*message_list));
				message_ring->clear();
			}
			else if (overage)
			{
				message_list = message_list_component::create();
			}
		}

		return message_list;
	}

	// Open
	void router_acceptor_component::async_open()
	{
		const auto self = shared_from_this();
		post([self]() { self->open(); });
	}

	void router_acceptor_component::open()
	{
		const auto owner = get_owner();
		const auto self = shared_from_this();

		try
		{
			set_status(owner, status::info);

			// Open
			const auto acceptor = get_acceptor();
			const auto router = owner->get<router_component>();
			acceptor->open(router->get_address(), router->get_port());

			// Accept
			acceptor->accept([self, owner, acceptor](const http_socket::ptr socket)
			{
				// Socket
				self->add_socket(owner, socket);
				//socket->set_keep_alive(true);

				// Read
				self->async_read(socket);
			});

			set_status(owner, status::success);
		}
		catch (const std::exception& ex)
		{
			wait(milliseconds::open, [self]() { self->async_open(); });

			set_status(owner, status::error);

			owner->log(ex);
		}
	}

	// Ping
	void router_acceptor_component::async_ping()
	{
		const auto self = shared_from_this();
		post([self]() { self->ping(); });
	}

	void router_acceptor_component::ping()
	{
		const auto owner = get_owner();

		try
		{
			const auto service = get_service();
			const auto self = shared_from_this();
			const auto router_list = owner->get<router_list_component>();

			for (const auto& e : *router_list)
			{
				if (e->enabled())
				{
					// Socket
					const auto socket = http_socket::create(service);
					socket->set_timeout(milliseconds::open);

					// Open
					const auto router = e->get<router_component>();

					socket->open(router->get_address(), router->get_port(), [self, socket, e](const boost::system::error_code& error)
					{
						const auto status = e->get<status_component>();
						status->set_status(!error ? eja::status::success : eja::status::error);
					});
				}
			}
		}
		catch (const std::exception& ex)
		{
			set_status(owner, status::error);

			owner->log(ex);
		}
	}

	// Timeout
	void router_acceptor_component::async_timeout()
	{
		const auto self = shared_from_this();
		wait(milliseconds::monitor, [self]() { self->timeout(); });
	}

	void router_acceptor_component::timeout()
	{
		const auto owner = get_owner();

		try
		{
			// Ping
			async_ping();

			// Group
			const auto group_list = entity_util::transform<group_map_component>(owner);
			for (const auto& group_entity : group_list)
			{
				// Client
				const auto map = group_entity->copy<client_map_component>();

				for (const auto& pair : *map)
				{
					const auto& e = pair.second;
					const auto timeout = e->get<timeout_component>();

					// In case they changed groups
					if (timeout->expired(milliseconds::timeout))
					{
						size_t removed;
						const auto client_map = group_entity->get<client_map_component>();
						{
							thread_lock(client_map);
							removed = client_map->erase(pair.first);
						}

						// Anti-troll
						if (removed)
						{
							troll_command cmd(owner);
							cmd.remove(e);
						}

						// Rooms
						const auto room_map = group_entity->get<room_map_component>();
						for (const auto& pair : *room_map)
						{
							const auto& e = pair.second;
							const auto client_map = e->get<client_map_component>();
							{
								thread_lock(client_map);
								client_map->erase(pair.first);
							}
						}

						clear_command cmd(owner);
						cmd.execute(e);
					}
				}

				// Message
				entity_util::expire<message_map_component>(group_entity);

				// Transfer
				entity_util::expire<transfer_map_component>(group_entity);
			}

			// Entity
			entity_util::expire<entity_map_component>(owner);

			// Sess
			entity_util::expire<session_map_component>(owner);

			// Transfer
			entity_util::expire<transfer_map_component>(owner);

			// Optimization: DO NOT remove empty groups!
			//
			// Cleanup (remove empties)
			//const auto group_map = owner->get<group_map_component>();
			//{
			//	thread_lock(group_map);
			//
			//	for (auto it = group_map->begin(); it != group_map->end(); /*++it*/)
			//	{
			//		const auto& pair = *it;
			//		const auto& e = pair.second;
			//		const auto client_map = e->get<client_map_component>();
			//
			//		thread_lock(client_map);
			//
			//		if (client_map->empty())
			//			group_map->erase(it++);
			//		else
			//			++it;
			//	}
			//}
		}
		catch (const std::exception& ex)
		{
			owner->log(ex);
		}

		// Repeat
		async_timeout();
	}

	// Read
	void router_acceptor_component::async_read(const http_socket::ptr& socket) const
	{
		const auto self = shared_from_this();
		post([self, socket]() { self->read(socket); });
	}

	void router_acceptor_component::read(const http_socket::ptr& socket)
	{
		entity::ptr entity;
		message_list_component::ptr message_list;
		const auto owner = get_owner();

		try
		{
			// 1) Read
			http_request request;

			socket->read(request);
			socket->set_timeout(milliseconds::socket);

			// 2) Validate
			if (request.is_get())
			{
				write(socket, http_code::redirect);

				// Close
				remove_socket(owner, socket);
				socket->close();

				return;
			}
			else if (!request.has_body())
			{
				// Close
				remove_socket(owner, socket);
				socket->close();

				return;
			}

			// 3) Get the entity
			if (request.has_session())
			{
				entity = get_entity(request);
				if (!entity)
				{
					write(socket, http_code::unauthorized);

					// Close
					remove_socket(owner, socket);
					socket->close();

					return;
				}

				// Timeout
				const auto timeout = entity->get<timeout_component>();
				timeout->update();
			}
			else
			{
				entity = router_factory::create_entity(owner);
			}

			// 4) Decrypt
			/*const*/ auto body = request.get_body();
			auto session = entity->get<session_component>();
			body = session->decrypt(body);

			// 5) Unpack
			message_unpacker unpacker;
			unpacker.unpack(body.data(), body.size());
			if (unpacker.empty())
			{
				// Close
				remove_socket(owner, socket);
				socket->close();

				return;
			}

			// 6) Pack
			message_packer packer;
			auto state = router_acceptor_state::read;

			for (const auto& message : unpacker)
			{
				switch (message->get_type())
				{
					case message_type::group_request:
					{
						// Message
						const auto request_message = std::static_pointer_cast<group_request_message>(message);

						// Command
						group_command cmd(owner);
						cmd.execute(entity, request_message);

						break;
					}
					case message_type::router_request:
					{
						// Message
						const auto request_message = std::static_pointer_cast<router_request_message>(message);

						// Command
						router_command cmd(owner);
						const auto response_message = cmd.execute(entity, socket, request_message);
						if (response_message)
							packer.push_back(response_message);

						// Session
						if (cmd.has_session())
							session = cmd.get_session();

						// State
						if (cmd.has_state())
							state = cmd.get_state();

						break;
					}
					case message_type::client_response:
					{
						// Message
						const auto response_message = std::static_pointer_cast<client_response_message>(message);

						// Command
						client_command cmd(owner);
						cmd.execute(entity, response_message);

						break;
					}
					default:
					{
						assert(false);

						break;
					}
				}
			}

			// 7) Message Queue
			message_list = get_messages(entity);
			if (message_list && !message_list->empty())
				packer.insert(packer.begin(), message_list->begin(), message_list->end());

			// 8) Write
			const auto response = get_response(packer, session);
			socket->write(response);

			// 6) State
			switch (state)
			{
				case router_acceptor_state::read:
				{
					socket->set_timeout(milliseconds::tunnel);
					async_read(socket);

					break;
				}
				case router_acceptor_state::none:
				{
					// Close
					remove_socket(owner, socket);
					socket->close();

					break;
				}
			}
		}
		catch (const CryptoPP::Exception& ex)
		{
			try
			{
				// Write
				const auto response = get_response(http_code::unauthorized);
				socket->write(response);
			}
			catch (const std::exception& /*ex*/) {}

			// Close
			remove_socket(owner, socket);
			socket->close();

			if (entity)
			{
				if (message_list)
				{
					// Retry
					add_messages(entity, message_list);
				}
				else
				{
					// Clear
					const auto session = entity->get<session_component>();
					if (session->invalid())
					{
						clear_command cmd(owner);
						cmd.execute(entity);
					}
				}
			}

			//owner->log(ex);
		}
		catch (const version_exception& ex)
		{
			try
			{
				// Write
				const auto response = get_response(http_code::not_implemented);
				socket->write(response);
			}
			catch (const std::exception& /*ex*/) { }

			// Close
			remove_socket(owner, socket);
			socket->close();

			if (entity)
			{
				if (message_list)
				{
					// Retry
					add_messages(entity, message_list);
				}
				else
				{
					// Clear
					const auto session = entity->get<session_component>();
					if (session->invalid())
					{
						clear_command cmd(owner);
						cmd.execute(entity);
					}
				}
			}

			//owner->log(ex);
		}
		catch (const not_implemented_exception& ex)
		{
			try
			{
				// Write
				const auto response = get_response(http_code::not_implemented);
				socket->write(response);
			}
			catch (const std::exception& /*ex*/) { }

			// Close
			remove_socket(owner, socket);
			socket->close();

			if (entity)
			{
				// Clear
				clear_command cmd(owner);
				cmd.execute(entity);
			}

			//owner->log(ex);
		}
		catch (const std::exception& ex)
		{
			// Close
			remove_socket(owner, socket);
			socket->close();

			if (entity)
			{
				if (message_list)
				{
					// Retry
					add_messages(entity, message_list);
				}
				else
				{
					// Clear
					const auto session = entity->get<session_component>();
					if (session->invalid())
					{
						clear_command cmd(owner);
						cmd.execute(entity);
					}
				}
			}

			//owner->log(ex);
		}
	}

	// Write
	void router_acceptor_component::async_write(const entity::ptr& entity) const
	{
		const auto self = shared_from_this();

		post([self, entity]()
		{
			const auto message_list = self->get_messages(entity);
			if (message_list && !message_list->empty())
				self->write(entity, message_list);
		});
	}

	void router_acceptor_component::write(const http_socket::ptr& socket, const http_code code)
	{
		const auto response = get_response(code);
		socket->write(response);
	}

	void router_acceptor_component::write(const entity::ptr& entity, const message_list_component::ptr& message_list)
	{
		http_socket::ptr socket;
		const auto owner = get_owner();

		try
		{
			// 1) Pack
			message_packer packer;
			packer.insert(packer.end(), message_list->begin(), message_list->end());

			// 2) Request
			const auto request = get_request(entity, packer);

			// 3) Socket
			socket = get_socket(entity, false);
			if (socket)
			{
				// 4) Host
				const auto host = socket->get_remote_address();
				const auto port = socket->get_remote_port();
				request->set_host(host, port);

				// 5) Write
				socket->set_timeout(milliseconds::socket);
				socket->write(request);

				// 5) Read
				const auto response = socket->read<http_response>();

				// 6) Socket
				add_socket(entity, socket);

				// 7) Timeout
				const auto timeout = entity->get<timeout_component>();
				timeout->update();

				// 8) Write?
				async_write(entity);
			}
			else
			{
				// Retry
				add_messages(entity, message_list);
			}
		}
		catch (const std::exception& ex)
		{
			//owner->log(ex);

			if (socket)
			{
				remove_socket(entity, socket);
				socket->close();
			}

			// Retry
			add_messages(entity, message_list);
		}
	}

	// Get
	http_request::ptr router_acceptor_component::get_request(const entity::ptr& entity, const message_packer& packer) const
	{

		// 1) Pack
		/*const*/ auto body = packer.pack();

		// 2) Encrypt
		const auto session = entity->get<session_component>();
		body = session->encrypt(body);

		// 3) Request
		const auto request = http_request::create(http::post);
		request->set_body(std::move(body));

		return request;
	}

	http_response::ptr router_acceptor_component::get_response(const message_packer& packer, const session_component::ptr& session) const
	{
		const auto response = get_response(http_code::ok);

		if (!packer.empty())
		{
			// Packer
			/*const*/ auto body = packer.pack();
			body = session->encrypt(body);

			// Body
			response->set_body(std::move(body));
		}

		return response;
	}

	http_response::ptr router_acceptor_component::get_response(const http_code code) const
	{
		// Response
		const auto response = http_response::create(code);

		if (code == http_code::redirect)
		{
			const auto owner = get_owner();
			const auto router = owner->get<router_component>();

			if (router->has_redirect())
			{
				response->set_code(http_code::redirect);
				response->set_location(router->get_redirect());
			}
			else
			{
				response->set_code(http_code::not_found);
			}
		}

		return response;
	}

	// Get
	entity::ptr router_acceptor_component::get_entity(const http_request& request) const
	{
		assert(request.has_session());

		const auto owner = get_owner();
		const auto& session_id = request.get_session();
		const auto session_map = owner->get<session_map_component>();
		{
			thread_lock(session_map);
			const auto it = session_map->find(session_id);
			if (it != session_map->end())
				return it->second;
		}

		return nullptr;
	}
}
