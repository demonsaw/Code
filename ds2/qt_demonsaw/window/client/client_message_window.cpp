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

#include <QHBoxLayout>
#include <QLabel>
#include <QScrollBar>
#include <QSplitter>
#include <QTabWidget>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

#include "client_message_window.h"
#include "command/client/request/client_group_request_command.h"
#include "component/time_component.h"
#include "component/chat_component.h"
#include "component/chat_idle_component.h"
#include "component/color_component.h"
#include "component/client/client_component.h"
#include "component/client/client_machine_component.h"
#include "component/client/client_option_component.h"
#include "component/router/router_component.h"
#include "window/client_window.h"
#include "window/main_window.h"
#include "resource/resource.h"

namespace eja
{
	// Constructor
	client_message_window::client_message_window(const entity::ptr entity, QWidget* parent /*= 0*/) : widget_window(entity, parent)
	{
		qRegisterMetaType<entity::ptr>("entity::ptr");

		create(parent);
		layout(parent);
		signal(parent);

		init();
	}

	// Interface
	void client_message_window::create(QWidget* parent /*= 0*/)
	{
		widget_window::create(parent);

		// Table
		m_message_table = new client_message_table(this);
		m_message_model = new client_message_model(m_entity, { "Time", "Name", "Message" }, { 72, 128, 72 }, this);
		init(m_message_table, m_message_model);

		m_client_table = new client_client_table(this);
		m_client_table->setMinimumWidth(160);
		m_client_table->setMaximumWidth(320);
		m_client_model = new client_client_model(m_entity, { "", "Name" }, { 28, 72 }, this);
		init(m_client_table, m_client_model);

		// Message
		m_text = new chat_text_edit(this);
		m_text->setObjectName("chat");
		m_text->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_text->setMinimumHeight(24);
		m_text->setFocusPolicy(Qt::StrongFocus);
		m_text->setTabChangesFocus(false);

		m_add_button = new QToolButton(this);
		m_add_button->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_add_button->setObjectName("chat");
		m_add_button->setIcon(QIcon(resource::tab::message));
		m_add_button->setToolTip("Send");
		m_add_button->setEnabled(false);
	}

	void client_message_window::layout(QWidget* parent /*= 0*/)
	{
		widget_window::layout(parent);

		// Button
		add_button(widget_button::remove, false);
		add_button(widget_button::clear);
		add_button(widget_button::refresh);

		// Message
		QWidget* hwidget = new QWidget(this);
		hwidget->setObjectName("chat");

		QHBoxLayout* hlayout = new QHBoxLayout(hwidget);
		hlayout->setContentsMargins(0, 0, 0, 0);
		hlayout->setSpacing(0);
		hlayout->setMargin(0);

		hlayout->addSpacing(3);
		hlayout->addWidget(m_add_button, 0, Qt::AlignTop | Qt::AlignRight);
		hlayout->addWidget(m_text);
		hwidget->setLayout(hlayout);

		// Table
		QSplitter* vsplitter = new QSplitter(Qt::Vertical);
		vsplitter->setContentsMargins(0, 0, 0, 0);
		vsplitter->setObjectName("chat");
		vsplitter->setChildrenCollapsible(false);
		vsplitter->addWidget(m_message_table);
		vsplitter->setStretchFactor(0, 32);
		vsplitter->addWidget(hwidget);
		vsplitter->setStretchFactor(1, 1);
		vsplitter->setHandleWidth(4);

		// Clients
		QSplitter* hsplitter = new QSplitter(Qt::Horizontal);
		hsplitter->setContentsMargins(0, 0, 0, 0);
		hsplitter->setObjectName("chat");
		hsplitter->setChildrenCollapsible(false);
		hsplitter->addWidget(vsplitter);
		hsplitter->setStretchFactor(0, 4);
		hsplitter->addWidget(m_client_table);
		hsplitter->setStretchFactor(1, 1);
		hsplitter->setHandleWidth(4);

		QVBoxLayout* vlayout = new QVBoxLayout(this);
		vlayout->setContentsMargins(0, 0, 0, 0);
		vlayout->setSpacing(0);
		vlayout->setMargin(0);
		vlayout->addWidget(hsplitter);

		setLayout(vlayout);
	}

