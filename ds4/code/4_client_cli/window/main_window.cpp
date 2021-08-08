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

#ifdef _MSC_VER
#pragma warning(disable: 4005 4075)
#endif

#include <algorithm>
#include <iostream>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include "component/callback/callback.h"
#include "component/callback/callback_service_component.h"
#include "component/client/client_component.h"
#include "component/client/client_service_component.h"
#include "component/group/group_component.h"
#include "component/io/file_component.h"
#include "component/io/folder_component.h"
#include "component/io/share_component.h"
#include "component/router/router_component.h"
#include "component/session/session_component.h"
#include "component/status/status_component.h"
#include "component/transfer/finished_component.h"
#include "component/transfer/transfer_component.h"
#include "component/transfer/upload_component.h"
#include "component/transfer/upload_service_component.h"

#include "entity/entity.h"
#include "entity/entity_util.h"
#include "security/filter/hex.h"
#include "system/type.h"
#include "utility/utf.h"
#include "utility/value.h"
#include "utility/io/file_util.h"
#include "utility/io/folder_util.h"
#include "window/main_window.h"

namespace eja
{
	// Static
	main_window::ptr main_window::s_ptr = nullptr;

	// Constructor
	main_window::main_window(int& argc, char* argv[]) : m_app(argc, argv)
	{
		// Singleton
		assert(!s_ptr);
		s_ptr = this;
	}

	// Interface
	void main_window::init()
	{
		m_app.read();

		m_app.init();

		// Callback
		for (const auto& e : m_app.get_clients())
		{
			const auto callback = e->find<callback_service_component>();
			callback->add(callback::finished | callback::add, [&](const entity::ptr entity) { on_finished_add(e, entity); });
			callback->add(callback::upload | callback::add, [&](const entity::ptr entity) { on_upload_add(e, entity); });
			callback->add(callback::upload | callback::update, [&]() { on_upload_update(e); });
		}
	}

	void main_window::update()
	{
		try
		{
			splash();

			while (m_active)
			{
				std::cout << "> ";

				std::string line;
				getline(std::cin, line);
				if (line.empty())
					continue;

				// Command
				const auto& command = line[0];

				switch (command)
				{
					case 'i':
					{
						info();
						break;
					}
					case 'r':
					{
						restart();
						break;
					}
					case 't':
					{
						transfers();
						break;
					}
					case 'q':
					case 'x':
					{
						m_active = false;
						break;
					}
					default:
					{
						menu();
						break;
					}
				}
			}
		}
		catch (const std::exception& ex)
		{
			std::cerr << ex.what() << std::endl;
		}
	}

