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

#include <cassert>
#include <string>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>

#include <QDebug>
#include <QFile>
#include <QFontDatabase>
#include <QMessageBox>
#include <QString>
#include <QTextCodec>

// HACK: There is a FOREACH conflict in boost and Qt
#include <boost/network/uri.hpp>

#include "application.h"
#include "command/client/request/client_quit_request_command.h"
#include "component/chat_idle_component.h"
#include "component/share_idle_component.h"
#include "component/client/client_component.h"
#include "component/client/client_idle_component.h"
#include "component/client/client_machine_component.h"
#include "component/client/client_option_component.h"
#include "component/router/router_idle_component.h"
#include "component/router/router_machine_component.h"
#include "component/router/router_option_component.h"
#include "component/server/server_component.h"
#include "component/server/server_machine_component.h"
#include "component/server/server_option_component.h"
#include "component/transfer/download_component.h"
#include "component/transfer/download_thread_component.h"
#include "component/transfer/finished_component.h"
#include "component/transfer/transfer_component.h"
#include "component/transfer/transfer_idle_component.h"
#include "entity/entity.h"
#include "object/status.h"
#include "resource/resource.h"
#include "security/base.h"
#include "window/window.h"

namespace eja
{
	// Constructor
	application::application(int& argc, char** argv) : QApplication(argc, argv), m_argc(argc), m_argv(argv)
	{
		// Locale
		QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8")); // ISO-8859-1

		// Qt
		qRegisterMetaType<u64>("u64");
		qRegisterMetaType<client_browse_request_command::ptr>("client_browse_request_command::ptr");
		qRegisterMetaType<client_hash_type>("client_hash_type");
		qRegisterMetaType<entity::ptr>("entity::ptr");
		qRegisterMetaType<file_filter>("file_filter");
		qRegisterMetaType<function_action>("function_action");
		qRegisterMetaType<group_cipher_type>("group_cipher_type");
		qRegisterMetaType<group_hash_type>("group_hash_type");
		qRegisterMetaType<http_status>("http_status");
		qRegisterMetaType<status>("status");
		qRegisterMetaType<transfer_type>("transfer_type");
		qRegisterMetaType<QVector<int>>("Vector<int>");

		// HACK: URI libraries have static initialization (not thread-safe)
		boost::network::uri::uri uri("");

		// HACK: Crypto++ libraries have static initialization (not thread-safe)		
		base64::encode("");
		base64::decode("");		
	}

	// Interface
	void application::init()
	{
		{
			// Routers
			const auto& routers = m_config.get_routers();
			auto_lock_ref(routers);

			for (const auto& entity : routers)
			{
				// Servers
				const auto server_vector = entity->get<server_entity_vector_component>();
				
				for (const auto& entity : server_vector->copy())
				{
					const auto option = entity->get<server_option_component>();
					if (option->enabled())
					{
						const auto machine = entity->get<server_machine_component>();
						machine->start();
					}
				}

				// Router
				const auto option = entity->get<router_option_component>();

				if (option->enabled())
				{
					const auto machine = entity->get<router_machine_component>();
					machine->set_threads(option->get_thread_pool());
					machine->start();

					const auto idle = entity->get<router_idle_component>();
					idle->start();
				}
			}
		}
		
		{
			// Clients
			const auto& clients = m_config.get_clients();
			auto_lock_ref(clients);

			for (const auto& entity : clients)
			{
				// Client
				const auto option = entity->get<client_option_component>();

				if (option->enabled())
				{
					const auto machine = entity->get<client_machine_component>();
					machine->start();

					const auto idle = entity->get<client_idle_component>();
					idle->start();

					const auto transfer = entity->get<transfer_idle_component>();
					transfer->start();
				}
			}
		}
	}

