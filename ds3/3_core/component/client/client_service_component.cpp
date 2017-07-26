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

#include "command/group_command.h"
#include "command/share_command.h"

#include "component/browse_component.h"
#include "component/endpoint_component.h"
#include "component/socket_component.h"
#include "component/state_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/client/client_acceptor_component.h"
#include "component/client/client_io_component.h"
#include "component/client/client_network_component.h"
#include "component/client/client_service_component.h"
#include "component/group/group_component.h"
#include "component/group/group_security_component.h"
#include "component/io/download_component.h"
#include "component/io/file_component.h"
#include "component/io/transfer_component.h"
#include "component/io/folder_component.h"
#include "component/io/save_component.h"
#include "component/io/share_component.h"
#include "component/io/upload_component.h"
#include "component/message/message_service_component.h"
#include "component/session/session_component.h"
#include "component/status/status_component.h"
#include "component/time/timer_component.h"
#include "component/transfer/transfer_service_component.h"

#include "entity/entity.h"
#include "entity/entity_factory.h"

#include "security/hash/hash.h"
#include "system/type.h"
#include "thread/thread_info.h"
#include "utility/value.h"
#include "utility/client/client_util.h"
#include "utility/io/file_util.h"

namespace eja
{
	// Operator
	client_service_component& client_service_component::operator=(const client_service_component& comp)
	{
		if (this != &comp)
			service_component::operator=(comp);

		return *this;
	}

	void client_service_component::update()
	{
		service_component::update();

		// Thread-safe
		const auto owner = get_entity();
		m_share_list = owner->copy<share_list_component>();

		m_group_list = copy_if_enabled<group_security_list_component>();		
		m_message_list = copy_if_enabled<message_list_component>();
		m_transfer_list = copy_if_enabled<transfer_list_component>();

		// Share		
		const auto share_list = owner->get<share_list_component>();
		const auto network = owner->get<client_network_component>();
		network->set_share(!share_list->empty());

		set_status();
	}

	// NOTE: This must happen AFTER we stop the service
	//
	void client_service_component::clear()
	{
		service_component::clear();

		// Callback
		const auto owner = get_entity();
		owner->call(callback_type::client, callback_action::clear);
		owner->call(callback_type::browse, callback_action::clear);
		owner->call(callback_type::group, callback_action::clear);
		owner->call(callback_type::message, callback_action::clear);
		owner->call(callback_type::search, callback_action::clear);
		//owner->call(callback_type::error, callback_action::clear);

		// Group		
		const auto group = owner->get<group_component>();
		group->clear();

		// Browse		
		const auto browse_map = owner->get<browse_map_component>();
		{
			thread_lock(browse_map);
			browse_map->clear();
		}

		const auto save_map = owner->get<save_map_component>();
		{
			thread_lock(save_map);
			save_map->clear();
		}

		// Thread-safe
		m_group_list.reset();
		m_share_list.reset();
		m_message_list.reset();
		m_transfer_list.reset();
	}

	// Utility
	void client_service_component::start()
	{
		if (valid())
			return;

		const auto owner = get_entity();
		const auto state = owner->get<state_component>();

		if (state->set(eja::state::idle, eja::state::none))
		{
			set_status(owner, status::info);

			// Acceptor
			const auto acceptor = owner->get<client_acceptor_component>();
			acceptor->start();

			service_component::start();

			if (thread_info::main())
				update();

			// Group
			async_group();

			// Queue
			async_queue();

			// Share			
			const auto share_map = owner->get<share_map_component>();
			if (share_map->empty())
				async_share();

			state->set(eja::state::running);
		}		
	}

	void client_service_component::restart()
	{
		if (invalid())
			return;

		const auto owner = get_entity();
		const auto state = owner->get<state_component>();

		if (state->set(eja::state::idle, eja::state::running))
		{
			// Stop
			set_status(owner, status::pending);

			// Acceptor
			const auto acceptor = owner->get<client_acceptor_component>();
			acceptor->restart();

			// Message
			for (const auto& e : *get_message_list())
			{
				const auto service = e->get<message_service_component>();
				service->stop();
			}

			service_component::stop();

			if (thread_info::main())
				update();

			// Start
			set_status(owner, status::info);

			service_component::start();

			// Group
			async_group();

			// Queue
			async_queue();

			// Share
			const auto share_map = owner->get<share_map_component>();
			if (share_map->empty())
				async_share();

			state->set(eja::state::running);
		}
	}

