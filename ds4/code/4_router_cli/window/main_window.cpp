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

#include "component/client/client_component.h"
#include "component/group/group_component.h"
#include "component/router/router_component.h"
#include "component/session/session_component.h"
#include "component/status/status_component.h"
#include "component/transfer/transfer_component.h"

#include "entity/entity.h"
#include "entity/entity_util.h"
#include "security/filter/hex.h"
#include "system/type.h"
#include "utility/utf.h"
#include "utility/value.h"
#include "window/main_window.h"

namespace eja
{
	// Static
	main_window::ptr main_window::s_ptr = nullptr;

	// Constructor
	main_window::main_window(cli_application& app) : m_app(app)
	{
		// Singleton
		assert(!s_ptr);
		s_ptr = this;
	}

	// Interface
	void main_window::run()
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
					case 'd':
					{
						dashboard();
						break;
					}
					case 'r':
					{
						restart();
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
		std::cout << boost::format("  %-4s %-10s\n") % "d" % "dashboard";
		std::cout << boost::format("  %-4s %-10s\n") % "r" % "restart";
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
	void main_window::dashboard() const
	{
		try
		{
			// Routers
			std::cout << std::endl;
			std::cout << boost::format("  %-36s %15s %5s %6s %5s\n") % "Router" % "Address" % "Port" % "State" % "Load";
			dash(74);

			const auto& routers = m_app.get_routers();
			{
				thread_lock(routers);

				for (const auto& e : routers)
				{
					const auto router = e->get<router_component>();
					const auto status = e->get<status_component>();
					const auto entity_map = e->get<entity_map_component>();

					std::cout << boost::format("%c %-36s %15s %5u %6s %5u\n") % (e->enabled() ? '+' : '-') % router->get_name() % router->get_address() % router->get_port() % status->get_description() % entity_map->size();

					// Servers
					const auto router_list = e->get<router_list_component>();

					if (!router_list->empty())
					{
						{
							thread_lock(router_list);

							for (const auto& e : *router_list)
							{
								const auto router = e->get<router_component>();
								const auto status = e->get<status_component>();

								std::cout << boost::format("  %c %-34s %15s %5u %6s\n") % (e->enabled() ? '+' : '-') % router->get_name() % router->get_address() % router->get_port() % status->get_description();
							}
						}

						std::cout << std::endl;
					}
				}
			}

			std::cout << std::endl;
		}
		catch (const std::exception& ex)
		{
			std::cerr << ex.what() << std::endl;
		}
	}

	void main_window::restart() const
	{
		std::cout << "Restarting..." << std::endl << std::endl;

		m_app.update();
	}
}
