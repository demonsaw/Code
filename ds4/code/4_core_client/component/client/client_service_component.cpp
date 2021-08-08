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
#include "command/clear_command.h"
#include "command/entropy_command.h"
#include "command/join_command.h"
#include "command/mute_command.h"
#include "command/room_command.h"
#include "command/router_command.h"
#include "command/search_command.h"
#include "command/share_command.h"
#include "command/transfer_command.h"

#include "component/endpoint_component.h"
#include "component/room_component.h"
#include "component/service/socket_component.h"
#include "component/io/file_component.h"
#include "component/io/folder_component.h"
#include "component/io/share_component.h"
#include "component/callback/callback.h"
#include "component/callback/callback_service_component.h"
#include "component/client/client_component.h"
#include "component/client/client_service_component.h"
#include "component/group/group_component.h"
#include "component/group/group_option_component.h"
#include "component/message/message_component.h"
#include "component/router/router_component.h"
#include "component/router/router_service_component.h"
#include "component/search/search_option_component.h"
#include "component/session/session_component.h"
#include "component/transfer/download_service_component.h"
#include "component/transfer/upload_service_component.h"

#include "data/request/browse_request_data.h"
#include "data/request/chat_request_data.h"
#include "data/request/join_request_data.h"
#include "data/request/mute_request_data.h"
#include "data/request/room_request_data.h"

#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "entity/entity_util.h"
#include "http/http_socket.h"
#include "security/hash/hash.h"
#include "thread/thread_info.h"
#include "utility/value.h"

namespace eja
{
	// Static
#if _MOBILE
	const size_t client_service_component::min_threads = 1;
	const size_t client_service_component::max_threads = 1;
	const size_t client_service_component::num_threads = 1;
#else
	const size_t client_service_component::min_threads = 2;
	const size_t client_service_component::max_threads = std::max(static_cast<size_t>(4), static_cast<size_t>(std::thread::hardware_concurrency()));
	const size_t client_service_component::num_threads = 4;
#endif

	// Constructor
	client_service_component::client_service_component(const size_t threads) : socket_service_component(threads)
	{
		m_group_list = group_list_component::create();
		m_share_list = share_list_component::create();
	}

	// Interface
	void client_service_component::clear()
	{
		assert(thread_info::main());

		socket_service_component::clear();

		// Group
		const auto owner = get_owner();
		const auto group = owner->get<group_component>();
		group->clear();

		// Message
		const auto message_ring = owner->get<message_ring_component>();
		{
			thread_lock(message_ring);
			message_ring->clear();
		}

		// Socket
		const auto socket_set = owner->get<socket_set_component>();
		{
			thread_lock(socket_set);
			socket_set->clear();
		}
	}

	// Utility
	void client_service_component::start()
	{
		assert(thread_info::main());

		if (invalid() && set_state(state::starting, state::none))
		{
			socket_service_component::start();

			// Router
			const auto owner = get_owner();
			const auto router_list = owner->get<router_list_component>();
			const auto router_entity = router_list->get_entity();
			const auto router = router_entity->get<router_component>();
			owner->set<router_component>(router->copy());

			// Session
			const auto session = owner->get<session_component>();
			if (session->get_password() != router->get_password())
				session->compute(router->get_password());

			// Copy
			set_group_list();
			//set_share_list();

			// Commands
			async_share();
			async_entropy();
			async_timeout();

			set_state(state::running);
		}
	}

