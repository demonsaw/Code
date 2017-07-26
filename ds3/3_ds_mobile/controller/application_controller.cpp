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

#include <cassert>
#include <stdlib.h>

#include <QCoreApplication>
#include <QFontDatabase>
#include <QQmlApplicationEngine>
#include <QRect>
#include <QScreen>
#include <QSortFilterProxyModel>
#include <QStyleHints>
#include <QTextCodec>
#include <QTextDecoder>
#include <QTimer>

#include "component/chat_component.h"
#include "component/chat_model_component.h"
#include "component/debug_model_component.h"
#include "component/endpoint_component.h"
#include "component/error_component.h"
#include "component/group_model_component.h"
#include "component/group_ui_component.h"
#include "component/message_model_component.h"
#include "component/message_wrapper_component.h"
#include "component/callback/callback_action.h"
#include "component/client/client_network_component.h"
#include "component/client/client_service_component.h"
#include "component/group/group_security_component.h"
#include "component/message/message_service_component.h"

#include "controller/application_controller.h"
#include "controller/chat_controller.h"
#include "controller/client_controller.h"
#include "controller/debug_controller.h"
#include "controller/envelope_controller.h"
#include "controller/group_controller.h"
#include "controller/message_controller.h"
#include "controller/router_controller.h"

#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "model/chat_model.h"
#include "model/client_model.h"
#include "model/debug_model.h"
#include "model/envelope_model.h"
#include "model/group_model.h"
#include "model/message_model.h"
#include "model/qml_sorted_model.h"
#include "resource/resource.h"
#include "security/filter/base.h"
#include "thread/thread_info.h"
#include "utility/clipboard.h"
#include "utility/minimize.h"
#include "utility/notification.h"
#include "utility/toast.h"
#include "utility/value.h"

namespace eja
{
	// Static
	application_controller::ptr application_controller::s_ptr = nullptr;

	// Constructor
	application_controller::application_controller(int argc, char* argv[]) : QGuiApplication(argc, argv), application(argc, argv), entity_callback(callback_type::window)
	{
		m_clipboard = new eja::clipboard(this);
		m_minimize = new minimize(this);
		m_notification = new notification(this);
		m_toast = new toast(this);
		m_vibrator = new vibrator(this);

		m_chat = new chat_controller(this);
		m_client = new client_controller(m_clients, this);
		m_debug = new debug_controller(this);
		m_envelope = new envelope_controller(this);
		m_group = new group_controller(this);		
		m_message = new message_controller(this);
		m_router = new router_controller(this);		

		// Locale
		const auto codec = QTextCodec::codecForName("UTF-8"); // UTF-8    ISO-8859-1
		QTextCodec::setCodecForLocale(codec);

		// Meta
		qRegisterMetaType<u16>("u16"); 
		qRegisterMetaType<size_t>("size_t");
		qRegisterMetaType<entity::ptr>("entity::ptr");
		qRegisterMetaType<std::shared_ptr<entity>>("std::shared_ptr<entity>");

		qmlRegisterType<StatusBar>("StatusBar", 0, 1, "StatusBar");

		// Font		
		QFontDatabase::addApplicationFont(font::awesome);
		
#if _MOBILE
		const auto rect = QGuiApplication::primaryScreen()->geometry();
#else
		const QRect rect(0, 0, 405, 720);
#endif
		const auto refWidth = 1080.0;
		const auto refHeight = 1920.0;
		const auto height = std::max(rect.width(), rect.height());
		const auto width = std::min(rect.width(), rect.height());		
		m_ratio = std::min(height / refHeight, width / refWidth);

		//const auto refDpi = 445.0;
		//const auto dpi = QGuiApplication::primaryScreen()->logicalDotsPerInch();
		//m_font_ratio = std::min((height * dpi) / (refHeight * refDpi), (width * dpi) / (refWidth * refDpi));
		//const auto ratio = std::min((height * refDpi) / (dpi * refHeight), (width * refDpi) / (dpi * refWidth));

		// GUI
		QStyleHints* styleHints = QGuiApplication::styleHints();
		styleHints->setMousePressAndHoldInterval(500);
		styleHints->setStartDragDistance(width / 28);

		QGuiApplication::setFont(QFont(font::awesome));
		//WIN32_ONLY(QGuiApplication::setFont(QFont("Segoe UI")));
		//MACX_ONLY(QGuiApplication::setFont(QFont("Helvetica Neue")));

		// HACK: Libraries have static initialization (not thread-safe)
		boost::network::uri::uri uri("");
		base64::encode("");
		base64::decode("");

		// Signal
		connect(this, &application_controller::set_status, this, &application_controller::on_set_status);
		connect(this, static_cast<void (application_controller::*)(const entity::ptr)>(&application_controller::update), this, static_cast<void (application_controller::*)(const entity::ptr)>(&application_controller::on_update));
		connect(this, &application_controller::applicationStateChanged, this, &application_controller::on_state_changed);

		// Callback
		add_callback(callback_action::update, [&](const entity::ptr entity) { update(entity); });
	}

