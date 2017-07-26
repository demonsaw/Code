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
#include <boost/format.hpp>

#include "upload_thread_component.h"
#include "component/timeout_component.h"
#include "component/timer_component.h"
#include "component/client/client_option_component.h"
#include "component/io/file_component.h"
#include "component/router/router_component.h"
#include "component/transfer/chunk_component.h"
#include "component/transfer/transfer_component.h"
#include "component/transfer/upload_machine_component.h"

namespace eja
{
	// Constructor
	upload_thread_component::upload_thread_component(const size_t size /*= 1*/) : m_size(size)
	{
		assert(size);		
	}

	// Interface
	void upload_thread_component::init()
	{
		machine_component::init();

		// Clear
		m_entities.clear();		

		// Multi-threading
		const auto owner = get_entity();
		const auto option = owner->get<client_option_component>();

		if (option && option->has_thread_size())
		{
			const auto file = owner->get<file_component>();
			if (file && (file->get_size() < option->get_thread_size()))
				m_size = 1;
		}

		// TODO: REMOVE ME FOR 2.7.0
		//
		const auto chunk = owner->get<chunk_component>();

		// Machine
		for (size_t i = 0; i < m_size; ++i)
		{
			const auto entity = entity::create(owner);
			entity->add<upload_machine_component>();
			
			// TODO: REMOVE ME FOR 2.7.0
			//
			if (i > 0)
			{
				entity->add<chunk_component>();
			}
			else
			{
				const auto cc = chunk_component::create();
				cc->set_offset(chunk->get_offset());
				cc->set_size(chunk->get_size());
				entity->add(cc);
			}

			m_entities.add(entity);
		}

		// Thread				
		const auto self = shared_from_this();
		std::thread thread([self, owner]() { self->main(); });
		thread.detach();
	}

	void upload_thread_component::shutdown()
	{
		machine_component::shutdown();

		{
			// Machine
			auto_lock_ref(m_entities);

			for (const auto& entity : m_entities)
			{
				const auto machine = entity->get<upload_machine_component>();
				if (machine)
					machine->shutdown();
			}
			
			m_entities.clear();
		}
	}

	void upload_thread_component::start()
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

		{
			// Machine
			auto_lock_ref(m_entities);

			for (const auto& entity : m_entities)
			{
				const auto machine = entity->get<upload_machine_component>();
				if (machine)
					machine->start();
			}
		}
	}

	void upload_thread_component::stop()
	{
		machine_component::stop();		

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

		// Quit
		m_quit.store(1);

		{
			// Machine
			auto_lock_ref(m_entities);

			for (const auto& entity : m_entities)
			{
				const auto machine = entity->get<upload_machine_component>();
				if (machine)
					machine->stop();
			}
		}
	}

	bool upload_thread_component::timeout() const
	{
		// Timeout
		const auto owner = get_entity();
		const auto timeout = owner->get<timeout_component>();
		if (!timeout)
			return false;

		// Option
		const auto option = owner->get<client_option_component>();
		if (!option)
			return false;

		return timeout->expired(option->get_upload_timeout());
	}

	void upload_thread_component::quit()
	{
		// Only do this once
		if (!m_quit.fetch_add(1))
		{
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

	bool upload_thread_component::add_chunk(const u64& chunk)
	{
		auto_lock_ref(m_mutex);

		const auto it = m_chunks.insert(chunk);
		return it.second;
	}

	// Utility
	bool upload_thread_component::running() const
	{
		if (machine_component::running())
			return true;

		auto_lock_ref(m_entities);

		for (const auto& entity : m_entities)
		{
			const auto machine = entity->get<upload_machine_component>();
			if (machine && machine->running())
				return true;
		}

		return false;
	}

	bool upload_thread_component::stoppable() const
	{
		auto_lock_ref(m_entities);

		for (const auto& entity : m_entities)
		{
			const auto machine = entity->get<upload_machine_component>();
			if (machine && machine->stoppable())
				return true;
		}

		return false;
	}

	bool upload_thread_component::removeable() const
	{
		if (inactive())
			return true;

		auto_lock_ref(m_entities);

		for (const auto& entity : m_entities)
		{
			const auto machine = entity->get<upload_machine_component>();
			if (machine && machine->removeable())
				return true;
		}

		return false;
	}

	// Mutator
	void upload_thread_component::set_status(const eja::status status)
	{
		// Disable once we've quit
		if (!m_quit.load() || (status == eja::status::none) || (status == eja::status::cancelled))
			machine_component::set_status(status);
	}

	// Callback	
	bool upload_thread_component::on_start()
	{
		return set_state(machine_state::idle);
	}
	
	bool upload_thread_component::on_stop()
	{		
		return set_state(machine_state::quit);
	}

	bool upload_thread_component::on_cancel()
	{
		// Timeout
		const auto owner = get_entity();
		const auto timeout = owner->get<timeout_component>();
		if (timeout)
			timeout->stop();

		// Transfer
		const auto transfer = owner->get<transfer_component>();
		if (transfer)
		{
			set_status(!transfer->empty() ? status::cancelled : status::none);
			transfer->done(true);
		}
		else
			set_status(status::cancelled);

		// Router
		const auto file = owner->get<file_component>();
		const auto router = owner->get<router_component>();
		if (file && router)
		{
			const auto str = boost::str(boost::format(" \"%s\" timed out on %s") % file->get_name() % router->get_address());
			log(str);
		}
		
		{
			// Machine
			auto_lock_ref(m_entities);

			for (const auto& entity : m_entities)
			{
				const auto machine = entity->get<upload_machine_component>();
				if (machine)
					machine->stop();
			}
		}

		return set_state(machine_state::quit);
	}

	bool upload_thread_component::on_quit()
	{
		// Timeout
		const auto owner = get_entity();
		const auto timeout = owner->get<timeout_component>();
		if (timeout)
			timeout->stop();

		// Timer
		const auto timer = owner->get<timer_component>();
		if (timer)
			timer->stop();

		return set_state(machine_state::none);
	}
}
