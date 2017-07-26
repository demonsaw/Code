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

#ifndef _EJA_APPLICALTION_CONTROLLER_H_
#define _EJA_APPLICALTION_CONTROLLER_H_

#include <memory>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "config/application.h"
#include "entity/entity_callback.h"
#include "system/type.h"
#include "utility/statusbar.h"
#include "utility/vibrator.h"

namespace eja
{
	class clipboard;
	class minimize;
	class notification;
	class toast;
	
	class chat_controller;
	class client_controller;
	class debug_controller;
	class envelope_controller;
	class group_controller;	
	class message_controller;
	class router_controller;

	class application_controller final : public QGuiApplication, public application, public entity_callback
	{
		Q_OBJECT;

	public:
		using ptr = application_controller*;

	private:
		static ptr s_ptr;

		bool m_enabled = true;
		QQmlApplicationEngine m_engine;
		std::shared_ptr<entity> m_backup;

		double m_ratio = 1.0;
		double m_font_ratio = 1.0;

		eja::clipboard* m_clipboard = nullptr;

		minimize* m_minimize = nullptr;
		notification* m_notification = nullptr;
		toast* m_toast = nullptr;
		vibrator* m_vibrator = nullptr;		

		// Controller
		chat_controller* m_chat = nullptr;
		client_controller* m_client = nullptr;
		debug_controller* m_debug = nullptr;
		envelope_controller* m_envelope = nullptr;
		group_controller* m_group = nullptr;		
		message_controller* m_message = nullptr;
		router_controller* m_router = nullptr;

	private:
		// Interface
		void on_update(const std::shared_ptr<entity> entity);

	private slots:
		void on_set_status(const std::shared_ptr<entity> entity);
		void on_state_changed(Qt::ApplicationState state);

	signals:
		void update(const std::shared_ptr<entity> entity);
		void set_status(const std::shared_ptr<entity> entity);
		void notification_clicked();

	public:
		application_controller(int argc, char* argv[]);

		// Interface
		Q_INVOKABLE virtual void init() override;
		Q_INVOKABLE virtual void shutdown() override;
		Q_INVOKABLE virtual void update() override;

		// Utility
		Q_INVOKABLE bool enabled() const { return m_enabled; }
		Q_INVOKABLE bool empty() const { return application::empty(); }
		Q_INVOKABLE int index() const { return m_index; }

		Q_INVOKABLE void add();
		Q_INVOKABLE void back();

		using application::load;
		virtual void load(const char* psz) override;
		Q_INVOKABLE void save() { application::save(); }
		void notify(const std::string& str) const;

		void toggle(const entity::ptr entity);
		void toggle();
		
		void start(const entity::ptr entity);
		void start();

		void stop(const entity::ptr entity);
		void stop();

		// Event
		virtual bool event(QEvent* e) override;

		// Has
		bool active() const { return applicationState() == Qt::ApplicationActive; }
		bool hidden() const { return applicationState() == Qt::ApplicationHidden; }
		bool inactive() const { return applicationState() == Qt::ApplicationInactive; }
		bool suspended() const { return applicationState() == Qt::ApplicationSuspended; }

		// Set
		virtual void set_entity(const entity::ptr entity) override;
		Q_INVOKABLE void set_entity(const int row);

		//
		// TODO: We need a generic static UI class
		//
		static void set_initial(const std::shared_ptr<entity> entity);

		// Get
		const chat_controller* get_chat_controller() const { return m_chat; }
		chat_controller* get_chat_controller() { return m_chat; }

		const client_controller* get_client_controller() const { return m_client; }
		client_controller* get_client_controller() { return m_client; }

		const debug_controller* get_debug_controller() const { return m_debug; }
		debug_controller* get_debug_controller() { return m_debug; }

		const group_controller* get_group_controller() const { return m_group; }
		group_controller* get_group_controller() { return m_group; }

		const envelope_controller* get_envelope_controller() const { return m_envelope; }
		envelope_controller* get_envelope_controller() { return m_envelope; }

		const message_controller* get_message_controller() const { return m_message; }
		message_controller* get_message_controller() { return m_message; }

		const router_controller* get_router_controller() const { return m_router; }
		router_controller* get_router_controller() { return m_router; }

		Q_INVOKABLE double get_ratio() const { return m_ratio; }
		Q_INVOKABLE double get_font_ratio() const { return m_font_ratio; }

		// Static
		static ptr create(int argc, char* argv[]);
		static ptr get() { return s_ptr; }
	};
}

#endif