	// Interface
	void application_controller::init()
	{		
		// Default
		if (!m_clients->empty())
		{
			// Group
			for (const auto& entity : m_clients)
			{
				entity->enable();

				// Options
				const auto network = entity->get<client_network_component>();
				network->set_trust(network_trust::none);
				network->set_search(false);
				network->set_upload(false);

				// Group
				const auto security_list = entity->get<group_security_list_component>();
				if (security_list->empty())
				{
					const auto e = entity_factory::create_group(entity);
					e->disable();

					security_list->push_back(e);
				}

				// Model
				application_controller::set_initial(entity);
			}
			
			auto it = m_clients->begin();
			std::advance(it, m_index);
			const auto entity = *it;
			set_entity(entity);

			// Status
			const auto status = entity->get<status_component>();
			const auto function = function::create([&](const entity::ptr entity) { set_status(entity); });
			status->add(function);

			application::init();
		}
		else
		{
			add();
		}

		//
		// TODO: Save out last selected client
		//

		// Controllers
		auto context = m_engine.rootContext();
		context->setContextProperty("App", this);		
		context->setContextProperty("Chat", m_chat);
		context->setContextProperty("Client", m_client);
		context->setContextProperty("Debug", m_debug);
		context->setContextProperty("Envelope", m_envelope);
		context->setContextProperty("Group", m_group);		
		context->setContextProperty("Message", m_message);
		context->setContextProperty("Router", m_router);

		context->setContextProperty("Clipboard", m_clipboard); 
		context->setContextProperty("Minimize", m_minimize);
		context->setContextProperty("Notification", m_notification);
		context->setContextProperty("Toast", m_toast);
		context->setContextProperty("Vibrator", m_vibrator);

		// Models
		context->setContextProperty("chat_model", m_chat->get_model());
		context->setContextProperty("client_model", m_client->get_model());
		context->setContextProperty("error_model", m_debug->get_model());
		context->setContextProperty("envelope_model", m_envelope->get_model());
		context->setContextProperty("group_model", m_group->get_model());		
		context->setContextProperty("message_model", m_message->get_model());

		// Proxy
		//context->setContextProperty("client_proxy", m_client->get_proxy());

		m_engine.load(QUrl(QLatin1String(qml::main)));
	}

