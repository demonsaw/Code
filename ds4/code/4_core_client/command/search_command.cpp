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
#include <unordered_map>
#include <boost/algorithm/string.hpp>

#include "command/client_command.h"
#include "command/search_command.h"
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

#include "data/request/search_request_data.h"
#include "data/response/search_response_data.h"
#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "entity/entity_util.h"
#include "factory/client_factory.h"
#include "utility/value.h"
#include "utility/io/folder_util.h"

namespace eja
{
	// Interface
	group_request_message::ptr search_command::execute(const entity::ptr& entity) const
	{
		// Message
		const auto message_entity = entity_factory::create_timeout(m_entity);
		message_entity->set_data(entity);

		const auto search = entity->get<search_component>();
		const auto message_id = entity_util::add<message_map_component>(m_entity, message_entity);
		search->set_id(message_id);

		// Callback
		m_entity->async_call(callback::search | callback::create, entity);

		// Data
		const auto search_option = entity->get<search_option_component>();
		const auto text = boost::to_lower_copy(search_option->get_text());
		const auto request_data = search_request_data::create(text);

		// Command
		group_command cmd(m_entity);
		const auto request_message = cmd.get_request_message(request_data);
		request_message->set_id(search->get_id());
		request_message->set_share(true);

		return request_message;
	}

	search_response_data::ptr search_command::execute(const client_request_message::ptr& request_message, const search_request_data::ptr& request_data) const
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
		size_t rows = search::min_rows;
		const auto response_data = search_response_data::create();

		std::unordered_map<std::string, file_data::ptr> file_map;
		std::unordered_map<std::string, folder_data::ptr> folder_map;
		const auto share_map = m_entity->get<share_map_component>();

		const auto quote = "\"";
		const auto& text = request_data->get_text();
		string_tokenizer tokens(text, boost::char_separator<char>(search::delimiters));

		if (boost::starts_with(text, quote))
		{
			// Folders
			bool init = true;
			std::unordered_map<std::string, folder_data::ptr> folder_temp_map;

			for (const std::string& token : tokens)
			{
				assert(!token.empty());
				if (token.size() < search::min_text)
					continue;

				thread_lock(share_map);

				const auto range = share_map->equal_range(token);

				for (auto it = range.first; it != range.second; ++it)
				{
					const auto& e = it->second;

					if (e->has<folder_component>())
					{
						const auto folder = e->get<folder_component>();

						if (init)
						{
							auto& fdata = folder_temp_map[folder->get_id()];

							if (!fdata)
							{
								fdata = folder_data::create();
								fdata->set_id(folder->get_id());

								fdata->set_name(!folder->is_drive() ? folder->get_name() : folder->get_path());
								fdata->set_size(folder->get_size());
								fdata->set_drive(folder->is_drive());
								fdata->set_time(folder->get_time());
								fdata->set_weight(1);
							}
							else
							{
								fdata->add_weight();
							}
						}
						else
						{
							const auto it = folder_map.find(folder->get_id());
							if (it != folder_map.end())
							{
								const auto& fdata = it->second;
								fdata->add_weight();

								const auto pair = std::make_pair(folder->get_id(), fdata);
								folder_temp_map.insert(pair);
							}
						}
					}
				}

				init = false;
				folder_map.clear();
				folder_map.swap(folder_temp_map);

				// Abort?
				if (folder_map.empty())
					break;
			}

			// Write
			for (const auto& pair : folder_map)
			{
				const auto& fdata = pair.second;
				response_data->add(fdata);

				if (response_data->get_folder_size() >= rows)
				{
					write(request_message, response_data);
					rows = std::min(rows << 1, search::max_rows);
					response_data->clear();
				}
			}

			// Files
			init = true;
			std::unordered_map<std::string, file_data::ptr> file_temp_map;

			for (const std::string& token : tokens)
			{
				assert(!token.empty());
				if (token.size() < search::min_text)
					continue;

				thread_lock(share_map);

				const auto range = share_map->equal_range(token);

				for (auto it = range.first; it != range.second; ++it)
				{
					const auto& e = it->second;

					if (e->has<file_component>())
					{
						const auto file = e->get<file_component>();

						if (init)
						{
							auto& fdata = file_temp_map[file->get_id()];

							if (!fdata)
							{
								fdata = file_data::create();
								fdata->set_id(file->get_id());

								fdata->set_name(file->get_name());
								fdata->set_size(file->get_size());
								fdata->set_time(file->get_time());
								fdata->set_weight(1);
							}
							else
							{
								fdata->add_weight();
							}
						}
						else
						{
							const auto it = file_map.find(file->get_id());
							if (it != file_map.end())
							{
								const auto& fdata = it->second;
								fdata->add_weight();

								const auto pair = std::make_pair(file->get_id(), fdata);
								file_temp_map.insert(pair);
							}
						}
					}
				}

				init = false;
				file_map.clear();
				file_map.swap(file_temp_map);

				// Abort?
				if (file_map.empty())
					break;
			}

			// Write
			for (const auto& pair : file_map)
			{
				const auto& fdata = pair.second;
				response_data->add(fdata);

				if (response_data->get_file_size() >= rows)
				{
					write(request_message, response_data);
					rows = std::min(rows << 1, search::max_rows);
					response_data->clear();
				}
			}
		}
		else
		{
			// Folders
			for (const std::string& token : tokens)
			{
				assert(!token.empty());
				if (token.size() < search::min_text)
					continue;

				thread_lock(share_map);

				const auto range = share_map->equal_range(token);

				for (auto it = range.first; it != range.second; ++it)
				{
					const auto& e = it->second;

					if (e->has<folder_component>())
					{
						const auto folder = e->get<folder_component>();
						auto& fdata = folder_map[folder->get_id()];

						if (!fdata)
						{
							fdata = folder_data::create();
							fdata->set_id(folder->get_id());

							fdata->set_name(!folder->is_drive() ? folder->get_name() : folder->get_path());
							fdata->set_size(folder->get_size());
							fdata->set_drive(folder->is_drive());
							fdata->set_time(folder->get_time());
							fdata->set_weight(1);

							response_data->add(fdata);

							// Write
							if (response_data->get_folder_size() >= rows)
							{
								write(request_message, response_data);
								rows = std::min(rows << 1, search::max_rows);
								response_data->clear();
							}
						}
						else
						{
							fdata->add_weight();
						}
					}
				}
			}

			// Files
			for (const std::string& token : tokens)
			{
				assert(!token.empty());
				if (token.size() < search::min_text)
					continue;

				thread_lock(share_map);

				const auto range = share_map->equal_range(token);

				for (auto it = range.first; it != range.second; ++it)
				{
					const auto& e = it->second;

					if (e->has<file_component>())
					{
						const auto file = e->get<file_component>();
						auto& fdata = file_map[file->get_id()];

						if (!fdata)
						{
							fdata = file_data::create();
							fdata->set_id(file->get_id());

							fdata->set_name(file->get_name());
							fdata->set_size(file->get_size());
							fdata->set_time(file->get_time());
							fdata->set_weight(1);

							response_data->add(fdata);

							// Write
							if (response_data->get_file_size() >= rows)
							{
								write(request_message, response_data);
								rows = std::min(rows << 1, search::max_rows);
								response_data->clear();
							}
						}
						else
						{
							fdata->add_weight();
						}
					}
				}
			}
		}

