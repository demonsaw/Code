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
#include <QDesktopServices>
#include <QMenu>
#include <QPoint>
#include <QToolBar>
#include <QToolButton>
#include <QUrl>

#include "client_share_tree.h"
#include "component/io/file_component.h"
#include "component/share_idle_component.h"
#include "delegate/share/share_name_delegate.h"
#include "resource/resource.h"
#include "window/client_window.h"
#include "window/main_window.h"

namespace eja
{
	// Constructor
	client_share_tree::client_share_tree(QWidget* parent /*= 0*/) : tree_window(parent)
	{ 
		create(parent);
		layout(parent);
		signal(parent);
	}

	// Interface
	void client_share_tree::create(QWidget* parent /*= 0*/)
	{
		tree_window::create(parent);

		// Action
		m_file_action = new QAction(tr("Open File"), this);
		m_file_action->setIcon(QIcon(resource::tab::file));

		m_folder_action = new QAction(tr("Open Folder"), this);
		m_folder_action->setIcon(QIcon(resource::tab::folder));

		m_remove_action = new QAction(tr("Remove"), this);
		m_remove_action->setIcon(QIcon(resource::menu::remove));

		m_refresh_action = new QAction(tr("Refresh"), this);
		m_refresh_action->setIcon(QIcon(resource::menu::refresh));
	}

	void client_share_tree::layout(QWidget* parent /*= 0*/)
	{
		// Delegate
		setItemDelegateForColumn(client_browse_model::column::name, new share_name_delegate(this));

		// Menu
		setContextMenuPolicy(Qt::CustomContextMenu);

		// Button
		add_button(tree_button::add);
		add_button(tree_button::remove, false);
		add_button(tree_button::refresh);

		// Custom
		m_add_button->setToolTip("Add Folder");
		m_remove_button->setToolTip("Remove Folder");
	}

	void client_share_tree::signal(QWidget* parent /*= 0*/)
	{
		connect(m_file_action, &QAction::triggered, this, &client_share_tree::open_file);
		connect(m_folder_action, &QAction::triggered, this, &client_share_tree::open_folder);
		connect(m_remove_action, &QAction::triggered, this, &client_share_tree::remove);
		connect(m_refresh_action, &QAction::triggered, this, &client_share_tree::refresh);

		connect(this, &client_share_tree::customContextMenuRequested, this, &client_share_tree::show_menu);
	}

	// Utility
	bool client_share_tree::is_file(const QModelIndexList& selected) const
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
			if (entity->has<file_component>())
				return true;
		}

		return false;
	}

	bool client_share_tree::is_folder(const QModelIndexList& selected) const
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
			if (entity->has<folder_component>())
				return true;
		}

		return false;
	}

	bool client_share_tree::removable(const QModelIndexList& selected) const
	{
		if (selected.empty())
			return false;

		const auto proxy = get_proxy_model();
		const auto model = get_source_model();

		const auto owner = model->get_entity();
		const auto share = owner->get<share_idle_component>();
		if (!share)
			return false;

		const auto& folders = share->get_folders();

		for (const auto& index : selected)
		{
			if (!index.isValid())
				continue;

			const auto entity = model->get_node(proxy->mapToSource(index));
			if (entity->has<folder_component>())
			{
				const auto folder = entity->get<folder_component>();
				if (folder && folders.has(folder))
					return true;
			}
		}

		return false;
	}

	// Slot
	void client_share_tree::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
	{
		m_remove_button->setEnabled(removable());

		tree_window::selectionChanged(selected, deselected);
	}

	void client_share_tree::open_file()
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
				if (!index.isValid())
					continue;

				const auto source_index = proxy->mapToSource(index);
				if (!source_index.isValid())
					continue;

				model->double_click(source_index);
			}
		});

		thread.detach();
	}

	void client_share_tree::open_folder()
	{
		QModelIndexList selected = selectionModel()->selectedRows();
		if (selected.empty())
			return;

		std::thread thread([this, selected]()
		{
			const auto proxy = get_proxy_model();
			const auto model = get_source_model();
			const auto owner = model->get_entity();
			const auto share_vector = owner->get<share_entity_vector_component>();
			if (!share_vector)
				return;

			for (const auto& index : selected)
			{
				if (!index.isValid())
					continue;

				const auto source_index = proxy->mapToSource(index);
				if (!source_index.isValid())
					continue;

				const auto entity = model->get_node(source_index);
				if (!entity)
					return;

				if (entity)
				{
					if (entity->has<file_component>())
					{
						const auto file = entity->get<file_component>();
						if (file)
						{
							const auto url = QUrl::fromLocalFile(QString::fromStdString(file->get_parent()));
							QDesktopServices::openUrl(url);
						}
					}
					else if (entity->has<folder_component>())
					{
						const auto folder = entity->get<folder_component>();
						if (folder)
						{
							const auto url = QUrl::fromLocalFile(QString::fromStdString(folder->get_path()));
							QDesktopServices::openUrl(url);
						}
					}
				}
			}
		});

		thread.detach();
	}

	void client_share_tree::show_menu(const QPoint& pos)
	{
		const auto index = indexAt(pos);
		if (!index.isValid())
			return;

		QMenu menu(this);

		if (is_file())
		{
			menu.addAction(m_file_action);
			menu.addAction(m_folder_action);
			menu.addSeparator();
		}
		else if (is_folder())
		{
			menu.addAction(m_folder_action);
			menu.addSeparator();
		}

		if (removable())
			menu.addAction(m_remove_action);

		menu.addAction(m_refresh_action);

		menu.exec(viewport()->mapToGlobal(pos));
	}
}