	void client_message_window::signal(QWidget* parent /*= 0*/)
	{
		widget_window::signal(parent);

		// Signal
		connect(this, &client_message_window::alert, m_message_model, &client_message_model::on_alert);
		connect(m_text, &chat_text_edit::submit_text, this, static_cast<void (client_message_window::*)()>(&client_message_window::add));
		connect(m_text, &chat_text_edit::textChanged, this, &client_message_window::on_text_changed);		
		connect(m_client_table, &table_window::clicked, this, &client_message_window::on_list_clicked);
		connect(m_add_button, &QToolButton::clicked, [this]() { on_add(); });

		// Event
		m_message_table->installEventFilter(this);

		// Callback		
		auto& functions = m_message_table->get_functions();
		functions.add(table_callback::remove, function::create([this]() { on_remove(); }));
		functions.add(table_callback::clear, function::create([this]() { on_clear(); }));
		functions.add(table_callback::click, function::create([this]() { on_click(); }));
	}

	void client_message_window::init()
	{
		widget_window::init();

		// Callback
		add_callback(function_type::message);
	}

	void client_message_window::update()
	{
		m_client_table->update(); 
		m_message_table->update();
	}

	void client_message_window::refresh()
	{
		if (m_refresh_button)
		{
			m_refresh_button->setEnabled(false);
			QApplication::setOverrideCursor(Qt::WaitCursor);

			QTimer::singleShot(default_timeout::client::refresh, [this]()
			{
				m_refresh_button->setEnabled(true);
				QApplication::restoreOverrideCursor();
			});
		}

		// Command
		const auto owner = m_entity->shared_from_this();

		std::thread thread([&, owner]()
		{
			const auto socket = http_socket::create();

			try
			{
				const auto option = owner->get<client_option_component>();
				if (!option)
					return;

				const auto router = owner->get<router_component>();
				if (!router)
					return;

				const auto client_vector = m_entity->get<client_entity_vector_component>();
				if (!client_vector)
					return;

				// Socket				
				socket->set_timeout(option->get_socket_timeout());
				socket->open(router->get_address(), router->get_port());

				// Command
				const auto request_command = client_group_request_command::create(owner, socket);
				const auto request_status = request_command->execute();

				switch (request_status.get_code())
				{
					case http_code::ok:
					{
						break;
					}
					case http_code::none:
					{
						owner->log("HTTP Timeout (client)");
						break;
					}
					case http_code::not_found:
					{
						owner->log("HTTP 404 Not Found (client)");
						break;
					}
					case http_code::not_implemented:
					{
						owner->log("HTTP 501 Not Implemented (client)");
						break;
					}
					case http_code::bad_request:
					{
						// If the router sends a bad_request this means that it's overloaded, try again
						owner->log("HTTP 400 bad Request (client)");
						break;
					}
					case http_code::service_unavailable:
					{
						owner->log("HTTP 503 Service Unavailable (client)");
						break;
					}
					case http_code::internal_server_error:
					{
						owner->log("HTTP 500 Internal Server Error (client)");
						break;
					}
					case http_code::unauthorized:
					{
						owner->log("HTTP 401 Unauthorized (client)");
						break;
					}
					default:
					{
						owner->log("HTTP Unknown Code (client)");
						break;
					}
				}
			}
			catch (std::exception& e)
			{
				owner->log(e);
			}
			catch (...)
			{
				owner->log("Unknown Exception (client)");
			}

			socket->close();
		});

		thread.detach();
	}

	// Callback
	void client_message_window::add(const entity::ptr entity)
	{
		// Chat
		const auto chat = entity->get<chat_component>();
		assert(chat && (chat->get_type() == chat_type::client));
		if (!chat || (chat->get_type() != chat_type::client))
			return;

		// Internal
		if (!entity->has<client_component>())
		{
			m_message_model->add(entity);
			return;
		}

		// Client List
		const auto client_vector = m_entity->get<client_entity_vector_component>();
		if (!client_vector)
			return;

		// Client
		const auto client = entity->get<client_component>();
		if (!client)
			return;

		// Color
		const auto& name = client->get_name();
		const auto color = color_component::create(name);
		entity->add(color);

		// Time
		entity->add<time_component>();

		auto owner = client_vector->find_if([&](const entity::ptr e)
		{
			const auto c = e->get<client_component>();
			return c && (c->get_id() == client->get_id());
		});

		if (owner)
		{
			// Existing
			const auto message_vector = owner->get<message_entity_vector_component>();
			if (!message_vector)
				return;

			const auto client = owner->get<client_component>();
			if (client && (client->get_name() != name))
			{
				client->set_name(name);
				client->set_join_name(name);
				
				const auto color = color_component::create(name);
				owner->add(color);
			}

			if (message_vector == m_entity->get<message_entity_vector_component>())
			{
				// If currently active, go through the model framework
				m_message_model->add(entity);
			}
			else
			{

				message_vector->add(entity);

				// Chat
				const auto chat = owner->get<chat_component>();
				if (chat)
					chat->set_unread();

				emit alert();
			}
		}
		else
		{
			// New
			owner = entity::create();
			owner->add(client);

			// Color
			const auto& name = client->get_name();
			const auto color = color_component::create(name);
			owner->add(color);

			// Chat
			// HACK: This isn't really a good design - move to a chat_options_component?
			const auto chat = chat_component::create();
			chat->set_unread();
			owner->add(chat);

			// Option
			owner->add<client_option_component>(m_entity);

			// Message
			const auto message_vector = message_entity_vector_component::create();
			message_vector->add(entity);
			owner->add(message_vector);

			m_client_model->add(owner);
			emit alert();
		}

		emit added(entity);
	}