		// Write
		return response_data->valid() ? response_data : nullptr;
	}

	void search_command::execute(const client_response_message::ptr& response_message, const search_response_data::ptr& response_data) const
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
			const auto& search_entity = message_entity->get_data();
			if (!search_entity)
				return;

			// 3) Client
			entity::ptr client_entity;
			const auto client_map = m_entity->get<client_map_component>();
			{
				thread_lock(client_map);
				const auto it = client_map->find(response_message->get_origin());
				if (it != client_map->end())
					client_entity = it->second;
			}

			if (client_entity)
			{
				// Client
				assert(client_entity->has<client_component>());

				// Folders
				const auto list = browse_list_component::create();
				const auto& folders = response_data->get_folders();

				for (const auto& fdata : folders)
				{
					if (fdata->invalid())
						continue;

					const auto entity = client_factory::create_search_folder(client_entity);
					const auto search = entity->get<search_component>();
					search->set_id(response_message->get_id());
					search->set_weight(fdata->get_weight());

					const auto folder = entity->get<folder_component>();
					folder->set_id(fdata->get_id());

					folder->set_path(fdata->get_name());
					folder->set_size(fdata->get_size());
					folder->set_drive(fdata->is_drive());
					folder->set_time(fdata->get_time());

					list->push_back(entity);
				}

				// Files
				const auto& files = response_data->get_files();

				for (const auto& fdata : files)
				{
					if (fdata->invalid())
						continue;

					const auto entity = client_factory::create_search_file(client_entity);
					const auto search = entity->get<search_component>();
					//search->set_id(response_message->get_id());
					search->set_weight(fdata->get_weight());

					const auto file = entity->get<file_component>();
					file->set_id(fdata->get_id());
					file->set_path(fdata->get_name());
					file->set_size(fdata->get_size());
					file->set_time(fdata->get_time());

					list->push_back(entity);
				}

				// Callback
				if (!list->empty())
				{
					const auto e = entity::create(client_entity);
					e->add(list);

					const auto search = e->add<search_component>();
					search->set_id(response_message->get_id());

					m_entity->async_call(callback::search | callback::add, e);
				}
			}
		}
	}

	// Write
	void search_command::write(const client_request_message::ptr& request_message, const search_response_data::ptr& response_data) const
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
	bool search_command::validate(const client_request_message::ptr& request_message, const search_request_data::ptr& request_data) const
	{
		return request_data->valid();
	}

	bool search_command::validate(const client_response_message::ptr& response_message, const search_response_data::ptr& response_data) const
	{
		return response_data->valid();
	}
}