	void application_controller::shutdown()
	{
		if (!empty())
		{
			// Status
			const auto status = m_entity->get<status_component>();
			status->clear();

			try
			{				
				application::shutdown();
			}
			catch (const std::exception& /*ex*/) {}
		}		
		
		// Wait (for quit)
		std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds::quit / 2));

		qApp->exit();
	}

	void application_controller::update()
	{
		// Spam
		if (!m_enabled)
			return;

		for (const auto& entity : m_clients)
		{
			const auto service = entity->get<client_service_component>();
			if (service->valid())
				service->restart();
		}

		// Spam
		m_enabled = false;
		QTimer::singleShot(ui::enter, [this]() { m_enabled = true; });
	}

	void application_controller::on_update(const entity::ptr entity)
	{
		if (entity->has<message_service_component>())
		{
			const auto service = entity->get<message_service_component>();
			service->async_restart();
		}
	}

	// Event
	bool application_controller::event(QEvent *e)
	{
		const auto type = e->type();
		if ((type == QEvent::Close) || (type == QEvent::Quit))
			shutdown();

		return QGuiApplication::event(e);
	}

	// Utility
	void application_controller::add()
	{
		// Entity
		const auto entity = entity_factory::create_client();
		entity->disable();

		// Client		
		auto endpoint = entity->get<endpoint_component>();
		endpoint->set_name(client::name);
		endpoint->set_color();

		// Group
		const auto security_list = entity->get<group_security_list_component>();
		{
			const auto e = entity_factory::create_group(entity);
			e->disable();

			security_list->push_back(e);
		}

		// Model
		application_controller::set_initial(entity);

		// Options
		const auto network = entity->get<client_network_component>();
		network->set_trust(network_trust::none); 
		network->set_search(false);
		network->set_upload(false);

		// Model
		entity->add<chat_model_component>();
		entity->add<debug_model_component>();
		entity->add<group_model_component>();
		entity->add<message_model_component>();
		entity->add<index_list_component>();

		// Router
		const auto message_list = entity->get<message_list_component>();
		const auto e = entity_factory::create_client_message(entity);
		message_list->push_back(e);

		endpoint = e->get<endpoint_component>();
		endpoint->set_name(router::enigma);
		endpoint->set_address(router::enigma);
		endpoint->set_port(router::port);

		// Status
		const auto status = entity->get<status_component>();
		const auto function = function::create([&](const entity::ptr entity) { set_status(entity); });
		status->add(function);

		m_backup = m_entity;

		set_entity(entity);
	}

	void application_controller::back()
	{
		assert(m_backup);

		set_entity(m_backup);

		m_backup.reset();
	}

	void application_controller::toggle()
	{
		const auto entity = get_entity();
		toggle(entity);
	}

	void application_controller::toggle(const entity::ptr entity)
	{
		if (entity->enabled())
			stop(entity);
		else
			start(entity);
	}

	void application_controller::start(const entity::ptr entity)
	{
		const auto service = entity->get<client_service_component>();

		if (service->invalid())
		{
			entity->enable();

			service->start();
		}
	}

	void application_controller::stop(const entity::ptr entity)
	{
		const auto service = entity->get<client_service_component>();

		if (service->valid())
		{
			service->async_quit();

			entity->disable();
		}
	}

	void application_controller::load(const char* psz)
	{
		application::load(psz);

		for (const auto& entity : m_clients)
		{
			entity->add<chat_model_component>();
			entity->add<debug_model_component>();
			entity->add<group_model_component>();
			entity->add<message_model_component>();
			entity->add<index_list_component>();
		}
	}

	void application_controller::notify(const std::string& str) const
	{
		m_notification->set_notification(str);
	}

	// Handlers
	void application_controller::on_set_status(const entity::ptr entity)
	{
		assert(thread_info::main());

		if (has_entity() && m_client->has_entity())
			emit m_client->statusChanged();
	}

	void application_controller::on_state_changed(Qt::ApplicationState state)
	{
		switch (state)
		{
			case Qt::ApplicationActive:
			{
				m_chat->update(); 
				m_client->update();
				m_debug->update();
				m_envelope->update(); 
				m_group->update();
				m_message->update();

				break;
			}
		}
	}

	// Set
	void application_controller::set_entity(const entity::ptr entity)
	{
		assert(thread_info::main());

		entity_callback::set_entity(entity);

		// Models		
		m_chat->set_entity(entity);
		m_client->set_entity(entity);
		m_debug->set_entity(entity);
		m_envelope->set_entity(entity); 
		m_group->set_entity(entity);
		m_router->set_entity(entity);
	}

	void application_controller::set_entity(const int row)
	{
		assert(thread_info::main());
		
		const auto model = m_client->get_model();
		const auto entity = model->get_entity(row);

		if (entity)
		{			
			set_entity(entity);
			m_index = row;
		}			
	}
	
	void application_controller::set_initial(const entity::ptr entity)
	{
		QString initial = "?";
		const auto endpoint = entity->get<endpoint_component>();

		if (endpoint->has_name())
		{
			QTextDecoder decoder(QTextCodec::codecForName("UTF-8"));
			const auto& name = endpoint->get_name();
			const auto psz = name.c_str();

			for (int i = 0; i < name.size(); i++)
			{
				initial = decoder.toUnicode(psz + i, 1);
				if (!initial.isEmpty())
					break;
			}
		}

		const auto group_ui = group_ui_component::create();
		group_ui->set_initial(initial);
		entity->add(group_ui);
	}

	// Static
	application_controller::ptr application_controller::create(int argc, char* argv[])
	{
		assert(thread_info::main() && !s_ptr);

		s_ptr = new application_controller(argc, argv);
		return s_ptr;
	}
}
