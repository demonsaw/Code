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

#include <thread>

#include <QAction>
#include <QMenu>
#include <QPoint>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>

#include "client_browse_tree.h"
#include "command/client/request/client_browse_request_command.h"
#include "component/browse_component.h"
#include "component/share_idle_component.h"
#include "component/client/client_option_component.h"
#include "component/io/folder_component.h"
#include "component/router/router_component.h"
#include "delegate/browse/browse_name_delegate.h"
#include "http/http_socket.h"
#include "model/client/client_browse_model.h"
#include "resource/resource.h"
#include "window/client_window.h"
#include "window/main_window.h"

namespace eja
{
	// Constructor
	client_browse_tree::client_browse_tree(QWidget* parent /*= 0*/) : tree_window(parent)
	{ 
		create(parent);
		layout(parent);
		signal(parent);
	}

	// Utility
	void client_browse_tree::create(QWidget* parent /*= 0*/)
	{
		tree_window::create(parent);

		// Action
		m_download_action = new QAction(tr("Download"), this);
		m_download_action->setIcon(QIcon(resource::menu::download));
		
		m_refresh_action = new QAction(tr("Refresh"), this);
		m_refresh_action->setIcon(QIcon(resource::menu::refresh));
	}

	void client_browse_tree::layout(QWidget* parent /*= 0*/)
	{
		// Delegate
		setItemDelegateForColumn(client_browse_model::column::name, new browse_name_delegate(this));

		// Menu
		setContextMenuPolicy(Qt::CustomContextMenu);

		// Button
		add_button(tree_button::refresh);
	}

	void client_browse_tree::signal(QWidget* parent /*= 0*/)
	{
		connect(m_download_action, &QAction::triggered, this, &client_browse_tree::execute);
		connect(m_refresh_action, &QAction::triggered, this, &client_browse_tree::refresh);
		connect(this, &client_browse_tree::customContextMenuRequested, this, &client_browse_tree::show_menu);
	}

	// Event
	bool client_browse_tree::downloadable(const QModelIndexList& selected) const
	{
		if (selected.empty())
			return false;

		const auto proxy = get_proxy_model();
		const auto model = get_source_model();

		for (const auto& index : selected)
		{
			if (!index.isValid())
				continue;

			const auto entity = model->get_node(proxy->mapToSource(index));
			if (entity->has<file_component>() || entity->has<folder_component>())
				return true;
		}

		return false;
	}

	// Slot
	void client_browse_tree::refresh()
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

		std::thread thread([this]()
		{
			const auto model = get_source_model();
			model->refresh();
		});

		thread.detach();
	}

	void client_browse_tree::execute()
	{
		QModelIndexList selected = selectionModel()->selectedRows();
		if (selected.empty())
			return;

		std::thread thread([this, selected]()
		{
			const auto proxy = get_proxy_model();
			const auto model = get_source_model();

			for (const auto& index : selected)
			{
				if (index.isValid())
					model->execute(proxy->mapToSource(index));
			}
		});
		
		thread.detach();
	}

	void client_browse_tree::show_menu(const QPoint& pos)
	{
		const auto index = indexAt(pos);
		if (!index.isValid())
			return;

		QMenu menu(this);

		if (downloadable())
			menu.addAction(m_download_action);

		if (!menu.isEmpty())
			menu.addSeparator();
		
		menu.addAction(m_refresh_action);

		menu.exec(viewport()->mapToGlobal(pos));
	}
}