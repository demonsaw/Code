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

#include "component/browse_component.h"
#include "component/chat_component.h"
#include "component/endpoint_component.h"
#include "component/error_component.h"
#include "component/header_component.h"
#include "component/notify_component.h"
#include "component/socket_component.h"
#include "component/state_component.h"
#include "component/tunnel_component.h"
#include "component/ui_component.h"
#include "component/version_component.h"

#include "component/callback/callback_component.h"
#include "component/client/client_acceptor_component.h"
#include "component/client/client_io_component.h"
#include "component/client/client_network_component.h"
#include "component/client/client_option_component.h"
#include "component/client/client_prime_component.h"
#include "component/client/client_service_component.h"

#include "component/group/group_component.h"
#include "component/group/group_security_component.h"

#include "component/io/download_component.h"
#include "component/io/file_component.h"
#include "component/io/folder_component.h"
#include "component/io/queue_component.h"
#include "component/io/save_component.h"
#include "component/io/share_component.h"
#include "component/io/transfer_component.h"
#include "component/io/upload_component.h"

#include "component/message/message_acceptor_component.h"
#include "component/message/message_network_component.h"
#include "component/message/message_service_component.h"

#include "component/search/keyword_component.h"
#include "component/search/search_component.h"

#include "component/session/session_component.h"
#include "component/session/session_security_component.h"
#include "component/status/status_component.h"

#include "component/time/time_component.h"
#include "component/time/timer_component.h"
#include "component/time/timeout_component.h"
#include "component/transfer/transfer_service_component.h"

#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "utility/value.h"

namespace eja
{
	entity::ptr entity_factory::create_client(const entity::ptr parent /*= nullptr*/)
	{
		// Entity
		const auto entity = entity::create();

		// Generic
		entity->add<callback_component>();
		entity->add<callback_map_component>();

		// Endpoint
		const auto endpoint = endpoint_component::create();
		endpoint->set_type(endpoint_type::client);
		entity->add(endpoint);

		entity->add<error_list_component>();			// UI
		entity->add<header_map_component>();
		entity->add<notify_component>();
		entity->add<socket_set_component>();
		entity->add<state_component>();
		entity->add<status_component>();
		entity->add<ui_component>();

		// Version
		const auto version = version_component::create(version::major, version::minor, version::patch);
		entity->add(version);

		// Browse
		entity->add<browse_list_component>();			// UI
		entity->add<browse_map_component>();

		// Chat
		entity->add<chat_list_component>();				// UI

		// Client
		entity->add<client_acceptor_component>();
		entity->add<client_io_component>();
		entity->add<client_list_component>();			// UI		
		entity->add<client_map_component>();
		entity->add<client_network_component>();
		entity->add<client_option_component>();
		entity->add<client_prime_component>();
		entity->add<client_service_component>();

		// Group
		entity->add<group_component>();
		entity->add<group_security_list_component>();	// UI

		// Router
		entity->add<message_list_component>();			// UI
		entity->add<transfer_list_component>();			// UI

		// Session
		entity->add<session_map_component>();
		entity->add<session_security_component>();

		// Share
		entity->add<file_map_component>();
		entity->add<folder_map_component>();
		entity->add<save_map_component>();
		entity->add<share_list_component>();			// UI
		entity->add<share_map_component>();

		// Transfer
		entity->add<download_list_component>();			// UI
		entity->add<download_map_component>();
		entity->add<download_multimap_component>();
		entity->add<queue_list_component>();			// UI
		entity->add<upload_list_component>();			// UI
		entity->add<upload_map_component>();
		entity->add<upload_multimap_component>();

		return entity;
	}

	entity::ptr entity_factory::create_client_browse(const entity::ptr parent /*= nullptr*/)
	{
		// Entity
		const auto entity = entity::create(parent);

		entity->add<browse_component>();
		entity->add<browse_list_component>();

		return entity;

	}

	entity::ptr entity_factory::create_client_chat(const entity::ptr parent /*= nullptr*/)
	{
		// Entity
		const auto entity = entity::create(parent);

		entity->add<chat_component>();
		entity->add<time_component>();

		return entity;
	}

	entity::ptr entity_factory::create_client_client(const entity::ptr parent /*= nullptr*/)
	{
		// Entity
		const auto entity = entity::create(parent);
		
		// Endpoint
		const auto endpoint = endpoint_component::create();
		endpoint->set_type(endpoint_type::client);
		entity->add(endpoint);

		entity->add<browse_component>(); 
		entity->add<browse_list_component>();
		entity->add<chat_list_component>();

		return entity;
	}

	entity::ptr entity_factory::create_client_download(const entity::ptr parent /*= nullptr*/)
	{
		// Entity
		const auto entity = entity::create(parent);

		entity->add<download_index_array_component>();
		entity->add<file_component>();
		entity->add<transfer_component>();
		entity->add<transfer_list_component>();
		entity->add<status_component>();
		entity->add<timeout_component>();
		entity->add<timer_component>();

		return entity;
	}

	entity::ptr entity_factory::create_client_message(const entity::ptr parent /*= nullptr*/)
	{
		// Entity
		const auto entity = entity::create(parent);

		// Endpoint
		const auto endpoint = endpoint_component::create();
		endpoint->set_type(endpoint_type::message);
		entity->add(endpoint);

		// Router internal store for entities
		entity->add<browse_list_component>();
		entity->add<client_list_component>();

		// Optimization: For searching for router (pm, browse, etc.)
		entity->add<client_map_component>();

		entity->add<group_component>();
		entity->add<group_security_list_component>();
		entity->add<message_service_component>();
		entity->add<session_component>();
		entity->add<socket_set_component>();
		entity->add<state_component>();
		entity->add<status_component>();
		entity->add<tunnel_list_component>();
		entity->add<version_component>();

		// TODO: Need a socket list component (optimization)
		//
		//

		return entity;
	}

