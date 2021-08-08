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

#include "command/clear_command.h"

#include "component/browse_component.h"
#include "component/room_component.h"
#include "component/callback/callback.h"
#include "component/client/client_component.h"
#include "component/io/file_component.h"
#include "component/io/folder_component.h"
#include "component/io/share_component.h"
#include "component/message/message_component.h"
#include "component/session/session_component.h"
#include "component/transfer/download_component.h"
#include "component/transfer/transfer_component.h"
#include "component/transfer/upload_component.h"

#include "entity/entity.h"

namespace eja
{
	// Interface
	void clear_command::execute() const
	{
		// Entity
		const auto client = m_entity->get<client_component>();
		client->set_id();

		client->set_join(false);
		client->set_troll(false);
		client->set_verified(false);

		// Callback (sync)
		m_entity->call(callback::browse | callback::clear);
		m_entity->call(callback::chat | callback::clear);
		m_entity->call(callback::client | callback::clear);
		m_entity->call(callback::download | callback::clear);
		m_entity->call(callback::error | callback::clear);
		m_entity->call(callback::finished | callback::clear);
		m_entity->call(callback::group | callback::clear);
		m_entity->call(callback::share | callback::clear);
		m_entity->call(callback::status | callback::clear);
		m_entity->call(callback::statusbar | callback::clear);
		m_entity->call(callback::upload | callback::clear);

		m_entity->async_call(callback::pm | callback::destroy);
		m_entity->async_call(callback::room | callback::destroy);
		m_entity->async_call(callback::search | callback::destroy);

		// NOTE: Session can only be destroyed here or in http_code::unauthorized
		const auto session = m_entity->get<session_component>();
		session->clear();

		// List
		const auto transfer_list = m_entity->get<transfer_list_component>();
		{
			thread_lock(transfer_list);
			transfer_list->clear();
		}

		// Map
		const auto browse_map = m_entity->get<browse_map_component>();
		{
			thread_lock(browse_map);
			browse_map->clear();
		}

		const auto client_map = m_entity->get<client_map_component>();
		{
			thread_lock(client_map);
			client_map->clear();
		}

		const auto download_map = m_entity->get<download_map_component>();
		{
			thread_lock(download_map);
			download_map->clear();
		}

		const auto entity_map = m_entity->get<entity_map_component>();
		{
			thread_lock(entity_map);
			entity_map->clear();
		}

		const auto file_map = m_entity->get<file_map_component>();
		{
			thread_lock(file_map);
			file_map->clear();
		}

		const auto folder_map = m_entity->get<folder_map_component>();
		{
			thread_lock(folder_map);
			folder_map->clear();
		}

		const auto message_map = m_entity->get<message_map_component>();
		{
			thread_lock(message_map);
			message_map->clear();
		}

		const auto room_map = m_entity->get<room_map_component>();
		{
			thread_lock(room_map);
			room_map->clear();
		}

		const auto share_map = m_entity->get<share_map_component>();
		{
			thread_lock(share_map);
			share_map->clear();
		}

		const auto transfer_map = m_entity->get<transfer_map_component>();
		{
			thread_lock(transfer_map);
			transfer_map->clear();
		}

		const auto upload_map = m_entity->get<upload_map_component>();
		{
			thread_lock(upload_map);
			upload_map->clear();
		}
	}
}
