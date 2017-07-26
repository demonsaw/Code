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
#include <QDockWidget>
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
#include "component/client/client_option_component.h"
#include "component/client/client_service_component.h"
#include "component/time/time_component.h"
#include "delegate/endpoint_delegate.h"
#include "delegate/time_delegate.h"
#include "delegate/chat/chat_row_delegate.h"
#include "delegate/chat/chat_text_delegate.h"
#include "entity/entity.h"
#include "entity/entity_factory.h"
#include "entity/entity_table_view.h"
#include "entity/entity_window.h"
#include "font/font_awesome.h"
#include "model/client/client_chat_model.h"
#include "thread/thread_info.h"
#include "utility/client/client_util.h"
#include "widget/text_edit_widget.h"
#include "widget/client/client_chat_widget.h"
#include "widget/client/client_pm_widget.h"
#include "window/main_window.h"

namespace eja
{
	// Constructor
	client_chat_widget::client_chat_widget(const entity::ptr entity, QWidget* parent /*= 0*/) : entity_widget(entity, "Chat", fa::comment, callback_type::chat, parent)
	{
		assert(thread_info::main());
		
		// Action
		m_chat_action = make_action("Message", fa::envelope);
		m_mute_action = make_action("Mute", fa::ban);
		m_unmute_action = make_action("Unmute", fa::circleo);
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

		m_table = new entity_table_view(m_entity, this);
		m_model = new client_chat_model(m_entity, { "", "Time", "Name", "Message" }, { 24, 96, 128, 256 }, this);
		init(m_table, m_model);

		// Header
		const auto hheader = m_table->horizontalHeader();
		hheader->setSectionResizeMode(client_chat_model::column::row, QHeaderView::Fixed);
		hheader->setSortIndicator(client_chat_model::column::time, Qt::SortOrder::AscendingOrder);
		hheader->setStretchLastSection(true);
		hheader->setObjectName("stretch");

		// Callback
		m_table->setColumnWidth(client_chat_model::column::row, 24);
		add_callback(callback_action::create, [&](const entity::ptr entity) { create(entity); });
		add_callback(callback_action::destroy, [&](const entity::ptr entity) { destroy(entity); });
		add_callback(callback_action::add, [&](const entity::ptr entity) { add(entity); });
		add_callback(callback_action::update, [&](const entity::ptr entity) { update(); });
		add_callback(callback_action::clear, [&](const entity::ptr entity){ m_model->clear(); });

		m_enter_button = new QPushButton(this);
		const auto endpoint = m_entity->get<endpoint_component>();
		m_enter_button->setText(QString("%1:").arg(endpoint->get_name().c_str()));
		m_enter_button->setObjectName("chat");
		m_enter_button->setToolTip("Send");
		m_enter_button->setEnabled(false);

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
		m_table->setItemDelegateForColumn(client_chat_model::column::row, new chat_row_delegate(m_table));
		m_table->setItemDelegateForColumn(client_chat_model::column::time, new time_delegate(m_table));
		m_table->setItemDelegateForColumn(client_chat_model::column::name, new endpoint_delegate(m_table));
		m_table->setItemDelegateForColumn(client_chat_model::column::text, new chat_text_delegate(m_table));

		// Signal
		connect(this, &client_chat_widget::visibilityChanged, this, &client_chat_widget::on_visibility_changed);
		connect(m_table, &entity_table_view::customContextMenuRequested, this, &client_chat_widget::show_menu);
		connect(m_table, &entity_table_view::doubleClicked, this, &client_chat_widget::on_double_click);
		connect(m_table->horizontalHeader(), &QHeaderView::sectionResized, this, &client_chat_widget::on_resize);

		// NOTE: Needed to format 1st row properly
		connect(m_table, &entity_table_view::resize, [&]()
		{
			if (m_model->rowCount() <= 1)
				on_update();
		});

		connect(m_chat_action, &QAction::triggered, this, &client_chat_widget::on_chat);
		connect(m_mute_action, &QAction::triggered, [&]() { on_mute(true); });
		connect(m_unmute_action, &QAction::triggered, [&]() { on_mute(false); });
		connect(m_remove_action, &QAction::triggered, [&]() { m_table ->remove(); });
		connect(m_update_action, &QAction::triggered, [&]() { update(); });

		connect(m_enter_button, &QPushButton::clicked, [&]() { on_add(); });
		connect(m_text, &text_edit_widget::submit_text, [&]() { on_add(); });
		connect(m_text, &text_edit_widget::textChanged, this, &client_chat_widget::on_text_changed);

		// Sort
		m_table->sortByColumn(client_chat_model::column::time);
	}

