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

#include "command/browse_command.h"
#include "command/chat_command.h"
#include "command/client_command.h"
#include "command/download_command.h"
#include "command/group_command.h"
#include "command/file_command.h"
#include "command/folder_command.h"
#include "command/handshake_command.h"
#include "command/join_command.h"
#include "command/quit_command.h"
#include "command/search_command.h"
#include "command/socket_command.h"
#include "command/tunnel_command.h"
#include "command/upload_command.h"

#include "component/browse_component.h"
#include "component/chat_component.h"
#include "component/endpoint_component.h"
#include "component/header_component.h"
#include "component/state_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/client/client_network_component.h"
#include "component/client/client_service_component.h"
#include "component/group/group_component.h"
#include "component/group/group_security_component.h"
#include "component/message/message_service_component.h"
#include "component/session/session_component.h"

#include "data/service_data.h"
#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "http/http_request.h"
#include "http/http_response.h"
#include "http/http_socket.h"

#include "message/message_packer.h"
#include "message/message_unpacker.h"

#include "message/request/request_message.h"
#include "message/request/browse_request_message.h"
#include "message/request/chat_request_message.h"
#include "message/request/client_request_message.h"
#include "message/request/download_request_message.h"
#include "message/request/file_request_message.h"
#include "message/request/handshake_request_message.h"
#include "message/request/join_request_message.h"
#include "message/request/quit_request_message.h"
#include "message/request/socket_request_message.h"
#include "message/request/tunnel_request_message.h"
#include "message/request/upload_request_message.h"

#include "message/response/response_message.h"
#include "message/response/browse_response_message.h"
#include "message/response/chat_response_message.h"
#include "message/response/client_response_message.h"
#include "message/response/download_response_message.h"
#include "message/response/file_response_message.h"
#include "message/response/handshake_response_message.h"
#include "message/response/join_response_message.h"
#include "message/response/quit_response_message.h"
#include "message/response/socket_response_message.h"
#include "message/response/tunnel_response_message.h"
#include "message/response/upload_response_message.h"

#include "system/type.h"
#include "thread/thread_info.h"
#include "utility/value.h"
#include "utility/io/folder_util.h"
#include "utility/router/router_util.h"

namespace eja
{
	// Constructor
	message_service_component::message_service_component() : m_tunnel_state(tunnel_state::none)
	{
		m_threads = network::num_threads;
	}

	// Operator
	message_service_component& message_service_component::operator=(const message_service_component& comp)
	{
		if (this != &comp)
			service_component::operator=(comp);

		return *this;
	}

	// Interface
	void message_service_component::update()
	{
		service_component::update();

		// Thread-safe
		m_group_list = copy_if_enabled<group_security_list_component>();
	}

	void message_service_component::clear()
	{
		service_component::clear();

		// Group
		const auto owner = get_entity();
		const auto group = owner->get<group_component>();
		group->clear();

		// Client
		const auto client_list = owner->get<client_list_component>();
		client_list->clear();

		// Browse
		const auto browse_list = owner->get<browse_list_component>();
		browse_list->clear();

		// Session
		const auto session = owner->get<session_component>();
		session->clear();

		// Thread-safe
		m_group_list.reset();
	}

	// Utility
	void message_service_component::start()
	{
		//assert(thread_info::main());

		if (valid())
			return;

		const auto owner = get_entity();
		const auto state = owner->get<state_component>();

		if (state->set(eja::state::idle, eja::state::none))
		{
			set_status(owner, status::info);

			service_component::start();

			// NOTE: Not thread safe
			//
			//if (thread_info::main())
				update();

			// Group
			async_group();

			state->set(eja::state::running);
		}
	}

	void message_service_component::async_restart()
	{
		//assert(thread_info::main());

		if (invalid())
			return;

		const auto owner = get_entity();
		const auto state = owner->get<state_component>();

		if (state->has(eja::state::idle))
		{			
			set_status(owner, status::pending);

			service_component::stop();

			// Start
			set_status(owner, status::info);

			service_component::start();

			if (thread_info::main())
				update();

			// Group
			async_group();

			state->set(eja::state::running);
		}		
	}

