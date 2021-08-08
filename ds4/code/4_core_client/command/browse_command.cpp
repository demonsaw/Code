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

#include "command/browse_command.h"
#include "command/client_command.h"
#include "command/group_command.h"

#include "component/browse_component.h"
#include "component/callback/callback.h"
#include "component/client/client_component.h"
#include "component/client/client_service_component.h"
#include "component/io/file_component.h"
#include "component/io/folder_component.h"
#include "component/io/share_component.h"
#include "component/message/message_component.h"
#include "component/search/search_component.h"
#include "component/search/search_option_component.h"

#include "data/request/browse_request_data.h"
#include "data/response/browse_response_data.h"
#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "entity/entity_util.h"
#include "factory/client_factory.h"
#include "utility/value.h"
#include "utility/io/folder_util.h"

namespace eja
{
	// Interface
	group_request_message::ptr browse_command::execute(const entity::ptr& entity) const
	{
		// Message
		const auto message_entity = entity_factory::create_timeout(m_entity);
		message_entity->set_data(entity);

		//const auto browse = entity->get<browse_component>();
		const auto message_id = entity_util::add<message_map_component>(m_entity, message_entity);
		//browse->set_id(message_id);

		// Data
		const auto request_data = browse_request_data::create();
		if (entity->has<folder_component>())
		{
			const auto folder = entity->get<folder_component>();
			request_data->set_folder(folder->get_id());
		}

		// Command
		group_command cmd(m_entity);
		const auto request_message = cmd.get_request_message(request_data);

		// NOTE: Find is needed to support clients & folders
		const auto client = entity->find<client_component>();
		request_message->set_id(message_id);
		request_message->set_client(client->get_id());
		request_message->set_share(true);
		request_message->set_self(true);

		return request_message;
	}

	browse_response_data::ptr browse_command::execute(const client_request_message::ptr& request_message, const browse_request_data::ptr& request_data) const
	{
		// Validate
		if (unlikely(!validate(request_message, request_data)))
			return nullptr;

		// Entity
		entity::ptr client_entity;
		const auto entity_map = m_entity->get<entity_map_component>();
		{
			thread_lock(entity_map);
			const auto it = entity_map->find(request_message->get_origin());
			if (it != entity_map->end())
				client_entity = it->second;
		}

		// Muted?
		if (likely(client_entity))
		{
			const auto client = client_entity->get<client_component>();
			if (client->is_mute())
				return nullptr;
		}

		// Response
		size_t rows = browse::min_rows;
		const auto response_data = browse_response_data::create();

		if (request_data->has_folder())
		{
			entity::ptr folder_entity;
			const auto folder_map = m_entity->get<folder_map_component>();
			{
				thread_lock(folder_map);
				const auto it = folder_map->find(request_data->get_folder());
				if (it != folder_map->end())
					folder_entity = it->second;
			}

			if (folder_entity)
			{
				// Folders
				const auto folder = folder_entity->get<folder_component>();
				const auto& folder_list = folder->get_folders();

				for (const auto& e : *folder_list)
				{
					// Folder
					const auto folder = e->get<folder_component>();
					if (folder->has_id())
					{
						const auto fdata = folder_data::create();
						fdata->set_id(folder->get_id());

						const auto folder = e->get<folder_component>();
						fdata->set_name(folder->get_name());
						fdata->set_size(folder->get_size());
						fdata->set_drive(folder->is_drive());
						fdata->set_time(folder->get_time());

						response_data->add(fdata);

						// Write
						if (response_data->get_folder_size() >= rows)
						{
							write(request_message, response_data);
							rows = std::min(rows << 1, browse::max_rows);
							response_data->clear();
						}
					}
				}

				// Files
				const auto& file_list = folder->get_files();

				for (const auto& e : *file_list)
				{
					// File
					const auto file = e->get<file_component>();
					if (file->has_id())
					{
						const auto fdata = file_data::create();
						fdata->set_id(file->get_id());

						const auto file = e->get<file_component>();
						fdata->set_name(file->get_name());
						fdata->set_size(file->get_size());
						fdata->set_time(file->get_time());

						response_data->add(fdata);

						// Write
						if (response_data->get_file_size() >= rows)
						{
							write(request_message, response_data);
							rows = std::min(rows << 1, browse::max_rows);
							response_data->clear();
						}
					}
				}
			}
		}
		else
		{
			for (const auto& e : *m_share_list)
			{
				if (e->has<folder_component>())
				{
					// Folder
					const auto folder = e->get<folder_component>();
					if (folder->has_id())
					{
						const auto fdata = folder_data::create();
						fdata->set_id(folder->get_id());

						fdata->set_name(!folder->is_drive() ? folder->get_name() : folder->get_path());
						fdata->set_size(folder->get_size());
						fdata->set_drive(folder->is_drive());
						fdata->set_time(folder->get_time());

						response_data->add(fdata);
					}
				}
				else
				{
					// File
					const auto file = e->get<file_component>();
					if (file->has_id())
					{
						const auto fdata = file_data::create();
						fdata->set_id(file->get_id());

						fdata->set_name(file->get_name());
						fdata->set_size(file->get_size());
						fdata->set_time(file->get_time());

						response_data->add(fdata);
					}
				}

				// Write
				if (response_data->get_size() >= rows)
				{
					write(request_message, response_data);
					rows = std::min(rows << 1, browse::max_rows);
					response_data->clear();
				}
			}
		}

		// Write
		return response_data->valid() ? response_data : nullptr;
	}

