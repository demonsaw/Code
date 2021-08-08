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
#include <QFont>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QListWidgetItem>
#include <QSplitter>
#include <QMenu>
#include <QPoint>
#include <QRect>
#include <QScrollBar>
#include <QTimer>
#include <QVBoxLayout>

#include "component/chat_component.h"
#include "component/room_component.h"
#include "component/callback/callback.h"
#include "component/client/client_component.h"
#include "component/client/client_service_component.h"

#include "delegate/chat/chat_delegate.h"
#include "delegate/chat/chat_row_delegate.h"

#include "entity/entity.h"
#include "entity/entity_table_view.h"
#include "factory/client_factory.h"
#include "font/font_awesome.h"
#include "model/room_model.h"
#include "resource/resource.h"
#include "thread/thread_info.h"
#include "widget/text_edit_widget.h"
#include "widget/emoji_widget.h"
#include "widget/room_widget.h"
#include "window/main_window.h"

namespace eja
{
	// Constructor
	room_widget::room_widget(const entity::ptr& entity, QWidget* parent /*= 0*/) : entity_dock_widget(entity, "Room", fa::hash_tag, parent)
	{
		assert(thread_info::main());

		// Ratio
		const auto ratio = resource::get_ratio();

		// Action
		m_message_action = make_action(" Message", fa::envelope);
		m_mute_action = make_action(" Mute", fa::ban);
		m_unmute_action = make_action(" Unmute", fa::circle_o);
		m_remove_action = make_action(" Remove", fa::minus);
		m_clear_action = make_action(" Clear", fa::trash);
		m_close_action = make_action(" Close", fa::close);

		// Menu
		add_button("Close", fa::close, [&]() { close(); });

		// Layout
		const auto hlayout = resource::get_hbox_layout();
		hlayout->addWidget(m_icon);
		hlayout->addWidget(m_title);
		hlayout->addStretch(1);
		hlayout->addWidget(m_toolbar);

		m_titlebar = new QWidget(this);
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
		m_text->setMinimumHeight(30);

		const auto text_layout = resource::get_vbox_layout();
		text_layout->setContentsMargins(0, resource::get_chat_edit_margin(), 0, 0);
		text_layout->addWidget(m_text);

		const auto room = m_entity->get<room_component>();
		m_title_text = QString::fromStdString(room->get_name());
		m_title->setText(m_title_text);
		setWindowTitle(m_title_text);

		m_table = new entity_table_view(m_entity, this);
		m_model = new room_model(m_entity, this);
		init(m_table, m_model);

		// Header
		const auto header = m_table->horizontalHeader();
		header->setObjectName("stretch");
		header->setSectionResizeMode(room_model::column::row, QHeaderView::Fixed);
		header->setSectionResizeMode(room_model::column::message, QHeaderView::Stretch);

		m_table->setItemDelegateForColumn(room_model::column::row, new chat_row_delegate(m_table));
		m_table->setItemDelegateForColumn(room_model::column::message, new chat_delegate(m_table));
		m_table->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
		m_table->set_column_sizes();

		// Scrollbar
		auto scrollbar = m_table->verticalScrollBar();
		scrollbar->setSingleStep(resource::get_row_height());

		QFont font(font_family::font_awesome);
		font.setPixelSize(resource::get_large_font_size());

		// Emoji
		m_emoji = new QPushButton(this);
		m_emoji->setFont(QFont(font_family::font_awesome));
		m_emoji->setText(QString(fa::smile_o));
		m_emoji->setObjectName("emoji");
		m_emoji->setCheckable(true);
		m_emoji->setFont(font);

		const auto emoji_layout = resource::get_vbox_layout();
		emoji_layout->setContentsMargins(resource::get_vertical_spacing(), resource::get_chat_edit_margin(), 0, 0);
		emoji_layout->addWidget(m_emoji);

		QWidget* emoji_widget = new QWidget(this);
		emoji_widget->setLayout(emoji_layout);
		emoji_widget->setObjectName("emoji");

		m_emoji_menu = main_window::get()->get_emoji();

		m_enter_button = new QPushButton(this);
		m_enter_button->setObjectName("chat_button");
		m_enter_button->setToolTip("Send");
		m_enter_button->setEnabled(false);

		const auto button_container = resource::get_vbox_layout();
		button_container->setContentsMargins(0, resource::get_chat_edit_margin(), 0, 0);
		button_container->addWidget(m_enter_button);

		QWidget* button_widget = new QWidget(this);
		button_widget->setLayout(button_container);
		button_widget->setObjectName("chat_button");

		const auto owner = m_entity->get_owner();
		const auto client = owner->get<client_component>();
		auto qname = QString::fromStdString(client->get_name());
		if (client->is_troll())
			qname += " [TROLL]:";

		m_enter_button->setText(qname);

		// Icon
		style_icon();

		// Layout
		const auto input_layout = resource::get_hbox_layout();
		input_layout->setSpacing(2 * ratio);
		input_layout->addWidget(emoji_widget, 0, Qt::AlignTop);
		input_layout->addWidget(button_widget, 0, Qt::AlignTop);
		input_layout->addLayout(text_layout);

		QWidget* widget = new QWidget(this);
		widget->setObjectName("border_solid");
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

		// Timer
		m_resize_timer = new QTimer(this);
		m_resize_timer->setSingleShot(true);

		m_spam_timer = new QTimer(this);
		m_spam_timer->setSingleShot(true);

		// Event
		m_table->installEventFilter(this);

		// Signal
		connect(this, &room_widget::visibilityChanged, this, &room_widget::on_visibility_changed);
		connect(this, &room_widget::customContextMenuRequested, this, &room_widget::show_menu);

		connect(m_table, &entity_table_view::customContextMenuRequested, this, &room_widget::show_menu);
		connect(m_table, &entity_table_view::doubleClicked, this, &room_widget::on_double_click);
		connect(header, &QHeaderView::sectionResized, this, &room_widget::on_resize);
		connect(header, &QHeaderView::customContextMenuRequested, [&](const QPoint& pos) { show_header_menu({ "Icon" }, { room_model::column::row }, m_table, m_model, pos); });

		connect(m_spam_timer, &QTimer::timeout, this, [&]
		{
			m_enabled = true;
			const auto qtext = m_text->toPlainText().trimmed();

			if (qtext.length() >= chat::min_text)
			{
				m_enter_button->setEnabled(true);
				m_enter_button->setStyleSheet(m_enter_style);
			}
		});

		connect(m_resize_timer, &QTimer::timeout, this, [&]
		{
			m_table->resizeRowsToContents();
			m_table->scroll();
		});

		connect(m_emoji, &QPushButton::clicked, this, [&] { show_emoji(true); });
		connect(m_enter_button, &QPushButton::clicked, [&]() { on_add(); });
		connect(m_text, &text_edit_widget::submit_text, [&]() { on_add(); });
		connect(m_text, &text_edit_widget::textChanged, this, &room_widget::on_text_changed);

		connect(m_message_action, &QAction::triggered, this, &room_widget::on_message);
		connect(m_mute_action, &QAction::triggered, [&]() { on_mute(true); });
		connect(m_unmute_action, &QAction::triggered, [&]() { on_mute(false); });
		connect(m_remove_action, &QAction::triggered, [&]() { m_table->remove(); });
		connect(m_clear_action, &QAction::triggered, [&]() { m_table->clear(); });
		connect(m_close_action, &QAction::triggered, [&]() { close(); });
	}

