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

#include <boost/algorithm/string.hpp>

#include "command/share_command.h"

#include "component/callback/callback.h"
#include "component/callback/callback_service_component.h"
#include "component/client/client_component.h"
#include "component/client/client_service_component.h"
#include "component/io/file_component.h"
#include "component/io/folder_component.h"
#include "component/io/share_component.h"

#include "entity/entity.h"
#include "security/hash/hash.h"
#include "utility/value.h"
#include "utility/io/file_util.h"
#include "utility/io/folder_util.h"

namespace eja
{
	// Add
	void share_command::add(const entity::ptr& entity) const
	{
		if (entity->has<file_component>())
		{
			add_file(entity);

			// Size
			const auto file = entity->get<file_component>();
			file->set_size();
		}
		else
		{
			add_folder(entity);
		}
	}

	void share_command::add_file(const entity::ptr& entity) const
	{
		try
		{
			// Id			
			const auto hash = get_hash();
			const auto file = entity->get<file_component>();
			const auto id = file_util::get_id(file, hash);
			file->set_id(id);

			// Keywords
			std::pair<file_map_component::iterator, bool> result;
			const auto file_map = m_entity->get<file_map_component>();
			{				
				const auto pair = std::make_pair(id, entity);

				thread_lock(file_map);
				result = file_map->insert(pair);
			}
			
			if (result.second)
			{
				const auto name = boost::algorithm::to_lower_copy(file->get_name());
				add(name, entity);
			}
		}
		catch (const std::exception& ex)
		{
			m_entity->log(ex);
		}		
	}

	void share_command::add_folder(const entity::ptr& entity) const
	{
		// Id
		const auto hash = get_hash();
		const auto folder = entity->get<folder_component>();
		const auto id = folder_util::get_id(folder, hash);
		folder->set_id(id);
		
		// Files
		size_t file_size = 0;		
		const auto files = folder->get_files();		
				
		for (const auto& e : files)
		{
			const auto file = e->get<file_component>();
			file_size += file->get_size();
			add_file(e);
		}

		folder->add_size(file_size);

		// Size
		entity::ptr parent;
		auto e = entity->get_parent();
		while (e && e->has<folder_component>())
		{
			parent = e;
			const auto folder = e->get<folder_component>();
			folder->add_size(file_size);
			e = e->get_parent();
		}

		// Callback
		m_entity->async_call(callback::share | callback::update, parent);

		// Folders
		const auto client_service = m_entity->get<client_service_component>();
		const auto folders = folder->get_folders();
		for (const auto& e : folders)
			client_service->async_share(e);

		// Keywords
		std::pair<file_map_component::iterator, bool> result;
		const auto folder_map = m_entity->get<folder_map_component>();
		{
			const auto pair = std::make_pair(id, entity);			

			thread_lock(folder_map);
			result = folder_map->insert(pair);
		}

		if (result.second)
		{
			const auto name = boost::algorithm::to_lower_copy(folder->get_name());
			add(name, entity);
		}		
	}

	void share_command::add(const std::string& name, const entity::ptr& entity) const
	{		
		const auto owner = get_entity();
		const auto share_map = owner->get<share_map_component>();
		
		// Unique tokens
		string_tokenizer tokens(name, boost::char_separator<char>(search::delimiters));
		std::set<std::string> set(tokens.begin(), tokens.end());
		
		for (const auto& token : set)
		{
			if (token.size() >= share::min_text)
			{
				const auto pair = std::make_pair(token, entity);

				thread_lock(share_map);
				share_map->insert(pair);
			}
		}
	}

	// Remove
	void share_command::remove(const entity::ptr& entity) const
	{
		if (entity->has<file_component>())
		{
			// File
			const auto file = entity->get<file_component>();
			const auto file_map = m_entity->get<file_map_component>();			
			{				
				thread_lock(file_map);
				file_map->erase(file->get_id());
			}

			// Keywords
			const auto name = boost::algorithm::to_lower_copy(file->get_name());
			remove(name, entity);
		}
		else
		{
			// Folder
			const auto folder = entity->get<folder_component>(); 
			const auto folder_map = m_entity->get<folder_map_component>();
			{
				thread_lock(folder_map);
				folder_map->erase(folder->get_id());
			}

			// Keywords
			const auto name = boost::algorithm::to_lower_copy(folder->get_name());
			remove(name, entity);

			// Sub-Folders
			const auto client_service = m_entity->get<client_service_component>();			
			const auto folders = folder->get_folders();
			for (const auto& e : folders)
				client_service->async_unshare(e);

			// Sub-Files
			const auto files = folder->get_files();
			for (const auto& e : files)
				client_service->async_unshare(e);
		}
	}

	void share_command::remove(const std::string& name, const entity::ptr& entity) const
	{
		const auto owner = get_entity();
		const auto share_map = owner->get<share_map_component>();
		string_tokenizer tokens(name, boost::char_separator<char>(search::delimiters));

		for (const std::string& token : tokens)
		{
			if (!token.empty())
			{
				std::pair<share_map_component::iterator, share_map_component::iterator> range;

				thread_lock(share_map);
				range = share_map->equal_range(token);

				for (auto it = range.first; it != range.second; )
				{
					if (entity == it->second)
						share_map->erase(it++);
					else
						++it;
				}
			}
		}
	}

	// Get
	hash::ptr share_command::get_hash() const
	{
		const auto client = m_entity->get<client_component>();
		const auto hash = hash::create(client->get_hash());
		hash->set_salt(client->get_salt());

		return hash;
	}
}
