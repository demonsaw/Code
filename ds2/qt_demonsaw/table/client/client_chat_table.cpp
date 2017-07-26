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

#include <QAction>
#include <QHeaderView>
#include <QMenu>
#include <QPoint>
#include <QScrollBar>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>

#include "client_chat_table.h"
#include "component/client/client_component.h"
#include "component/client/client_option_component.h"
#include "delegate/client_name_delegate.h"
#include "delegate/chat/chat_message_delegate.h"
#include "delegate/chat/chat_time_delegate.h"
#include "model/client/client_chat_model.h"
#include "window/client_window.h"
#include "window/main_window.h"
#include "resource/resource.h"

namespace eja
{
	// Constructor
	client_chat_table::client_chat_table(QWidget* parent /*= 0*/) : table_window(parent)
	{
		create(parent);
		layout(parent);
		signal(parent);

		setTextElideMode(Qt::ElideNone);
		setMinimumHeight(96);
	}

	// Interface
	void client_chat_table::init()
	{
		table_window::init();

		// Function
		const auto model = qobject_cast<client_chat_model*>(get_source_model());
		if (model)
		{
			model->set_resize_function([this](const int row)
			{
				if (row < int_max)
					resizeRowToContents(row);
				else
					resizeRowsToContents();

				scroll();
			});
		}			
	}

	void client_chat_table::create(QWidget* parent /*= 0*/)
	{
		table_window::create(parent);

		// Action
		m_mute_action = new QAction(tr("Mute"), this);
		m_mute_action->setIcon(QIcon(resource::menu::mute));

		m_unmute_action = new QAction(tr("Unmute"), this);
		m_unmute_action->setIcon(QIcon(resource::menu::unmute));

		m_remove_action = new QAction(tr("Remove"), this);
		m_remove_action->setIcon(QIcon(resource::menu::remove));

		m_clear_action = new QAction(tr("Clear"), this);
		m_clear_action->setIcon(QIcon(resource::menu::clear));

		m_refresh_action = new QAction(tr("Refresh"), this);
		m_refresh_action->setIcon(QIcon(resource::menu::refresh));
	}

	void client_chat_table::layout(QWidget* parent /*= 0*/)
	{
		// Delegate
		setItemDelegateForColumn(client_chat_model::column::time, new chat_time_delegate(this));
		setItemDelegateForColumn(client_chat_model::column::name, new client_name_delegate(this));
		setItemDelegateForColumn(client_chat_model::column::message, new chat_message_delegate(this));

		// Menu
		setContextMenuPolicy(Qt::CustomContextMenu);
	}

	void client_chat_table::signal(QWidget* parent /*= 0*/)
	{
		// Menu
		connect(m_mute_action, &QAction::triggered, this, &client_chat_table::mute);
		connect(m_unmute_action, &QAction::triggered, this, &client_chat_table::unmute);

		connect(m_remove_action, &QAction::triggered, [this]
		{
			const auto function = m_functions.get(table_callback::remove);
			if (function)
				function->call();
		});

		connect(m_clear_action, &QAction::triggered, [this]
		{
			const auto function = m_functions.get(table_callback::clear);
			if (function)
				function->call();
		});

		connect(m_refresh_action, &QAction::triggered, [this]
		{
			const auto function = m_functions.get(table_callback::refresh);
			if (function)
				function->call();
		});

		connect(this, &client_chat_table::customContextMenuRequested, this, &client_chat_table::show_menu);

		// Table
		const auto hheader = horizontalHeader();
		connect(hheader, &QHeaderView::sectionClicked, [=](int index) { resizeRowsToContents(); });
	}

