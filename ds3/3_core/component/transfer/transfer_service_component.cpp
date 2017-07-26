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

#include "command/download_command.h"
#include "command/group_command.h"
#include "command/handshake_command.h"
#include "command/quit_command.h"
#include "command/upload_command.h"

#include "component/endpoint_component.h"
#include "component/header_component.h"
#include "component/state_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/client/client_option_component.h"
#include "component/group/group_component.h"
#include "component/group/group_security_component.h"
#include "component/session/session_component.h"
#include "component/transfer/transfer_service_component.h"

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
#include "message/request/download_request_message.h"
#include "message/request/handshake_request_message.h"
#include "message/request/quit_request_message.h"
#include "message/request/upload_request_message.h"

#include "message/response/response_message.h"
#include "message/response/download_response_message.h"
#include "message/response/handshake_response_message.h"
#include "message/response/quit_response_message.h"
#include "message/response/upload_response_message.h"

#include "security/security.h"
#include "system/type.h"
#include "thread/thread_info.h"
#include "utility/value.h"
#include "utility/client/client_util.h"
#include "utility/router/router_util.h"

namespace eja
{
	// Constructor
	transfer_service_component::transfer_service_component()
	{
		m_threads = network::min_threads;
	}

	// Operator
	transfer_service_component& transfer_service_component::operator=(const transfer_service_component& comp)
	{
		if (this != &comp)
			service_component::operator=(comp);

		return *this;
	}

	// Interface
	void transfer_service_component::update()
	{
		assert(thread_info::main());

		service_component::update();

		// Thread-safe
		m_group_list = copy_if_enabled<group_security_list_component>();
	}

	void transfer_service_component::clear()
	{
		//assert(thread_info::main());

		service_component::clear();

		// Group
		const auto owner = get_entity();
		const auto group = owner->get<group_component>();
		group->clear();

		// Thread-safe
		m_group_list.reset();
	}

	// Utility
	void transfer_service_component::start()
	{
		assert(thread_info::main());

		if (unlikely(valid()))
			return;

		const auto owner = get_entity();
		const auto state = owner->get<state_component>();

		if (state->set(eja::state::idle, eja::state::none))
		{
			set_status(owner, status::info);

			service_component::start();

			update();

			// Group
			async_group();

			state->set(eja::state::running);
		}
	}

	void transfer_service_component::restart()
	{
		assert(thread_info::main());

		if (unlikely(invalid()))
			return;

		const auto owner = get_entity();
		const auto state = owner->get<state_component>();

		if (state->set(eja::state::idle, eja::state::running))
		{
			// Stop
			set_status(owner, status::pending);

			service_component::stop();

			// Start
			set_status(owner, status::info);

			service_component::start();

			update();

			// Group
			async_group();

			state->set(eja::state::running);
		}
	}

	void transfer_service_component::stop()
	{
		assert(thread_info::main());

		if (unlikely(invalid()))
			return;

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

	void transfer_service_component::close()
	{
		//assert(thread_info::main());

		// Sockets
		const auto owner = get_entity();
		const auto socket_set = owner->get<socket_set_component>();
		{
			thread_lock(socket_set);
			socket_set->clear();
		}
	}

	void transfer_service_component::pause(const message_packer::ptr packer)
	{
		thread_lock(m_packer_list);
		m_packer_list->push_back(packer);
	}

	void transfer_service_component::resume(const service_data::ptr data /*= nullptr*/)
	{
		thread_lock(m_packer_list);
		
		for (const auto& packer : m_packer_list)
			write_request(packer, data);

		m_packer_list->clear();
	}

	// Download
	void transfer_service_component::async_download(const entity::ptr data)
	{
		const auto self = shared_from_this();
		post([self, data]() { self->download(data); });
	}

	void transfer_service_component::download(const entity::ptr data)
	{
		const auto owner = get_entity();

		try
		{
			// Entity
			const auto parent = owner->get_parent();

			// Command
			client_download_command cmd(parent);
			const auto request = cmd.execute(owner, data);

			if (request)
			{
				const auto data = service_data::create(service_action::download);
				write_request(request, data);
			}
		}
		catch (const std::exception& ex)
		{
			set_status(owner, status::error);

			owner->log(ex);
		}
	}

	// Group
	void transfer_service_component::async_group()
	{
		const auto self = shared_from_this();
		post([self]() { self->group(); });
	}

	void transfer_service_component::group()
	{
		const auto owner = get_entity();

		try
		{
			// Session
			const auto session = owner->get<session_component>();
			session->clear();

			// Command
			client_group_command cmd(owner);
			const auto group_list = get_group_security_list();
			cmd.execute(group_list);

			// Callback
			owner->call(callback_type::group, callback_action::update);

			// Handshake
			async_handshake();
		}
		catch (const std::exception& ex)
		{
			set_status(owner, status::error);

			owner->log(ex);
		}
	}

	// Handshake
	void transfer_service_component::async_handshake()
	{
		const auto self = shared_from_this();
		post([self]() { self->handshake(); });
	}

	void transfer_service_component::handshake()
	{
		const auto owner = get_entity();

		try
		{
			const auto parent = owner->get_parent();

			// Command
			client_handshake_command cmd(parent);
			const auto request = cmd.execute(owner, false);

			const auto data = service_data::create(service_action::handshake);
			write_request(request, data);
		}
		catch (const std::exception& ex)
		{
			set_status(owner, status::error);

			owner->log(ex);
		}
	}

	// Quit
	void transfer_service_component::async_quit()
	{
		if (unlikely(invalid()))
			return;

		// Status
		const auto owner = get_entity();
		set_status(owner, status::pending);

		// Quit
		const auto self = shared_from_this();
		dispatch([self]() { self->quit(); });

		// Stop			
		const auto state = owner->get<state_component>();

		if (state->set(eja::state::idle, eja::state::running))
		{
			std::thread thread([self, owner, state]()
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds::quit));

				// Stop
				self->service_component::stop();

				// Status
				self->set_status(owner, status::none);

				state->set(eja::state::none);
			});

