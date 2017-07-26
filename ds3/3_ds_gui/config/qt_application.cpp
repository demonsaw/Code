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

// HACK: There is a FOREACH conflict in boost and Qt
#include <boost/network/uri.hpp>

#include <QFile>
#include <QIcon>
#include <QStandardPaths>
#include <QStyleFactory>
#include <QTextCodec>

#include "component/endpoint_component.h"
#include "component/pixmap_component.h"
#include "component/client/client_io_component.h"
#include "component/group/group_security_component.h"
#include "component/status/status_component.h"
#include "config/qt_application.h"
#include "entity/entity.h"
#include "proxy/unfocus_proxy.h"
#include "resource/resource.h"
#include "security/filter/base.h"
#include "utility/value.h"

namespace eja
{
	// Constructor
	qt_application::qt_application(int argc, char* argv[]) : application(argc, argv), QApplication(argc, argv)
	{		
		// Locale
		QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

		// Meta
		qRegisterMetaType<entity::ptr>("entity::ptr");
		qRegisterMetaType<std::shared_ptr<entity>>("std::shared_ptr<entity>");

		// Style
		setStyle(new unfocus_proxy);

		// Theme
		set_theme();

		// HACK: Libraries have static initialization (not thread-safe)
		boost::network::uri::uri uri("");
		base64::encode("");
		base64::decode("");
	}

	// Interface
	void qt_application::init()
	{
		application::init();

		// TODO
		//
		//
	}

	void qt_application::shutdown()
	{
		application::shutdown();

		// TODO
		//
	}

	void qt_application::update()
	{
		application::update();

		// TODO
		//
	}

	void qt_application::clear()
	{
		application::clear();

		// TODO
		//
	}

	// Utility
	void qt_application::load(const char* psz)
	{
		application::load(psz);

		// Routers
		for (const auto& entity : m_routers)
		{			
			init_group(entity);

			const auto message_list = entity->get<message_list_component>();
			for (const auto& entity : *message_list)
				init_group(entity);
		}

		// Clients
		for (const auto& entity : m_clients)
		{
			init_client(entity);

			init_group(entity);

			const auto message_list = entity->get<message_list_component>();
			for (const auto& entity : *message_list)
				init_group(entity);

			const auto transfer_list = entity->get<transfer_list_component>();
			for (const auto& entity : *transfer_list)
				init_group(entity);
		}
	}

	void qt_application::init_client(const entity::ptr entity)
	{
		// Save
		const auto io = entity->get<client_io_component>();
		if (!io->has_path())
		{
			const auto qpath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
			io->set_path(qpath.toStdString());
		}
	}

	void qt_application::init_group(const entity::ptr entity)
	{
		const auto security_list = entity->get<group_security_list_component>();

		for (const auto& e : *security_list)
		{
			// Pixmap
			const auto pixmap = pixmap_component::create();
			e->add(pixmap);

			// Group
			const auto security = e->get<group_security_component>();
			if (security->get_type() == group_security_type::file)
			{
				// Group					
				const auto& entropy = security->get_entropy();
				pixmap->set_path(entropy);
			}
		}
	}

	// Set
	void qt_application::set_theme()
	{
		application::set_theme();

		// Style
		QFile file(resource::style);
		file.open(QFile::ReadOnly);
		QString style = QLatin1String(file.readAll());
		qApp->setStyleSheet(style);
	}

	void qt_application::set_theme(const std::string& theme)
	{
		application::set_theme(theme);

		QFile file(QString::fromStdString(theme));

		if (file.open(QFile::ReadOnly))
		{
			QString style = QLatin1String(file.readAll());
			qApp->setStyleSheet(style);
		}
		else
		{
			set_theme();
		}
	}
}