	void client_service_component::restart()
	{
		if (valid() && set_state(state::restarting, state::running))
		{
			const auto owner = get_owner();

			if (thread_info::main())
			{
				socket_service_component::stop();

				// Router
				const auto client = owner->get<client_component>();
				if (!client->is_update())
				{
					// Service
					const auto router_service = owner->get<router_service_component>();
					router_service->stop();

					// Status
					set_status(owner, status::none);
					set_state(state::none);

					// Callback
					owner->async_call(callback::status | callback::clear);
				}
				else
				{
					client->set_update(false);

					// TODO: Remove some duplicate code with stop()
					//
					//
					// Service
					const auto router_service = owner->get<router_service_component>();
					router_service->async_quit();
					router_service->stop();

					const auto download_service = owner->get<download_service_component>();
					download_service->set_threads(client->get_downloads());
					download_service->restart();

					const auto upload_service = owner->get<upload_service_component>();
					upload_service->set_threads(client->get_uploads());
					upload_service->restart();

					// Command
					clear_command cmd(owner);
					cmd.execute();

					// Status
					set_status(owner, status::none);
					set_state(state::none);
				}

				start();
			}
			else
			{
				// Wait
				const auto self = shared_from_this();
				wait(milliseconds::restart, [self]()
				{
					const auto owner = self->get_owner();
					self->set_status(owner, status::pending);

					// Commands
					self->async_entropy();
				});

				set_state(state::running);
			}
		}
	}

	void client_service_component::stop()
	{
		assert(thread_info::main());

		if (valid() && set_state(state::stopping, state::running))
		{
			socket_service_component::stop();

			// Service
			const auto owner = get_owner();
			const auto router_service = owner->get<router_service_component>();
			router_service->async_quit();
			router_service->stop();

			const auto download_service = owner->get<download_service_component>();
			download_service->stop();

			const auto upload_service = owner->get<upload_service_component>();
			upload_service->stop();

			// Command
			clear_command cmd(owner);
			cmd.execute();

			// Status
			set_status(owner, status::none);
			set_state(state::none);
		}
	}

	// Browse
	void client_service_component::async_browse(const entity::ptr& entity)
	{
		const auto owner = get_owner();

		try
		{
			// Command
			browse_command cmd(owner);
			const auto request_message = cmd.execute(entity);

			// Write
			const auto router_service = owner->get<router_service_component>();
			router_service->async_write(request_message);
		}
		catch (const std::exception& ex)
		{
			set_status(owner, status::error);
			owner->log(ex);
		}
	}

	// Chat
	void client_service_component::async_chat(const std::string& text)
	{
		const auto owner = get_owner();

		try
		{
			chat_command cmd(owner);
			const auto request_message = cmd.execute(text);

			// Write
			const auto router_service = owner->get<router_service_component>();
			router_service->async_write(request_message);
		}
		catch (const std::exception& ex)
		{
			set_status(owner, status::error);
			owner->log(ex);
		}
	}

	void client_service_component::async_chat(const entity::ptr& entity, const std::string& text)
	{
		const auto owner = get_owner();

		try
		{
			chat_command cmd(owner);
			const auto request_message = cmd.execute(entity, text);

			// Write
			const auto router_service = owner->get<router_service_component>();
			router_service->async_write(request_message);
		}
		catch (const std::exception& ex)
		{
			set_status(owner, status::error);
			owner->log(ex);
		}
	}

	void client_service_component::async_chat(const std::string& room, const std::string& text)
	{
		const auto owner = get_owner();

		try
		{
			chat_command cmd(owner);
			const auto request_message = cmd.execute(text);
			request_message->set_room(room);

			// Write
			const auto router_service = owner->get<router_service_component>();
			router_service->async_write(request_message);
		}
		catch (const std::exception& ex)
		{
			set_status(owner, status::error);
			owner->log(ex);
		}
	}

	// Download
	void client_service_component::async_download(const entity::ptr& entity)
	{
		const auto owner = get_owner();

		try
		{
			// Command
			transfer_command cmd(owner);
			const auto request_message = cmd.execute(entity);

			// Write
			const auto router_service = owner->get<router_service_component>();
			router_service->async_write(request_message);
		}
		catch (const std::exception& ex)
		{
			set_status(owner, status::error);
			owner->log(ex);
		}
	}

	// Entropy
	void client_service_component::async_entropy()
	{
		const auto self = shared_from_this();
		post([self]() { self->entropy(); });
	}