	void message_service_component::restart()
	{
		//assert(thread_info::main());

		if (invalid())
			return;

		const auto owner = get_entity();
		const auto state = owner->get<state_component>();

		if (state->set(eja::state::idle, eja::state::running))
		{
			set_status(owner, status::pending);

			service_component::stop();

			// Start
			set_status(owner, status::info);

			service_component::start();

			if (thread_info::main())
				update();

			// Group
			async_group();

			state->set(eja::state::running);
		}
	}

	void message_service_component::stop()
	{
		if (invalid())
			return;
		
		// Stopping
		const auto owner = get_entity();		
		const auto state = owner->get<state_component>();

		if (state->set(eja::state::idle, eja::state::running))
		{		
			service_component::stop();

			// Status
			set_status(owner, status::none);

			state->set(eja::state::none);
		}
	}

	void message_service_component::close()
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

		// Tunnels
		m_tunnel_state = tunnel_state::none;

		const auto tunnel_list = owner->get<tunnel_list_component>();
		{
			thread_lock(tunnel_list);
			for (auto it = tunnel_list->begin(); it != tunnel_list->end(); )
			{
				const auto socket = *it;
				tunnel_list->erase(it++);
				socket->close();
			}

			tunnel_list->clear();
		}
	}

	// Browse
	void message_service_component::async_browse(const entity::ptr data)
	{
		assert(thread_info::main());

		const auto self = shared_from_this();
		post([self, data]() { self->browse(data); });
	}

	void message_service_component::browse(const entity::ptr data)
	{
		const auto owner = get_entity();

		try
		{
			// Entity			
			const auto parent = owner->get_parent();

			// Command
			client_browse_command cmd(parent);
			const auto request = cmd.execute(owner, data);

			const auto state = service_data::create(service_type::client, service_action::browse, data);
			write_request(request, state);
		}
		catch (const std::exception& ex)
		{
			owner->log(ex);
		}
	}

	// Chat
	void message_service_component::async_chat(const entity::ptr data, const entity::ptr client /*= nullptr*/)
	{
		assert(thread_info::main());

		const auto self = shared_from_this();
		post([self, data, client]() { self->chat(client, data); });
	}

	void message_service_component::chat(const entity::ptr data, const entity::ptr client)
	{
		const auto owner = get_entity();

		try
		{
			// Entity
			const auto parent = owner->get_parent();

			// Command
			client_chat_command cmd(parent);
			const auto request = cmd.execute(owner, client, data);

			const auto state = service_data::create(service_type::client, service_action::chat);
			write_request(request, state);
		}
		catch (const std::exception& ex)
		{
			owner->log(ex);
		}
	}

	// Client
	void message_service_component::async_client()
	{
		assert(thread_info::main());

		const auto self = shared_from_this();
		post([self]() { self->client(); });
	}

	void message_service_component::client()
	{
		const auto owner = get_entity();

		try
		{
			// Entity
			const auto parent = owner->get_parent();			

			// Command
			client_client_command cmd(parent);
			const auto request = cmd.execute(owner);

			const auto state = service_data::create(service_type::client, service_action::client);
			write_request(request, state);
		}
		catch (const std::exception& ex)
		{
			owner->log(ex);
		}
	}

	// File
	void message_service_component::async_file(const entity::ptr data)
	{
		const auto self = shared_from_this();
		post([self, data]() { self->file(data); });
	}

	void message_service_component::file(const entity::ptr data)
	{
		const auto owner = get_entity();

		try
		{
			// Entity
			const auto parent = owner->get_parent();

			// Command
			client_file_command cmd(parent);
			const auto request = cmd.execute(owner, data);

			if (request)
			{
				const auto state = service_data::create(service_type::client, service_action::file, data);
				write_request(request, state);
			}
			else
			{
				// Log
				owner->log("No transfer routers are available");
			}
		}
		catch (const std::exception& ex)
		{
			owner->log(ex);
		}
	}

	// Folder
	void message_service_component::async_folder(const entity::ptr data, const std::string& root)
	{
		//assert(thread_info::main());

		const auto self = shared_from_this();
		post([self, data, root]() { self->folder(data, root); });
	}

	void message_service_component::async_folder_client(const entity::ptr data, const std::string& root)
	{
		//assert(thread_info::main());

		const auto self = shared_from_this();
		post([self, data, root]() { self->folder_client(data, root); });
	}

	void message_service_component::folder(const entity::ptr data, const std::string& root)
	{
		const auto owner = get_entity();

		try
		{
			// Path
			boost::filesystem::path path(root);
			const auto folder = data->get<folder_component>();
			path /= folder->get_name();

			// Folder
			folder_util::create(path);

			const auto parent = owner->get_parent();
			const auto browse = data->get<browse_component>();

			if (browse->empty())
			{
				browse->expand();

				// Command
				client_folder_command cmd(parent);
				const auto request = cmd.execute(owner, data, path.string());

				const auto state = service_data::create(service_type::client, service_action::browse, data);
				write_request(request, state);
			}
			else
			{
				const auto service = parent->get<client_service_component>();
				const auto browse_list = data->get<browse_list_component>();

				for (const auto& e : *browse_list)
				{
					if (e->has<file_component>())
					{
						if (service->valid())
							service->async_file(e, path);
					}
					else if (e->has<folder_component>())
					{
						async_folder(e, path.string());
					}
				}
			}
		}
		catch (const std::exception& ex)
		{
			owner->log(ex);
		}
	}

	void message_service_component::folder_client(const entity::ptr data, const std::string& root)
	{
		const auto owner = get_entity();

		try
		{
			// Path
			boost::filesystem::path path(root);
			const auto folder = data->get<folder_component>();
			path /= folder->get_name();

			// Folder
			folder_util::create(path);

			const auto parent = owner->get_parent();
			const auto browse = data->get<browse_component>();

			if (browse->empty())
			{
				browse->expand();

				// Command
				client_folder_command cmd(parent);
				const auto request = cmd.execute(owner, data, path.string());

				const auto state = service_data::create(service_type::client, service_action::browse, data);
				write_request(request, state);
			}
			else
			{
				const auto service = parent->get<client_service_component>();
				const auto browse_list = data->get<browse_list_component>();

				for (const auto& e : *browse_list)
				{
					if (e->has<file_component>())
					{
						if (service->valid())
						{
							const auto endpoint = data->find<endpoint_component>();
							const auto client = endpoint->get_entity();
							service->async_file(e, path, client);
						}
					}
					else if (e->has<folder_component>())
					{
						async_folder_client(e, path.string());
					}
				}
			}
		}
		catch (const std::exception& ex)
		{
			owner->log(ex);
		}
	}

	// Group
	void message_service_component::async_group()
	{
		// Async
		const auto self = shared_from_this();
		post([self]() { self->group(); });
	}

	void message_service_component::group()
	{
		const auto owner = get_entity();

		try
		{			
			// Command
			client_group_command cmd(owner);
			const auto group_list = get_group_security_list();
			cmd.execute(group_list);

			// Callback			
			owner->call(callback_type::group, callback_action::update);

			// Message
			async_handshake();
		}
		catch (const std::exception& ex)
		{
			set_status(owner, status::error);

			owner->log(ex);
		}
	}

	// Handshake
	void message_service_component::async_handshake()
	{
		const auto self = shared_from_this();
		post([self]() { self->handshake(); });
	}

	void message_service_component::handshake()
	{
		const auto owner = get_entity();

		try
		{
			const auto parent = owner->get_parent();

			// Command
			client_handshake_command cmd(parent);
			const auto request = cmd.execute(owner);

			const auto state = service_data::create(service_type::message, service_action::handshake);
			write_request(request, state);
		}
		catch (const std::exception& ex)
		{
			set_status(owner, status::error);

			owner->log(ex);
		}
	}

	// Join
	void message_service_component::async_join()
	{
		const auto self = shared_from_this();
		post([self]() { self->join(); });
	}

	void message_service_component::join()
	{
		const auto owner = get_entity();

		try
		{
			// Entity
			const auto parent = owner->get_parent();

			// Command
			client_join_command cmd(parent);
			const auto request = cmd.execute(owner);

			const auto state = service_data::create(service_type::message, service_action::join);
			write_request(request, state);
		}
		catch (const std::exception& ex)
		{
			set_status(owner, status::error);

			owner->log(ex);
		}
	}

	// Quit
	void message_service_component::async_quit()
	{
		//assert(thread_info::main());

		if (invalid())
			return;

		// Status
		const auto owner = get_entity();
		set_status(owner, status::pending);

		// State
		const auto state = owner->get<state_component>();

		if (state->set(eja::state::quitting, eja::state::running))
		{			
			// Command	
			const auto self = shared_from_this();
			dispatch([self]() { self->quit(); });

			// Callback
			std::thread thread([self, owner, state]()
			{				
				std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds::quit));

				self->service_component::stop();

				// Status
				self->set_status(owner, status::none);

				state->set(eja::state::none);
			});

			thread.detach();
		}
		else
		{
			service_component::stop();

			// Status
			set_status(owner, status::none);

			state->set(eja::state::none);
		}
	}

	void message_service_component::quit()
	{
		const auto owner = get_entity();

		try
		{
			// Entity
			const auto parent = owner->get_parent();

			// Command
			client_quit_command cmd(parent);
			const auto request = cmd.execute(owner);

			const auto state = service_data::create(service_type::client, service_action::quit, 0);
			write_request(request, state);
		}
		catch (const std::exception& ex)
		{
			owner->log(ex);
		}
	}

	// Search
	void message_service_component::async_search(const entity::ptr data)
	{
		const auto self = shared_from_this();
		post([self, data]() { self->search(data); });
	}

	void message_service_component::search(const entity::ptr data)
	{
		const auto owner = get_entity();

		try
		{
			// Entity
			const auto parent = owner->get_parent();

			// Command
			client_search_command cmd(parent);
			const auto request = cmd.execute(owner, data);

			// Data
			const auto state = service_data::create(service_type::client, service_action::search, data);
			write_request(request, state);
		}
		catch (const std::exception& ex)
		{
			owner->log(ex);
		}
	}

	// Socket
	void message_service_component::async_socket()
	{
		const auto self = shared_from_this();
		post([self]() { self->socket(); });
	}

	void message_service_component::socket()
	{
		const auto owner = get_entity();

		try
		{
			// Entity
			const auto parent = owner->get_parent();

			// Command
			client_socket_command cmd(parent);
			const auto request = cmd.execute(owner);

			// Data
			const auto state = service_data::create(service_type::client, service_action::socket);
			write_request(request, state);
		}
		catch (const std::exception& ex)
		{
			owner->log(ex);
		}
	}

	// Tunnel
	void message_service_component::async_tunnel()
	{
		// State
		tunnel_state value = tunnel_state::none;
		if (!m_tunnel_state.compare_exchange_strong(value, tunnel_state::idle))
			return;

		// Close
		const auto owner = get_entity();
		const auto tunnel_list = owner->get<tunnel_list_component>();
		{
			thread_lock(tunnel_list);
			for (auto it = tunnel_list->begin(); it != tunnel_list->end(); )
			{
				const auto socket = *it;
				tunnel_list->erase(it++);
				socket->close();
			}

			tunnel_list->clear();
		}

		const auto self = shared_from_this();
		post([self]() { self->tunnel(); });

		m_tunnel_state = tunnel_state::running;
	}

	void message_service_component::tunnel()
	{
		const auto owner = get_entity();

		try
		{
			// Entity			
			const auto parent = owner->get_parent();

			// Command
			client_tunnel_command cmd(parent);
			const auto request = cmd.execute(owner);

			const auto state = service_data::create(service_type::transfer, service_action::tunnel);
			write_request(request, state);
		}
		catch (const std::exception& ex)
		{
			set_status(owner, status::error);

			owner->log(ex);
		}
	}

	// Request
	void message_service_component::read_request(const http_socket::ptr socket)
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
					// Restart
					if (self->valid())
					{
						// Socket
						self->remove_tunnel(owner, socket);

						// Tunnel
						tunnel_state value = tunnel_state::running;
						if (self->m_tunnel_state.compare_exchange_strong(value, tunnel_state::none))
						{
							// Status
							self->set_status(owner, status::info);

							// Retry
							self->wait(rand() % milliseconds::retry, [self]() { self->async_tunnel(); });
						}
					}
				}
			}
			catch (const std::exception& ex)
			{
				// Restart
				if (self->valid())
				{
					// Socket
					self->remove_tunnel(owner, socket);

					// Tunnel
					tunnel_state value = tunnel_state::running;
					if (self->m_tunnel_state.compare_exchange_strong(value, tunnel_state::none))
					{
						// Status
						self->set_status(owner, status::info);

						// Retry
						self->wait(rand() % milliseconds::retry, [self]() { self->async_tunnel(); });
					}

					owner->log(ex);
				}
			}
		});
	}	

	void message_service_component::read_request(const http_socket::ptr socket, const http_request::ptr request)
	{
		const auto owner = get_entity();

		try
		{			
			// Request			
			auto request_body = request->get_body();
			if (request_body.empty())
				return write_not_found(socket);

			// Header
			const auto parent = owner->get_parent();
			const auto header_map = parent->get<header_map_component>();

			// Response	
			const auto response = http_response::create(http_code::ok);
			response->set_headers(*header_map);

			// Group			
			const auto group = owner->get<group_component>();
			if (group->valid())
				request_body = group->decrypt(request_body);

			// Session			
			const auto session = owner->get<session_component>();
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
						if (router_util::unmuted(parent, data_request->get_client()))
						{
							client_browse_command cmd(parent);
							const auto request = std::static_pointer_cast<browse_request_message>(message);
							const auto response = cmd.execute(owner, request);
							if (response)
								packer.push_back(response);
						}

						break;
					}
					case message_type::browse_response:
					{
						client_browse_command cmd(parent);
						const auto response = std::static_pointer_cast<browse_response_message>(message);
						cmd.execute(owner, response);

						break;
					}
					case message_type::chat_request:
					{
						if (router_util::unmuted(parent, data_request->get_client()))
						{
							client_chat_command cmd(parent);
							const auto request = std::static_pointer_cast<chat_request_message>(message);
							const auto response = cmd.execute(owner, request);
							if (response)
								packer.push_back(response);
						}

						break;
					}
					case message_type::file_request:
					{
						if (router_util::unmuted(parent, data_request->get_client()))
						{
							client_file_command cmd(parent);
							const auto request = std::static_pointer_cast<file_request_message>(message);
							const auto response = cmd.execute(owner, request);
							if (response)
								packer.push_back(response);
						}

						break;
					}
					case message_type::file_response:
					{
						client_file_command cmd(parent);
						const auto response = std::static_pointer_cast<file_response_message>(message);
						cmd.execute(owner, response);

						break;
					}
					case message_type::search_request:
					{
						if (router_util::unmuted(parent, data_request->get_client()))
						{
							client_search_command cmd(parent);
							const auto request = std::static_pointer_cast<search_request_message>(message);
							const auto response = cmd.execute(owner, request);
							if (response)
								packer.push_back(response);
						}

						break;
					}
					case message_type::search_response:
					{
						client_search_command cmd(parent);
						const auto response = std::static_pointer_cast<search_response_message>(message);
						cmd.execute(owner, response);

						break;
					}
					default:
					{
						assert(false);
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

	void message_service_component::write_request(const message_packer::ptr packer, const service_data::ptr data /*= nullptr*/)
	{
		// Callback
		const auto owner = get_entity();		
		const auto parent = owner->get_parent();
		const auto e = entity_factory::create_statusbar(statusbar::message);
		parent->call(callback_type::status, callback_action::update, e);

		// Pool
		auto socket = get_socket(owner);
		if (socket->valid())
			return write_request(socket, packer, data);

		// Async		
		const auto endpoint = owner->get<endpoint_component>();
		const auto self = shared_from_this();

		socket->open(endpoint->get_address(), endpoint->get_port(), [self, owner, data, socket, packer](const boost::system::error_code& error)
		{
			try
			{				
				if (!error)
				{
					//socket->set_keep_alive(true);

					self->write_request(socket, packer, data);
				}
				else
				{
					if (self->valid())
					{
						// Status
						self->set_status(owner, status::error);

						// Retry
						self->wait(milliseconds::open, [self, packer, data]() { self->write_request(packer, data); });

						owner->log(error);
					}					
				}
			}
			catch (const std::exception& ex)
			{
				if (self->valid())
				{
					// Status
					self->set_status(owner, status::error);

					// Retry
					self->wait(milliseconds::open, [self, packer, data]() { self->write_request(packer, data); });

					owner->log(ex);
				}				
			}
		});
	}
	
	void message_service_component::write_request(const http_socket::ptr socket, const message_packer::ptr packer, const service_data::ptr data /*= nullptr*/)
	{
		const auto owner = get_entity();
		const auto self = shared_from_this();

		try
		{			
			// Request
			/*const*/ auto request_body = packer->pack();

			// Session
			const auto session = owner->get<session_component>();
			request_body = session->encrypt(request_body);

			// Group
			const auto group = owner->get<group_component>();
			if (group->valid())
				request_body = group->encrypt(request_body);

			const auto request = http_request::create(http::post);
			request->set_body(std::move(request_body));

			// Headers
			const auto parent = owner->get_parent();
			const auto header_map = parent->get<header_map_component>();
			request->set_headers(*header_map);

			// Session
			request->set_session(session->get_id());

			const auto endpoint = owner->get<endpoint_component>();
			request->set_host(endpoint->get_address());

			// Write
			socket->write<http_request>(request, [self, owner, data, socket, request, packer](const boost::system::error_code& error, const http_request::ptr request)
			{
				try
				{
					if (!error)
					{
						self->read_response(socket, packer, data);
					}
					else
					{
						if (self->valid())
						{
							// Status
							self->set_status(owner, status::info);

							// Retry
							if (data)
							{
								if (data->has_retry(true))
									self->wait(rand() % milliseconds::retry, [self, packer, data]() { self->write_request(packer, data); });
								else
									owner->log(error);
							}
							else
							{
								self->wait(rand() % milliseconds::retry, [self, packer, data]() { self->write_request(packer, data); });

								owner->log(error);
							}
						}						
					}
				}
				catch (const std::exception& ex)
				{
					if (self->valid())
					{
						// Status
						self->set_status(owner, status::error);

						// Retry
						if (data)
						{
							if (data->has_retry(true))
								self->wait(rand() % milliseconds::retry, [self, packer, data]() { self->write_request(packer, data); });
							else
								owner->log(ex);
						}
						else
						{
							self->wait(rand() % milliseconds::retry, [self, packer, data]() { self->write_request(packer, data); });

							owner->log(ex);
						}
					}					
				}				
			});
		}
		catch (const std::exception& ex)
		{
			if (valid())
			{
				// Status
				set_status(owner, status::error);

				// Retry
				wait(rand() % milliseconds::retry, [self, packer, data]() { self->write_request(packer, data); });

				owner->log(ex);
			}
		}
	}

	// Response
	void message_service_component::read_response(const http_socket::ptr socket, const message_packer::ptr packer, const service_data::ptr data /*= nullptr*/)
	{
		const auto owner = get_entity();
		const auto self = shared_from_this();

		// Read
		socket->read<http_response>([self, owner, data, socket, packer](const boost::system::error_code& error, const http_response::ptr response)
		{
			try
			{
				if (!error)
				{
					const auto& hstatus = response->get_status();

					switch (hstatus.get_code())
					{
						case http_code::ok:
						{
							self->read_response(response, data);

							switch (data->get_action())
							{
								case service_action::handshake:
								{
									self->add_socket(owner, socket);

									self->async_join();

									break;
								}
								case service_action::join:
								{
									self->add_socket(owner, socket);

									self->async_tunnel();

									break;
								}
								case service_action::tunnel:
								{
									// Socket
									self->add_socket(owner, socket);

									// Status
									self->set_status(owner, status::success);

									const auto parent = owner->get_parent();
									if (parent)
									{
										const auto network = parent->get<client_network_component>();
										if (network->get_mode() == network_mode::tunnel)
										{
											const auto delay = 1000 / network->get_sockets();
											for (auto i = 0; i < network->get_sockets(); ++i)
												self->wait(delay * i, [self]() { self->async_socket(); });
										}
									}

									break;
								}
								case service_action::socket:
								{
									self->add_tunnel(owner, socket);

									// Socket
									socket->set_timeout(milliseconds::tunnel);

									// Read
									self->read_request(socket);

									break;
								}
								case service_action::quit:
								{
									// Status
									self->set_status(owner, status::pending);

									break;
								}
								default:
								{
									self->set_status(owner, status::success);

									if (data->is_client_type() || data->is_message_type())
										self->add_socket(owner, socket);

									break;
								}
							}

							break;
						}
						case http_code::unauthorized:
						{
							if (self->valid())
							{
								// Socket
								self->add_socket(owner, socket);

								// NOTE: Ignore transfers
								if (data->is_client_type() || data->is_message_type())
								{
									// Status
									self->set_status(owner, status::error);

									// Log
									owner->log(hstatus);
								}

								// Restart
								const auto parent = owner->get_parent();
								if (parent)
								{
									const auto state = owner->get<state_component>();
									if (state->set(eja::state::idle, eja::state::running))
									{
										self->wait(milliseconds::restart, [self, owner, parent]()
										{
											parent->call(callback_type::window, callback_action::update, owner);
										});
									}
								}
							}
							else
							{
								// Status
								if (owner->disabled())
									self->set_status(owner, status::none);
							}

							break;
						}
						case http_code::not_implemented:
						{							
							if (self->valid())
							{
								// Socket
								self->add_socket(owner, socket);

								// Status
								self->set_status(owner, status::error);

								// Log
								const auto text = "Please upgrade to the newest version of Demonsaw.";
								owner->log(text);

								// Chat Message
								const auto parent = owner->get_parent();
								if (parent)
								{
									const auto e = entity_factory::create_client_chat(parent);
									e->add<endpoint_component>(owner);

									const auto chat = e->get<chat_component>();
									chat->set_text(text);
									chat->set_type(chat_type::system);

									// Callback
									parent->call(callback_type::chat, callback_action::add, e);
								}
							}

							break;
						}
						default:
						{							
							if (self->valid())
							{
								// Socket
								self->add_socket(owner, socket);

								// Status
								self->set_status(owner, status::error);

								// Retry
								self->wait(rand() % milliseconds::retry, [self, packer, data]() { self->write_request(packer, data); });

								// Log
								owner->log(hstatus);
							}

							break;
						}
					}
				}
				else
				{
					if (self->valid())
					{
						// Status
						self->set_status(owner, status::info);

						// Retry
						if (data)
						{
							if (data->has_retry(true))
								self->wait(rand() % milliseconds::retry, [self, packer, data]() { self->write_request(packer, data); });
							else
								owner->log(error);
						}
						else
						{
							self->wait(rand() % milliseconds::retry, [self, packer, data]() { self->write_request(packer, data); });

							owner->log(error);
						}
					}
				}
			}
			catch (const std::exception& ex)
			{
				if (self->valid())
				{
					// Status
					self->set_status(owner, status::error);

					// Retry					
					if (data)
					{
						if (data->has_retry(true))
							self->wait(rand() % milliseconds::retry, [self, packer, data]() { self->write_request(packer, data); });
						else
							owner->log(ex);
					}
					else
					{
						self->wait(rand() % milliseconds::retry, [self, packer, data]() { self->write_request(packer, data); });

						owner->log(ex);
					}
				}
			}
		});
	}

	void message_service_component::read_response(const http_response::ptr response, const service_data::ptr data /*= nullptr*/)
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
			const auto session = owner->get<session_component>();
			response_body = session->decrypt(response_body);

			// Unpack
			message_unpacker unpacker;
			unpacker.unpack(response_body.data(), response_body.size());
			if (unpacker.empty())
				return;

			// Message
			const auto parent = owner->get_parent();

			for (const auto& message : unpacker)
			{
				switch (message->get_type())
				{
					case message_type::browse_callback:
					{
						client_browse_command cmd(parent);
						const auto response = std::static_pointer_cast<browse_callback_message>(message);
						const auto& e = data->get_entity();
						cmd.execute(owner, e, response);

						break;
					}
					case message_type::browse_response:
					{
						client_browse_command cmd(parent);
						const auto response = std::static_pointer_cast<browse_response_message>(message);
						cmd.execute(owner, response);

						break;
					}
					case message_type::chat_response:
					{
						client_chat_command cmd(parent);
						const auto response = std::static_pointer_cast<chat_response_message>(message);
						cmd.execute(owner, response);

						break;
					}
					case message_type::client_response:
					{
						client_client_command cmd(parent);
						const auto response = std::static_pointer_cast<client_response_message>(message);
						cmd.execute(owner, response);

						break;
					}
					case message_type::download_response:
					{
						client_download_command cmd(parent);
						const auto response = std::static_pointer_cast<download_response_message>(message);
						cmd.execute(owner, response);

						break;
					}
					case message_type::file_callback:
					{
						client_file_command cmd(parent);
						const auto response = std::static_pointer_cast<file_callback_message>(message);
						const auto& e = data->get_entity();
						cmd.execute(owner, e, response);

						break;
					}
					case message_type::file_response:
					{
						client_file_command cmd(parent);
						const auto response = std::static_pointer_cast<file_response_message>(message);
						cmd.execute(owner, response);

						break;
					}
					case message_type::handshake_response:
					{
						client_handshake_command cmd(parent);
						const auto response = std::static_pointer_cast<handshake_response_message>(message);
						cmd.execute(owner, response);

						break;
					}
					case message_type::join_response:
					{
						client_join_command cmd(parent);
						const auto response = std::static_pointer_cast<join_response_message>(message);
						cmd.execute(owner, response);

						break;
					}
					case message_type::quit_response:
					{
						client_quit_command cmd(parent);
						const auto response = std::static_pointer_cast<quit_response_message>(message);
						cmd.execute(owner, response);

						break;
					}
					case message_type::search_callback:
					{
						client_search_command cmd(parent);
						const auto response = std::static_pointer_cast<search_callback_message>(message);
						const auto& e = data->get_entity();
						cmd.execute(owner, e, response);

						break;
					}
					case message_type::search_response:
					{
						client_search_command cmd(parent);
						const auto response = std::static_pointer_cast<search_response_message>(message);
						cmd.execute(owner, response);

						break;
					}
					case message_type::socket_response:
					{
						client_socket_command cmd(parent);
						const auto response = std::static_pointer_cast<socket_response_message>(message);
						cmd.execute(owner, response);

						break;
					}
					case message_type::tunnel_response:
					{
						client_tunnel_command cmd(parent);
						const auto response = std::static_pointer_cast<tunnel_response_message>(message);
						cmd.execute(owner, response);

						break;
					}
					case message_type::upload_response:
					{
						client_upload_command cmd(parent);
						const auto response = std::static_pointer_cast<upload_response_message>(message);
						cmd.execute(owner, response);

						break;
					}
					default:
					{
						assert(false);
						return;
					}
				}
			}
		}
		catch (const std::exception& ex)
		{
			if (valid())
				owner->log(ex);
		}		
	}

	void message_service_component::write_response(const http_socket::ptr socket)
	{		
		// Response
		const auto response = http_response::create();

		// Headers
		const auto owner = get_entity();
		const auto parent = owner->get_parent();
		const auto header_map = parent->get<header_map_component>();
		response->set_headers(*header_map);

		// Redirect		
		const auto network = parent->get<client_network_component>();
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

	void message_service_component::write_response(const http_socket::ptr socket, const http_code code, const async_state state)
	{		
		// Response
		const auto response = http_response::create(code);

		// Headers
		const auto owner = get_entity();
		const auto parent = owner->get_parent();
		const auto header_map = parent->get<header_map_component>();
		response->set_headers(*header_map);

		// Write
		write_response(socket, response, state);
	}

	void message_service_component::write_response(const http_socket::ptr socket, const http_response::ptr response, const async_state state)
	{
		const auto owner = get_entity();
		const auto self = shared_from_this();

		// Write
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
						default:
						{
							break;
						}
					}
				}
			}
			catch (const std::exception& ex)
			{
				if (self->valid())
					owner->log(ex);
			}
		});
	}

	// Set
	void message_service_component::set_status()
	{
		if (invalid())
			return;

		const auto owner = get_entity();
		const auto parent = owner->get_parent();
		const auto service = parent->get<client_service_component>();
		service->set_status();
	}
}
