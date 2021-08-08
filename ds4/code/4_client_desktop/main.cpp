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

#define _SPLASH (1)

#include <cstdlib>
#include <iostream>

#include <QApplication>
#include <QBitmap>
#include <QCoreApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QFrame>
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QSplashScreen>
#include <QStandardPaths>
#include <QTimer>

#include "resource/resource.h"
#include "utility/value.h"
#include "window/main_window.h"

using namespace eja;

void init()
{
	srand(static_cast<unsigned int>(time(0)));
}

int main(int argc, char* argv[])
{
	init();

	qt_application app(argc, argv);

#if _SPLASH
	QPixmap pixmap(resource::logo);
	const auto size = QDesktopWidget().availableGeometry().size();
	pixmap = pixmap.scaledToHeight(size.height() / 3, Qt::SmoothTransformation);

	QSplashScreen* splash = new QSplashScreen(pixmap, Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
	splash->setMask(pixmap.mask());
#endif

	try
	{
#if _MACX
		if (argc <= 1)
		{
			auto qpath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
			if (!QDir(qpath).exists())
				QDir().mkdir(qpath);

			// File
			qpath.append("/").append(software::config);
			const auto path = qpath.toStdString();
			app.set_path(path);
		}
#endif
		app.read();

		if (app.exists())
			return 0;
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what() << std::endl;
	}

#if _SPLASH
	if (app.has_clients())
		splash->show();
#endif

	app.scale();

	main_window wnd(app);
	wnd.init();
	wnd.show();

#if _SPLASH
	QTimer::singleShot(500, splash, [&]()
	{
		splash->close();
		delete splash;
	});
#endif

	return app.exec();
}