	void client_service_component::entropy()
	{
		// Status
		const auto owner = get_owner();
		set_status(owner, status::pending);

		try
		{
			// Command
			entropy_command cmd(owner);
			cmd.set_group_list(m_group_list);
			cmd.execute();

			// Callback
			owner->async_call(callback::group | callback::update);

			// Message
			async_router();
		}
		catch (const std::exception& ex)
		{
			// Restart
			restart();

			set_status(owner, status::error);

			owner->log(ex);
		}
	}

	// Join
	void client_service_component::async_join()
	{
		const auto owner = get_owner();

		try
		{
			// Command
			join_command cmd(owner);
			const auto request_message = cmd.execute();

			// Write
			const auto router_service = owner->get<router_service_component>();
			router_service->async_write(request_message);
		}
		catch (const std::exception& ex)
		{
			set_status(owner, status::error);
			owner->log(ex);
		}
	}

	// Mute
	void client_service_component::async_mute(const entity::ptr& entity, const mute_request_data::ptr& request_data) const
	{
		const auto owner = get_owner();

		try
		{
			mute_command cmd(owner);
			const auto request_message = cmd.execute(entity, request_data);

			// Write
			const auto router_service = owner->get<router_service_component>();
			router_service->async_write(request_message);
		}
		catch (const std::exception& ex)
		{
			set_status(owner, status::error);
			owner->log(ex);
		}
	}

	void client_service_component::async_mute(const entity::ptr& entity, const bool value) const
	{
		const auto request_data = mute_request_data::create();

		if (value)
			request_data->set_add(true);
		else
			request_data->set_remove(true);

		async_mute(entity, request_data);
	}

	// Room
	void client_service_component::async_room()
	{
		// Data
		const auto request_data = room_request_data::create();

		async_room(request_data);
	}

	void client_service_component::async_room(const room_request_data::ptr& request_data)
	{
		const auto owner = get_owner();

		try
		{
			// Message
			room_command cmd(owner);
			const auto request_message = cmd.execute(request_data);

			// Write
			const auto router_service = owner->get<router_service_component>();
			router_service->async_write(request_message);
		}
		catch (const std::exception& ex)
		{
			set_status(owner, status::error);
			owner->log(ex);
		}
	}

	void client_service_component::async_room_browse(const entity::ptr& entity)
	{
		const auto owner = get_owner();

		try
		{
			// Data
			const auto room = entity->get<room_component>();
			const auto request_data = room_request_data::create(room->get_id());
			request_data->set_browse(true);

			// Message
			room_command cmd(owner);
			const auto request_message = cmd.execute(entity, request_data);

			// Write
			const auto router_service = owner->get<router_service_component>();
			router_service->async_write(request_message);
		}
		catch (const std::exception& ex)
		{
			set_status(owner, status::error);
			owner->log(ex);
		}
	}

	void client_service_component::async_room_join(const entity::ptr& entity)
	{
		// Data
		const auto room = entity->get<room_component>();
		const auto request_data = room_request_data::create(room->get_id());
		request_data->set_join(true);

		async_room(request_data);
	}

	void client_service_component::async_room_quit(const entity::ptr& entity)
	{
		// Data
		const auto room = entity->get<room_component>();
		const auto request_data = room_request_data::create(room->get_id());
		request_data->set_quit(true);

		async_room(request_data);
	}

	// Router
	void client_service_component::async_router()
	{
		const auto self = shared_from_this();
		dispatch([self]() { self->router(); });
	}

	void client_service_component::router()
	{
		const auto owner = get_owner();

		try
		{
			// Message
			const auto router_service = owner->get<router_service_component>();
			router_service->start();

			const auto client = owner->get<client_component>();
			const auto download_service = owner->get<download_service_component>();
			download_service->set_threads(client->get_downloads());
			download_service->start();

			const auto upload_service = owner->get<upload_service_component>();
			upload_service->set_threads(client->get_uploads());
			upload_service->start();
		}
		catch (const std::exception& ex)
		{
			set_status(owner, status::error);

			owner->log(ex);
		}
	}

