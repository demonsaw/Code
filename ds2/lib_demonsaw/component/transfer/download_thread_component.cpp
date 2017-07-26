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
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include "download_thread_component.h"
#include "command/client/request/client_handshake_request_command.h"
#include "command/client/request/client_transfer_request_command.h"
#include "command/client/request/client_quit_request_command.h"
#include "component/timeout_component.h"
#include "component/timer_component.h"
#include "component/client/client_option_component.h"
#include "component/io/file_component.h"
#include "component/io/folder_component.h"
#include "component/router/router_component.h"
#include "component/transfer/chunk_component.h"
#include "component/transfer/download_machine_component.h"
#include "component/transfer/transfer_component.h"

namespace eja
{
	// Constructor
	download_thread_component::download_thread_component(const size_t size /*= 1*/) : m_size(size), m_thread_size(size)
	{
		assert(size);
	}

	// Interface
	void download_thread_component::init()
	{
		machine_component::init();

		// List
		m_entities.clear();

		// Multi-threading
		const auto owner = get_entity();
		const auto option = owner->get<client_option_component>();

		if (option && option->has_thread_size())
		{
			const auto file = owner->get<file_component>();
			if (file && (file->get_size() < option->get_thread_size()))
				m_size = m_thread_size = 1;
		}

		// Machine		
		for (size_t i = 0; i < m_size; ++i)
		{
			const auto entity = entity::create(owner);
			entity->add<download_machine_component>();
			entity->add<chunk_component>();
			m_entities.add(entity);
		}

		// Thread
		const auto self = shared_from_this();
		std::thread thread([self, owner]() { self->main(); });
		thread.detach();
	}

	void download_thread_component::shutdown()
	{
		machine_component::shutdown();

		{
			// Machine
			auto_lock_ref(m_entities);

			for (const auto& entity : m_entities)
			{
				const auto machine = entity->get<download_machine_component>();
				if (machine)
					machine->shutdown();
			}

			m_entities.clear();
		}

		close();

		kill();
	}

	void download_thread_component::start()
	{
		machine_component::start();

		// Quit
		m_quit.store(0);

		// Status
		set_status(status::pending);

		// Timeout
		const auto owner = get_entity();
		const auto timeout = owner->get<timeout_component>();
		if (timeout)
			timeout->start();

		// Timer		
		const auto timer = owner->get<timer_component>();
		if (timer)
			timer->start();

		// Transfer
		const auto transfer = owner->get<transfer_component>();
		if (transfer)
		{
			// Chunk
			const auto chunk = owner->get<chunk_component>(false);
			if (chunk)
			{
				const auto next = get_next_chunk(chunk->get_size());
				transfer->set_size(next);
				chunk->set_offset(next);				
			}				

			// Transfer
			transfer->reset();
		}

		// Server
		owner->remove<session_component>();
		owner->remove<router_component>();
		owner->remove<router_security_component>();
	}

	void download_thread_component::stop()
	{
		machine_component::stop();

		// Quit
		m_quit.store(1);

		// Timeout
		const auto owner = get_entity();
		const auto timeout = owner->get<timeout_component>();
		if (timeout)
			timeout->stop();

		// Timer
		const auto timer = owner->get<timer_component>();
		if (timer)
			timer->stop();

		// Status
		set_status(status::none);

		{
			// Machine
			auto_lock_ref(m_entities);

			size_t i = 0;
			for (const auto& entity : m_entities)
			{
				if (++i > m_thread_size)
					break;

				const auto machine = entity->get<download_machine_component>();
				if (machine)
					machine->stop();
			}
		}
	}

	bool download_thread_component::timeout() const
	{
		if (!m_timeout)
			return false;

		// Timeout
		const auto owner = get_entity();
		const auto timeout = owner->get<timeout_component>();
		if (!timeout)
			return false;

		// Option
		const auto option = owner->get<client_option_component>();
		if (!option)
			return false;

		return timeout->expired(option->get_download_timeout());
	}

	void download_thread_component::quit()
	{
		// Only do this once
		if (!m_quit.fetch_add(1))
		{
			m_timeout = false;

			// Timeout
			const auto owner = get_entity();
			const auto timeout = owner->get<timeout_component>();
			if (timeout)
				timeout->stop();

			// Timer
			const auto timer = owner->get<timer_component>();
			if (timer)
				timer->stop();

			kill();
		}
	}

	bool download_thread_component::add_chunk(const u64& chunk)
	{
		auto_lock_ref(m_mutex);

		const auto it = m_chunks.insert(chunk);
		return it.second;
	}

