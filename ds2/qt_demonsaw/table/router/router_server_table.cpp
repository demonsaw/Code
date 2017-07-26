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
#include <QToolBar>
#include <QToolButton>

#include "router_server_table.h"
#include "delegate/server/server_status_delegate.h"
#include "model/router/router_server_model.h"
#include "resource/resource.h"

namespace eja
{
	// Constructor
	router_server_table::router_server_table(QWidget* parent /*= 0*/) : table_window(parent)
	{ 
		create(parent); 
		layout(parent);
		signal(parent);
	}

	// Interface
	void router_server_table::create(QWidget* parent /*= 0*/)
	{
		table_window::create(parent);

		// HACK: Should be done via create function (virtual)
		//
		// Sort
		const auto hheader = horizontalHeader();
		hheader->setSortIndicator(get_sort_column(), Qt::SortOrder::AscendingOrder);

		// Action
		m_remove_action = new QAction(tr("Remove"), this);
		m_remove_action->setIcon(QIcon(resource::menu::remove));		

		m_refresh_action = new QAction(tr("Refresh"), this);
		m_refresh_action->setIcon(QIcon(resource::menu::refresh));
	}

	void router_server_table::layout(QWidget* parent /*= 0*/)
	{
		// Delegate
		setItemDelegateForColumn(router_server_model::column::name, new server_status_delegate(this));

		// Menu
		setContextMenuPolicy(Qt::CustomContextMenu);

		// Button
		add_button(table_button::add);
		add_button(table_button::remove, false);
		add_button(table_button::refresh);

		// Custom
		m_add_button->setToolTip("Add Data Router");
		m_remove_button->setToolTip("Remove Data Router");
	}

	void router_server_table::signal(QWidget* parent /*= 0*/)
	{
		connect(m_remove_action, &QAction::triggered, this, &router_server_table::remove);
		connect(m_refresh_action, &QAction::triggered, this, &router_server_table::refresh);
		connect(this, &router_server_table::customContextMenuRequested, this, &router_server_table::show_menu);
	}

	// Slot
	void router_server_table::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
	{
		const auto enabled = selected.count() > 0;
		m_remove_button->setEnabled(enabled);

		// Event
		table_window::selectionChanged(selected, deselected);
	}

	void router_server_table::show_menu(const QPoint& pos)
	{
		const auto index = indexAt(pos);
		if (!index.isValid())
			return;

		QMenu menu(this);
		menu.addAction(m_remove_action);
		menu.addSeparator();

		menu.addAction(m_refresh_action); 

		menu.exec(viewport()->mapToGlobal(pos));
	}
}