	// Interface
	void room_widget::on_add()
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
		const auto owner = m_entity->get_owner();
		const auto client_service = owner->get<client_service_component>();
		if (client_service->valid())
		{
			// Add
			const auto entity = client_factory::create_chat(owner);
			const auto chat = entity->get<chat_component>();
			const auto text = qtext.toStdString();
			chat->set_text(text);

			m_model->add(entity);
			resize();

			// Callback
			const auto room = m_entity->get<room_component>();
			client_service->async_chat(room->get_id(), text);
		}

		// Cursor
		m_text->clear();
		m_text->setFocus();

		// Spam
		m_enabled = false;
		m_enter_button->setEnabled(false);
		m_enter_button->setStyleSheet("");
		m_spam_timer->start(ui::enter);
	}

	void room_widget::resize()
	{
		assert(thread_info::main());

		const auto row = m_model->size() - 1;
		m_table->resizeRowToContents(row);
		m_table->scroll();
	}

	void room_widget::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		// Unselect
		auto editting = false;

		if (m_table->editting())
		{
			const auto chat_list = m_entity->get<chat_list_component>();
			if (!chat_list->empty())
			{
				const auto owner = entity->get_owner();
				const auto e = chat_list->back();
				if (e->is_owner(owner))
				{
					const auto index = m_model->get_index(e);
					if (index.isValid())
					{
						const auto model = m_table->selectionModel();
						model->select(index, QItemSelectionModel::Deselect);
						editting = true;
					}
				}
			}
		}

		m_model->add(entity);

		// Select
		if (editting)
		{
			const auto chat_list = m_entity->get<chat_list_component>();
			m_table->selectRow(chat_list->size() - 1);
		}

		resize();
	}

	void room_widget::on_update()
	{
		assert(thread_info::main());

		// Input
		const auto owner = m_entity->get_owner();
		const auto client = owner->get<client_component>();
		auto qname = QString::fromStdString(client->get_name());
		if (client->is_troll())
			qname += " [TROLL]:";

		m_enter_button->setText(qname);

		style_icon();

		m_model->update();
	}

	void room_widget::on_update(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto row = m_model->get_row(entity);
		if (row != npos)
		{
			m_table->resizeRowToContents(row);
			m_table->scroll();
		}
	}

	void room_widget::on_clear()
	{
		assert(thread_info::main());

		m_model->clear();
	}

	void room_widget::on_message()
	{
		assert(thread_info::main());

		m_table->execute([&](const QModelIndex& index)
		{
			const auto entity = m_model->get_entity(index);
			if (entity)
			{
				const auto client_entity = entity->get_owner();
				if (!m_entity->is_owner(client_entity))
					m_entity->async_call(callback::pm | callback::create, client_entity);
			}
		});
	}

	void room_widget::on_double_click(const QModelIndex& index)
	{
		assert(thread_info::main());

		const auto entity = m_model->get_entity(index);
		if (entity)
		{
			const auto client_entity = entity->get_owner();
			if (!m_entity->is_owner(client_entity))
			{
				switch (index.column())
				{
					case room_model::column::message:
					{
						const auto rect = m_table->visualRect(index);
						const auto pos = m_table->viewport()->mapFromGlobal(QCursor::pos());
						if ((rect.y() < 0) || (pos.y() >= (rect.y() + resource::get_row_height())))
							break;

						// break;
					}
					case room_model::column::row:
					{
						const auto client = client_entity->get<client_component>();
						if (!client->is_mute())
						{
							const auto owner = client_entity->get_owner();
							const auto client_map = owner->get<client_map_component>();
							{
								thread_lock(client_map);
								if (client_map->find(client->get_id()) != client_map->end())
									m_entity->async_call(callback::pm | callback::create, client_entity);
							}
						}

						break;
					}
				}
			}
		}
	}

	// Utility
	void room_widget::auto_scroll()
	{
		assert(thread_info::main());

		QTimer::singleShot(100, [&]() { m_table->scrollToBottom(); });
	}

	void room_widget::on_mute(const bool value)
	{
		assert(thread_info::main());

		m_table->execute([&](const QModelIndex index)
		{
			const auto entity = m_model->get_entity(index);
			if (entity)
			{
				const auto client_entity = entity->get_owner();
				if (!m_entity->is_owner(client_entity))
				{
					const auto client = client_entity->get<client_component>();
					client->set_mute(value);

					// Client
					m_entity->async_call(callback::client | callback::update, client_entity);

					// Chat
					m_entity->async_call(callback::chat | callback::update);

					// Room
					m_entity->async_call(callback::room | callback::update);

					// Browse
					if (client->is_share())
						m_entity->async_call(callback::browse | callback::update, client_entity);

					// Pm
					m_entity->async_call(callback::pm | callback::update, client_entity);

					// Service
					const auto owner = m_entity->get_owner();
					const auto client_service = owner->get<client_service_component>();
					client_service->async_mute(client_entity, value);
				}
			}
		});
	}

	void room_widget::on_resize()
	{
		assert(thread_info::main());

		// Get Viewport
		QRect view_rect = m_table->viewport()->rect();

		// Get Top And Bottom Rows
		const auto top_index = m_table->indexAt(view_rect.topLeft());
		if (!top_index.isValid())
			return;

		const auto bottom_index = m_table->indexAt(QPoint(view_rect.x(), view_rect.y() + view_rect.height() - resource::get_row_height() / 2));
		int end = bottom_index.row();
		if (!bottom_index.isValid())
			end = m_table->model()->rowCount() - 1;

		// Resize Rows in Viewport
		int start = top_index.row();
		for (int i = start; i <= end; ++i)
			m_table->resizeRowToContents(i);

		m_table->scroll();

		m_resize_timer->start(sec_to_ms(1));
	}

	void room_widget::style_icon()
	{
		const auto room = m_entity->get<room_component>();
		if (room->has_color())
		{
			const auto& app = main_window::get_app();
			if (app.is_user_colors())
			{
				const auto owner = m_entity->get_owner();
				const auto client = owner->get<client_component>();
				m_enter_style = QString("QPushButton {color: rgba(%1, %2, %3, %4);}").arg(client->get_red()).arg(client->get_green()).arg(client->get_blue()).arg(client->get_alpha());

				QColor color = QColor(room->get_color());
				QString rgb = QString("rgba(%1, %2, %3, %4)").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha());
				m_icon->setStyleSheet(QString("QLabel#menubar_icon {color: %1; padding-left: 5px;}").arg(rgb));
			}
			else
			{
				m_enter_style.clear();
				m_enter_button->setStyleSheet(m_enter_style);
				m_icon->setStyleSheet("");
			}
		}
	}

	void room_widget::on_text_changed()
	{
		assert(thread_info::main());

		const auto qtext = m_text->toPlainText().trimmed();
		if (m_enabled && (qtext.length() >= chat::min_text))
		{
			m_enter_button->setEnabled(true);
			m_enter_button->setStyleSheet(m_enter_style);

			if (qtext.size() > chat::max_text)
			{
				const auto ltext = qtext.left(chat::max_text);
				m_text->setPlainText(ltext);
			}
		}
		else
		{
			m_enter_button->setEnabled(false);
			m_enter_button->setStyleSheet("");
		}
	}

	void room_widget::on_visibility_changed(bool visible)
	{
		assert(thread_info::main());

		if (visible && !visibleRegion().isEmpty())
		{
			m_text->setFocus();
			m_text->selectAll();

			if (m_init)
			{
				m_init = false;
				m_table->resizeRowsToContents();
				m_table->scrollToBottom();
			}
		}
	}

	// Has
	bool room_widget::has_client() const
	{
		assert(thread_info::main());

		const auto selected = m_table->selectionModel()->selectedRows();
		if (!selected.empty())
		{
			const auto owner = m_entity->get_owner();
			const auto client_map = owner->get<client_map_component>();

			for (const auto& index : selected)
			{
				const auto entity = m_model->get_entity(index);
				if (!entity || entity->is_owner(owner))
					return false;

				const auto client_entity = entity->get_owner();
				if (!client_entity->has<client_component>())
					return false;

				const auto client = client_entity->get<client_component>();
				{
					thread_lock(client_map);
					if (client_map->find(client->get_id()) == client_map->end())
						return false;
				}
			}
		}

		return true;
	}

	// Is
	bool room_widget::is_muted() const
	{
		assert(thread_info::main());

		const auto selected = m_table->selectionModel()->selectedRows();
		if (!selected.empty())
		{
			for (const auto& index : selected)
			{
				const auto entity = m_model->get_entity(index);
				if (entity)
				{
					const auto client_entity = entity->get_owner();
					const auto client = client_entity->get<client_component>();
					if (client->is_mute())
						return true;
				}
			}
		}

		return false;
	}

	// Event
	void room_widget::closeEvent(QCloseEvent* event)
	{
		assert(thread_info::main());

		if (isVisible())
		{
			hide();

			// Callback
			const auto owner = m_entity->get_owner();
			owner->async_call(callback::room | callback::destroy, m_entity);
			owner->async_call(callback::client | callback::clear, m_entity);
		}
	}

	bool room_widget::eventFilter(QObject* object, QEvent* event)
	{
		if (event->type() == QEvent::KeyPress)
		{
			const auto ke = static_cast<QKeyEvent*>(event);

			switch (ke->key())
			{
				case Qt::Key_Up:
				case Qt::Key_Down:
				case Qt::Key_PageUp:
				case Qt::Key_PageDown:
				case Qt::Key_Delete:
				case Qt::Key_Shift:
				case Qt::Key_Escape:
				{
					return entity_dock_widget::eventFilter(object, event);
				}
			}

			if (!(ke->modifiers() & Qt::ControlModifier))
			{
				m_text->setFocus();
				return qApp->sendEvent(m_text, event);
			}
		}

		return entity_dock_widget::eventFilter(object, event);
	}

	void room_widget::resizeEvent(QResizeEvent* event)
	{
		if (m_emoji_enabled)
			size_emoji();
	}

	void room_widget::size_emoji()
	{
		const auto ratio = resource::get_ratio();

		const size_t emoji_cell = 30 * ratio;
		const size_t emoji_height = 200 * ratio;
		const size_t emoji_width = width() / 2;

		m_emoji_menu->setFixedSize(emoji_width - (emoji_width % emoji_cell) - (8 * ratio), emoji_height);
		m_emoji_menu->move(rect().left() + width() / 4, rect().bottom() - emoji_height - m_text->height() - (4 * ratio));
	}

	void room_widget::show_emoji(bool show)
	{
		if (show)
		{
			m_emoji_enabled = true;
			m_emoji_menu->setParent(this);
			m_emoji_menu->set_activator_button(m_emoji);
			m_emoji->setStyleSheet(m_enter_style);

			connect(m_emoji_menu, &emoji_widget::emoji_clicked, this, [&](QListWidgetItem* item) { m_cursor.insertText(item->text()); });
			connect(m_emoji_menu, &emoji_widget::closing, this, [&] { show_emoji(false); });
			size_emoji();

			m_emoji_menu->toggle();
			m_cursor = m_text->textCursor();
		}
		else
		{
			m_emoji_enabled = false;
			m_emoji_menu->setParent(main_window::get());
			m_emoji->setStyleSheet("QPushButton {background: transparent; border: 0; font-size: 20px; }");

			disconnect(m_emoji_menu, 0, 0, 0);
		}
	}

	// Menu
	void room_widget::show_menu(const QPoint& pos)
	{
		assert(thread_info::main());

		QMenu menu;
		menu.setStyleSheet(QString("QMenu::item { height:%1px; width:%2px; }").arg(resource::get_row_height()).arg(resource::get_menu_width()));

		const auto owner = m_entity->get_owner();
		const auto client_service = owner->get<client_service_component>();

		if (client_service->valid() && !m_model->empty())
		{
			const auto index = m_table->indexAt(pos);
			if (index.isValid())
			{
				if (has_client())
				{
					if (!is_muted())
					{
						menu.addAction(m_message_action);
						menu.addAction(m_mute_action);
						menu.addSeparator();
					}
					else
					{
						menu.addAction(m_unmute_action);
						menu.addSeparator();
					}
				}

				menu.addAction(m_remove_action);
			}

			menu.addAction(m_clear_action);
			menu.addSeparator();
		}

		menu.addAction(m_close_action);

		menu.exec(m_table->viewport()->mapToGlobal(pos));
	}
}
