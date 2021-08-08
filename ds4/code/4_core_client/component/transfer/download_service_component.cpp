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
#include <random>
#include <boost/bind.hpp>

#include "command/download_command.h"
#include "command/handshake_command.h"
#include "command/quit_command.h"
#include "command/router_command.h"

#include "component/io/file_component.h"
#include "component/router/router_component.h"
#include "component/service/socket_component.h"
#include "component/service/strand_component.h"
#include "component/session/session_component.h"
#include "component/time/timeout_component.h"
#include "component/transfer/chunk_component.h"
#include "component/transfer/transfer_component.h"
#include "component/transfer/download_service_component.h"

#include "entity/entity.h"
#include "http/http_request.h"
#include "http/http_response.h"
#include "http/http_throttle.h"
#include "message/message_packer.h"
#include "message/message_unpacker.h"
#include "message/request/router_request_message.h"

#include "system/type.h"
#include "thread/thread_info.h"
#include "utility/value.h"

namespace eja
{
	// Main - State Machine
	void download_service_component::async_main(const entity::ptr& entity)
	{
		const auto self = shared_from_this();
		const auto strand = entity->get<strand_component>();
		strand->post([self, entity]() { self->main(entity); });
	}

	void download_service_component::main(const entity::ptr& entity)
	{
		try
		{
			const auto router_entity = get_entity(entity);
			if (router_entity)
			{
				// Init
				if (!router_entity->has<socket_component>())
				{
					const auto service = get_service();
					router_entity->add<socket_component>(service);
				}

				// Command
				const auto session = router_entity->get<session_component>();
				if (session->valid())
				{
					// Throttle
					const auto throttle = http_download_throttle::get();
					if (throttle->enabled() && throttle->invalid())
					{
						// Wait
						std::random_device rnd;
						std::uniform_int_distribution<u32> dist(transfer::wait, transfer::queue);
						async_main(entity, dist(rnd));

						return;
					}

					download(entity, router_entity);
				}
				else
				{
					handshake(entity, router_entity);
				}
			}
			else
			{
				// Wait
				async_main(entity, milliseconds::error);
			}

		}
		catch (const std::exception& ex)
		{
			set_status(entity, status::error);
			log(ex);

			const auto transfer = entity->get<transfer_component>();
			transfer->error();
		}
	}

	// Start
	void download_service_component::async_start(const entity::ptr& entity)
	{
		assert(thread_info::main());

		try
		{
			// Init
			if (!entity->has<strand_component>())
			{
				const auto service = get_service();
				entity->add<strand_component>(service);
			}

			// Main
			async_main(entity);
		}
		catch (const std::exception& ex)
		{
			set_status(entity, status::error);
			log(ex);

			const auto transfer = entity->get<transfer_component>();
			transfer->error();
		}
	}

	// Stop
	void download_service_component::async_stop(const entity::ptr& entity)
	{
		assert(thread_info::main());

		if (entity->has<strand_component>())
		{
			try
			{
				// Loop
				const auto self = shared_from_this();
				const auto transfer_list = entity->get<transfer_list_component>();
				{
					thread_lock(transfer_list);
					for (const auto& e : *transfer_list)
					{
						const auto strand = entity->get<strand_component>();
						strand->post([self, entity, e]() { self->quit(entity, e); });
					}
				}
			}
			catch (const std::exception& ex)
			{
				//set_status(entity, status::error);
				log(ex);
			}
		}
	}

	// Download
	void download_service_component::download(const entity::ptr& entity, const entity::ptr& router_entity)
	{
		try
		{
			// Command
			download_command cmd(entity);
			const auto request_message = cmd.execute(router_entity);

			// Write
			if (request_message)
				write(entity, router_entity, request_message, download_service_state::download);
			else
				async_main(entity);
		}
		catch (const std::exception& ex)
		{
			set_status(entity, status::error);
			log(ex);

			const auto transfer = entity->get<transfer_component>();
			transfer->error();
		}
	}

	// Handshake
	void download_service_component::handshake(const entity::ptr& entity, const entity::ptr& router_entity)
	{
		try
		{
			set_status(entity, status::info);

			// Command
			handshake_command cmd(router_entity);
			const auto request_message = cmd.execute();

			// Write
			write(entity, router_entity, request_message, download_service_state::handshake);
		}
		catch (const std::exception& ex)
		{
			set_status(entity, status::error);
			log(ex);

			const auto transfer = entity->get<transfer_component>();
			transfer->error();
		}
	}

	// Quit
	void download_service_component::quit(const entity::ptr& entity, const entity::ptr& router_entity)
	{
		try
		{
			const auto session = router_entity->get<session_component>();
			if (session->valid())
			{
				// Command
				download_command cmd(entity);
				const auto request_message = cmd.execute(router_entity, download_request_data::remove);

				// Write
				write(entity, router_entity, request_message, download_service_state::quit);
			}
		}
		catch (const std::exception& ex)
		{
			//set_status(entity, status::error);
			log(ex);
		}
	}

