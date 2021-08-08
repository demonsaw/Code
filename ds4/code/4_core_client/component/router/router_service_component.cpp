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

#include "command/client_command.h"
#include "command/entropy_command.h"
#include "command/group_command.h"
#include "command/handshake_command.h"
#include "command/join_command.h"
#include "command/ping_command.h"
#include "command/quit_command.h"
#include "command/room_command.h"
#include "command/router_command.h"
#include "command/info_command.h"
#include "command/tunnel_command.h"

#include "component/browse_component.h"
#include "component/chat_component.h"
#include "component/endpoint_component.h"
#include "component/service/socket_component.h"
#include "component/callback/callback.h"
#include "component/callback/callback_service_component.h"
#include "component/client/client_component.h"
#include "component/group/group_component.h"
#include "component/group/group_option_component.h"
#include "component/message/message_component.h"
#include "component/router/router_component.h"
#include "component/router/router_service_component.h"
#include "component/service/service_state_component.h"
#include "component/session/session_component.h"
#include "component/io/share_component.h"

#include "data/request/handshake_request_data.h"
#include "data/request/join_request_data.h"
#include "data/request/ping_request_data.h"
#include "data/request/tunnel_request_data.h"

#include "data/response/handshake_response_data.h"
#include "data/response/join_response_data.h"

#include "entity/entity.h"
#include "entity/entity_factory.h"
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

#include "thread/thread_info.h"
#include "utility/value.h"
#include "utility/io/folder_util.h"

namespace eja
{
	// Enum
	enum class write_location { none, open, read, write, parse };

	// Static
	const size_t router_service_component::min_threads = 2;
	const size_t router_service_component::max_threads = 4;	// std::max(static_cast<size_t>(4), static_cast<size_t>(std::thread::hardware_concurrency()));
	const size_t router_service_component::num_threads = 4;

	// Interface
	void router_service_component::clear()
	{
		socket_service_component::clear();

		// Tunnel
		m_tunnel.reset();
	}

	// Utility
	void router_service_component::start()
	{
		if (invalid() && set_state(state::starting, state::none))
		{
			socket_service_component::start();

			// Commands
			async_handshake();
			async_ping();

			set_state(state::running);
		}
	}

	void router_service_component::async_restart()
	{
		// NOTE: Service cannot user a timer to wait and then stop itself
		const auto self = shared_from_this();
		std::thread thread([self]() { self->restart(); });
		thread.detach();
	}

	void router_service_component::restart()
	{
		if (valid() && set_state(state::restarting, state::running))
		{
			socket_service_component::restart();

			// Commands
			async_handshake();
			async_ping();

			set_state(state::running);
		}
	}

	void router_service_component::stop()
	{
		if (valid() && set_state(state::stopping, state::running))
		{
			socket_service_component::stop();

			set_state(state::none);
		}
	}

	void router_service_component::close()
	{
		socket_service_component::close();

		// Tunnel
		if (m_tunnel)
			m_tunnel->close();

		// Cache
		{
			thread_lock(m_cache);
			for (const auto& socket : *m_cache)
				socket->close();

			m_cache->clear();
		}
	}

	// Utility
	void router_service_component::add_cache(const http_socket::ptr& socket)
	{
		assert(socket);

		thread_lock(m_cache);
		m_cache->insert(socket);
	}

	void router_service_component::remove_cache(const http_socket::ptr& socket)
	{
		assert(socket);

		thread_lock(m_cache);
		m_cache->erase(socket);
	}

	// Handshake
	void router_service_component::async_handshake()
	{
		const auto self = shared_from_this();
		post_strand([self]() { self->handshake(); });
	}

	void router_service_component::handshake()
	{
		const auto owner = get_owner();

		try
		{
			// Command
			handshake_command cmd(owner);
			const auto request_message = cmd.execute();

			// Write
			write(request_message, router_service_state::handshake);
		}
		catch (const std::exception& ex)
		{
			set_status(owner, status::error);
			owner->log(ex);
		}
	}

	// Join
	void router_service_component::async_join()
	{
		const auto self = shared_from_this();
		post_strand([self]() { self->join(); });
	}

	void router_service_component::join()
	{
		const auto owner = get_owner();

		try
		{
			join_command cmd(owner);
			const auto request_message = cmd.execute();

			// Write
			write(request_message, router_service_state::join);
		}
		catch (const std::exception& ex)
		{
			set_status(owner, status::error);
			owner->log(ex);
		}
	}