	void browse_command::execute(const client_response_message::ptr& response_message, const browse_response_data::ptr& response_data) const
	{
		// Validate
		if (unlikely(!validate(response_message, response_data)))
			return;

		// 1) Message
		entity::ptr message_entity;
		const auto message_map = m_entity->get<message_map_component>();
		{
			thread_lock(message_map);
			const auto it = message_map->find(response_message->get_id());
			if (it != message_map->end())
				message_entity = it->second;
		}

		if (message_entity)
		{
			// 2) Entity
			const auto& browse_entity = message_entity->get_data();
			if (!browse_entity)
				return;

			// NOTE: Different because we are search & browse folders
			const auto client_entity = !browse_entity->is_owner(m_entity) ? browse_entity->get_owner() : browse_entity;
			assert(client_entity->has<client_component>());

			// Folders
			const auto list = browse_list_component::create();
			const auto browse_list = browse_entity->get<browse_list_component>();
			const auto& folders = response_data->get_folders();

			for (const auto& fdata : folders)
			{
				if (fdata->valid())
				{
					// Entity
					const auto entity = client_factory::create_browse_folder(client_entity, browse_entity);

					// Folder
					const auto folder = entity->get<folder_component>();
					folder->set_id(fdata->get_id());

					folder->set_path(fdata->get_name());
					folder->set_size(fdata->get_size());
					folder->set_drive(fdata->is_drive());
					folder->set_time(fdata->get_time());

					// Download
					if (browse_list->is_download())
					{
						assert(browse_list->has_path());

						// Folder
						boost::filesystem::path path(browse_list->get_path());
						path /= folder->get_data();
						folder_util::create(path);

						const auto bl = entity->get<browse_list_component>();
						bl->set_download(true);
						bl->set_expanded(true);
						bl->set_path(path);

						if (browse_list->is_search())
						{
							bl->set_search(true);

							entity->add<search_component>(browse_entity);

							// Service
							const auto client_service = m_entity->get<client_service_component>();
							client_service->async_browse(entity);

							list->push_back(entity);
						}
						else
						{
							// Service
							const auto client_service = m_entity->get<client_service_component>();
							client_service->async_browse(entity);

							list->push_back(entity);
						}
					}
					else if (browse_list->is_search())
					{
						const auto bl = entity->get<browse_list_component>();
						bl->set_search(true);

						entity->add<search_component>(browse_entity);

						list->push_back(entity);
					}
					else
					{
						list->push_back(entity);
					}
				}
			}

			// Files
			const auto& files = response_data->get_files();

			for (const auto& fdata : files)
			{
				if (fdata->valid())
				{
					// TODO: browse_list_component is NOT needed for search!
					//
					//
					// Entity
					const auto entity = client_factory::create_browse_file(client_entity, browse_entity);

					// File
					const auto file = entity->get<file_component>();
					file->set_id(fdata->get_id());
					file->set_path(fdata->get_name());
					file->set_size(fdata->get_size());
					file->set_time(fdata->get_time());

					// Download
					if (browse_list->is_download())
					{
						assert(browse_list->has_path());

						// File
						const auto& path = browse_list->get_path();
						const auto download_entity = client_factory::create_download(m_entity, entity, path);

						// Callback
						m_entity->async_call(callback::download | callback::add, download_entity);
					}

					if (browse_list->is_search())
					{
						entity->add<search_component>(browse_entity);

						list->push_back(entity);
					}
					else
					{
						list->push_back(entity);
					}
				}
			}

			// Callback
			if (!list->empty())
			{
				// Size
				if (browse_entity->is_owner(m_entity))
				{
					const auto client = browse_entity->get<client_component>();

					for (const auto& e : *list)
					{
						if (e->has<folder_component>())
						{
							const auto folder = e->get<folder_component>();
							client->add_size(folder->get_size());
						}
						else //if (e->has<file_component>())
						{
							const auto file = e->get<file_component>();
							client->add_size(file->get_size());
						}
					}
				}

				const auto e = entity::create(client_entity);
				e->set_parent(browse_entity);
				e->add(list);

				// Callback
				if (browse_list->is_search())
				{
					e->add<search_component>(browse_entity);

					// NOTE: Track the message_id
					const auto browse = e->add<browse_component>();
					browse->set_id(response_message->get_id());

					m_entity->async_call(callback::search | callback::add, e);
				}
				else
				{
					// NOTE: Track the message_id
					const auto browse = e->add<browse_component>();
					browse->set_id(response_message->get_id());

					m_entity->async_call(callback::browse | callback::add, e);
				}
			}
		}
	}

	// Write
	void browse_command::write(const client_request_message::ptr& request_message, const browse_response_data::ptr& response_data) const
	{
		// Command
		client_command cmd(m_entity);

		// Message
		const auto response_message = cmd.execute(request_message, response_data);

		// Write
		const auto router_service = m_entity->get<router_service_component>();
		router_service->async_write(response_message);
	}

	// Utility
	bool browse_command::validate(const client_request_message::ptr& request_message, const browse_request_data::ptr& request_data) const
	{
		return m_share_list != nullptr;
	}

	bool browse_command::validate(const client_response_message::ptr& response_message, const browse_response_data::ptr& response_data) const
	{
		return response_data->valid();
	}
}