	void client_service_component::stop()
	{
		//assert(thread_info::main());

		if (invalid())
			return;

		const auto owner = get_entity();
		const auto state = owner->get<state_component>();

		if (state->set(eja::state::idle, eja::state::running))
		{
			// Callback
			owner->call(callback_type::download, callback_action::clear);
			owner->call(callback_type::upload, callback_action::clear);
			owner->call(callback_type::transfer, callback_action::clear);
			owner->call(callback_type::chat, callback_action::clear);

			// Acceptor
			const auto acceptor = owner->get<client_acceptor_component>();
			acceptor->stop();

			// Message
			const auto message_list = get_message_list();
			for (const auto& e : *message_list)
			{
				const auto service = e->get<message_service_component>();
				service->stop();
			}

			unshare();

			service_component::stop();

			// Status
			set_status(owner, status::none);

			state->set(eja::state::none);
		}		
	}

	// Browse
	void client_service_component::async_browse(const entity::ptr data)
	{
		assert(thread_info::main());

		const auto owner = get_entity();
		const auto router = client_util::get_router(owner, data);

		if (router)
		{
			const auto service = router->get<message_service_component>();
			service->async_browse(data);
		}
	}
	
	// Chat
	void client_service_component::async_chat(const entity::ptr data, const entity::ptr client /*= nullptr*/)
	{
		assert(thread_info::main());

		const auto message_list = get_message_list();
		for (const auto& e : *message_list)
		{
			const auto service = e->get<message_service_component>();
			service->async_chat(data, client);
		}
	}

	// Client
	void client_service_component::async_client()
	{
		assert(thread_info::main());

		// Clear
		const auto owner = get_entity();
		const auto browse_map = owner->get<browse_map_component>();
		{
			thread_lock(browse_map);
			browse_map->clear();
		}

		const auto save_map = owner->get<save_map_component>();
		{
			thread_lock(save_map);
			save_map->clear();
		}

		const auto message_list = get_message_list();
		for (const auto& e : *message_list)
		{
			const auto service = e->get<message_service_component>();
			service->async_client();
		}
	}

	// Download
	void client_service_component::async_download(const entity::ptr data)
	{
		assert(thread_info::main());

		// Message
		const auto message_list = get_message_list();
		for (const auto& e : *message_list)
		{
			const auto service = e->get<message_service_component>();
			service->async_file(data);
		}
	}

	// File
	void client_service_component::async_file(const entity::ptr data, const entity::ptr client /*= nullptr*/)
	{
		//assert(thread_info::main());

		// Folder
		const auto owner = get_entity();
		const auto io = owner->get<client_io_component>();
		const auto folder = folder_component::create(io->get_path());

		async_file(data, folder, client);
	}

	void client_service_component::async_file(const entity::ptr data, const std::string& path, const entity::ptr client /*= nullptr*/)
	{
		//assert(thread_info::main());

		// Folder
		const auto folder = folder_component::create(path);

		async_file(data, folder, client);
	}

	void client_service_component::async_file(const entity::ptr data, const boost::filesystem::path& path, const entity::ptr client /*= nullptr*/)
	{
		//assert(thread_info::main());

		// Folder
		const auto folder = folder_component::create(path);

		async_file(data, folder, client);
	}

	void client_service_component::async_file(const std::shared_ptr<entity> data, const folder_component::ptr folder, const entity::ptr client /*= nullptr*/)
	{
		//assert(thread_info::main());

		// Transfer routers
		const auto owner = get_entity();
		const auto transfer_list = get_transfer_list();

		if (transfer_list->empty())
		{
			// Status
			const auto status = owner->get<status_component>();
			status->set_status(eja::status::warning);

			// Log
			owner->log("No transfer routers are available");

			return;
		}
		
		bool invalid = true;

		for (const auto& e : *transfer_list)
		{
			const auto status = e->get<status_component>();
			if (!status->is_error())
			{
				invalid = false;
				break;
			}
		}

		if (invalid)
		{
			// Status
			const auto status = owner->get<status_component>();
			status->set_status(eja::status::warning);

			// Log
			owner->log("No transfer routers are available");

			return;
		}

		// Async
		const auto self = shared_from_this();
		post([self, data, folder, client]() { self->file(data, folder, client); });
	}

