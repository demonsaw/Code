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

#include "component/callback/callback_service_component.h"

#include "entity/entity.h"
#include "http/http_request.h"
#include "http/http_response.h"
#include "http/http_socket.h"
#include "message/message_packer.h"
#include "thread/thread_info.h"
#include "utility/value.h"

namespace eja
{
	// Interface
	void callback_service_component::clear()
	{
		service_component::clear();

		thread_lock(m_map);
		m_map->clear();
	}

	// Utility
	void callback_service_component::start()
	{
		if (invalid() && set_state(state::starting, state::none))
		{
			service_component::start();

			set_state(state::running);
		}
	}

	void callback_service_component::restart()
	{
		if (invalid() && set_state(state::restarting, state::running))
		{
			service_component::restart();

			set_state(state::running);
		}
	}

	void callback_service_component::stop()
	{
		if (valid() && set_state(state::stopping, state::running))
		{
			service_component::stop();

			set_state(state::none);
		}
	}

	// Add
	void callback_service_component::add(const size_t key, const function::ptr& func)
	{
		const auto pair = std::make_pair(key, func);

		thread_lock(m_map);
		m_map.insert(pair);
	}

	// Remove
	void callback_service_component::remove(const size_t key)
	{
		thread_lock(m_map);
		m_map->erase(key);
	}

	void callback_service_component::remove(const size_t key, const function::ptr& func)
	{
		thread_lock(m_map);
		const auto range = m_map.equal_range(key);
		//assert(range.first != range.second);

		for (auto it = range.first; it != range.second; )
		{
			if (func == it->second)
				m_map->erase(it++);
			else
				++it;
		}
	}

	void callback_service_component::remove(const function::ptr& func)
	{
		// O(n)
		thread_lock(m_map);
		for (auto it = m_map.begin(); it != m_map->end(); )
		{
			if (func == it->second)
				m_map->erase(it++);
			else
				++it;
		}
	}

	// Call
	void callback_service_component::async_call(const size_t key) const
	{
		const auto self = shared_from_this();
		post_strand([self, key]() { self->call(key); });
	}

	void callback_service_component::call(const size_t key) const
	{
		thread_lock(m_map);

		const auto range = m_map.equal_range(key);
		//assert(range.first != range.second);

		for (auto it = range.first; it != range.second; ++it)
		{
			const auto& func = it->second;
			func->call();
		}
	}

	// Quit
	void callback_service_component::async_quit(const std::string& address, const u16 port, const http_request::ptr request)
	{
		const auto self = shared_from_this();
		post([self, address, port, request]() { self->quit(address, port, request); });
	}

	void callback_service_component::quit(const std::string& address, const u16 port, const http_request::ptr request)
	{
		http_socket::ptr socket;

		try
		{
			// 1) Open
			const auto service = get_service();
			socket = http_socket::create(service);
			socket->set_timeout(milliseconds::open);
			socket->open(address, port);

			// 2) Write
			socket->set_timeout(milliseconds::socket);
			socket->write(request);

			// 3) Read
			http_response response;
			socket->read(response);

			// 4) Close
			socket->close();
		}
		catch (const std::exception& /*ex*/)
		{
			if (socket)
				socket->close();

			//log(ex);
		}
	}
}