	// Interface
	void client_chat_widget::on_create(const entity::ptr entity)
	{
		assert(thread_info::main());

		// TODO: Crashes here sometimes!
		if (!entity->has_parent())
			return;

		// HACK: We need this in a worker thread to avoid mutex re-entrancy
		QTimer::singleShot(0, [this, entity]()
		{
			// Search
			const auto main = main_window::get();
			const auto window = main->get_window();

			if (window)
			{
				client_pm_widget* widget = nullptr;
				const auto endpoint = entity->find<endpoint_component>();

				thread_lock(m_map);
				const auto it = m_map.find(endpoint->get_id());
				
				if (it == m_map.end())
				{
					widget = new client_pm_widget(entity);
					auto pair = std::make_pair(endpoint->get_id(), widget);
					m_map.insert(pair);

					widget->setAllowedAreas(Qt::TopDockWidgetArea);
					window->addDockWidget(Qt::TopDockWidgetArea, widget);
					window->tabifyDockWidget(this, widget);
				}
				else
				{
					widget = it->second;
				}

				// HACK: Need to do this to raise the tab to foreground
				QTimer::singleShot(0, [widget]() { widget->raise(); });
			}
		});
	}

	void client_chat_widget::on_destroy(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto endpoint = entity->get<endpoint_component>();

		thread_lock(m_map);
		m_map->erase(endpoint->get_id());
	}

	void client_chat_widget::on_add()
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
		const auto service = m_entity->get<client_service_component>();
		if (service->valid())
		{
			const auto entity = entity_factory::create_client_chat(m_entity);
			const auto chat = entity->get<chat_component>();
			chat->set_text(qtext.toStdString());

			// Callback
			const auto client = m_entity->get<endpoint_component>();
			if (client->unmuted())
				m_entity->call(callback_type::chat, callback_action::add, entity);

			service->async_chat(entity);
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

	void client_chat_widget::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		// Redirect to pm window?
		const auto chat = entity->get<chat_component>();		

		if (chat->has_client())
		{
			thread_lock(m_map);

			const auto it = m_map.find(chat->get_client());
			if (it == m_map->end())
			{
				// Add to private chat list
				const auto parent = entity->get_parent();
				if (parent)
				{
					const auto chat_list = parent->get<chat_list_component>();
					chat_list->push_back(entity);
				}
			}

			// NOTE: Add pm response to main chat as well
			if (!entity->has<endpoint_component>())
				return;
			
			// Notify
			const auto window = main_window::get();

			if (visibleRegion().isEmpty())
			{
				const auto notify = m_entity->get<notify_component>();
				if (!notify->has_pm())
				{
					// Notify
					notify->set_pm(true);					
					window->notify(m_entity);
				}
			}

			if (!window->isActiveWindow())
			{
				const auto option = m_entity->get<client_option_component>();
				if (option->has_flash())
					window->flash();

				if (option->has_beep())
					QApplication::beep();
			}

			// Open pm tab
			if (it == m_map->end())
			{
				const auto parent = entity->get_parent();
				if (parent)
				{
					// HACK: We need this in a worker thread to avoid mutex re-entrancy
					QTimer::singleShot(0, [this, parent]()
					{
						// Search
						const auto main = main_window::get();
						const auto window = main->get_window(parent->get_parent());

						if (window)
						{
							auto widget = new client_pm_widget(parent);
							widget->setAllowedAreas(Qt::TopDockWidgetArea);
							window->addDockWidget(Qt::TopDockWidgetArea, widget);
							window->tabifyDockWidget(this, widget);

							const auto endpoint = parent->find<endpoint_component>();
							auto pair = std::make_pair(endpoint->get_id(), widget);

							thread_lock(m_map);
							m_map.insert(pair);
						}
					});
				}
			}
		}
		else
		{
			// Notify
			const auto window = main_window::get();

			if (visibleRegion().isEmpty())
			{
				const auto notify = m_entity->get<notify_component>();
				if (!notify->has_chat())
				{
					// Notify
					const auto endpoint = entity->get<endpoint_component>();
					if (endpoint->get_type() == endpoint_type::client)
					{
						notify->set_chat(true);						
						window->notify(m_entity);
					}					
				}
			}

			if (!window->isActiveWindow())
			{
				const auto option = m_entity->get<client_option_component>();
				if (option->has_flash())
					window->flash();
				
				if (option->has_beep())
					QApplication::beep();
			}
		}

		m_model->add(entity);

		const auto row = m_model->get_row(entity);
		if (row != npos)
		{
			// TODO: The following forces row height of 23
			//
			//setRowHeight(int row, int height);
			m_table->resizeRowToContents(row);
			m_table->scroll();
		}
	}

	void client_chat_widget::on_update()
	{
		assert(thread_info::main());

		// Spam
		auto button = get_button(fa::bolt);
		if (button)
		{
			button->setEnabled(false);
			QTimer::singleShot(ui::refresh, [&]() { set_enabled(fa::bolt); });
		}

		m_model->update();
		m_table->resizeRowsToContents();

		// Name
		const auto endpoint = m_entity->get<endpoint_component>();
		m_enter_button->setText(QString("%1:").arg(endpoint->get_name().c_str()));
	}