	// Ping
	void router_service_component::async_ping()
	{
		const auto self = shared_from_this();
		wait(milliseconds::ping, [self]() { self->ping(); });
	}

	void router_service_component::ping()
	{
		const auto owner = get_owner();

		try
		{
			// NOTE: Wait until we have joined the router
			const auto client = owner->get<client_component>();

			if (client->has_id())
			{
				// Command
				ping_command cmd(owner);
				const auto request_message = cmd.execute();

				// Write
				write(request_message);
			}
		}
		catch (const std::exception& ex)
		{
			//set_status(owner, status::error);
			owner->log(ex);
		}

		// Repeat
		async_ping();
	}

	// Quit
	void router_service_component::async_quit()
	{
		const auto owner = get_owner();
		const auto session = owner->get<session_component>();

		if (session->valid())
		{
			// Command
			quit_command cmd(owner);
			const auto request_message = cmd.execute();

			// Request
			const auto request = get_request(request_message);
			const auto router = owner->get<router_component>();

			const auto callback = owner->get<callback_service_component>();
			callback->async_quit(router->get_address(), router->get_port(), request);
		}
	}

	// Tunnel
	void router_service_component::async_tunnel()
	{
		// TODO: Assert that we have tunneling enabled!
		//
		//

		const auto self = shared_from_this();
		post_strand([self]() { self->tunnel(); });
	}

	void router_service_component::tunnel()
	{
		const auto owner = get_owner();

		try
		{
			// Command
			tunnel_command cmd(owner);
			const auto request_message = cmd.execute();

			// Write
			write(request_message, router_service_state::tunnel);
		}
		catch (const std::exception& ex)
		{
			set_status(owner, status::error);
			owner->log(ex);
		}
	}

	// Read
	void router_service_component::async_read(const http_socket::ptr& socket)
	{
		const auto self = shared_from_this();
		post([self, socket]() { self->read(socket); });
	}

	void router_service_component::read(const http_socket::ptr& socket)
	{
		const auto owner = get_owner();
		const auto service = get_service();

		try
		{
			// 0) Network Activity
			owner->async_call(callback::statusbar | callback::update);

			// 1) Read
			const auto request = socket->read<http_request>();

			// 2) Write
			socket->set_timeout(milliseconds::socket);
			const auto response = get_response(http_code::ok);
			socket->write(response);

			// 3) Async
			socket->set_timeout(milliseconds::tunnel);
			async_read(socket);

			// 4) Execute
			async_parse(request);
		}
		catch (const std::exception& ex)
		{
			if (!service->stopped())
			{
				set_status(owner, status::info);
				//owner->log(ex);

				//const auto str = boost::str(boost::format("Socket Tunnel Error: %s") % ex.what());
				//owner->log(str);

				// Tunnel
				if (m_tunnel)
					m_tunnel->close();

				// Tunnel
				async_tunnel();
			}
		}
	}

	// Write
	void router_service_component::async_write()
	{
		const auto self = shared_from_this();
		post([self]() { self->write(); });
	}

	void router_service_component::write()
	{
		const auto message_list = get_messages();
		if (message_list && !message_list->empty())
		{
			message_packer packer;
			packer.insert(packer.end(), message_list->begin(), message_list->end());

			write(packer, message_list);
		}
	}