	u64 download_thread_component::get_next_chunk(const u64& size)
	{
		const auto owner = get_entity();

		if (size)
		{
			// File		
			const auto file = owner->get<file_component>();

			if (file)
			{
				auto_lock_ref(m_mutex);

				for (u64 i = 0; i < file->get_size(); i += size)
				{
					if (m_chunks.find(i) == m_chunks.end())
						return i;
				}
			}
		}

		// Transfer
		const auto transfer = owner->get<transfer_component>();
		return transfer ? transfer->get_size() : 0;
	}

	// Utility
	bool download_thread_component::running() const
	{
		if (machine_component::running())
			return true;

		size_t i = 0;

		auto_lock_ref(m_entities);

		for (const auto& entity : m_entities)
		{
			if (++i > m_thread_size)
				break;

			const auto machine = entity->get<download_machine_component>();
			if (machine && machine->running())
				return true;
		}

		return false;
	}

	bool download_thread_component::stoppable() const
	{
		size_t i = 0;

		auto_lock_ref(m_entities);

		for (const auto& entity : m_entities)
		{
			if (++i > m_thread_size)
				break;

			const auto machine = entity->get<download_machine_component>();
			if (machine && machine->stoppable())
				return true;
		}

		return false;
	}

	bool download_thread_component::resumable() const
	{
		if (machine_component::resumable())
			return true;

		size_t i = 0;

		auto_lock_ref(m_entities);

		for (const auto& entity : m_entities)
		{
			if (++i > m_thread_size)
				break;

			const auto machine = entity->get<download_machine_component>();
			if (machine && machine->resumable())
				return true;
		}

		return false;
	}

	bool download_thread_component::removeable() const
	{
		// The following DOESNT FUCKING WORK!!!
		//if (machine_component::removeable())
		if (inactive())
			return true;

		size_t i = 0;

		auto_lock_ref(m_entities);

		for (const auto& entity : m_entities)
		{
			if (++i > m_thread_size)
				break;

			const auto machine = entity->get<download_machine_component>();
			if (machine && machine->removeable())
				return true;
		}

		return false;
	}

	void download_thread_component::close()
	{
		try
		{
			m_socket->close();
		}
		catch (...) {}
	}

	// Mutator
	void download_thread_component::set_status(const eja::status status)
	{
		// Disable once we've quit
		if (!m_quit.load() || (status == eja::status::none) || (status == eja::status::cancelled))
			machine_component::set_status(status);
	}

	// Callback
	bool download_thread_component::on_start()
	{
		try
		{
			close();

			m_timeout = true;

			const auto owner = get_entity();

			if (owner->has<router_component>(false))
			{
				// Data Router
				const auto option = owner->get<client_option_component>();
				if (!option)
					return set_error();

				const auto router = owner->get<router_component>(false);
				if (!router)
					return set_error();

				m_socket->set_timeout(option->get_socket_timeout());
				m_socket->open(router->get_address(), router->get_port());

				owner->add<session_component>();

				set_status(status::info);
				set_state(machine_state::handshake);
			}
			else
			{
				// Folder
				const auto folder = owner->get<folder_component>();
				if (!folder)
					return set_error();

				if (!folder->exists())
					folder_util::create(folder);

				// File
				const auto file = owner->get<file_component>();
				if (!file)
					return set_error();

				if (!file->exists())
					file_util::create(file, file->get_size());

				// Router					
				const auto option = owner->get<client_option_component>();
				if (!option)
					return set_error();

				const auto router = owner->get<router_component>();
				if (!router)
					return set_error();

				m_socket->set_timeout(option->get_socket_timeout());
				m_socket->open(router->get_address(), router->get_port());

				set_status(status::info);
				set_state(machine_state::transfer);

			}
		}
		catch (std::exception& e)
		{
			set_status(status::error);
			set_state(machine_state::sleep, default_timeout::error);
			log(e);
		}
		catch (...)
		{
			set_status(status::error);
			set_state(machine_state::sleep, default_timeout::error);
			log();
		}

		return machine_component::on_start();
	}

	bool download_thread_component::on_restart()
	{
		set_status(status::pending);

		{
			// Machine
			auto_lock_ref(m_entities);

			size_t i = 0;
			for (const auto& entity : m_entities)
			{
				if (++i > m_thread_size)
					break;

				const auto machine = entity->get<download_machine_component>();
				if (machine)
					machine->restart();
			}
		}

		return machine_component::on_restart();
	}

	bool download_thread_component::on_stop()
	{
		m_timeout = false;

		return set_state(machine_state::idle);
	}