	void client_message_window::remove(const entity::ptr entity)
	{
		m_message_model->remove(entity);
	}

	void client_message_window::clear(const entity::ptr entity)
	{
		m_selected_entity = nullptr;
		m_message_model->clear();
		m_client_model->clear();

		emit cleared(entity);
	}

	// Slot
	void client_message_window::on_add()
	{
		assert(thread::main());

		if (!m_enabled)
			return;

		if (!selected())
			return;

		const auto machine = m_entity->get<client_machine_component>();
		if (!machine || !machine->running())
			return;

		const auto qmessage = m_text->toPlainText().trimmed();
		if (qmessage.isEmpty())
			return;

		// UI
		m_text->clear();
		m_text->setFocus();

		m_enabled = false;
		m_add_button->setEnabled(false);

		QTimer::singleShot(default_message::timeout, [this]()
		{
			m_enabled = true;
			const auto qmessage = m_text->toPlainText().trimmed();
			m_add_button->setEnabled(static_cast<size_t>(qmessage.length()) >= default_message::min_text);
		});

		// Chat		
		const auto chat = chat_component::create();
		chat->set_type(chat_type::client);
		const auto message = qmessage.toStdString();
		chat->set_text(message);

		const auto entity = entity::create();
		entity->add(chat);

		// Model
		m_message_model->add(entity);

		// Client		
		const auto idle = m_entity->get<chat_idle_component>();
		if (idle)
		{
			const auto selected = get_selected();
			if (selected)
				idle->add(message, selected);
		}

		// Tab
		on_tab_clicked(static_cast<int>(client_tab::message));
	}

	void client_message_window::on_remove()
	{
		assert(thread::main());

		m_text->setFocus();
		m_message_table->remove();

		// Tab
		on_tab_clicked(static_cast<int>(client_tab::message));
	}

	void client_message_window::on_clear()
	{
		assert(thread::main());

		m_text->clear();
		m_text->setFocus();
		m_add_button->setEnabled(false);
		m_remove_button->setEnabled(false);
		m_message_table->clear();

		// Tab
		on_tab_clicked(static_cast<int>(client_tab::message));
	}

	void client_message_window::on_clear(const entity::ptr entity)
	{
		assert(thread::main());

		m_client_table->clearSelection();
	}

	void client_message_window::on_refresh(const entity::ptr entity)
	{
		assert(thread::main());

		if (m_selected_entity)
		{
			const auto client_vector = m_entity->get<client_entity_vector_component>();
			if (!client_vector)
			{
				m_message_model->set_component<message_entity_vector_component>();
				return;
			}

			const auto row = client_vector->get(m_selected_entity);
			if (row == type::npos)
			{
				m_message_model->set_component<message_entity_vector_component>();
				return;
			}

			const auto index = m_client_model->index(row, 0);
			if (!index.isValid())
			{
				m_message_model->set_component<message_entity_vector_component>();
				return;
			}

			const auto proxy = m_client_table->get_proxy_model();
			const auto& sindex = proxy->mapFromSource(index);
			if (!sindex.isValid())
			{
				m_message_model->set_component<message_entity_vector_component>();
				return;
			}

			m_client_table->setCurrentIndex(sindex);

			m_message_table->refresh();
		}
		else
		{
			m_client_table->clearSelection();

			m_message_model->set_component<message_entity_vector_component>();
		}

		// Focus
		m_text->clear();
		m_text->setFocus();

		set_status(client_statusbar::message, m_message_model->rowCount());
	}

	void client_message_window::on_list_clicked(const QModelIndex& index)
	{
		assert(thread::main());

		m_selected_entity = get_entity(index);
		if (!m_selected_entity)
			return;

		const auto message_vector = m_selected_entity->get<message_entity_vector_component>();
		if (!message_vector)
			return;

		m_message_model->set_component(message_vector);
		m_text->setFocus();

		const auto qmessage = m_text->toPlainText().trimmed();
		m_add_button->setEnabled(static_cast<size_t>(qmessage.length()) >= default_message::min_text);

		// Chat
		const auto chat = m_selected_entity->get<chat_component>();
		if (chat)
			chat->set_read();

		// Scroll
		m_message_table->refresh();
		m_message_table->scroll();

		set_status(client_statusbar::message, m_message_model->rowCount());
	}

