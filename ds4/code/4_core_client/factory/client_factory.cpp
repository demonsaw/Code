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

#include <boost/format.hpp>

#include "component/browse_component.h"
#include "component/chat_component.h"
#include "component/error_component.h"
#include "component/room_component.h"
#include "component/version_component.h"
#include "component/callback/callback_service_component.h"
#include "component/client/client_component.h"
#include "component/client/client_service_component.h"
#include "component/group/group_component.h"
#include "component/group/group_option_component.h"
#include "component/io/file_component.h"
#include "component/io/folder_component.h"
#include "component/io/share_component.h"
#include "component/message/message_component.h"
#include "component/router/router_component.h"
#include "component/router/router_service_component.h"
#include "component/search/search_component.h"
#include "component/search/search_option_component.h"
#include "component/service/socket_component.h"
#include "component/service/strand_component.h"
#include "component/session/session_component.h"
#include "component/session/session_option_component.h"
#include "component/status/status_component.h"
#include "component/time/time_component.h"
#include "component/time/timer_component.h"
#include "component/time/timeout_component.h"
#include "component/transfer/chunk_component.h"
#include "component/transfer/download_component.h"
#include "component/transfer/download_service_component.h"
#include "component/transfer/finished_component.h"
#include "component/transfer/transfer_component.h"
#include "component/transfer/upload_component.h"
#include "component/transfer/upload_service_component.h"
#include "component/verified/verified_option_component.h"

#include "entity/entity.h"
#include "factory/client_factory.h"
#include "utility/value.h"

namespace eja
{
	entity::ptr client_factory::create_client(const entity::ptr& owner /*= nullptr*/)
	{
		// Entity
		const auto e = entity::create(owner);

		// Components
		e->add<browse_list_component>();
		e->add<chat_list_component>();
		e->add<client_component>();

		if (!owner)
		{
			// Components
			e->add<callback_service_component>();
			e->add<client_service_component>();
			e->add<download_service_component>();
			e->add<upload_service_component>();

			e->add<group_component>();
			e->add<group_list_component>();

			e->add<router_component>();
			e->add<router_list_component>();
			e->add<router_service_component>();

			e->add<session_component>();
			e->add<session_option_component>();
			e->add<status_component>();

			e->add<verified_option_component>();

			const auto message_ring = message_ring_component::create();
			message_ring->set_capacity(client::num_ring);
			e->add(message_ring);

			// Data
			e->add<browse_map_component>();
			e->add<client_map_component>();
			e->add<download_map_component>();
			e->add<entity_map_component>();
			e->add<file_map_component>();
			e->add<folder_map_component>();
			e->add<message_map_component>();
			e->add<room_map_component>();
			e->add<share_map_component>();
			e->add<socket_set_component>();
			e->add<transfer_map_component>();
			e->add<upload_map_component>();

			// UI
			e->add<client_list_component>();
			e->add<download_list_component>();
			e->add<error_list_component>();
			e->add<finished_list_component>();
			e->add<finished_map_component>();
			e->add<share_list_component>();
			e->add<transfer_list_component>();
			e->add<upload_list_component>();
		}

		return e;
	}

	entity::ptr client_factory::create_browse_file(const entity::ptr& owner, const entity::ptr& parent)
	{
		// Entity
		const auto e = entity::create(owner);
		e->set_parent(parent);

		// Components
		//e->add<browse_component>();
		e->add<file_component>();

		return e;
	}

	entity::ptr client_factory::create_browse_folder(const entity::ptr& owner, const entity::ptr& parent)
	{
		// Entity
		const auto e = entity::create(owner);
		e->set_parent(parent);

		// Components
		//e->add<browse_component>();
		e->add<browse_list_component>();
		e->add<folder_component>();

		return e;
	}

	entity::ptr client_factory::create_chat(const entity::ptr& owner)
	{
		// Entity
		const auto e = entity::create(owner);

		// Components
		e->add<chat_component>();
		e->add<time_component>();

		return e;
	}

	entity::ptr client_factory::create_chat(const entity::ptr& owner, std::string&& text)
	{
		// Entity
		const auto e = entity::create(owner);

		// Components
		e->add<chat_component>(std::move(text));
		e->add<time_component>();

		return e;
	}

	entity::ptr client_factory::create_chat(const entity::ptr& owner, const std::string& text)
	{
		// Entity
		const auto e = entity::create(owner);

		// Components
		e->add<chat_component>(text);
		e->add<time_component>();

		return e;
	}

	entity::ptr client_factory::create_download(const entity::ptr& owner, const entity::ptr& file_entity)
	{
		const auto client = owner->get<client_component>();
		return create_download(owner, file_entity, client->get_path());
	}