	// Wait
	void download_service_component::async_wait(const entity::ptr& entity, const size_t ms, download_service_func func)
	{
		assert(ms);

		// Timer
		const auto service = get_service();
		const auto timer = std::make_shared<boost::asio::steady_timer>(*service);
		timer->expires_from_now(std::chrono::milliseconds(ms));

		// Strand
		const auto self = shared_from_this();
		const auto strand = entity->get<strand_component>();
		timer->async_wait(strand->get_strand()->wrap(boost::bind(&download_service_component::wait, self, entity, timer, func)));
	}

	void download_service_component::wait(const entity::ptr& entity, const timer_ptr timer, download_service_func func)
	{
		const auto transfer = entity->get<transfer_component>();
		if (transfer->is_running())
			(this->*func)(entity);
	}

	// Write
	void download_service_component::write(const entity::ptr& entity, const entity::ptr& router_entity, const message_packer& packer, const download_service_state state)
	{
		const auto service = get_service();
		const auto socket = router_entity->get<socket_component>();
		const auto transfer = entity->get<transfer_component>();

		try
		{
			// 0) Network Activity
			async_call(callback::statusbar | callback::update);

			// 1) Request
			const auto request = get_request(router_entity, packer);

			// 2) Open
			if (!socket->valid())
			{
				set_status(entity, status::info);

				socket->set_timeout(milliseconds::open);
				const auto router = router_entity->get<router_component>();
				socket->open(router->get_address(), router->get_port());
				//socket->set_keep_alive(true);
			}

			// 3) Write
			socket->set_timeout(milliseconds::socket);
			socket->write(request);

			// 4) Read
			const auto response = socket->read<http_response>();

			// 5) Parse
			parse(entity, router_entity, response);

			// 6) State
			if (response->is_ok())
			{
				switch (state)
				{
					case download_service_state::handshake:
					{
						if (transfer->is_running())
							async_main(entity);

						break;
					}
					case download_service_state::download:
					{
						if (transfer->is_running())
						{
							if (response->has_body())
							{
								async_main(entity);
							}
							else
							{
								transfer->wait();

								// Wait
								async_main(entity, transfer->get_wait());

								const auto timeout = entity->get<timeout_component>();
								if (timeout->expired(download::pending))
									set_status(entity, status::pending);
							}
						}
						else if (transfer->is_done())
						{
							async_call(m_action | callback::update);
						}

						break;
					}
					case download_service_state::quit:
					{
						// Session
						const auto session = router_entity->get<session_component>();
						session->clear();

						// Socket
						socket->close();

						break;
					}
				}
			}
		}
		catch (const std::exception& ex)
		{
			if (!service->stopped() && !transfer->is_done())
			{
				set_status(entity, status::error);
				log(ex);

				// Session
				const auto session = router_entity->get<session_component>();
				session->clear();

				// Socket
				socket->close();

				// Wait
				async_main(entity, milliseconds::error);
			}
		}
	}

	// Parse
	void download_service_component::parse(const entity::ptr& entity, const entity::ptr& router_entity, const http_response::ptr& response)
	{
		const auto& status = response->get_status();

		switch (status.get_code())
		{
			case http_code::ok:
			{
				// Body
				if (response->has_body())
				{
					// Session
					const auto session = router_entity->get<session_component>();
					const auto body = session->decrypt(response->get_body());

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
							case message_type::router_response:
							{
								// Response
								const auto response_message = std::static_pointer_cast<router_response_message>(message);

								// Command
								router_command cmd(entity);
								cmd.execute(router_entity, response_message);

								break;
							}
							default:
							{
								assert(false);
							}
						}
					}
				}

				break;
			}
			case http_code::none:
			case http_code::unauthorized:
			{
				set_status(entity, eja::status::warning);
				log(status);

				// Session
				const auto session = router_entity->get<session_component>();
				session->clear();

				// Command
				async_main(entity, milliseconds::error);

				break;
			}
			case http_code::not_implemented:
			default:
			{
				set_status(entity, eja::status::error);
				log(status);

				// Transfer
				const auto transfer = entity->get<transfer_component>();
				transfer->error();

				// Session
				const auto session = router_entity->get<session_component>();
				session->clear();

				// Socket
				const auto socket = router_entity->get<socket_component>();
				socket->close();

				break;
			}
		}
	}

	// Get
	http_request::ptr download_service_component::get_request(const entity::ptr& entity, const message_packer& packer) const
	{
		// 1) Pack
		/*const*/ auto body = packer.pack();

		// 2) Encrypt
		const auto session = entity->get<session_component>();
		body = session->encrypt(body);

		// 3) Request
		const auto request = http_request::create();
		request->set_body(std::move(body));
		request->set_session(session->get_id());

		const auto router = entity->get<router_component>();
		request->set_host(router->get_address(), router->get_port());

		return request;
	}

	entity::ptr download_service_component::get_entity(const entity::ptr& entity) const
	{
		const auto transfer_list = entity->get<transfer_list_component>();
		{
			thread_lock(transfer_list);
			return transfer_list->get_entity();
		}
	}
}