	void client_message_window::on_text_changed()
	{
		assert(thread::main());

		if (!m_enabled)
			return;

		if (!selected())
			return;

		const auto machine = m_entity->get<client_machine_component>();
		if (!machine || !machine->running())
			return;

		const auto qmessage = m_text->toPlainText().trimmed();
		m_add_button->setEnabled(static_cast<size_t>(qmessage.length()) >= default_message::min_text);

		if (static_cast<size_t>(qmessage.size()) > default_message::max_text)
		{
			const auto text = qmessage.left(default_message::max_text);
			m_text->setPlainText(text);
		}
	}

	void client_message_window::on_tab_clicked(int index)
	{
		assert(thread::main());

		if (static_cast<int>(client_statusbar::message) != index)
			return;

		const auto mw = main_window::get_instance();
		const auto ew = mw->get_entity_window(m_entity);
		if (ew)
		{
			const auto tw = ew->get_main_tab();
			if (tw)
			{
				const auto index = static_cast<int>(client_statusbar::message);
				tw->setTabIcon(index, QIcon(resource::tab::message));
			}
		}
	}

	void client_message_window::on_click()
	{
		assert(thread::main());

		const auto enabled = m_message_table->selectionModel()->selectedIndexes().count() > 0;
		m_remove_button->setEnabled(enabled);
	}

	// Utility
	void client_message_window::post_create()
	{
		// Tab
		const auto mw = main_window::get_instance();
		const auto ew = mw->get_entity_window(m_entity);
		if (ew)
		{
			const auto tw = ew->get_main_tab();
			if (tw)
				connect(tw, &QTabWidget::tabBarClicked, this, &client_message_window::on_tab_clicked);
		}
	}

	bool client_message_window::selected() const
	{
		const auto selected = m_client_table->selectionModel()->selectedIndexes();

		return !selected.empty();
	}

	// Event
	void client_message_window::showEvent(QShowEvent* event)
	{
		// Timer
		QTimer::singleShot(0, m_text, SLOT(setFocus()));
		QTimer::singleShot(0, m_text, SLOT(selectAll()));

		// Event
		QWidget::showEvent(event);
	}

	bool client_message_window::eventFilter(QObject* obj, QEvent* e)
	{
		if (e->type() == QEvent::KeyPress)
		{
			const auto ke = static_cast<QKeyEvent*>(e);

			switch (ke->key())
			{
				case Qt::Key_Up:
				case Qt::Key_Down:
				case Qt::Key_PageUp:
				case Qt::Key_PageDown:
				case Qt::Key_Shift:
				{
					return QObject::eventFilter(obj, e);
				}
			}

			if (!(ke->modifiers() & Qt::ControlModifier))
			{
				m_text->setFocus();
				return qApp->sendEvent(m_text, e);
			}
		}

		return QObject::eventFilter(obj, e);
	}

	// Accessor
	entity::ptr client_message_window::get_entity(const QModelIndex& index) const
	{
		if (!index.isValid())
			return nullptr;

		const auto proxy = m_client_table->get_proxy_model();
		const auto sindex = proxy->mapToSource(index);
		if (!sindex.isValid())
			return nullptr;

		const auto owner = m_client_model->get_entity();
		if (!owner)
			return nullptr;

		const auto client_vector = owner->get<client_entity_vector_component>();
		if (!client_vector)
			return nullptr;

		const size_t row = static_cast<size_t>(sindex.row());
		if (row == type::npos)
			return nullptr;

		return client_vector->get(row);
	}

	entity::ptr client_message_window::get_selected() const
	{
		const auto client_vector = m_entity->get<client_entity_vector_component>();
		if (!client_vector)
			return nullptr;

		const auto selected = m_client_table->selectionModel()->selectedIndexes();
		if (selected.empty())
			return nullptr;

		const auto& sindex = *selected.begin();
		if (!sindex.isValid())
			return nullptr;

		const auto proxy = m_client_table->get_proxy_model();
		const auto index = proxy->mapToSource(sindex);
		if (!index.isValid())
			return nullptr;

		const auto owner = m_client_model->get_entity();
		if (!owner)
			return nullptr;

		const size_t row = static_cast<size_t>(index.row());
		if (row == type::npos)
			return nullptr;

		return client_vector->get(row);
	}

	size_t client_message_window::get_selected_count() const
	{
		return m_client_table->selectionModel()->selectedIndexes().count();
	}
}
