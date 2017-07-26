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

#include <QAction>
#include <QHeaderView>
#include <QLabel>
#include <QSplitter>
#include <QString>
#include <QMenu>
#include <QPoint>
#include <QPushButton>
#include <QTimer>
#include <QToolBar>
#include <QVBoxLayout>

#include "component/chat_component.h"
#include "component/endpoint_component.h"
#include "component/notify_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/client/client_service_component.h"
#include "component/time/time_component.h"
#include "delegate/endpoint_delegate.h"
#include "delegate/time_delegate.h"
#include "delegate/chat/chat_text_delegate.h"
#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "entity/entity_table_view.h"
#include "font/font_awesome.h"
#include "model/client/client_pm_model.h"
#include "thread/thread_info.h"
#include "widget/text_edit_widget.h"
#include "widget/client/client_pm_widget.h"
#include "window/main_window.h"

namespace eja
{
	// Constructor
	client_pm_widget::client_pm_widget(const entity::ptr entity, QWidget* parent /*= 0*/) : entity_widget(entity, "Private Message", fa::envelope, callback_type::chat, parent)
	{
		assert(thread_info::main());
		
		// Action
		m_remove_action = make_action("Remove", fa::minus);
		m_update_action = make_action("Update", fa::bolt);

		// Menu	
		add_button("Remove", fa::minus, [&]() { m_table->clear(); });
		add_button("Update", fa::bolt, [&]() { update(); });
		add_button("Popout", fa::clone, [&]() { popout(); });
		add_button("Close", fa::close, [&]() { close(); });

		// Layout
		auto hlayout = new QHBoxLayout;
		hlayout->setContentsMargins(0, 0, 0, 0);
		hlayout->setSpacing(0);
		hlayout->setMargin(0);

		hlayout->addWidget(m_icon);
		hlayout->addWidget(m_title);
		hlayout->addStretch(1);
		hlayout->addWidget(m_toolbar);

		m_titlebar = new QWidget(this);
		m_titlebar->installEventFilter(this);
		m_titlebar->setObjectName("menubar");
		m_titlebar->setLayout(hlayout);
		setTitleBarWidget(m_titlebar);

		// Pane
		m_text = new text_edit_widget(this);
		m_text->setObjectName("chat");
		m_text->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		m_text->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		m_text->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_text->setFocusPolicy(Qt::StrongFocus);
		m_text->setTabChangesFocus(false);
		m_text->setMinimumHeight(26);

		auto endpoint = m_entity->get<endpoint_component>();
		m_title_text = QString("Private Message: %1").arg(endpoint->get_name().c_str());
		m_title->setText(m_title_text);

		m_table = new entity_table_view(m_entity, this);
		m_model = new client_pm_model(m_entity, { "Time", "Name", "Message" }, { 96, 128, 256 }, this);
		init(m_table, m_model);

		// Header		
		const auto hheader = m_table->horizontalHeader();
		hheader->setSortIndicator(client_pm_model::column::time, Qt::SortOrder::AscendingOrder);
		hheader->setStretchLastSection(true);

		// Callback			
		add_callback(callback_action::add, [&](const entity::ptr entity) { add(entity); });
		add_callback(callback_action::update, [&](const entity::ptr entity) { update(); });
		add_callback(callback_action::clear, [&](const entity::ptr entity){ m_model->clear(); });

		m_enter_button = new QPushButton(this);		
		m_enter_button->setObjectName("chat");
		m_enter_button->setToolTip("Send");
		m_enter_button->setEnabled(false);

		const auto p = m_entity->get_parent();
		endpoint = p->get<endpoint_component>();
		m_enter_button->setText(QString("%1:").arg(endpoint->get_name().c_str()));

		QHBoxLayout* input_layout = new QHBoxLayout;
		input_layout->setContentsMargins(0, 0, 0, 0);
		input_layout->setSpacing(0);
		input_layout->setMargin(0);

		input_layout->addWidget(m_enter_button, 0, Qt::AlignTop);
		input_layout->addWidget(m_text);

		QWidget* widget = new QWidget(this);
		widget->setObjectName("border");
		widget->setLayout(input_layout);

		QSplitter* splitter = new QSplitter(Qt::Vertical);
		splitter->setContentsMargins(0, 0, 0, 0);
		splitter->setChildrenCollapsible(false);
		splitter->addWidget(m_table);
		splitter->setStretchFactor(0, 32);
		splitter->addWidget(widget);
		splitter->setStretchFactor(1, 1);
		splitter->setHandleWidth(4);
		setWidget(splitter);

		// Delegate
		m_table->setItemDelegateForColumn(client_pm_model::column::time, new time_delegate(m_table));
		m_table->setItemDelegateForColumn(client_pm_model::column::name, new endpoint_delegate(m_table));
		m_table->setItemDelegateForColumn(client_pm_model::column::text, new chat_text_delegate(m_table));

		// Signal
		connect(m_table, &entity_table_view::customContextMenuRequested, this, &client_pm_widget::show_menu);
		connect(this, &client_pm_widget::visibilityChanged, this, &client_pm_widget::on_visibility_changed);		
		connect(m_table->horizontalHeader(), &QHeaderView::sectionResized, this, &client_pm_widget::on_resize);
		connect(m_table, &entity_table_view::resize, [&]() { on_update(); });

		connect(m_remove_action, &QAction::triggered, [&]() { m_table->remove(); });
		connect(m_update_action, &QAction::triggered, [&]() { m_table->update(); });

		connect(m_enter_button, &font_button::clicked, [&]() { on_add(); });
		connect(m_text, &text_edit_widget::submit_text, [&]() { on_add(); });
		connect(m_text, &text_edit_widget::textChanged, this, &client_pm_widget::on_text_changed);

		// Sort
		m_table->sortByColumn(client_pm_model::column::time);
	}