	bool download_thread_component::on_cancel()
	{
		// Quit
		m_quit.store(1);

		// Timeout
		const auto owner = get_entity();
		const auto timeout = owner->get<timeout_component>();
		if (timeout)
			timeout->stop();

		// Transfer		
		const auto transfer = owner->get<transfer_component>();
		if (transfer && !transfer->done())
		{
			// Retry
			const auto option = owner->get<client_option_component>();
			m_retry = option && option->get_max_retries() && (transfer->updated() || (transfer->get_retries() < option->get_max_retries()));

			if (m_retry)
			{
				// File
				const auto file = owner->get<file_component>();
				if (file)
				{
					const auto str = boost::str(boost::format("Restarting %s") % file->get_name());
					log(str);
				}
			}
			else
			{
				// Router
				const auto file = owner->get<file_component>();
				const auto router = owner->get<router_component>();
				if (file && router)
				{
					const auto str = boost::str(boost::format(" \"%s\" timed out on %s") % file->get_name() % router->get_address());
					log(str);
				}
			}
		}

		// Status
		set_status(status::cancelled);

		{
			// Machine
			auto_lock_ref(m_entities);

			size_t i = 0;
			for (const auto& entity : m_entities)
			{
				if (++i > m_thread_size)
					break;

				const auto machine = entity->get<download_machine_component>();
				if (machine)
					machine->stop();
			}
		}

		return set_state(machine_state::quit);
	}

	bool download_thread_component::on_transfer()
	{
		try
		{
			const auto owner = get_entity();
			const auto request_command = client_transfer_request_command::create(owner, m_socket);
			const auto request_status = request_command->execute();

			switch (request_status.get_code())
			{
				case http_code::ok:
				{
					// Router
					m_thread_size = m_size;

					if (m_size > 1)
					{
						const auto router = owner->get<router_component>();

						if (router)
						{
							// HACK: Support older routers
							const auto& version = router->get_version();
							if (version.empty() || boost::starts_with(version, "2.5"))
								m_thread_size = 1;
						}
					}					

					if (owner->has<router_component>(false) && !owner->has<session_component>(false))
					{
						set_status(status::info);
						set_state(machine_state::start);
					}
					else
					{
						{
							auto_lock_ref(m_entities);

							size_t i = 0;
							for (const auto& entity : m_entities)
							{
								if (++i > m_thread_size)
									break;

								const auto machine = entity->get<download_machine_component>();
								if (machine)
									machine->start();
							}
						}

						set_status(status::info);
						set_state(machine_state::idle);

						close();
					}

					break;
				}
				case http_code::not_implemented:
				{
					m_retry = false;

					set_status(status::error);
					set_state(machine_state::quit);

					const auto router = owner->get<router_component>();
					if (router)
					{
						const auto str = boost::str(boost::format("Download is disabled on \"%s\".") % router->get_address());
						log(str);
					}

					break;
				}
				case http_code::none:
				case http_code::bad_request:
				case http_code::service_unavailable:
				{
					set_status(status::warning);
					set_state(machine_state::sleep, default_timeout::error);

					// If the router sends a bad_request this means that it's overloaded, try again
					const auto router = owner->get<router_component>();
					if (router)
					{
						const auto str = boost::str(boost::format("Router \"%s\" is busy.") % router->get_address());
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

		return machine_component::on_transfer();
	}

	bool download_thread_component::on_handshake()
	{
		try
		{
			const auto owner = get_entity();
			const auto request_command = client_handshake_request_command<router_type::transfer>::create(owner, m_socket);
			const auto request_status = request_command->execute();

			switch (request_status.get_code())
			{
				case http_code::ok:
				{
					set_status(status::info);
					set_state(machine_state::transfer);

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
						const auto str = boost::str(boost::format("Router \"%s\" is busy.") % router->get_address());
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

	bool download_thread_component::on_quit()
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
		catch (...) {}

		// Timeout
		const auto timeout = owner->get<timeout_component>();
		if (timeout)
			timeout->stop();

		// Timer
		const auto timer = owner->get<timer_component>();
		if (timer)
			timer->stop();

		// Retry
		if (m_retry)
		{
			m_retry = false;

			// Transfer
			const auto owner = get_entity();
			const auto transfer = owner->get<transfer_component>();

			if (transfer)
			{
				// Transfer				
				transfer->add_retry();
				transfer->reset();

				// Chunk
				const auto chunk = owner->get<chunk_component>();
				if (chunk)
					chunk->set_offset(transfer->get_size());
			}

			// Queue
			call(function_type::download, function_action::remove, owner);
			call(function_type::transfer, function_action::add, owner);
		}
		else
		{
			// Finished
			call(function_type::download, function_action::remove, owner);
			call(function_type::finished, function_action::add, owner);
		}

		close();

		return set_state(machine_state::none);
	}
}
