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
#pragma warning(disable: 4005)
#endif

#define _SPLASH (_WIN32 && 1)

#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QFont>
#include <QFontDatabase>
#include <QPixmap>
#include <QSplashScreen>
#include <QTimer>

#include "resource/resource.h"
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
	qt_application app(argc, argv);

	// Font
	WIN32_ONLY(QApplication::setFont(QFont("Segoe UI")));
	MACX_ONLY(QApplication::setFont(QFont("Helvetica Neue")));

	QFontDatabase::addApplicationFont(resource::font_awesome);
	QApplication::setFont(QFont(demonsaw::font_awesome), "QTabBar");
	QApplication::setFont(QFont(demonsaw::font_awesome), "QToolBar");

#if _SPLASH
	QPixmap pixmap(resource::logo);
	QSplashScreen splash(pixmap);	
#endif

	try
	{
#if _MACX
		if (argc <= 1)
		{
			// Path
			const auto qfile = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).append("/").append(demonsaw::config);
			const auto file = qfile.toStdString();

			app.set_path(path.toStdString());
		}
#endif
		app.load();

#if _SPLASH
		if (!app.empty())
			splash.show();
#endif
	}
	catch (const std::exception&) { }
	
	main_window wnd(app);
	wnd.init();
	wnd.show();

#if _SPLASH
	QTimer::singleShot(500, &splash, SLOT(close()));
#endif

	return app.exec();
}
