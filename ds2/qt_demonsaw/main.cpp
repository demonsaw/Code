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

#include <QMessageBox>
#include <QString>
#include <QRgb>
#include <QTimer>

#include "application.h"
#include "proxy/no_focus_proxy_style.h"
#include "security/checksum.h"
#include "utility/default_value.h"
#include "window/main_window.h"

// Using
using namespace eja;
using namespace std;

int main(int argc, char *argv[])
{
	bool status = false;
	application app(argc, argv);
	app.setStyle(new no_focus_proxy_style);

#if _WIN32
	// Splash
	splash_dialog* splash = app.get_splash();
	splash->show();
#endif

	try
	{
		status = app.load();
	}
	catch (...) {}
	
	const auto window = main_window::create();
	window->init();	
	
	app.init();		
	window->show_ex();
	
	// Linger
#if _WIN32
	QTimer::singleShot(default_timeout::client::refresh, splash, SLOT(close()));
#endif

	/*if (!status)
	{
		QMessageBox messageBox;
		messageBox.critical(window, "Error", "Unable to read demonsaw.xml!");
	}*/

	return app.exec();
}
