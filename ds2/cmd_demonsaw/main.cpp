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

// Global
runtime_controller g_runtime;
bool g_active = true;

// Menu
void display_credits()
{
	std::cout << std::endl;
	std::cout << "demonsaw 2.7.2" << std::endl;
	std::cout << "May 29th, 2016" << std::endl;
	std::cout << "Copyright 2014-2016 Demonsaw, LLC All Rights Reserved" << std::endl;
	std::cout << "Believe in the Right to Share" << std::endl;
	std::cout << default_value::website << std::endl;
	std::cout << std::endl;
}

void display_splash()
{
	display_credits();

	std::cout << "(Press 'h' for help)" << std::endl;
	std::cout << std::endl;
}

void display_menu()
{
	std::cout << std::endl;
	std::cout << boost::format("  %-4s %-10s\n") % "c" % "clients";
	std::cout << boost::format("  %-4s %-10s\n") % "r" % "routers";
	std::cout << boost::format("  %-4s %-10s\n") % "t" % "transfers";
	std::cout << std::endl;

	std::cout << boost::format("  %-4s %-10s\n") % "h" % "help";
	std::cout << boost::format("  %-4s %-10s\n") % "e" % "restart";
	std::cout << boost::format("  %-4s %-10s\n") % "f" % "refresh";
	std::cout << boost::format("  %-4s %-10s\n") % "q" % "quit";
	std::cout << boost::format("  %-4s %-10s\n") % "v" % "version";		
	std::cout << std::endl;
}

// Interface
void load(int argc, char* argv[])
{
	bool status = false;
	const char* path = (argc > 1) ? argv[1] : "demonsaw.xml";

	try
	{		
		status = g_runtime.load(path);
	}
	catch (...) { }

	if (!status)
	{
		std::cerr << "ERROR: Unable to load " << path << std::endl;
		exit(EXIT_SUCCESS);
	}
}

void monitor(int argc, char* argv[])
{
	// Create monitor thread
	std::thread thread([]()
	{
		while (g_active)
		{
			// Refresh
			boost::filesystem::path refresh("refresh");

			if (file_util::exists(refresh))
			{
				file_util::remove(refresh);
				g_runtime.refresh();
			}

			// Restart
			boost::filesystem::path restart("restart");

			if (file_util::exists(restart))
			{
				file_util::remove(restart);
				g_runtime.restart();
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(default_timeout::watchdog));
		}
	});

	thread.detach();
}

void init()
{
	g_runtime.init();
}

void shutdown()
{
	g_runtime.shutdown();
}

void update()
{
	try
	{
		while (g_active)
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
					g_runtime.clients();
					break;
				}
				case 'e':
				{
					g_runtime.restart();
					break;
				}
				case 'f':
				{
					g_runtime.refresh();
					break;
				}
				case 'r':
				{
					g_runtime.routers();
					break;
				}
				case 't':
				{
					g_runtime.transfers();
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
					g_active = false;
					g_runtime.shutdown();
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
		std::cerr << "ERROR: Unknown exception in update loop" << std::endl;
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

	load(argc, argv);
	monitor(argc, argv);
	display_splash();

	init();
	update();
	shutdown();

#if 0
	fout.close();
#endif

	return 0;
}