	void client_service_component::file(const entity::ptr data, const folder_component::ptr folder, const entity::ptr client /*= nullptr*/)
	{
		// Entity
		const auto owner = get_entity();
		const auto entity = entity_factory::create_client_download(owner);
		const auto id = security::get_id();
		entity->set_id(id);

		// Client
		if (client)
			entity->add<endpoint_component>(client);

		// Folder
		entity->add(folder);

		// File
		boost::filesystem::path path(folder->get_data());
		const auto file = data->get<file_component>();
		path /= file->get_data();

		if (!file_util::exists(path))
			file_util::create(path, file->get_size());

		const auto local_file = entity->get<file_component>();
		local_file->set_id(file->get_id());
		local_file->set_size(file->get_size());
		local_file->set_path(path);

		// Transfer
		const auto transfer = entity->get<transfer_component>();

		if (!file->get_size())
		{
			// Status
			const auto status = entity->get<status_component>();
			status->set_status(eja::status::success);

			transfer->stop();
			return;
		}

		// Chunk
		const auto network = owner->get<client_network_component>();
		const auto chunk_size = network->get_chunk();
		transfer->set_chunk(chunk_size);

		const auto download_index = entity->get<download_index_array_component>();
		const auto chunks = (file->get_size() / chunk_size) + ((file->get_size() % chunk_size) ? 1 : 0);
		for (auto i = 0; i < chunks; ++i)
			download_index->push_back(i);

		// Queue?
		const auto download_map = owner->get<download_map_component>();
		{
			const auto pair = std::make_pair(entity->get_id(), entity);

			thread_lock(download_map);

			if (download_map->size() < network->get_downloads())
			{
				download_map->insert(pair);

				// Callback
				owner->call(callback_type::download, callback_action::add, entity);
			}				
			else
			{
				// Callback
				owner->call(callback_type::queue, callback_action::add, entity);
			}				
		}
	}

	// Folder
	void client_service_component::async_folder(const entity::ptr data)
	{
		assert(thread_info::main());

		// Folder
		const auto owner = get_entity();
		const auto io = owner->get<client_io_component>();
		const auto& path = io->get_path();

		async_folder(data, path);
	}

	void client_service_component::async_folder(const entity::ptr data, const boost::filesystem::path& path)
	{
		assert(thread_info::main());

		async_folder(data, path.string());
	}

	void client_service_component::async_folder(const entity::ptr data, const std::string& path)
	{
		assert(thread_info::main());

		const auto owner = get_entity();
		const auto router = client_util::get_router(owner, data);

		if (router)
		{
			const auto service = router->get<message_service_component>();
			service->async_folder(data, path);
		}
	}

	// Folder (no swarm)
	void client_service_component::async_folder_client(const entity::ptr data)
	{
		assert(thread_info::main());

		// Folder
		const auto owner = get_entity();
		const auto io = owner->get<client_io_component>();
		const auto& path = io->get_path();

		async_folder_client(data, path);
	}

	void client_service_component::async_folder_client(const entity::ptr data, const boost::filesystem::path& path)
	{
		assert(thread_info::main());

		async_folder_client(data, path.string());
	}

	void client_service_component::async_folder_client(const entity::ptr data, const std::string& path)
	{
		assert(thread_info::main());

		const auto owner = get_entity();
		const auto router = client_util::get_router(owner, data);

		if (router)
		{
			const auto service = router->get<message_service_component>();
			service->async_folder_client(data, path);
		}
	}

	// Group
	void client_service_component::async_group()
	{		
		const auto self = shared_from_this();
		post([self]() { self->group(); });
	}

	void client_service_component::group()
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
			const auto message_list = get_message_list();

