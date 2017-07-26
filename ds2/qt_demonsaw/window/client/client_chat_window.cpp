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
#include <QKeyEvent>
#include <QLabel>
#include <QSplitter>
#include <QTimer>
#include <QTabWidget>
#include <QToolButton>
#include <QVBoxLayout>

#include "client_chat_window.h"
#include "component/chat_component.h"
#include "component/chat_idle_component.h"
#include "component/color_component.h"
#include "component/client/client_component.h"
#include "component/client/client_machine_component.h"
#include "window/client_window.h"
#include "window/main_window.h"
#include "resource/resource.h"

namespace eja
{
	// Constructor
	client_chat_window::client_chat_window(const entity::ptr entity, QWidget* parent /*= 0*/) : widget_window(entity, parent)
	{
		create(parent);
		layout(parent);
		signal(parent);

		init();
	}

	// Interface
	void client_chat_window::update()
	{
		m_chat_table->update();
	}

	void client_chat_window::create(QWidget* parent /*= 0*/)
	{
		widget_window::create(parent);

		// Table
		m_chat_table = new client_chat_table(this);
		m_chat_model = new client_chat_model(m_entity, { "Time", "Name", "Message" }, { 72, 128, 72 }, this);
		init(m_chat_table, m_chat_model);

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
		m_add_button->setIcon(QIcon(resource::tab::chat));
		m_add_button->setToolTip("Send");
		m_add_button->setEnabled(false);
	}

	void client_chat_window::layout(QWidget* parent /*= 0*/)
	{
		widget_window::layout(parent);

		// Button
		add_button(widget_button::remove, false);
		add_button(widget_button::clear);
		add_button(widget_button::refresh);

		// Layout		
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

		// Splitter
		QSplitter* splitter = new QSplitter(Qt::Vertical);
		splitter->setContentsMargins(0, 0, 0, 0);
		splitter->setObjectName("chat");
		splitter->setChildrenCollapsible(false);
		splitter->addWidget(m_chat_table);
		splitter->setStretchFactor(0, 20);
		splitter->addWidget(hwidget);
		splitter->setStretchFactor(1, 1);
		splitter->setHandleWidth(4);

		QVBoxLayout* vlayout = new QVBoxLayout(this);
		vlayout->setContentsMargins(0, 0, 0, 0);
		vlayout->setSpacing(0);
		vlayout->setMargin(0);
		vlayout->addWidget(splitter);

		setLayout(vlayout);
	}

	void client_chat_window::signal(QWidget* parent /*= 0*/)
	{
		widget_window::signal(parent);

		// Signal
		connect(m_text, &chat_text_edit::submit_text, this, &client_chat_window::on_add);
		connect(m_text, &chat_text_edit::textChanged, this, &client_chat_window::on_text_changed);
		connect(m_add_button, &QToolButton::clicked, [this]() { on_add(); });
	
		// Event
		m_chat_table->installEventFilter(this);

		// Callback		
		auto& functions = m_chat_table->get_functions();
		functions.add(table_callback::remove, function::create([this]() { on_remove(); }));
		functions.add(table_callback::clear, function::create([this]() { on_clear(); }));
		functions.add(table_callback::click, function::create([this]() { on_click(); }));
	}

	void client_chat_window::refresh()
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

		emit refreshed();
	}

	// Slot
	void client_chat_window::on_add()
	{
		assert(thread::main());

		if (!m_enabled)
			return;

		const auto qmessage = m_text->toPlainText().trimmed();
		if (qmessage.isEmpty())
			return;

		const auto machine = m_entity->get<client_machine_component>();
		if (!machine || !machine->running())
			return;

		// UI
		m_text->clear();
		m_text->setFocus();

		m_enabled = false;
		m_add_button->setEnabled(false);

		QTimer::singleShot(default_chat::timeout, [this]()
		{
			m_enabled = true;
			const auto qmessage = m_text->toPlainText().trimmed();
			m_add_button->setEnabled(static_cast<size_t>(qmessage.length()) >= default_chat::min_text);
		});

		// Chat
		const auto chat = chat_component::create();
		chat->set_type(chat_type::group);

		const auto message = qmessage.toStdString();
		chat->set_text(message);

		// Entity
		const auto entity = entity::create();
		entity->add(chat);

		// Model
		m_chat_model->add(entity);

		// Queue
		const auto idle = m_entity->get<chat_idle_component>();
		if (idle)
			idle->add(message);

		// Tab
		on_tab_clicked(static_cast<int>(client_tab::chat));
	}

	void client_chat_window::on_remove()
	{
		assert(thread::main());

		m_text->setFocus();
		m_chat_table->remove();

		// Tab
		on_tab_clicked(static_cast<int>(client_tab::chat));
	}

	void client_chat_window::on_clear()
	{
		assert(thread::main());

		m_text->clear();
		m_text->setFocus();
		m_add_button->setEnabled(false);
		m_remove_button->setEnabled(false);
		m_chat_table->clear();

		// Tab
		on_tab_clicked(static_cast<int>(client_tab::chat));
	}

	void client_chat_window::on_refresh()
	{
		assert(thread::main());

		m_chat_table->refresh();

		// Tab
		on_tab_clicked(static_cast<int>(client_tab::chat));
	}

	void client_chat_window::on_click()
	{
		assert(thread::main());

		const auto enabled = m_chat_table->selectionModel()->selectedRows().count() > 0;
		m_remove_button->setEnabled(enabled);
	}

	void client_chat_window::on_text_changed()
	{
		assert(thread::main());

		if (!m_enabled)
			return;

		const auto qmessage = m_text->toPlainText().trimmed();
		m_add_button->setEnabled(static_cast<size_t>(qmessage.length()) >= default_chat::min_text);

		if (static_cast<size_t>(qmessage.size()) > default_chat::max_text)
		{
			const auto text = qmessage.left(default_chat::max_text);
			m_text->setPlainText(text);
		}
	}

	void client_chat_window::on_tab_clicked(int index)
	{
		assert(thread::main());

		if (static_cast<int>(client_statusbar::chat) != index)
			return;

		const auto mw = main_window::get_instance();
		const auto ew = mw->get_entity_window(m_entity);
		if (ew)
		{
			const auto tw = ew->get_main_tab();
			if (tw)
			{
				const auto index = static_cast<int>(client_statusbar::chat);
				tw->setTabIcon(index, QIcon(resource::tab::chat));
			}
		}
	}

	// Utility
	void client_chat_window::post_create()
	{
		// Tab
		const auto mw = main_window::get_instance();
		const auto ew = mw->get_entity_window(m_entity);
		if (ew)
		{
			const auto tw = ew->get_main_tab();
			if (tw)
				connect(tw, &QTabWidget::tabBarClicked, this, &client_chat_window::on_tab_clicked);
		}
	}

	// Event
	void client_chat_window::showEvent(QShowEvent* event)
	{
		// Timer
		QTimer::singleShot(0, m_text, SLOT(setFocus()));
		QTimer::singleShot(0, m_text, SLOT(selectAll()));

		// Event
		QWidget::showEvent(event);
	}

	bool client_chat_window::eventFilter(QObject* obj, QEvent* e)
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
}