	entity::ptr entity_factory::create_client_search(const entity::ptr parent /*= nullptr*/)
	{
		// Entity
		const auto entity = entity::create(parent);

		entity->add<keyword_component>();
		entity->add<search_list_component>();
		entity->add<search_map_component>();

		return entity;
	}

	entity::ptr entity_factory::create_client_transfer(const entity::ptr parent /*= nullptr*/)
	{
		// Entity
		const auto entity = entity::create(parent);

		// Endpoint
		const auto endpoint = endpoint_component::create();
		endpoint->set_type(endpoint_type::transfer);
		entity->add(endpoint);

		entity->add<group_component>();
		entity->add<group_security_list_component>();
		entity->add<session_component>();
		entity->add<socket_set_component>();
		entity->add<state_component>();
		entity->add<status_component>();
		entity->add<transfer_service_component>();
		entity->add<version_component>();

		// TODO: Need a socket list component (optimization)
		//
		//

		return entity;
	}

	entity::ptr entity_factory::create_client_upload(const entity::ptr parent /*= nullptr*/)
	{
		// Entity
		const auto entity = entity::create(parent);

		// Shared across threads		
		entity->add<file_component>();
		entity->add<transfer_component>();
		entity->add<status_component>();
		entity->add<timeout_component>();
		entity->add<timer_component>();
		entity->add<transfer_list_component>();
		entity->add<upload_index_map_component>();
		entity->add<upload_index_set_component>();

		return entity;
	}

	// Message
	entity::ptr entity_factory::create_message(const entity::ptr parent /*= nullptr*/)
	{
		// Entity
		const auto entity = entity::create();

		// NOTE: These need to be added first!
		//
		// Callback
		entity->add<callback_component>();
		entity->add<callback_map_component>();		

		// Client
		entity->add<client_list_component>();			// UI
		entity->add<client_map_component>();

		// Endpoint
		const auto endpoint = endpoint_component::create();
		endpoint->set_type(endpoint_type::message);
		entity->add(endpoint);

		entity->add<error_list_component>();			// UI
		entity->add<header_map_component>();		
		entity->add<socket_set_component>();
		entity->add<state_component>();
		entity->add<status_component>();
		entity->add<ui_component>();

		// Version
		const auto version = version_component::create(version::major, version::minor, version::patch);
		entity->add(version);

		// Group
		entity->add<group_component>();
		entity->add<group_map_component>();
		entity->add<group_security_list_component>();	// UI
		entity->add<group_troll_map_component>();

		// Router
		entity->add<message_acceptor_component>();
		entity->add<message_list_component>();			// UI
		entity->add<message_network_component>();
		entity->add<transfer_list_component>();			// UI

		// Session
		entity->add<session_list_component>();			// UI
		entity->add<session_map_component>();

		// Transfer
		entity->add<download_list_component>();			// UI
		entity->add<download_map_component>();

		return entity;
	}

	entity::ptr entity_factory::create_message_client(const entity::ptr parent /*= nullptr*/)
	{
		// Entity
		const auto entity = entity::create(parent);

		// Endpoint
		const auto endpoint = endpoint_component::create();
		endpoint->set_type(endpoint_type::client);
		entity->add(endpoint);

		entity->add<group_component>();
		entity->add<status_component>();
		entity->add<timeout_component>();
		entity->add<tunnel_list_component>();
		entity->add<version_component>();

		// Client
		entity->add<client_network_component>();

		// Session
		entity->add<session_component>();
		entity->add<session_security_component>();

		return entity;
	}

	entity::ptr entity_factory::create_message_transfer(const entity::ptr parent /*= nullptr*/)
	{
		// Entity
		const auto entity = entity::create(parent);

		// Endpoint
		const auto endpoint = endpoint_component::create();
		endpoint->set_type(endpoint_type::transfer);
		entity->add(endpoint);

		entity->add<status_component>();

		return entity;
	}

	// Transfer
	entity::ptr entity_factory::create_transfer_download(const entity::ptr parent /*= nullptr*/)
	{
		// Entity
		const auto entity = entity::create(parent);
		
		entity->add<download_index_list_component>();
		entity->add<file_component>(); 
		entity->add<timeout_component>();
		entity->add<upload_data_list_component>();

		return entity;
	}

	// Error
	entity::ptr entity_factory::create_error(const std::string& text, const entity::ptr parent /*= nullptr*/)
	{
		// Entity
		const auto entity = entity::create(parent);

		const auto error = error_component::create(text);
		entity->add(error);

		return entity;
	}

	// File
	entity::ptr entity_factory::create_file(const entity::ptr parent /*= nullptr*/)
	{
		const auto entity = entity::create(parent);

		entity->add<file_component>();

		return entity;
	}

	// Folder
	entity::ptr entity_factory::create_folder(const entity::ptr parent /*= nullptr*/)
	{
		const auto entity = entity::create(parent);

		entity->add<folder_component>();

		return entity;
	}

	// Group
	entity::ptr entity_factory::create_group(const entity::ptr parent /*= nullptr*/)
	{
		const auto entity = entity::create(parent);

		entity->add<group_component>();
		entity->add<group_security_component>();
		entity->add<status_component>();

		return entity;
	}

	// Type
	entity::ptr entity_factory::create_statusbar(const statusbar& sb, const entity::ptr parent /*= nullptr*/)
	{
		const auto entity = entity::create(parent);

		const auto bar = statusbar_component::create(sb);
		entity->add(bar);

		return entity;
	}
}
