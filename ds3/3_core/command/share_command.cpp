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

#include "command/share_command.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/client/client_io_component.h"
#include "component/client/client_service_component.h"
#include "component/io/file_component.h"
#include "component/io/folder_component.h"
#include "component/io/share_component.h"
#include "entity/entity.h"
#include "security/hash/md.h"
#include "utility/value.h"
#include "utility/io/file_util.h"
#include "utility/io/folder_util.h"

namespace eja
{
	// Client
	void client_share_command::add(const entity::ptr entity)
	{
		const auto hash = get_hash();

		if (entity->has<folder_component>())
		{
			// Folder
			std::pair<folder_map_component::iterator, bool> result;
			const auto folder_map = m_entity->get<folder_map_component>();
			const auto folder = entity->get<folder_component>();
			{
				const auto id = folder_util::get_id(folder, hash);
				const auto pair = std::make_pair(id, entity);
				folder->set_id(id);

				thread_lock(folder_map);
				result = folder_map->insert(pair);
			}

			// Sub-Folders
			const auto service = m_entity->get<client_service_component>();
			const auto folders = folder->get_folders();
			for (const auto& e : folders)
				service->async_share(e);

			// Sub-Files
			const auto files = folder->get_files();
			for (const auto& e : files)
				service->async_share(e);

			// Keywords
			if (result.second)
			{
				const auto name = boost::algorithm::to_lower_copy(folder->get_name());
				add(name, entity);
			}
		}
		else
		{
			// File
			std::pair<file_map_component::iterator, bool> result;
			const auto file_map = m_entity->get<file_map_component>();
			const auto file = entity->get<file_component>();
			{
				const auto id = file_util::get_id(file, hash);				
				const auto pair = std::make_pair(id, entity);
				file->set_id(id);

				thread_lock(file_map);				
				result = file_map->insert(pair);
			}

			// Folders
			auto parent = entity->get_parent();
			while (parent && parent->has<folder_component>())
			{
				const auto folder = parent->get<folder_component>();
				folder->add_size(file->get_size());

				// Callback
				m_entity->call(callback_type::share, callback_action::update, parent);

				parent = parent->get_parent();
			}

			// Keywords
			if (result.second)
			{
				const auto name = boost::algorithm::to_lower_copy(file->get_name());
				add(name, entity);
			}			
		}
	}

	void client_share_command::remove(const entity::ptr entity)
	{
		const auto hash = get_hash();

		if (entity->has<folder_component>())
		{
			// Folder
			const auto folder_map = m_entity->get<folder_map_component>();
			const auto folder = entity->get<folder_component>();
			const auto id = folder_util::get_id(folder, hash);
			{
				thread_lock(folder_map);
				folder_map->erase(id);
			}

			// Keywords
			const auto name = boost::algorithm::to_lower_copy(folder->get_name());
			remove(name, entity);

			// Sub-Folders
			const auto service = m_entity->get<client_service_component>();
			const auto folders = folder->get_folders();
			for (const auto& e : folders)
				service->async_unshare(e);

			// Sub-Files
			const auto files = folder->get_files();
			for (const auto& e : files)
				service->async_unshare(e);
		}
		else
		{
			// File
			const auto file_map = m_entity->get<file_map_component>();
			const auto file = entity->get<file_component>();
			const auto id = file_util::get_id(file, hash);
			{
				thread_lock(file_map);
				file_map->erase(id);
			}

			// Keywords
			const auto name = boost::algorithm::to_lower_copy(file->get_name());
			remove(name, entity);
		}
	}

	// Utility
	void client_share_command::add(const std::string& name, const entity::ptr entity)
	{
		const auto owner = get_entity();
		if (!owner)
			return;

		const auto share_map = owner->get<share_map_component>();
		string_tokenizer tokens(name, boost::char_separator<char>(search::delimiters));

		for (const std::string& token : tokens)
		{
			if (token.empty())
				continue;

			const auto pair = std::make_pair(token, entity);

			thread_lock(share_map);
			share_map->insert(pair);
		}
	}

	void client_share_command::remove(const std::string& name, const entity::ptr entity)
	{
		const auto owner = get_entity();
		if (!owner)
			return;

		const auto share_map = owner->get<share_map_component>();
		string_tokenizer tokens(name, boost::char_separator<char>(search::delimiters));

		for (const std::string& token : tokens)
		{
			if (token.empty())
				continue;

			thread_lock(share_map);
			auto range = share_map->equal_range(token);

			for (auto it = range.first; it != range.second; )
			{
				if (entity == it->second)
					share_map->erase(it++);
				else
					++it;
			}
		}
	}

	// Get
	hash::ptr client_share_command::get_hash() const
	{
		const auto io = m_entity->get<client_io_component>();
		const auto hash = hash::create(io->get_hash());
		hash->set_salt(io->get_salt());

		return hash;
	}
}