	void main_window::shutdown()
	{
		m_app.shutdown();

		// Wait (for quit)
		std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds::quit));

		// NOTE: Give as much time as possible for the async_quit to finish
		//
		for (const auto& e : m_app.get_clients())
		{
			// Callback
			const auto callback = e->get<callback_service_component>();
			callback->stop();
		}
	}

	// Utility
	void main_window::credits() const
	{
		std::cout << std::endl;
		std::cout << software::title << std::endl;
		std::cout << software::author << std::endl;
		std::cout << software::website << std::endl;
		std::cout << software::date << std::endl;
		std::cout << std::endl;
	}

	void main_window::dash(const size_t num) const
	{
		for (size_t i = 0; i < num; ++i)
			std::cout << '-';

		std::cout << std::endl;
	}

	void main_window::menu() const
	{
		std::cout << std::endl;
		std::cout << boost::format("  %-4s %-10s\n") % "i" % "info";
		std::cout << boost::format("  %-4s %-10s\n") % "r" % "restart";
		std::cout << boost::format("  %-4s %-10s\n") % "t" % "transfers";
		std::cout << boost::format("  %-4s %-10s\n") % "x" % "exit";
		std::cout << std::endl;
	}

	void main_window::splash() const
	{
		credits();

		std::cout << "(Press '?' for help)" << std::endl;
		std::cout << std::endl;
	}

	// Commands
	void main_window::info() const
	{
		try
		{
			// Routers
			std::cout << std::endl;
			std::cout << boost::format("  %-57s %6s %12s\n") % "Name" % "Type" % "Status";
			dash(79);

			for (const auto& e : m_app.get_clients())
			{
				if (e->enabled())
				{
					const auto client = e->get<client_component>();
					const auto status = e->get<status_component>();

					auto name = utf::strip(client->get_name());
					boost::trim(name);
					std::cout << boost::format("  %-57s %6s %12s\n") % name % "client" % status->get_description();

					// Routers
					const auto router_list = e->get<router_list_component>();

					for (const auto& e : *router_list)
					{
						if (e->enabled())
						{
							const auto router = e->get<router_component>();
							std::cout << boost::format("  - %-55s %6s %12s\n") % boost::str(boost::format("%s:%u") % router->get_address() % router->get_port()) % "router" % status->get_description();
						}
					}

					// Shares
					const auto share_list = e->get<share_list_component>();

					for (const auto& e : *share_list)
					{
						if (e->enabled())
						{
							if (e->has<folder_component>())
							{
								const auto folder = e->get<folder_component>();
								std::cout << boost::format("  - %-55s %6s %12s\n") % folder->get_path() % "folder" % folder_util::get_size(folder->get_size());
							}
							else
							{
								const auto file = e->get<file_component>();
								std::cout << boost::format("  - %-55s %6s %12s\n") % file->get_path() % "file" % file_util::get_size(file->get_size());
							}
						}
					}

					// Uploads
					const auto upload_list = e->get<upload_list_component>();
					{
						thread_lock(upload_list);

						for (const auto& e : *upload_list)
						{
							const auto file = e->get<file_component>();
							const auto transfer = e->get<transfer_component>();
							std::cout << boost::format("  - %-55s %6s %12s\n") % file->get_name() % "upload" % boost::str(boost::format("%.2f%%") % transfer->get_percent());
						}
					}

					std::cout << std::endl;
				}
			}
		}
		catch (const std::exception& ex)
		{
			std::cerr << ex.what() << std::endl;
		}
	}

	void main_window::restart()
	{
		std::cout << "Restarting..." << std::endl << std::endl;

		shutdown();

		init();
	}

	void main_window::transfers() const
	{
		// Routers
		std::cout << std::endl;
		std::cout << boost::format("  %s\n") % "Name";
		dash(79);

		for (const auto& e : m_app.get_clients())
		{
			if (e->enabled())
			{
				const auto client = e->get<client_component>();
				const auto status = e->get<status_component>();

				const auto name = utf::strip(client->get_name());
				std::cout << boost::format("  %s\n") % name;

				// Files
				std::vector<entity::ptr> finished_vector;
				const auto finished_map = e->get<finished_map_component>();
				{
					const auto file_map = e->get<file_map_component>();

					thread_lock(finished_map);
					finished_vector.reserve(finished_map->size());

					for (const auto& pair : *finished_map)
					{
						const auto it = file_map->find(pair.first);
						if (it != file_map->end())
							finished_vector.push_back(it->second);
					}
				}

				std::sort(finished_vector.begin(), finished_vector.end(), [&](const entity::ptr& lentity, const entity::ptr& rentity)
				{
					const auto lfile = lentity->get<file_component>();
					const auto rfile = rentity->get<file_component>();

					const auto& lname = lfile->get_name();
					const auto& rname = rfile->get_name();

					return std::lexicographical_compare(lname.begin(), lname.end(), rname.begin(), rname.end(), boost::is_iless());
				});

				for (const auto& e : finished_vector)
				{
					const auto file = e->get<file_component>();
					const auto it = finished_map->find(file->get_id());
					if (it != finished_map->end())
						std::cout << boost::format("  %3u  %s\n") % it->second % file->get_name();
				}

				std::cout << std::endl;
			}
		}
	}

	// Finished
	void main_window::on_finished_add(const entity::ptr& owner, const entity::ptr& entity) const
	{
		const auto finished_map = owner->get<finished_map_component>();
		{
			thread_lock(finished_map);

			const auto file = entity->get<file_component>();
			const auto it = finished_map->find(file->get_id());

			if (it != finished_map->end())
			{
				it->second++;
			}
			else
			{
				const auto pair = std::make_pair(file->get_id(), 1);
				finished_map->insert(pair);
			}
		}
	}

	// Upload
	void main_window::on_upload_add(const entity::ptr& owner, const entity::ptr& entity) const
	{
		// Transfer
		const auto transfer = entity->get<transfer_component>();
		transfer->start();

		// Upload
		const auto upload_list = owner->get<upload_list_component>();
		{
			thread_lock(upload_list);
			upload_list->push_back(entity);
		}

		const auto upload_map = owner->get<upload_map_component>();
		{
			const auto pair = std::make_pair(transfer->get_id(), entity);

			thread_lock(upload_map);
			upload_map->insert(pair);
		}

		// Service
		const auto upload_service = owner->get<upload_service_component>();
		upload_service->async_start(entity);
	}

	void main_window::on_upload_update(const entity::ptr& owner) const
	{
		// Uploads
		std::vector<std::shared_ptr<entity>> remove_list;

		const auto upload_list = owner->get<upload_list_component>();
		{
			thread_lock(upload_list);

			for (const auto& e : *upload_list)
			{
				// Verify
				const auto transfer = e->get<transfer_component>();
				if (transfer->is_running())
				{
					// Timeout
					const auto timeout = e->get<timeout_component>();
					if (!timeout->expired(upload::timeout))
						continue;

					// Cancel
					transfer->cancel();
				}

				// Clear
				const auto upload_map = owner->get<upload_map_component>();
				{
					thread_lock(upload_map);
					upload_map->erase(transfer->get_id());
				}

				// Remove
				remove_list.push_back(e);
				owner->async_call(callback::finished | callback::add, e);

				// Service
				const auto upload_service = owner->get<upload_service_component>();
				upload_service->async_stop(e);
			}

			// Remove
			for (const auto& e : remove_list)
			{
				const auto it = std::find(upload_list->begin(), upload_list->end(), e);
				if (it != upload_list->end())
					upload_list->erase(it);
			}
		}
	}
}
