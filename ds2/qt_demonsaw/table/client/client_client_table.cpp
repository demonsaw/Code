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

#include "client_client_table.h"
#include "component/chat_component.h"
#include "component/client/client_component.h"
#include "delegate/client_name_delegate.h"
#include "delegate/message/message_number_delegate.h"
#include "model/client/client_client_model.h"
#include "resource/resource.h"

namespace eja
{
	// Constructor
	client_client_table::client_client_table(QWidget* parent /*= 0*/) : table_window(parent)
	{
		create(parent);
		layout(parent);
		signal(parent);
	}

	// Interface
	void client_client_table::create(QWidget* parent /*= 0*/)
	{
		table_window::create(parent);

		// HACK: Should be done via create function (virtual)
		//
		// Sort
		const auto hheader = horizontalHeader();
		hheader->setSortIndicator(get_sort_column(), Qt::SortOrder::AscendingOrder);
		hheader->setObjectName("client");

		setSelectionMode(QAbstractItemView::SingleSelection);
		setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		setAlternatingRowColors(false);

		// Action
		m_mute_action = new QAction(tr("Mute"), this);
		m_mute_action->setIcon(QIcon(resource::menu::mute));

		m_unmute_action = new QAction(tr("Unmute"), this);
		m_unmute_action->setIcon(QIcon(resource::menu::unmute));
	}

	void client_client_table::layout(QWidget* parent /*= 0*/)
	{
		// Delegate
		setItemDelegateForColumn(client_client_model::name, new client_name_delegate(this));
		setItemDelegateForColumn(client_client_model::number, new message_number_delegate(this));

		// Menu
		setContextMenuPolicy(Qt::CustomContextMenu);

		// Button
		add_button(table_button::remove, false);
		add_button(table_button::clear);
	}

	void client_client_table::signal(QWidget* parent /*= 0*/)
	{
		// Menu
		connect(m_mute_action, &QAction::triggered, this, &client_client_table::mute);
		connect(m_unmute_action, &QAction::triggered, this, &client_client_table::unmute);

		connect(this, &client_client_table::customContextMenuRequested, this, &client_client_table::show_menu);
	}

	// Event
	bool client_client_table::muteable(const QModelIndexList& selected) const
	{
		if (selected.empty())
			return false;

		const auto proxy = get_proxy_model();
		const auto model = get_source_model();
		const auto owner = model->get_entity();

		const auto client_vector = owner->get<client_entity_vector_component>();
		if (!client_vector)
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
			const auto entity = client_vector->get(row);

			if (entity)
			{
				const auto client = entity->get<client_component>();
				if (client && !mute_map->has(client->get_id()))
					return true;
			}
		}

		return false;
	}

	bool client_client_table::unmuteable(const QModelIndexList& selected) const
	{
		if (selected.empty())
			return false;

		const auto proxy = get_proxy_model();
		const auto model = get_source_model();
		const auto owner = model->get_entity();

		const auto client_vector = owner->get<client_entity_vector_component>();
		if (!client_vector)
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
			const auto entity = client_vector->get(row);

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
	void client_client_table::mute()
	{
		QModelIndexList selected = selectionModel()->selectedRows();
		if (selected.empty())
			return;

		const auto proxy = get_proxy_model();
		const auto model = get_source_model();
		const auto owner = model->get_entity();

		const auto client_vector = owner->get<client_entity_vector_component>();
		if (!client_vector)
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
			const auto entity = client_vector->get(row);

			if (entity)
			{
				const auto client = entity->get<client_component>();
				if (client)
					mute_map->add(client->get_id(), entity);
			}
		}
	}

	void client_client_table::unmute()
	{
		QModelIndexList selected = selectionModel()->selectedRows();
		if (selected.empty())
			return;

		const auto proxy = get_proxy_model();
		const auto model = get_source_model();
		const auto owner = model->get_entity();

		const auto client_vector = owner->get<client_entity_vector_component>();
		if (!client_vector)
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
			const auto entity = client_vector->get(row);

			if (entity)
			{
				const auto client = entity->get<client_component>();
				if (client)
					mute_map->remove(client->get_id());
			}
		}
	}

	void client_client_table::show_menu(const QPoint& pos)
	{
		const auto index = indexAt(pos);
		if (!index.isValid())
			return;

		QMenu menu(this);

		if (muteable())
			menu.addAction(m_mute_action);
		else if (unmuteable())
			menu.addAction(m_unmute_action);

		menu.exec(viewport()->mapToGlobal(pos));
	}
}
