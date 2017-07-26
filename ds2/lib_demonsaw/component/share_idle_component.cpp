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

#include "share_idle_component.h"

#include "component/client/client_security_component.h"
#include "utility/io/file_util.h"
#include "utility/io/folder_util.h"

namespace eja
{
	// Interface
	void share_idle_component::init()
	{
		idle_component::init();

		start();
	}

	void share_idle_component::shutdown()
	{
		idle_component::shutdown();

		m_service->stop();

		if (has_entity())
			clear();
	}

	void share_idle_component::update()
	{
		idle_component::update();

		try
		{
			m_service->stop();

			const auto owner = get_entity();
			const auto folder_map = owner->get<folder_map_component>();
			folder_map->clear();

			const auto file_map = owner->get<file_map_component>();
			file_map->clear();

			// Folders
			const auto hash = get_hash();
			const auto folders = get_folders();
			const auto self = shared_from_this();
			auto_lock_ref(folders);

			for (const auto& folder : folders)
			{
				folder->set_size(0);
				folder_map->add(folder->get_id(), folder);
								
				m_service->post([self, owner, folder]()
				{
					const auto share = std::static_pointer_cast<share_idle_component>(self);
					share->add(folder, false);

					// Update
					self->call(function_type::share, function_action::refresh, owner);
				});
			}

			// Files
			const auto files = get_files();
			auto_lock_ref(files);

			for (const auto& file : files)
			{
				file->set_size(0);
				file_map->add(file->get_id(), file);

				add(file, false);
			}
		}
		catch (std::exception& e)
		{
			log(e);
		}
		catch (...)
		{
			log();
		}
	}

	// Callback
	bool share_idle_component::on_run()
	{
		m_service->reset();
		m_service->run();

		return idle_component::on_run();
	}

	// Utility
	void share_idle_component::clear()
	{		
		const auto owner = get_entity();
		const auto folder_map = owner->get<folder_map_component>();
		folder_map->clear();

		const auto file_map = owner->get<file_map_component>();
		file_map->clear();
	}

	// Path
	void share_idle_component::add(const std::string& path, const hash::ptr hash)
	{
		if (!boost::filesystem::exists(path))
			return;

		try
		{
			if (boost::filesystem::is_directory(path))
			{
				// Folder
				const auto folder = folder_component::create(path);
				const auto id = folder_util::get_id(folder, hash);
				folder->set_id(id);

				const auto self = shared_from_this();
				m_service->post([self, folder]()
				{ 
					const auto share = std::static_pointer_cast<share_idle_component>(self);
					share->add(folder, false);
				});

				m_folders.add(folder);
			}
			else
			{
				// File
				const auto file = file_component::create(path);
				const auto id = file_util::get_id(file, hash);
				file->set_id(id);

				const auto owner = get_entity();
				const auto file_map = owner->get<file_map_component>();
				file_map->add(id, file);

				m_files.add(file);
			}			
		}
		catch (std::exception& e)
		{
			log(e);
		}
		catch (...)
		{
			log();
		}
	}

	void share_idle_component::add(const folder::ptr folder, const hash::ptr hash, const bool share /*= true*/)
	{
		try
		{
			if (share)
				m_folders.add(folder);		
			
			// Folders
			const auto owner = get_entity();
			const auto folder_map = owner->get<folder_map_component>();
			folder_map->add(folder->get_id(), folder);

			const auto folders = folder->get_folders(true);
			auto_lock_ref(folders);

			for (const auto& item : folders)
			{
				try
				{
					const auto id = folder_util::get_id(item, hash);
					item->set_id(id);

					const auto self = shared_from_this();
					m_service->post([self, folder, item]()
					{
						const auto share = std::static_pointer_cast<share_idle_component>(self);
						share->add(item, false);
					});
				}
				catch (std::exception& e)
				{
					log(e);
				}
				catch (...)
				{
					log();
				}
			}

			// Files
			const auto file_map = owner->get<file_map_component>();
			const auto files = folder->get_files(true);
			auto_lock_ref(files);

			for (const auto& item : files)
			{
				try
				{
					const auto id = file_util::get_id(item, hash);
					item->set_id(id);

					file_map->add(id, item);
				}
				catch (std::exception& e)
				{
					log(e);
				}
				catch (...)
				{
					log();
				}
			}

			if (!folders.empty() || !files.empty())
				folder->set_size(1);
		}
		catch (std::exception& e)
		{
			log(e);
		}
		catch (...)
		{
			log();
		}
	}