			if (!message_list->empty())
			{
				for (const auto& e : *message_list)
				{
					const auto service = e->get<message_service_component>();
					service->start();
				}
			}
			else
			{
				set_status(owner, status::warning);

				owner->log("No message routers are available");
			}
		}
		catch (const std::exception& ex)
		{
			// Status
			set_status(owner, status::error);

			// Log
			owner->log(ex);
		}
	}

	// Message
	void client_service_component::async_message()
	{
		assert(thread_info::main());	

		update();

		const auto message_list = get_message_list();
		for (const auto& e : *message_list)
		{
			const auto service = e->get<message_service_component>();
			if (service->valid())
				service->restart();
		}
	}

	void client_service_component::async_message(const entity::ptr router)
	{
		assert(thread_info::main());

		update();

		const auto service = router->get<message_service_component>();
		if (service->valid())
			service->restart();
	}

	// Queue
	void client_service_component::async_queue()
	{
		const auto self = shared_from_this();

		wait(milliseconds::queue, [self]()
		{
			if (self->valid())
				self->queue();
		});
	}

	void client_service_component::queue()
	{
		try
		{
			// Callback
			const auto owner = get_entity();
			owner->call(callback_type::queue, callback_action::update);

			// Repeat
			async_queue();
		}
		catch (const std::exception& ex)
		{
			log(ex);
		}
	}

	// Quit
	void client_service_component::async_quit()
	{		
		if (invalid())
			return;

		// Status
		const auto owner = get_entity();
		set_status(owner, status::pending);

		const auto state = owner->get<state_component>();

		if (state->set(eja::state::idle, eja::state::running))
		{
			// Callback
			owner->call(callback_type::download, callback_action::clear);
			owner->call(callback_type::upload, callback_action::clear);
			owner->call(callback_type::transfer, callback_action::clear);
			owner->call(callback_type::chat, callback_action::clear);

			// Downloads
			const auto download_multimap = owner->get<download_multimap_component>();
			{
				thread_lock(download_multimap);
				download_multimap->clear();
			}

			const auto download_map = owner->get<download_map_component>();
			{
				thread_lock(download_map);
				download_map->clear();
			}

			// Uploads
			const auto upload_multimap = owner->get<upload_multimap_component>();
			{
				thread_lock(upload_multimap);
				upload_multimap->clear();
			}

			const auto upload_map = owner->get<upload_map_component>();
			{
				thread_lock(upload_map);
				upload_map->clear();
			}

			// Acceptor
			const auto acceptor = owner->get<client_acceptor_component>();
			acceptor->stop();

			// Message
			const auto message_list = get_message_list();
			for (const auto& e : *message_list)
			{
				const auto service = e->get<message_service_component>();
				service->async_quit();
			}

			unshare();

			// Thread
			const auto self = shared_from_this();

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
	}

	// Resume
	void client_service_component::async_resume(const entity::ptr data)
	{
		assert(thread_info::main());

		// Id
		if (!data->has_id())
			return;

		// Transfer routers
		const auto owner = get_entity();
		const auto transfer_list = get_transfer_list();
		
		if (transfer_list->empty())
		{
			// Status
			const auto status = owner->get<status_component>();
			status->set_status(eja::status::warning);

			// Log
			owner->log("No transfer routers are available");
			
			return;
		}
		else
		{
			bool invalid = true;
			for (const auto& e : *transfer_list)
			{
				const auto status = e->get<status_component>();
				if (!status->is_error())
				{
					invalid = false;
					break;
				}
			}

			if (invalid)
			{
				// Status
				const auto status = owner->get<status_component>();
				status->set_status(eja::status::warning);

				// Log
				owner->log("No transfer routers are available");

				return;
			}
		}

		// No dupes
		const auto file = data->get<file_component>();
		const auto network = owner->get<client_network_component>();

		// New Id (otherwise we're screwed)
		const auto id = security::get_id();		
		data->set_id(id);

		const auto download_map = owner->get<download_map_component>();
		{
			const auto pair = std::make_pair(data->get_id(), data);

			thread_lock(download_map);

			if (download_map->size() >= network->get_downloads())
			{
				const auto str = boost::str(boost::format("Unable to download \"%s\" because you're already downloading %u file(s)") % file->get_name() % network->get_downloads());
				return owner->log(str);
			}

			download_map->insert(pair);
		}

		// Status
		const auto status = data->get<status_component>();
		status->set_status(eja::status::pending);

		// Async
		const auto self = shared_from_this();
		post([self, data]() { self->resume(data); });
	}

	void client_service_component::resume(const entity::ptr data)
	{
		// Transfer
		const auto transfer = data->get<transfer_component>();
		transfer->resume();	

		// Message
		const auto message_list = get_message_list();
		for (const auto& e : *message_list)
		{
			const auto service = e->get<message_service_component>();
			service->async_file(data);
		}
	}

	// Search
	void client_service_component::async_search(const entity::ptr data)
	{
		assert(thread_info::main());		

		// Id
		if (!data->has_id())
		{
			const auto id = security::get_id();
			data->set_id(id);
		}

		const auto message_list = get_message_list();
		for (const auto& e : *message_list)
		{
			const auto service = e->get<message_service_component>();
			service->async_search(data);
		}
	}

	// Share
	void client_service_component::async_share()
	{
		// Thread-safe
		if (thread_info::main())
		{
			const auto owner = get_entity();
			m_share_list = owner->copy<share_list_component>();
		}			

		unshare();

		// Async
		const auto self = shared_from_this();
		post([self]() { self->share(); });
	}

	void client_service_component::async_share(const entity::ptr data)
	{
		// Thread-safe
		if (thread_info::main())
		{
			const auto owner = get_entity();
			
			if (!m_share_list->empty())
			{
				m_share_list = owner->copy<share_list_component>(); 				
			}
			else
			{
				unshare();

				return restart();
			}
		}

		const auto self = shared_from_this();
		post([self, data]() { self->share(data); });
	}

	void client_service_component::share()
	{
		const auto share_list = get_share_list();

		for (const auto& entity : *share_list)
			async_share(entity);
	}

	void client_service_component::share(const entity::ptr data)
	{
		const auto owner = get_entity();

		try
		{			
			client_share_command command(owner);
			command.add(data);
		}
		catch (const std::exception& ex)
		{
			owner->log(ex);
		}
	}

	// Transfer
	void client_service_component::async_transfer()
	{
		assert(thread_info::main());

		update();

		const auto transfer_list = get_transfer_list();
		for (const auto& e : *transfer_list)
		{
			const auto service = e->get<transfer_service_component>();
			if (service->valid())
				service->restart();
		}
	}

	void client_service_component::async_transfer(const entity::ptr router)
	{
		assert(thread_info::main());

		update();

		const auto service = router->get<transfer_service_component>();
		if (service->valid())
			service->restart();
	}

	// Unshare
	void client_service_component::async_unshare()
	{
		// Thread-safe
		if (thread_info::main())
		{
			const auto owner = get_entity();
			m_share_list = owner->copy<share_list_component>();
		}

		const auto self = shared_from_this();
		post([self]() { self->unshare(); });
	}

	void client_service_component::async_unshare(const entity::ptr data)
	{
		// Thread-safe
		if (thread_info::main())
		{
			const auto owner = get_entity();

			if (m_share_list->size() > 1)
			{
				m_share_list = owner->copy<share_list_component>();
			}
			else
			{
				unshare();

				return restart();
			}
		}

		const auto self = shared_from_this();
		post([self, data]() { self->unshare(data); });
	}

	void client_service_component::unshare()
	{
		// Callback
		const auto owner = get_entity();
		owner->call(callback_type::share, callback_action::clear);

		// Share
		const auto share_list = get_share_list();

		for (const auto& entity : *share_list)
		{
			if (entity->has<folder_component>())
			{
				const auto folder = entity->get<folder_component>();
				folder->clear_children();
			}
		}

		// Keywords		
		const auto share_map = owner->get<share_map_component>();
		{
			thread_lock(share_map);
			share_map->clear();
		}

		// Folders
		const auto folder_map = owner->get<folder_map_component>();
		{
			thread_lock(folder_map);
			folder_map->clear();
		}

		// Files
		const auto file_map = owner->get<file_map_component>();
		{
			thread_lock(file_map);
			file_map->clear();
		}
	}

	void client_service_component::unshare(const entity::ptr data)
	{
		const auto owner = get_entity();

		try
		{			
			client_share_command command(owner);
			command.remove(data);
		}
		catch (const std::exception& ex)
		{
			owner->log(ex);
		}
	}

	// Set
	void client_service_component::set_status()
	{	
		if (invalid())
			return;

		// Empty
		const auto owner = get_entity();
		const auto status = owner->get<status_component>();

		const auto message_list = get_message_list();
		if (message_list->empty())
		{
			owner->log("No message routers are available");

			return status->set_status(eja::status::warning);
		}

		// Message
		size_t value[static_cast<size_t>(eja::status::max)] = { 0 };
		
		for (const auto& router : *message_list)
		{
			const auto status = router->get<status_component>();
			const auto pos = static_cast<size_t>(status->get_status());
			++value[pos];
		}

		// Error
		if (value[static_cast<size_t>(eja::status::error)])
			return status->set_status((value[static_cast<size_t>(eja::status::error)] < message_list->size()) ? eja::status::warning : eja::status::error);

		// Warning
		if (value[static_cast<size_t>(eja::status::warning)])
			return status->set_status(eja::status::warning);
		
		// Pending
		if (value[static_cast<size_t>(eja::status::pending)])
			return status->set_status(eja::status::pending);

		// Info
		if (value[static_cast<size_t>(eja::status::info)])
			return status->set_status(eja::status::info);

		// Success
		if (value[static_cast<size_t>(eja::status::success)])
			return status->set_status(eja::status::success);
	}
}