	void client_chat_widget::on_double_click(const QModelIndex& index)
	{
		assert(thread_info::main());

		if (!index.isValid())
			return;

		const auto proxy = m_table->get_proxy_model();
		const auto sindex = proxy->mapToSource(index);
		if (!sindex.isValid())
			return;

		switch (sindex.column())
		{
			case client_chat_model::column::row:
			case client_chat_model::column::name:
			case client_chat_model::column::time:
			{
				const auto entity = m_model->get_entity(sindex);
				if (entity)
				{
					const auto parent = entity->get_parent();
					if (parent)
						m_entity->call(callback_type::chat, callback_action::create, parent);
				}

				return;
			}
		}

		m_table->double_click();
	}

	void client_chat_widget::on_chat()
	{
		assert(thread_info::main());

		m_table->execute([&](const QModelIndex& index)
		{
			const auto entity = m_model->get_entity(index);
			if (entity)
			{
				const auto parent = entity->get_parent();
				if (parent)
					m_entity->call(callback_type::chat, callback_action::create, parent);
			}
		});
	}

	void client_chat_widget::on_resize()
	{
		assert(thread_info::main());

		m_table->resizeRowsToContents();
	}	

	void client_chat_widget::on_text_changed()
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

	// Utility
	void client_chat_widget::on_mute(const bool mute)
	{
		assert(thread_info::main());

		const auto selected = m_table->selectionModel()->selectedRows();

		if (!selected.empty())
		{
			const auto proxy = m_table->get_proxy_model();

			for (const auto& index : selected)
			{
				if (index.isValid())
				{
					const auto sindex = proxy->mapToSource(index);
					if (sindex.isValid())
					{
						const auto entity = m_model->get_entity(sindex);
						if (entity)
						{
							const auto parent = entity->get_parent();
							if (!parent)
								continue;

							const auto endpoint = parent->get<endpoint_component>();
							endpoint->set_mute(mute);

							// Self?
							if (m_entity->equals<endpoint_component>(parent))
							{
								// HACK: Associate m_entity and the client list version
								//
								const auto entity = client_util::get_client(m_entity, endpoint->get_id());
								if (entity)
								{
									const auto endpoint = entity->get<endpoint_component>();
									endpoint->set_mute(mute);
								}								
							}

							// Callback
							m_entity->call(callback_type::client, callback_action::update, parent);
						}
					}
				}
			}
		}
	}

	// Event
	void client_chat_widget::showEvent(QShowEvent* event)
	{
		if (!visibleRegion().isEmpty())
		{
			m_text->setFocus();
			m_text->selectAll();
		}

		// Event
		entity_widget::showEvent(event);
	}

	void client_chat_widget::on_visibility_changed(bool visible)
	{
		assert(thread_info::main());

		if (visible && !visibleRegion().isEmpty())
		{
			m_text->setFocus();
			m_text->selectAll();

			 // Notify
			const auto notify = m_entity->get<notify_component>();

			if (notify->has_chat() || notify->has_pm())
			{
				notify->clear();
				const auto window = main_window::get();
				window->notify(m_entity);
			}
		}
	}

	// Menu
	void client_chat_widget::show_menu(const QPoint& pos)
	{
		assert(thread_info::main());

		const auto index = m_table->indexAt(pos);
		if (!index.isValid())
			return;

		QMenu menu(this);

		// Mute
		const auto state = get_mute();
		if ((state & mute_state::unmuted) == mute_state::unmuted)
			menu.addAction(m_mute_action);

		// Unmute
		if ((state & mute_state::muted) == mute_state::muted)
			menu.addAction(m_unmute_action);

		if (has_parent())
			menu.addAction(m_chat_action);

		if (!menu.isEmpty())
			menu.addSeparator();

		menu.addAction(m_remove_action);
		menu.addAction(m_update_action);

		menu.exec(m_table->viewport()->mapToGlobal(pos));
	}

	// Has
	bool client_chat_widget::has_parent() const
	{
		assert(thread_info::main());

		const auto selected = m_table->selectionModel()->selectedRows();

		if (!selected.empty())
		{
			const auto proxy = m_table->get_proxy_model();

			for (const auto& index : selected)
			{
				if (index.isValid())
				{
					const auto sindex = proxy->mapToSource(index);
					if (sindex.isValid())
					{
						const auto entity = m_model->get_entity(sindex);
						if (!entity || !entity->has_parent())
							continue;

						if (!entity->is_parent(m_entity))
							return true;
					}
				}
			}
		}

		return false;
	}

	// Get
	size_t client_chat_widget::get_mute()
	{
		assert(thread_info::main());

		size_t state = mute_state::none;

		const auto selected = m_table->selectionModel()->selectedRows();

		if (!selected.empty())
		{
			const auto proxy = m_table->get_proxy_model();

			for (const auto& index : selected)
			{
				if (index.isValid())
				{
					const auto sindex = proxy->mapToSource(index);
					if (sindex.isValid())
					{
						const auto entity = m_model->get_entity(sindex);
						if (entity)
						{
							const auto parent = entity->get_parent();
							if (parent)
							{
								const auto endpoint = parent->get<endpoint_component>();
								state |= endpoint->unmuted() ? mute_state::unmuted : mute_state::muted;
							}							
						}
					}
				}
			}
		}

		return state;
	}
}
