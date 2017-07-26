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

#ifndef _EJA_APPLICATION_
#define _EJA_APPLICATION_

#include <QApplication>
#include <QColor>
#include <QPixmap>

#include "controller/client_controller.h"
#include "controller/router_controller.h"
#include "dialog/splash_dialog.h"
#include "entity/entity.h"
#include "object/io/file.h"
#include "system/function/function_action.h"
#include "xml/config_file.h"

/*
The QApplication class manages the GUI application's control flow and main settings. It contains the main event loop, 
where all events from the window system and other sources are processed and dispatched. It also handles the application's 
initialization and finalization, and provides session management. It also handles most system-wide and application-wide 
settings. For any GUI application that uses Qt, there is precisely one QApplication object, no matter whether the 
application has 0, 1, 2 or more windows at any time.

The application object can hold data related to different components of the application. QApplication object is always 
available in your application through the static QCoreApplication::instance() call (or qApp macro), making it a good placeholder 
for objects, which otherwise would need to be global. This gives an additional benefit, because sometimes when you construct some 
global objects (for example timers), you get a message, that you can't construct such an object without a QApplication object. 
This is a way to bypass such messages.
*/

namespace eja
{
	class application final : public QApplication
	{
	private:
		config_file m_config;		

		int m_argc;
		char** m_argv;
		splash_dialog* m_splash = nullptr;

	public:
		application(int& argc, char** argv);
		virtual ~application() override { }

		// Interface
		void init();
		void shutdown();

		// Utility
		bool load();
		bool load(const std::string& path) { return load(path.c_str()); }
		bool load(const char* path);

		bool save();
		bool save(const std::string& path) { return save(path.c_str()); }
		bool save(const char* path);

		bool has_splash() const { return m_splash; }

		// Accessor		
		splash_dialog* get_splash(const QPixmap& pixmap);
		splash_dialog* get_splash(const QImage& image) { return get_splash(QPixmap::fromImage(image)); }
		splash_dialog* get_splash(const QString& path) { return get_splash(QPixmap(path)); }
		splash_dialog* get_splash();

		// Accessor
		config_file& get_config() { return m_config; }		
		client_controller& get_clients() { return m_config.get_clients(); }
		router_controller& get_routers() { return m_config.get_routers(); }
	};

}

#endif