	entity::ptr client_factory::create_download(const entity::ptr& owner, const entity::ptr& file_entity, const boost::filesystem::path& parent)
	{
		// Entity
		const auto e = entity::create(owner);

		// Components
		e->add<chunk_set_component>();
		e->add<status_component>();
		e->add<timeout_component>();
		e->add<timer_component>();
		e->add<transfer_list_component>();
		e->add<transfer_map_component>();

		// Transfer
		const auto client = owner->get<client_component>();
		const auto transfer = e->add<transfer_component>();
		transfer->set_chunk_size(client->get_chunk_size());
		transfer->set_buffer_size(client->get_buffer_size());

		// File
		const auto file = e->add<file_component>(file_entity);
		boost::filesystem::path path(parent);

#if _WSTRING
		path /= file->get_wname();
#else
		path /= file->get_name();
#endif
		file->set_path(path);

		// Chunks
		const auto chunks = (file->get_size() / transfer->get_chunk_size()) + ((file->get_size() % transfer->get_chunk_size()) ? 1 : 0);
		const auto chunk_list = e->add<chunk_list_component>();
		for (auto i = 0; i < chunks; ++i)
			chunk_list->push_back(i);

		return e;
	}

	entity::ptr client_factory::create_download_router(const entity::ptr& owner, const entity::ptr& router_entity)
	{
		// Entity
		const auto e = entity::create(owner);

		// Components
		e->add<chunk_list_component>();
		e->add<router_component>(router_entity);
		e->add<session_component>();
		e->add<session_option_component>();
		//e->add<timeout_component>();

		return e;
	}

	entity::ptr client_factory::create_file(const entity::ptr& owner)
	{
		// Entity
		const auto e = entity::create(owner);

		// Components
		e->add<file_component>();

		return e;
	}

	entity::ptr client_factory::create_folder(const entity::ptr& owner)
	{
		// Entity
		const auto e = entity::create(owner);

		// Components
		e->add<folder_component>();

		return e;
	}

	entity::ptr client_factory::create_group(const entity::ptr& owner)
	{
		// Entity
		const auto e = entity::create(owner);

		// Components
		e->add<group_component>();
		e->add<group_list_component>();

		return e;
	}

	entity::ptr client_factory::create_group_option(const entity::ptr& owner)
	{
		// Entity
		const auto e = entity::create(owner);

		// Components
		e->add<group_option_component>();
		e->add<status_component>();

		return e;
	}

	entity::ptr client_factory::create_motd(const entity::ptr& owner)
	{
		// Entity
		const auto e = entity::create(owner);

		// Components
		e->add<router_component>(owner);
		e->add<chat_component>();
		e->add<time_component>();

		return e;
	}

	entity::ptr client_factory::create_room(const entity::ptr& owner)
	{
		// Entity
		const auto e = entity::create(owner);

		// Components
		e->add<room_component>();
		e->add<chat_list_component>();
		e->add<client_list_component>();

		return e;
	}

	entity::ptr client_factory::create_router(const entity::ptr& owner)
	{
		// Entity
		const auto e = entity::create(owner);

		// Components
		e->add<router_component>();

		return e;
	}

	entity::ptr client_factory::create_search(const entity::ptr& owner, const std::string& text /*= ""*/)
	{
		// Entity
		const auto e = entity::create(owner);

		// Components
		e->add<browse_list_component>();
		e->add<browse_map_component>();
		e->add<search_component>();
		e->add<search_option_component>(text);

		return e;
	}

	entity::ptr client_factory::create_search_file(const entity::ptr& owner)
	{
		// Entity
		const auto e = entity::create(owner);

		// Components
		e->add<file_component>();
		e->add<search_component>();

		return e;
	}

	entity::ptr client_factory::create_search_folder(const entity::ptr& owner)
	{
		// Entity
		const auto e = entity::create(owner);

		// Components
		e->add<folder_component>();
		e->add<search_component>();
		e->add<browse_list_component>();

		return e;
	}

	entity::ptr client_factory::create_upload(const entity::ptr& owner, const entity::ptr& file_entity)
	{
		// Entity
		const auto e = entity::create(owner);

		// Components
		e->add<chunk_component>();
		e->add<file_component>(file_entity);
		e->add<router_component>();
		e->add<session_component>();
		e->add<session_option_component>();
		e->add<status_component>();
		e->add<timeout_component>();
		e->add<timer_component>();

		// Transfer
		const auto client = owner->get<client_component>();
		const auto transfer = e->add<transfer_component>();
		transfer->set_chunk_size(client->get_chunk_size());

		return e;
	}
}
