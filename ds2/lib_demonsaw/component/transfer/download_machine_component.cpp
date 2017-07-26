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

#include "download_machine_component.h"
#include "command/client/request/client_download_request_command.h"
#include "command/client/request/client_handshake_request_command.h"
#include "command/client/request/client_quit_request_command.h"
#include "component/timeout_component.h"
#include "component/timer_component.h"
#include "component/client/client_option_component.h"
#include "component/io/file_component.h"
#include "component/router/router_component.h"
#include "component/transfer/chunk_component.h"
#include "component/transfer/download_component.h"
#include "component/transfer/download_thread_component.h"
#include "component/transfer/transfer_component.h"

namespace eja
{
	// Interface
	void download_machine_component::init()
	{
		machine_component::init();

		// Thread
		const auto owner = get_entity();
		const auto self = shared_from_this();
		std::thread thread([self, owner]() { self->main(); });
		thread.detach();
	}

	void download_machine_component::shutdown()
	{
		machine_component::shutdown();		

		close();

		kill();
	}

	void download_machine_component::stop()
	{
		ignore(true);

		machine_component::stop();
	}

	// Callback
	bool download_machine_component::on_none()
	{
		const auto owner = get_entity();
		const auto transfer = owner->get<transfer_component>();

		if (transfer && transfer->done())
		{
			// Move to finished tab
			const auto parent = owner->get_entity();
			call(function_type::download, function_action::remove, parent);
			call(function_type::finished, function_action::add, parent);
		}

		return machine_component::on_none();
	}

	bool download_machine_component::on_start()
	{
		const auto owner = get_entity();
		const auto transfer = owner->get<transfer_component>();
		if (!transfer)
			return set_error();

		try
		{
			close();

			// Option
			const auto owner = get_entity();
			const auto option = owner->get<client_option_component>();
			if (!option)
				return set_error();

			// Router
			const auto router = owner->get<router_component>();
			if (!router)
				return set_error();

			// Socket
			m_socket->set_timeout(option->get_socket_timeout());
			m_socket->open(router->get_address(), router->get_port());

			owner->add<session_component>();

			set_state(machine_state::handshake);
		}
		catch (std::exception& e)
		{
			if (transfer->done())
			{
				set_status(status::none);
				set_state(machine_state::quit);
			}
			else
			{
				set_status(status::error);
				set_state(machine_state::sleep, default_timeout::error);
				log(e);
			}
		}
		catch (...)
		{
			if (transfer->done())
			{
				set_status(status::none);
				set_state(machine_state::quit);
			}
			else
			{
				set_status(status::error);
				set_state(machine_state::sleep, default_timeout::error);
				log();
			}
		}

		return machine_component::on_start();
	}

	bool download_machine_component::on_restart()
	{
		set_status(status::pending);

		return machine_component::on_restart();
	}
	
	bool download_machine_component::on_stop()
	{
		ignore(false);

		return set_state(machine_state::quit);
	}

	bool download_machine_component::on_handshake()
	{
		const auto owner = get_entity();

		try
		{			
			const auto request_command = client_handshake_request_command<router_type::transfer>::create(owner, m_socket);
			const auto request_status = request_command->execute();

			switch (request_status.get_code())
			{
				case http_code::ok:
				{
					set_status(status::info);
					set_state(machine_state::download);
					
					break;
				}
				case http_code::none:
				case http_code::bad_request:
				{			
					set_status(status::warning);
					set_state(machine_state::sleep, default_timeout::error);
					
					// If the router sends a bad_request this means that it's overloaded, try again
					const auto router = owner->get<router_component>();
					if (router)
					{
						const auto str = boost::str(boost::format("\"%s\" is busy") % router->get_address());
						log(str);
					}

					break;
				}
				default:
				{
					set_status(status::error);
					set_state(machine_state::restart, default_timeout::error);
					log(request_status);

					break;
				}
			}
		}
		catch (std::exception& e)
		{
			const auto transfer = owner->get<transfer_component>();

			if (transfer && transfer->done())
			{
				set_status(status::none);
				set_state(machine_state::quit);
			}
			else
			{
				set_status(status::error);
				set_state(machine_state::restart, default_timeout::error);
				log(e);
			}
		}
		catch (...)
		{
			const auto transfer = owner->get<transfer_component>();

			if (transfer && transfer->done())
			{
				set_status(status::none);
				set_state(machine_state::quit);
			}
			else
			{
				set_status(status::error);
				set_state(machine_state::restart, default_timeout::error);
				log();
			}
		}

		return machine_component::on_handshake();
	}

