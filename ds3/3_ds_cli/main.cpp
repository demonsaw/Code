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

#include <iostream>

#include "config/cli_application.h"
#include "thread/thread_info.h"
#include "utility/value.h"
#include "window/main_window.h"

using namespace eja;

int main(int argc, char* argv[])
{
	srand(static_cast<unsigned int>(time(0)));

	// Thread
	thread_info::init();

	// App
	cli_application app(argc, argv);

	try
	{
		app.load();
	}
	catch (const cpptoml::parse_exception& e)
	{
		std::cerr << "Failed to load \"" << app.get_path() << "\" : " << e.what() << std::endl;
		exit(-1);
	}

	main_window wnd(app);

	wnd.init();
	wnd.update();
	wnd.shutdown();

	return 0;
}