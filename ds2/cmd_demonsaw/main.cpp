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

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <thread>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "controller/runtime_controller.h"
#include "utility/io/file_util.h"

// Using
using namespace eja;
using namespace std;

// Menu
void display_credits()
{
	std::cout << "\ndemonsaw 2.7.2\n";
	std::cout << "May 29th, 2016\n";
	std::cout << "Copyright 2014-2016 Demonsaw, LLC All Rights Reserved\n";
	std::cout << "Believe in the Right to Share\n";
	std::cout << default_value::website << "\n\n";
}

void display_splash()
{
	display_credits();
	std::cout << "(Press 'h' for help)\n\n";
}

void display_menu()
{
	std::cout << "\n";
	std::cout << boost::format("  %-4s %-10s\n") % "c" % "clients";
	std::cout << boost::format("  %-4s %-10s\n") % "r" % "routers";
	std::cout << boost::format("  %-4s %-10s\n") % "t" % "transfers";
	std::cout << "\n";

	std::cout << boost::format("  %-4s %-10s\n") % "h" % "help";
	std::cout << boost::format("  %-4s %-10s\n") % "e" % "restart";
	std::cout << boost::format("  %-4s %-10s\n") % "f" % "refresh";
	std::cout << boost::format("  %-4s %-10s\n") % "q" % "quit";
	std::cout << boost::format("  %-4s %-10s\n") % "v" % "version";		
	std::cout << "\n";
}

// Interface
void load(int argc, char* argv[], runtime_controller* ctrl)
{
	bool status = false;
	const char* path = (argc > 1) ? argv[1] : "demonsaw.xml";

	try
	{		
		status = ctrl->load(path);
	}
	catch (...) { }

	if (!status)
	{
		std::cerr << "ERROR: Unable to load " << path << "\n";
		std::exit(EXIT_FAILURE);
	}
}

void monitor(int argc, char* argv[], bool* active, runtime_controller* ctrl)
{
	// Create monitor thread
	std::thread thread([]()
	{
		while (*active)
		{
			// Refresh
			boost::filesystem::path refresh("refresh");

			if (file_util::exists(refresh))
			{
				file_util::remove(refresh);
				ctrl->refresh();
			}

			// Restart
			boost::filesystem::path restart("restart");

			if (file_util::exists(restart))
			{
				file_util::remove(restart);
				ctrl->restart();
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(default_timeout::watchdog));
		}
	});

	thread.detach();
}

void init(runtime_controller* ctrl)
{
	ctrl->init();
}

void shutdown(runtime_controller* ctrl)
{
	ctrl->shutdown();
}

void update(bool* active, runtime_controller* ctrl)
{
	try
	{
		while (*active)
		{
			std::cout << "demonsaw> ";

			std::string line;
			getline(std::cin, line);
			if (line.empty())
				continue;

			// Command
			const auto& command = line[0];

			switch (command)
			{
				case 'c':
				{
					ctrl->clients();
					break;
				}
				case 'e':
				{
					ctrl->restart();
					break;
				}
				case 'f':
				{
					ctrl->refresh();
					break;
				}
				case 'r':
				{
					ctrl->routers();
					break;
				}
				case 't':
				{
					ctrl->transfers();
					break;
				}			
				case 'v':
				{
					display_credits();
					break;
				}
				case 'q':
				case 'x':
				{
					*active = false;
					ctrl->shutdown();
					break;
				}
				case 'h':
				default:
				{
					display_menu();
					break;
				}
			}
		}
	}
	catch (...)
	{
		std::cerr << "ERROR: Unknown exception in update loop\n"
	}
}

int main(int argc, char* argv[])
{
#if 0
	// Redirect cerr to file
	std::ofstream fout("demonsaw.log");
	std::cerr.rdbuf(fout.rdbuf());
#endif

	// HACK: Crypto++ libraries have static initialization (not thread-safe)
	base64::encode("");
	base64::decode("");

	runtime_controller g_runtime;
	bool g_active = true;

	load(argc, argv, &g_runtime);
	monitor(argc, argv);
	display_splash();

	init(&g_runtime);
	update(&g_active, &g_runtime);
	shutdown(&g_runtime);

#if 0
	fout.close();
#endif

	return 0;
}
