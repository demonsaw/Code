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

#include "component/endpoint_component.h"
#include "component/version_component.h"
#include "component/session/session_component.h"
#include "component/io/download_component.h"
#include "component/message/message_acceptor_component.h"
#include "component/status/status_component.h"

#include "entity/entity.h"
#include "security/filter/base.h"
#include "utility/value.h"
#include "utility/io/file_util.h"
#include "window/main_window.h"

namespace eja
{
	// Interface
	void main_window::init()
	{
		m_app.init();
	}

	void main_window::update()
	{
		try
		{
			splash();

			while (m_active)
			{
				std::cout << "ds: ";

				std::string line;
				getline(std::cin, line);
				if (line.empty())
					continue;

				// Command
				const auto& command = line[0];

				switch (command)
				{
					case 'e':
					{
						restart();
						break;
					}
					case 'r':
					{
						routers();
						break;
					}					
					case 'v':
					{
						credits();
						break;
					}
					case 'q':
					case 'x':
					{
						m_active = false;
						break;
					}
					case 'h':
					default:
					{
						menu();
						routers();
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
	}

	void main_window::clear()
	{
		m_app.clear();
	}

	// Utility
	void main_window::print(const char ch, const size_t num)
	{
		for (size_t i = 0; i < num; ++i)
			std::cout << ch;
	}

	void main_window::space(const size_t num)
	{
		print(' ', num);
		std::cout << std::endl;
	}

	void main_window::dash(const size_t num)
	{
		print('-', num);
		std::cout << std::endl;
	}

	void main_window::endl()
	{
		std::cout << std::endl;
	}

	void main_window::credits()
	{
		std::cout << std::endl;
		std::cout << demonsaw::title << std::endl;
		std::cout << demonsaw::copyright << std::endl;
		std::cout << demonsaw::motto << std::endl;		
		std::cout << demonsaw::website << std::endl;
		std::cout << demonsaw::author << std::endl;
		std::cout << std::endl;
	}

	void main_window::splash()
	{
		credits();

		std::cout << "(Press 'h' for help)" << std::endl;
	}

	void main_window::menu()
	{
		std::cout << std::endl;
		std::cout << boost::format("  %-4s %-10s\n") % "r" % "routers";		
		std::cout << boost::format("  %-4s %-10s\n") % "e" % "restart";		
		std::cout << boost::format("  %-4s %-10s\n") % "v" % "version";

		std::cout << std::endl;
		std::cout << boost::format("  %-4s %-10s\n") % "h" % "help";
		std::cout << boost::format("  %-4s %-10s\n") % "x" % "exit";
		std::cout << std::endl;
	}

	// Commands
	void main_window::restart()
	{
		std::cout << "Restarting..." << std::endl << std::endl;

		// Routers
		const auto& routers = m_app.get_routers();

		for (const auto& e : routers)
		{
			if (e->disabled())
				continue;

			const auto acceptor = e->get<message_acceptor_component>();
			if (acceptor->valid())
				acceptor->restart();
		}
	}

	void main_window::routers()
	{
		const auto& routers = m_app.get_routers();
		if (routers.empty())
			return;

		try
		{
			// Routers
			endl();
			std::cout << "Routers" << std::endl;
			std::cout << boost::format("+ %-20s %-28s %5u %7s %7s %8u %9u %7u\n") % "Name" % "Address" % "Port" % "Status" % "Version" % "Sessions" % "Downloads" % "Clients";
			dash(100);

			const auto& routers = m_app.get_routers();
			{
				thread_lock(routers);

				for (const auto& entity : routers)
				{
					const auto endpoint = entity->get<endpoint_component>();
					const auto status = entity->get<status_component>();
					const auto version = entity->get<version_component>();

					const auto client_map = entity->get<client_map_component>();
					const auto download_map = entity->get<download_map_component>();
					const auto session_map = entity->get<session_map_component>();

					std::cout << boost::format("+ %-20s %-28s %5u %7s %7s %8u %9u %7u\n") % endpoint->get_name() % endpoint->get_address() % endpoint->get_port() % status->get_description() % version->str() % session_map->size() % download_map->size() % client_map->size();

					// Servers
					const auto transfer_list = entity->get<transfer_list_component>();
					{
						thread_lock(transfer_list);

						for (const auto& e : *transfer_list)
						{
							const auto endpoint = e->get<endpoint_component>();
							const auto status = e->get<status_component>();

							std::cout << boost::format("  %-20s %-28s %5u %7s\n") % endpoint->get_name() % endpoint->get_address() % endpoint->get_port() % status->get_description();
						}
					}
				}
			}			

			endl();
		}
		catch (const std::exception& ex)
		{
			std::cerr << ex.what() << std::endl;
		}
	}
}
