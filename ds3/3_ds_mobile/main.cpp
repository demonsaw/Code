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

#include <stdlib.h>

#include <QGuiApplication>
#include <QStandardPaths>
//#include <QSurfaceFormat>

#include "controller/application_controller.h"
#include "system/type.h"
#include "thread/thread_info.h"
#include "utility/value.h"

using namespace eja;

int main(int argc, char *argv[])
{
	// RNG
	srand(static_cast<unsigned int>(time(0)));

	// Thread
	thread_info::init();

	// App	
	const auto app = application_controller::create(argc, argv);

	// Options
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

#if _MOBILE
	const auto qpath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).append("/").append(demonsaw::config);
	const auto path = qpath.toStdString();
	app->set_path(path);
#endif

	try
	{		
		app->load();		
	}
	catch (const std::exception& ex) 
	{ 
		//std::cerr << ex.what() << std::endl;
	}
	
	app->init();

	return app->exec();
}