	bool download_machine_component::on_download()
	{
		const auto owner = get_entity();
		const auto transfer = owner->get<transfer_component>();
		if (!transfer)
			return set_error();

		try
		{
			const auto request_command = client_download_request_command::create(owner, m_socket);
			const auto request_status = request_command->execute();

			switch (request_status.get_code())
			{
				case http_code::ok:
				{
					const auto chunk = owner->get<chunk_component>(false);
					if (!chunk)
						return set_error();

					if (chunk->has_data())
					{						
						bool loop = true;

						while (loop)
						{
							const auto file = owner->get<file_component>();
							if (!file)
								return set_error();

							const auto size = file->write(chunk->get_data(), chunk->get_offset());
							
							// DEBUG
							////////////////////////////////////////////////////////////////////////////////////////								
							//const auto percent = (static_cast<float>(chunk->get_offset() + chunk->get_size()) / static_cast<float>(file->get_size())) * 100.0f;
							//const auto debug = boost::str(boost::format("DOWN [%d]: %s %d - %d %0.2f%% (%d) (success)") % transfer->get_id() % file->get_name() % chunk->get_offset() % (chunk->get_offset() + chunk->get_data().size()) % percent % chunk->get_size());
							//const auto debug = boost::str(boost::format("DOWN [%d] %d / %d") % (chunk->get_offset() / chunk->get_size()) % chunk->get_offset() % chunk->get_size());
							//log(debug);
							////////////////////////////////////////////////////////////////////////////////////////

							if (size == chunk->get_size())
							{
								set_status(status::success);
								
								// Transfer
								const auto thread = owner->get<download_thread_component>();
								if (!thread || thread->add_chunk(chunk->get_offset()))
									transfer->update(chunk->get_size());

								// Chunk
								chunk->set_ack(true);
								chunk->set_size(0);
								chunk->set_data();

								// Timeout
								const auto timeout = owner->get<timeout_component>();
								if (timeout)
									timeout->update();

								loop = false;

							}
							else
							{
								set_status(status::warning);

								const auto file = owner->get<file_component>();
								if (file)
								{
									const auto str = boost::str(boost::format("Unable to open \"%s\" for writing.") % file->get_name());
									log(str);
								}								

								std::this_thread::sleep_for(std::chrono::milliseconds(default_timeout::client::file));
								loop = !timeout();
							}
						}
					}
					else
					{
						// TODO: Allow timeout of gray, but re-queue all timeouts!
						//
						// Transfer
						if (!transfer->updated() && !transfer->get_retries())
						{
							// Timeout
							const auto timeout = owner->get<timeout_component>();
							if (timeout)
								timeout->update();
						}

						// Delay
						const auto delay = request_command->has_delay() ? request_command->get_delay() : default_timeout::router::download;

						set_status(status::pending);						
						set_state(machine_state::sleep, delay);
					}
					
					break;
				}
				case http_code::none:
				case http_code::bad_request:
				{
					set_status(status::warning);
					set_state(machine_state::sleep, default_timeout::error);
					
					// If the router sends a bad_request this means that it's overloaded, try again
					/*const auto router = owner->get<router_component>();
					if (router)
					{
						const auto str = boost::str(boost::format("Router \"%s\" is busy.") % router->get_address());
						log(str);
					}*/

					break;
				}
				case http_code::not_implemented:
				{
					set_status(status::error);
					set_state(machine_state::quit);
					
					const auto router = owner->get<router_component>();
					if (router)
					{
						const auto str = boost::str(boost::format("Not implemented: %s") % router->get_address());
						log(str);
					}

					break;
				}
				case http_code::not_found:
				{
					const auto owner = get_entity();

					if (transfer->done())
					{
						// Timer
						const auto timer = owner->get<timer_component>();
						if (timer && timer->running())
							timer->stop();

						set_status(status::none);
					}

					set_state(machine_state::quit);
										
					break;
				}
				default:
				{
					if (transfer->done())
					{
						set_status(status::none);
						set_state(machine_state::quit);
					}
					else
					{
						set_status(status::error);
						set_state(machine_state::restart, default_timeout::error);
						log(request_status);
					}

					break;
				}
			}
		}
		catch (std::exception& e)
		{
			if (transfer->done())
			{
				set_status(status::none);
				set_state(machine_state::quit);
			}
			else
			{
				set_status(status::error);
				set_state(machine_state::restart, default_timeout::error);
				log(e);
			}
		}
		catch (...)
		{
			if (transfer->done())
			{
				set_status(status::none);
				set_state(machine_state::quit);
			}
			else
			{
				set_status(status::error);
				set_state(machine_state::restart, default_timeout::error);
				log();
			}
		}

		return machine_component::on_download();
	}

	bool download_machine_component::on_quit()
	{
		const auto owner = get_entity();

		try
		{			
			// Session			
			if (owner->has<session_component>(false))
			{
				const auto request_command = client_quit_request_command::create(owner, m_socket);
				request_command->execute();
			}
		}
		catch (...) { }

		// Chunk			
		const auto chunk = owner->get<chunk_component>(false);
		if (chunk)
		{
			chunk->clear();
			chunk->shrink();
		}

		// Transfer
		const auto transfer = owner->get<transfer_component>();
		if (!transfer || transfer->done())
		{
			// Thread
			const auto thread = owner->get<download_thread_component>();
			if (thread)
				thread->quit();

			set_state(machine_state::none);
		}			
		else
		{
			set_state(machine_state::idle);
		}

		close();

		return machine_component::on_quit();
	}

	// Utility
	void download_machine_component::close()
	{
		try
		{
			m_socket->close();
		}
		catch (...) {}
	}

	// Mutator
	void download_machine_component::set_status(const eja::status status)
	{
		// HACK: Not needed
		//machine_component::set_status(status);

		const auto owner = get_entity();
		const auto thread = owner->get<download_thread_component>();
		if (thread)
			thread->set_status(status);
	}
}
