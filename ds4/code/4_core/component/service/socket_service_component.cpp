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

#include <random>
#include <boost/format.hpp>

#include "component/service/socket_component.h"
#include "component/service/socket_service_component.h"

#include "entity/entity.h"
#include "http/http_socket.h"
#include "utility/value.h"

namespace eja
{
	// Operator
	socket_service_component& socket_service_component::operator=(const socket_service_component& comp)
	{
		if (this != &comp)
			service_component::operator=(comp);

		return *this;
	}

	// Utility
	void socket_service_component::close()
	{
		service_component::close();

		// Sockets
		const auto owner = get_owner();
		const auto socket_set = owner->get<socket_set_component>();
		{
			thread_lock(socket_set);
			for (const auto& socket : *socket_set)
				socket->close();

			// NOTE: Duplicate code in clear() for safety
			socket_set->clear();
		}
	}

	void socket_service_component::close(const entity::ptr& entity)
	{
		// Socket
		const auto socket_set = entity->get<socket_set_component>();
		{
			thread_lock(socket_set);
			for (const auto& socket : *socket_set)
				socket->close();

			// NOTE: Duplicate code in clear() for safety
			socket_set->clear();
		}
	}

	// Add
	void socket_service_component::add_socket(const http_socket::ptr& socket)
	{
		const auto owner = get_owner();
		add_socket(owner, socket);
	}

	void socket_service_component::add_socket(const entity::ptr& owner, const http_socket::ptr& socket)
	{
		const auto socket_set = owner->get<socket_set_component>();
		{
			thread_lock(socket_set);
			socket_set->insert(socket);
		}
	}

	// Remove
	void socket_service_component::remove_socket(const http_socket::ptr& socket)
	{
		const auto owner = get_owner();
		remove_socket(owner, socket);
	}

	void socket_service_component::remove_socket(const entity::ptr& owner, const http_socket::ptr& socket)
	{
		const auto socket_set = owner->get<socket_set_component>();
		{
			thread_lock(socket_set);
			socket_set->erase(socket);
		}
	}

	// Has
	bool socket_service_component::has_socket() const
	{
		const auto owner = get_owner();
		return has_socket(owner);
	}

	bool socket_service_component::has_socket(const entity::ptr& owner) const
	{
		const auto socket_set = owner->get<socket_set_component>();
		{
			thread_lock(socket_set);
			return !socket_set->empty();
		}
	}

	// Get
	http_socket::ptr socket_service_component::get_socket(const bool create /*= true*/)
	{
		const auto owner = get_owner();
		return get_socket(owner, create);
	}

	http_socket::ptr socket_service_component::get_socket(const entity::ptr& owner, const bool create /*= true*/)
	{
		const auto socket_set = owner->get<socket_set_component>();
		{
			thread_lock(socket_set);

			if (!socket_set->empty())
			{
				const auto it = socket_set->begin();
				const auto socket = *it;
				socket_set->erase(it);

				return socket;

				/*if (socket_set->size() == 1)
				{
					const auto it = socket_set->begin();
					const auto socket = *it;
					socket_set->clear();

					return socket;
				}
				else
				{
					std::random_device rnd;
					std::uniform_int_distribution<size_t> dist(0, socket_set->size() - 1);
					const auto i = dist(rnd);

					const auto it = std::next(socket_set->begin(), i);
					const auto socket = *it;
					socket_set->erase(it);

					return socket;
				}*/
			}
		}

		return create ? http_socket::create(get_service()) : nullptr;
	}
}