			thread.detach();
		}
	}

	void transfer_service_component::quit()
	{
		const auto owner = get_entity();

		try
		{			
			const auto parent = owner->get_parent();

			// Command
			client_quit_command cmd(parent);
			const auto request = cmd.execute(owner);

			const auto data = service_data::create(service_action::quit, 0);
			write_request(request, data);
		}
		catch (const std::exception& ex)
		{
			owner->log(ex);
		}
	}

	// Upload
	void transfer_service_component::async_upload(const entity::ptr data)
	{
		const auto self = shared_from_this();
		post([self, data]() { self->upload(data); });
	}

	void transfer_service_component::upload(const entity::ptr data)
	{
		const auto owner = get_entity();

		try
		{
			const auto parent = owner->get_parent();

			// Command
			client_upload_command cmd(parent);
			const auto request = cmd.execute(owner, data);

			if (request)
			{
				const auto data = service_data::create(service_action::upload);
				write_request(request, data);
			}
		}
		catch (const std::exception& ex)
		{
			set_status(owner, status::error);

			owner->log(ex);
		}
	}

	// Request
	void transfer_service_component::write_request(const message_packer::ptr packer, const service_data::ptr data /*= nullptr*/)
	{
		// Callback
		const auto owner = get_entity();
		const auto parent = owner->get_parent();
		const auto e = entity_factory::create_statusbar(statusbar::transfer);
		parent->call(callback_type::status, callback_action::update, e);

		// Socket		
		auto socket = get_socket(owner);
		if (socket->valid())
			return write_request(socket, packer, data);

		// Async
		const auto self = shared_from_this();
		const auto endpoint = owner->get<endpoint_component>();

		socket->open(endpoint->get_address(), endpoint->get_port(), [self, owner, socket, packer, data](const boost::system::error_code& error)
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
					}					
				}
			}
			catch (const std::exception& /*ex*/)
			{
				if (self->valid())
				{
					// Status
					self->set_status(owner, status::error);

					// Retry
					self->wait(milliseconds::open, [self, packer, data]() { self->write_request(packer, data); });
				}				
			}		
		});
	}

	void transfer_service_component::write_request(const http_socket::ptr socket, const message_packer::ptr packer, const service_data::ptr data /*= nullptr*/)
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
			socket->write<http_request>(request, [self, owner, socket, request, packer, data](const boost::system::error_code& error, const http_request::ptr request)
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
	void transfer_service_component::read_response(const http_socket::ptr socket, const message_packer::ptr packer, const service_data::ptr data /*= nullptr*/)
	{
		const auto owner = get_entity();
		const auto self = shared_from_this();

		// Read
		socket->read<http_response>([self, owner, socket, packer, data](const boost::system::error_code& error, const http_response::ptr response)
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
							// Socket
							self->add_socket(owner, socket);

							// Response
							self->read_response(response, data);

							// Status
							self->set_status(owner, status::success);

							// Resume
							if (data && data->is_handshake_action())
								self->resume(data);
							
							break;
						}
						case http_code::unauthorized:
						{
							if (self->valid())
							{
								// Socket
								self->add_socket(owner, socket);

								// Pause
								self->pause(packer);

								// Restart
								const auto state = owner->get<state_component>();
								if (state->set(eja::state::idle, eja::state::running))
								{
									// Status
									self->set_status(owner, status::error);

									self->wait(milliseconds::restart, [self, owner, state]()
									{
										// Group
										self->async_group();

										state->set(eja::state::running);
									});
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
							// Socket
							self->add_socket(owner, socket);

							// Status
							self->set_status(owner, status::error);

							owner->log("Please upgrade to the newest version of Demonsaw.");

							break;
						}
						default:
						{							
							// Socket
							self->add_socket(owner, socket);

							// Status
							self->set_status(owner, status::error);

							owner->log(hstatus);

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

	void transfer_service_component::read_response(const http_response::ptr response, const service_data::ptr data /*= nullptr*/)
	{		
		// Response
		auto response_body = response->get_body();
		if (response_body.empty())
			return;

		// Group		
		const auto owner = get_entity();
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
				case message_type::download_response:
				{
					client_download_command cmd(parent);
					const auto response = std::static_pointer_cast<download_response_message>(message);
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
				case message_type::quit_response:
				{
					client_quit_command cmd(parent);
					const auto response = std::static_pointer_cast<quit_response_message>(message);
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
}