	// Event
	bool client_chat_table::muteable(const QModelIndexList& selected) const
	{
		if (selected.empty())
			return false;

		const auto proxy = get_proxy_model();
		const auto model = get_source_model();
		const auto owner = model->get_entity();

		const auto chat_vector = owner->get<chat_entity_vector_component>();
		if (!chat_vector)
			return false;

		const auto mute_map = owner->get<mute_entity_map_component>();
		if (!mute_map)
			return false;

		for (const auto& index : selected)
		{
			if (!index.isValid())
				continue;

			const auto source_index = proxy->mapToSource(index);
			if (!source_index.isValid())
				continue;

			const auto row = static_cast<size_t>(source_index.row());
			const auto entity = chat_vector->get(row);

			if (entity)
			{
				const auto client = entity->get<client_component>();
				if (client && !mute_map->has(client->get_id()))
					return true;
			}
		}

		return false;
	}

	bool client_chat_table::unmuteable(const QModelIndexList& selected) const
	{
		if (selected.empty())
			return false;

		const auto proxy = get_proxy_model();
		const auto model = get_source_model();
		const auto owner = model->get_entity();

		const auto chat_vector = owner->get<chat_entity_vector_component>();
		if (!chat_vector)
			return false;

		const auto mute_map = owner->get<mute_entity_map_component>();
		if (!mute_map)
			return false;

		for (const auto& index : selected)
		{
			if (!index.isValid())
				continue;

			const auto source_index = proxy->mapToSource(index);
			if (!source_index.isValid())
				continue;

			const auto row = static_cast<size_t>(source_index.row());
			const auto entity = chat_vector->get(row);

			if (entity)
			{
				const auto client = entity->get<client_component>();
				if (client && mute_map->has(client->get_id()))
					return true;
			}
		}

		return false;
	}

	// Slot
	void client_chat_table::refresh()
	{		
		resizeRowsToContents();

		table_window::refresh();
	}

	void client_chat_table::mute()
	{
		QModelIndexList selected = selectionModel()->selectedRows();
		if (selected.empty())
			return;

		const auto proxy = get_proxy_model();
		const auto model = get_source_model();
		const auto owner = model->get_entity();

		const auto chat_vector = owner->get<chat_entity_vector_component>();
		if (!chat_vector)
			return;

		const auto mute_map = owner->get<mute_entity_map_component>();
		if (!mute_map)
			return;

		for (const auto& index : selected)
		{
			if (!index.isValid())
				continue;

			const auto source_index = proxy->mapToSource(index);
			if (!source_index.isValid())
				continue;

			const auto row = static_cast<size_t>(source_index.row());
			const auto entity = chat_vector->get(row);

			if (entity)
			{
				const auto client = entity->get<client_component>();
				if (client)
					mute_map->add(client->get_id(), entity);
			}
		}
	}

	void client_chat_table::unmute()
	{
		QModelIndexList selected = selectionModel()->selectedRows();
		if (selected.empty())
			return;

		const auto proxy = get_proxy_model();
		const auto model = get_source_model();
		const auto owner = model->get_entity();

		const auto chat_vector = owner->get<chat_entity_vector_component>();
		if (!chat_vector)
			return;

		const auto mute_map = owner->get<mute_entity_map_component>();
		if (!mute_map)
			return;

		for (const auto& index : selected)
		{
			if (!index.isValid())
				continue;

			const auto source_index = proxy->mapToSource(index);
			if (!source_index.isValid())
				continue;

			const auto row = static_cast<size_t>(source_index.row());
			const auto entity = chat_vector->get(row);

			if (entity)
			{
				const auto client = entity->get<client_component>();
				if (client)
					mute_map->remove(client->get_id());
			}
		}
	}

	void client_chat_table::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
	{
		const auto function = m_functions.get(table_callback::click);
		if (function)
			function->call();

		// Event
		table_window::selectionChanged(selected, deselected);
	}

	void client_chat_table::show_menu(const QPoint& pos)
	{
		const auto index = indexAt(pos);
		if (!index.isValid())
			return;

		QMenu menu(this);

		if (muteable())
			menu.addAction(m_mute_action);
		else if (unmuteable())
			menu.addAction(m_unmute_action);

		if (!menu.isEmpty())
			menu.addSeparator();

		menu.addAction(m_remove_action);
		menu.addAction(m_clear_action);
		menu.addAction(m_refresh_action);

		menu.exec(viewport()->mapToGlobal(pos));
	}
}