	// Search
	void client_service_component::async_search(const entity::ptr& entity)
	{
		const auto owner = get_owner();

		try
		{
			search_command cmd(owner);
			const auto request_message = cmd.execute(entity);

			// HACK: Empty string to bring up the search window
			const auto search_option = entity->get<search_option_component>();
			if (search_option->has_text())
			{
				// Write
				const auto router_service = owner->get<router_service_component>();
				router_service->async_write(request_message);
			}
		}
		catch (const std::exception& ex)
		{
			set_status(owner, status::error);
			owner->log(ex);
		}
	}

	// Share
	void client_service_component::async_share()
	{
		assert(thread_info::main());

		unshare();

		set_share_list();

		share();
	}

	void client_service_component::share()
	{
		const auto self = shared_from_this();
		for (const auto& e : *m_share_list)
			post([self, e]() { self->share(e); });
	}

	void client_service_component::async_share(const entity::ptr& entity)
	{
		if (thread_info::main())
			set_share_list();

		const auto self = shared_from_this();
		post([self, entity]() { self->share(entity); });
	}

	void client_service_component::share(const entity::ptr& entity)
	{
		const auto owner = get_owner();

		try
		{
			share_command cmd(owner);
			cmd.add(entity);
		}
		catch (const std::exception& ex)
		{
			owner->log(ex);
		}
	}

	// Timeout
	void client_service_component::async_timeout()
	{
		const auto self = shared_from_this();
		wait(milliseconds::monitor, [self]() { self->timeout(); });
	}

	void client_service_component::timeout()
	{
		const auto owner = get_owner();

		try
		{
			// Message
			entity_util::expire<message_map_component>(owner);
		}
		catch (const std::exception& ex)
		{
			owner->log(ex);
		}

		// Repeat
		async_timeout();
	}

	// Unshare
	void client_service_component::async_unshare()
	{
		assert(thread_info::main());

		const auto self = shared_from_this();
		post([self]() { self->unshare(); });
	}

	void client_service_component::unshare()
	{
		// Files
		const auto owner = get_owner();
		const auto file_map = owner->get<file_map_component>();
		{
			thread_lock(file_map);
			file_map->clear();
		}

		// Folders
		const auto folder_map = owner->get<folder_map_component>();
		{
			thread_lock(folder_map);
			folder_map->clear();
		}

		// Keywords
		const auto share_map = owner->get<share_map_component>();
		{
			thread_lock(share_map);
			share_map->clear();
		}

		// Callback
		owner->call(callback::share | callback::clear);
	}

	void client_service_component::async_unshare(const entity::ptr& entity)
	{
		if (thread_info::main())
			set_share_list();

		if (!m_share_list->empty())
		{
			const auto self = shared_from_this();
			post([self, entity]() { self->unshare(entity); });
		}
		else
		{
			// Optimization: Just unshare() when removing the last share
			async_unshare();
		}
	}

	void client_service_component::unshare(const entity::ptr& entity)
	{
		const auto owner = get_owner();

		try
		{
			share_command cmd(owner);
			cmd.remove(entity);
		}
		catch (const std::exception& ex)
		{
			owner->log(ex);
		}
	}

	// Set
	void client_service_component::set_status(const entity::ptr& entity, const status value) const
	{
		socket_service_component::set_status(entity, value);

		// Callback
		entity->async_call(callback::status | callback::update);
	}

	void client_service_component::set_group_list()
	{
		// Clear
		m_group_list->clear();

		// Copy
		const auto owner = get_owner();
		const auto group_list = owner->get<group_list_component>();
		std::copy(group_list->begin(), group_list->end(), std::back_inserter(*m_group_list));
	}

	void client_service_component::set_share_list()
	{
		// Clear
		m_share_list->clear();

		// Copy
		const auto owner = get_owner();
		const auto share_list = owner->get<share_list_component>();
		std::copy_if(share_list->begin(), share_list->end(), std::back_inserter(*m_share_list), [&](const entity::ptr e) { return e->enabled(); });

		// CLient
		const auto client = owner->get<client_component>();
		client->set_share(!m_share_list->empty());

		// Router
		const auto router_service = owner->get<router_service_component>();
		router_service->set_share_list(m_share_list);
	}
}