	void router_service_component::write(const message_packer& packer, const router_service_state state)
	{
		const auto owner = get_owner();
		const auto service = get_service();

		http_socket::ptr socket;
		write_location location = write_location::none;

		try
		{
			// 0) Network Activity
			owner->async_call(callback::statusbar | callback::update);

			// 1) Request
			const auto request = get_request(packer);

			// 2) Open
			socket = get_socket(owner);
			add_cache(socket);

			if (!socket->valid())
			{
				location = write_location::open;
				const auto router = owner->get<router_component>();

				socket->set_timeout(milliseconds::open);
				socket->open(router->get_address(), router->get_port());
				//socket->set_keep_alive(true);

				set_status(owner, status::info);
			}

			// 3) Write
			location = write_location::write;
			socket->set_timeout(milliseconds::socket);
			socket->write(request);

			// 4) Read
			location = write_location::read;
			const auto response = socket->read<http_response>();

			// 5) Socket
			if (response->is_ok())
			{
				switch (state)
				{
					case router_service_state::join:
					case router_service_state::tunnel:
					{
						m_tunnel = socket;

						break;
					}
					default:
					{
						add_socket(owner, socket);

						break;
					}
				}
			}

			// 6) Parse
			location = write_location::parse;
			parse(response);

			// 7) Command
			if (response->is_ok())
			{
				switch (state)
				{
					case router_service_state::handshake:
					{
						async_join();

						break;
					}
					case router_service_state::join:
					case router_service_state::tunnel:
					{
						set_status(owner, status::success);

						// Read
						socket->set_timeout(milliseconds::tunnel);
						async_read(socket);

						break;
					}
					default:
					{
						assert(false);

						break;
					}
				}
			}

			// 8) Cache
			remove_cache(socket);
		}
		catch (const std::exception& ex)
		{
			// Cache
			if (socket)
				remove_cache(socket);

			if (!service->stopped())
			{
				switch (location)
				{
					case write_location::write:
					case write_location::read:
					{
						if (state == router_service_state::tunnel)
						{
							set_status(owner, status::info);
							owner->log(ex);

							// Tunnel
							async_tunnel();
						}
						else
						{
							set_status(owner, status::error);
							owner->log(ex);

							// Wait
							const auto self = shared_from_this();
							wait(milliseconds::error, [self]() { self->async_restart(); });
						}

						break;
					}
					default:
					{
						set_status(owner, status::error);
						owner->log(ex);

						// Wait
						const auto self = shared_from_this();
						const auto ms = (location == write_location::open) ? milliseconds::open : milliseconds::error;
						wait(ms, [self]() { self->async_restart(); });

						break;
					}
				}
			}
		}
	}

	void router_service_component::write(const message_packer& packer, const message_list_component::ptr& message_list)
	{
		const auto owner = get_owner();
		const auto service = get_service();

		http_socket::ptr socket;
		write_location location = write_location::none;

		try
		{
			// 0) Network Activity
			owner->async_call(callback::statusbar | callback::update);

			// 1) Request
			const auto request = get_request(packer);

			// 2) Open
			socket = get_socket(owner);
			add_cache(socket);

			if (!socket->valid())
			{
				location = write_location::open;
				const auto router = owner->get<router_component>();

				socket->set_timeout(milliseconds::open);
				socket->open(router->get_address(), router->get_port());
				//socket->set_keep_alive(true);
			}

			// 3) Write
			location = write_location::write;
			socket->set_timeout(milliseconds::socket);
			socket->write(request);

			// 4) Read
			location = write_location::read;
			const auto response = socket->read<http_response>();

			// 5) Socket
			if (response->is_ok())
				add_socket(owner, socket);

			// 6) Parse
			location = write_location::parse;
			parse(response);

			// 7) Cache
			remove_cache(socket);
		}
		catch (const std::exception& ex)
		{
			// Cache
			if (socket)
				remove_cache(socket);

			if (!service->stopped())
			{
				// Retry
				switch (location)
				{
					case write_location::write:
					{
						//set_status(owner, status::warning);
						//owner->log(ex);

						const auto str = boost::str(boost::format("Socket Write Error: %s") % ex.what());
						owner->log(str);

						// Queue
						add_messages(message_list);

						async_write();

						break;
					}
					case write_location::read:
					{
						//set_status(owner, status::warning);
						//owner->log(ex);

						const auto str = boost::str(boost::format("Socket Read Error: %s") % ex.what());
						owner->log(str);

						break;
					}
					default:
					{
						//set_status(owner, status::error);
						//owner->log(ex);

						const auto str = boost::str(boost::format("Socket Unknown Error: %s") % ex.what());
						owner->log(str);

						// Wait
						const auto self = shared_from_this();
						const auto ms = (location == write_location::open) ? milliseconds::open : milliseconds::error;
						wait(ms, [self]() { self->write(); });

						break;
					}
				}
			}
		}
	}

	// Parse
	void router_service_component::async_parse(const http_request::ptr& request)
	{
		if (request->has_body())
		{
			const auto self = shared_from_this();
			post([self, request]() { self->parse(request); });
		}
	}

	void router_service_component::parse(const http_request::ptr& request)
	{
		assert(request->has_body());

		try
		{
			const auto& body = request->get_body();
			parse(body);
		}
		catch (const std::exception& ex)
		{
			const auto service = get_service();
			if (!service->stopped())
			{
				const auto owner = get_owner();
				owner->log(ex);
			}
		}
	}