	// Interface
	void client_pm_widget::on_add()
	{
		assert(thread_info::main());

		// Spam
		if (!m_enabled || !m_enter_button->isEnabled())
			return;

		const auto qtext = m_text->toPlainText().trimmed();
		const auto qlength = static_cast<size_t>(qtext.length() - qtext.count(' '));
		if (qlength < chat::min_text)
			return;

		// Service
		const auto service = m_entity->find<client_service_component>();
		if (service->valid())
		{
			const auto endpoint = m_entity->get<endpoint_component>();
			if (endpoint->has_id())
			{
				const auto parent = m_entity->get_parent();
				const auto entity = entity_factory::create_client_chat(parent);
				const auto chat = entity->get<chat_component>();
				chat->set_text(qtext.toStdString());
				
				// Callback
				const auto client = parent->get<endpoint_component>();
				if (client->unmuted())
				{
					chat->set_client(endpoint->get_id());
					m_entity->call(callback_type::chat, callback_action::add, entity);
					service->async_chat(entity, m_entity);
				}
				else
				{
					// Do not allow self-muted pm's to flood the network
					if (!m_entity->equals<endpoint_component>(parent))
						service->async_chat(entity, m_entity);
				}
			}
		}

		// Cursor
		m_text->clear();
		m_text->setFocus();

		// Spam
		m_enabled = false;
		m_enter_button->setEnabled(false);

		QTimer::singleShot(ui::enter, [this]()
		{
			m_enabled = true;
			const auto qtext = m_text->toPlainText().trimmed();
			m_enter_button->setEnabled(static_cast<size_t>(qtext.length()) >= chat::min_text);
		});
	}

	void client_pm_widget::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());
		
		const auto chat = entity->get<chat_component>();
		if (!chat->has_client())
			return;

		const auto endpoint = m_entity->get<endpoint_component>();
		if (endpoint->get_id() != chat->get_client())
			return;

		// Model
		m_model->add(entity);

		// Format
		const auto row = m_model->get_row(entity);
		if (row != npos)
		{
			// TODO: THe following forces row height of 23
			// 
			//setRowHeight(int row, int height);
			m_table->resizeRowToContents(row);			
			m_table->scroll();
		}

		// Notify
		if (!visibleRegion().isEmpty())
		{
			const auto parent = m_entity->get_parent();
			const auto notify = parent->get<notify_component>();

			if (notify->has_pm())
			{
				notify->set_pm(false);
				const auto window = main_window::get();
				window->notify(parent);
			}
		}		
	}

	void client_pm_widget::on_update()
	{
		assert(thread_info::main());

		// Spam
		auto button = get_button(fa::bolt);
		if (button)
		{
			button->setEnabled(false);
			QTimer::singleShot(ui::refresh, [&]() { set_enabled(fa::bolt); });
		}

		m_table->update();
		m_table->resizeRowsToContents();

		// Name
		const auto parent = m_entity->get_parent();
		const auto endpoint = parent->get<endpoint_component>();
		m_enter_button->setText(QString("%1:").arg(endpoint->get_name().c_str()));
	}

	// Event
	void client_pm_widget::on_resize()
	{
		assert(thread_info::main());

		m_table->resizeRowsToContents();
	}

	void client_pm_widget::on_text_changed()
	{
		assert(thread_info::main());

		const auto qtext = m_text->toPlainText().trimmed();
		m_enter_button->setEnabled(m_enabled && static_cast<size_t>(qtext.length()) >= chat::min_text);

		if (static_cast<size_t>(qtext.size()) > chat::max_text)
		{
			const auto ltext = qtext.left(chat::max_text);
			m_text->setPlainText(ltext);
		}
	}

	void client_pm_widget::showEvent(QShowEvent* event)
	{
		if (!visibleRegion().isEmpty())
		{
			m_text->setFocus();
			m_text->selectAll();
		}

		// Event
		entity_widget::showEvent(event);
	}

	void client_pm_widget::on_visibility_changed(bool visible)
	{
		assert(thread_info::main());

		if (visible && !visibleRegion().isEmpty())
		{
			m_text->setFocus();
			m_text->selectAll();

			// Notify
			const auto parent = m_entity->get_parent();
			const auto notify = parent->get<notify_component>();

			if (notify->has_pm())
			{
				notify->set_pm(false);
				const auto window = main_window::get();
				window->notify(parent);
			}
		}
	}

	void client_pm_widget::closeEvent(QCloseEvent* event)
	{
		assert(thread_info::main());

		QDockWidget::closeEvent(event);

		// Callback
		m_entity->call(callback_type::chat, callback_action::destroy, m_entity);

		// HACK: Defer deletion so we don't crash due to timer
		QTimer::singleShot(ui::enter, [this]() { delete this; });
	}

	// Menu
	void client_pm_widget::show_menu(const QPoint& pos)
	{
		assert(thread_info::main());

		const auto index = m_table->indexAt(pos);
		if (!index.isValid())
			return;

		QMenu menu(this);
		menu.addAction(m_remove_action);
		menu.addAction(m_update_action);

		menu.exec(m_table->viewport()->mapToGlobal(pos));
	}
}