	void share_idle_component::add(const file::ptr file, const hash::ptr hash, const bool share /*= true*/)
	{
		try
		{
			if (share)
				m_files.add(file);

			const auto id = file_util::get_id(file, hash);
			file->set_id(id);

			const auto owner = get_entity();
			const auto file_map = owner->get<file_map_component>();
			file_map->add(id, file);
		}
		catch (std::exception& e)
		{
			log(e);
		}
		catch (...)
		{
			log();
		}
	}

	void share_idle_component::remove(const std::string& path, const hash::ptr hash)
	{
		if (!boost::filesystem::exists(path))
			return;

		try
		{
			if (boost::filesystem::is_directory(path))
			{
				const auto folder = folder_component::create(path);
				const auto id = folder_util::get_id(folder, hash);
				folder->set_id(id);

				const auto self = shared_from_this();
				m_service->post([self, folder]()
				{
					const auto share = std::static_pointer_cast<share_idle_component>(self);
					share->remove(folder);
				});
			}
			else
			{				
				const auto file = file_component::create(path);
				const auto id = file_util::get_id(file, hash);

				const auto owner = get_entity();
				const auto file_map = owner->get<file_map_component>();
				file_map->remove(id);
			}
		}
		catch (std::exception& e)
		{
			log(e);
		}
		catch (...)
		{
			log();
		}
	}

	void share_idle_component::remove(const folder::ptr folder, const hash::ptr hash, const bool share /*= true*/)
	{
		try
		{
			if (share)
				m_folders.remove(folder);

			// Folders
			const auto owner = get_entity();
			const auto folder_map = owner->get<folder_map_component>();
			folder_map->remove(folder->get_id());

			const auto folders = folder->get_folders();
			auto_lock_ref(folders);

			for (const auto& item : folders)
			{
				try
				{
					const auto& id = item->get_id();
					folder_map->remove(id);
					
					const auto self = shared_from_this();
					m_service->post([self, item]()
					{
						const auto share = std::static_pointer_cast<share_idle_component>(self);
						share->remove(item, false);
					});
				}
				catch (std::exception& e)
				{
					log(e);
				}
				catch (...)
				{
					log();
				}
			}

			// Files
			const auto file_map = owner->get<file_map_component>();
			const auto files = folder->get_files();
			auto_lock_ref(files);

			for (const auto& item : files)
			{
				try
				{
					const auto& id = item->get_id();
					file_map->remove(id);
				}
				catch (std::exception& e)
				{
					log(e);
				}
				catch (...)
				{
					log();
				}
			}
		}
		catch (std::exception& e)
		{
			log(e);
		}
		catch (...)
		{
			log();
		}
	}

	void share_idle_component::remove(const file::ptr file, const hash::ptr hash, const bool share /*= true*/)
	{
		try
		{
			if (share)
				m_files.remove(file);

			const auto owner = get_entity();
			const auto file_map = owner->get<file_map_component>();
			file_map->remove(file->get_id());
		}
		catch (std::exception& e)
		{
			log(e);
		}
		catch (...)
		{
			log();
		}
	}

	// Accessor
	hash::ptr share_idle_component::get_hash() const
	{
		const auto owner = get_entity();
		const auto security = owner->get<client_security_component>();
		if (!security)
			return md5::create();

		// Hash
		auto hash = security->get_hash();
		if (!hash)
			hash = md5::create();

		const auto& salt = security->get_salt();
		hash->set_salt(salt);
		return hash;
	}
}