	void application::shutdown()
	{		
		// Client
		boost::thread_group threads;
		const auto& clients = m_config.get_clients();
		auto_lock_ref(clients);

		for (const auto& entity : clients)
		{
			threads.create_thread([entity]()
			{
				const auto service = std::make_shared<boost::asio::io_service>();
				const auto timer = std::make_shared<boost::asio::deadline_timer>(*service);
				timer->expires_from_now(boost::posix_time::milliseconds(default_timeout::client::shutdown));

				service->post([entity, timer]()
				{
					const auto machine = entity->get<client_machine_component>();
					if (machine)
					{
						if (machine->success())
						{
							const auto socket = http_socket::create();

							try
							{
								const auto router = entity->get<router_component>();
								if (router)
								{
									// Socket
									const auto option = entity->get<client_option_component>();
									if (option)
										socket->set_timeout(option->get_socket_timeout()); 
									
									socket->open(router->get_address(), router->get_port());

									// Quit
									const auto request_command = client_quit_request_command::create(entity, socket);
									request_command->execute();
								}
							}
							catch (...) {}

							socket->close();
						}
					}

					timer->cancel();
				});

				service->run();

				// Machine
				const auto machine = entity->get<client_machine_component>();
				if (machine)
					machine->stop();

				// Client
				const auto client_idle = entity->get<client_idle_component>();
				if (client_idle)
					client_idle->stop();

				// Chat
				const auto chat_idle = entity->get<chat_idle_component>();
				if (chat_idle)
					chat_idle->stop();

				// Option
				const auto option = entity->get<client_option_component>();
				if (option && option->has_partial())
				{
					// Download
					const auto download_vector = entity->get<download_entity_vector_component>();
					if (download_vector)
					{
						for (const auto& entity : download_vector->copy())
						{
							const auto transfer = entity->get<transfer_component>();
							if (transfer && transfer->is_download() && !transfer->done())
							{
								const auto file = entity->get<file_component>();
								if (file)
									file_util::remove(file);
							}
						}
					}

					// Finished
					const auto finished_vector = entity->get<finished_entity_vector_component>();
					if (finished_vector)
					{
						for (const auto& entity : finished_vector->copy())
						{							
							const auto transfer = entity->get<transfer_component>();
							if (transfer && transfer->is_download() && !transfer->done())
							{
								const auto thread = entity->get<download_thread_component>();
								if (thread && thread->invalid())
								{
									const auto file = entity->get<file_component>();
									if (file)
										file_util::remove(file);
								}
							}
						}
					}
				}

				entity->shutdown();
			});
		}
		
		threads.join_all();

		// Router
		const auto& routers = m_config.get_routers();
		auto_lock_ref(routers);

		for (const auto& entity : routers)
		{
			// Server
			const auto server_vector = entity->get<server_entity_vector_component>();
			for (const auto& entity : server_vector->copy())
			{
				const auto machine = entity->get<server_machine_component>();
				if (machine)
					machine->stop();

				entity->shutdown();
			}

			// Machine
			const auto machine = entity->get<router_machine_component>();
			if (machine)
				machine->stop();

			entity->shutdown();
		}

		try
		{
			save();
		}
		catch (const std::exception& ex)
		{
			qDebug() << ex.what();
		}
	}

	// Utility
	bool application::load()
	{
		assert(m_argc && m_argv);

		// Path
		boost::filesystem::path path(m_argv[0]);

		// Font
		{
			//QFile file(resource::font::noto);
			//file.open(QFile::ReadOnly);
			//int id = QFontDatabase::addApplicationFontFromData(file.readAll());
			//QString family = QFontDatabase::applicationFontFamilies(id).at(0);
			//QFont font(family, 12);
#if _WIN32			
			QFont font("Segoe UI", 12);
			QApplication::setFont(font);
#endif
		}

		// Style
		{
			boost::filesystem::path style_path = path.parent_path();
			style_path /= default_value::style;

			QFile file(QString::fromStdString(style_path.string()));
			if (file.open(QFile::ReadOnly))
			{
				QString style = QLatin1String(file.readAll());
				setStyleSheet(style);
			}
			else
			{
				QFile file(resource::style);
				file.open(QFile::ReadOnly);
				QString style = QLatin1String(file.readAll());
				setStyleSheet(style);
			}
		}

		// XML
		if (m_argc > 1)
			return load(m_argv[1]);

		boost::filesystem::path config_path = path.parent_path();
		config_path /= default_file::config;

		return load(config_path.string());
	}

	bool application::load(const char* path)
	{
		assert(path);

		return m_config.read(path);
	}

	// Save
	bool application::save()
	{
		assert(m_argc && m_argv);

		if (m_argc > 1)
			return save(m_argv[1]);

		boost::filesystem::path path(m_argv[0]);
		boost::filesystem::path file_path = path.parent_path();
		file_path /= default_file::config;

		return save(file_path.string());
	}

	bool application::save(const char* path)
	{
		assert(path);

		return m_config.write(path);
	}

	// Accessor
	splash_dialog* application::get_splash(const QPixmap& pixmap)
	{
		if (!has_splash())
			m_splash = new splash_dialog(pixmap);

		return m_splash;
	}
	
	splash_dialog* application::get_splash()
	{
		if (!has_splash())
			m_splash = new splash_dialog;

		return m_splash;
	}
}