	void router_service_component::parse(const http_response::ptr& response)
	{
		const auto code = response->get_code();

		switch (code)
		{
			case http_code::ok:
			{
				// Body
				if (response->has_body())
				{
					const auto& body = response->get_body();
					parse(body);
				}

				break;
			}
			case http_code::unauthorized:
			{
				const auto owner = get_owner();
				set_status(owner, status::error);
				owner->log("Invalid session key or password.");

				// Clear the Id
				const auto client = owner->get<client_component>();
				client->set_id();

				// NOTE: Session can only be destroyed here or in client_service_component::stop
				const auto session = owner->get<session_component>();
				session->clear();

				// Callback
				owner->call(callback::browse | callback::clear);
				owner->call(callback::client | callback::clear);
				owner->async_call(callback::room | callback::destroy);

				// Wait
				const auto self = shared_from_this();
				wait(milliseconds::error, [self]() { self->async_restart(); });

				break;
			}
			case http_code::not_implemented:
			{
				const auto owner = get_owner();
				set_status(owner, status::cancelled);
				owner->log("Please upgrade to the newest version of the software.");

				break;
			}
			default:
			{
				assert(false);

				const auto owner = get_owner();
				//set_status(owner, status::warning);
				owner->log(response->get_description());

				break;
			}
		}
	}

	void router_service_component::parse(const std::string& str)
	{
		// Session
		const auto owner = get_owner();
		const auto session = owner->get<session_component>();
		auto body = session->decrypt(str);

		// Unpack
		message_unpacker unpacker;
		unpacker.unpack(body.data(), body.size());
		if (unpacker.empty())
			return;

		// Message
		for (const auto& message : unpacker)
		{
			switch (message->get_type())
			{
				case message_type::client_request:
				{
					// Response
					const auto request_message = std::static_pointer_cast<client_request_message>(message);

					// Command
					client_command cmd(owner);
					cmd.set_share_list(m_share_list);
					const auto response = cmd.execute(request_message);
					if (response)
						async_write(response);

					break;
				}
				case message_type::client_response:
				{
					// Response
					const auto response_message = std::static_pointer_cast<client_response_message>(message);

					// Command
					client_command cmd(owner);
					cmd.execute(response_message);

					break;
				}
				case message_type::router_request:
				{
					// Response
					const auto request_message = std::static_pointer_cast<router_request_message>(message);

					// Command
					router_command cmd(owner);
					cmd.execute(request_message);

					break;
				}
				case message_type::router_response:
				{
					// Response
					const auto response_message = std::static_pointer_cast<router_response_message>(message);

					// Command
					router_command cmd(owner);
					cmd.execute(response_message);

					break;
				}
				default:
				{
					assert(false);

					break;
				}
			}
		}
	}

	// Utility
	void router_service_component::add_messages(const message_list_component::ptr& message_list)
	{
		const auto owner = get_owner();
		const auto message_ring = owner->get<message_ring_component>();
		{
			thread_lock(message_ring);

			// Discard the oldest messages that won't fit
			const auto count = std::min(message_ring->capacity() - message_ring->size(), message_list->size());
			std::copy(message_list->rbegin(), message_list->rbegin() + count, std::front_inserter(*message_ring));
		}
	}

	message_list_component::ptr router_service_component::get_messages(const size_t overage /*= 0*/)
	{
		const auto owner = get_owner();
		message_list_component::ptr message_list;
		const auto message_ring = owner->get<message_ring_component>();
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

	// Set
	void router_service_component::set_status(const entity::ptr& entity, const status value) const
	{
		socket_service_component::set_status(entity, value);

		// Callback
		entity->async_call(callback::status | callback::update);
	}

	// Get
	http_request::ptr router_service_component::get_request(const message_packer& packer) const
	{
		// 1) Pack
		/*const*/ auto body = packer.pack();

		// 2) Encrypt
		const auto owner = get_owner();
		const auto session = owner->get<session_component>();
		body = session->encrypt(body);

		// 3) Request
		const auto request = http_request::create();
		request->set_body(std::move(body));

		const auto router = owner->get<router_component>();
		request->set_host(router->get_address(), router->get_port());
		request->set_session(session->get_id());

		return request;
	}

	http_response::ptr router_service_component::get_response(const message_packer& packer) const
	{
		// 1) Pack
		/*const*/ auto body = packer.pack();

		// 2) Encrypt
		const auto owner = get_owner();
		const auto session = owner->get<session_component>();
		body = session->encrypt(body);

		// 3) Response
		const auto response = http_response::create(http_code::ok);
		response->set_body(std::move(body));

		return response;
	}

	http_response::ptr router_service_component::get_response(const http_code code) const
	{
		// Response
		const auto response = http_response::create(code);

		return response;
	}
}
