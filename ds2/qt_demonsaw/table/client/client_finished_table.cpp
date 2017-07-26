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
#include <QHeaderView>
#include <QMenu>
#include <QPoint>
#include <QToolButton>
#include <QUrl>

#include "client_finished_table.h"
#include "component/io/folder_component.h"
#include "component/transfer/finished_component.h"
#include "component/transfer/download_thread_component.h"
#include "delegate/transfer/transfer_name_delegate.h"
#include "delegate/transfer/transfer_progress_delegate.h"
#include "delegate/transfer/transfer_status_delegate.h"
#include "delegate/transfer/transfer_type_delegate.h"
#include "model/client/client_finished_model.h"
#include "resource/resource.h"
#include "window/client_window.h"

namespace eja
{
	// Constructor
	client_finished_table::client_finished_table(QWidget* parent /*= 0*/) : table_window(parent)
	{
		create(parent);
		layout(parent);
		signal(parent);
	}

	// Interface
	void client_finished_table::create(QWidget* parent /*= 0*/)
	{
		table_window::create(parent);

		// HACK: Should be done via create function (virtual)
		//
		// Sort
		const auto hheader = horizontalHeader();
		hheader->setSortIndicator(get_sort_column(), Qt::SortOrder::AscendingOrder);

		// Action
		m_file_action = new QAction(tr("Open File"), this);
		m_file_action->setIcon(QIcon(resource::tab::file));

		m_folder_action = new QAction(tr("Open Folder"), this);
		m_folder_action->setIcon(QIcon(resource::tab::folder));

		m_resume_action = new QAction(tr("Resume"), this);
		m_resume_action->setIcon(QIcon(resource::menu::start));

		m_remove_action = new QAction(tr("Remove"), this);
		m_remove_action->setIcon(QIcon(resource::menu::remove));

		m_clear_action = new QAction(tr("Clear"), this);
		m_clear_action->setIcon(QIcon(resource::menu::clear));
	}

	void client_finished_table::layout(QWidget* parent /*= 0*/)
	{
		// Delegate
		setItemDelegateForColumn(client_finished_model::column::status, new transfer_status_delegate(this));
		setItemDelegateForColumn(client_finished_model::column::file, new transfer_name_delegate(this));
		setItemDelegateForColumn(client_transfer_model::column::type, new transfer_type_delegate(this));
		setItemDelegateForColumn(client_finished_model::column::progress, new transfer_progress_delegate(this));

		// Menu
		setContextMenuPolicy(Qt::CustomContextMenu);

		// Button
		add_button(table_button::remove, false);
		add_button(table_button::clear);
	}

	void client_finished_table::signal(QWidget* parent /*= 0*/)
	{
		connect(m_file_action, &QAction::triggered, this, &client_finished_table::open_file);
		connect(m_folder_action, &QAction::triggered, this, &client_finished_table::open_folder);
		connect(m_resume_action, &QAction::triggered, this, &client_finished_table::resume);
		connect(m_remove_action, &QAction::triggered, this, &client_finished_table::remove);
		connect(m_clear_action, &QAction::triggered, this, &client_finished_table::clear);

		connect(this, &client_finished_table::customContextMenuRequested, this, &client_finished_table::show_menu);
	}

	// Event
	bool client_finished_table::resumable(const QModelIndexList& selected) const
	{
		if (selected.empty())
			return false;

		const auto proxy = get_proxy_model();
		const auto model = get_source_model();
		const auto owner = model->get_entity();

		const auto finished_vector = owner->get<finished_entity_vector_component>();
		if (!finished_vector)
			return false;

		for (const auto& index : selected)
		{
			if (!index.isValid())
				continue;

			const auto source_index = proxy->mapToSource(index);
			if (!source_index.isValid())
				continue;

			const auto row = static_cast<size_t>(source_index.row());
			const auto entity = finished_vector->get(row);

			if (entity)
			{
				const auto transfer = entity->get<transfer_component>();
				if (transfer && transfer->is_download() && !transfer->done())
					return true;
			}
		}

		return false;
	}

	// Slot
	void client_finished_table::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
	{
		const auto enabled = selected.count() > 0;
		m_remove_button->setEnabled(enabled);

		// Event
		table_window::selectionChanged(selected, deselected);
	}

	void client_finished_table::open_file()
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

	void client_finished_table::open_folder()
	{
		QModelIndexList selected = selectionModel()->selectedRows();
		if (selected.empty())
			return;

		std::thread thread([this, selected]()
		{
			const auto proxy = get_proxy_model();
			const auto model = get_source_model();
			const auto owner = model->get_entity();
			const auto finished_vector = owner->get<finished_entity_vector_component>();
			if (!finished_vector)
				return;

			for (const auto& index : selected)
			{
				if (!index.isValid())
					continue;

				const auto source_index = proxy->mapToSource(index);
				if (!source_index.isValid())
					continue;

				const auto row = static_cast<size_t>(source_index.row());
				const auto entity = finished_vector->get(row);

				if (entity)
				{
					const auto transfer = entity->get<transfer_component>();
					if (!transfer)
						continue;

					switch (transfer->get_type())
					{
						case transfer_type::download:
						{
							const auto folder = entity->get<folder_component>();
							if (folder)
							{
								const auto url = QUrl::fromLocalFile(QString::fromStdString(folder->get_path()));
								QDesktopServices::openUrl(url);
							}

							break;
						}
						case transfer_type::upload:
						{
							const auto file = entity->get<file_component>();
							if (file)
							{
								const auto url = QUrl::fromLocalFile(QString::fromStdString(file->get_parent()));
								QDesktopServices::openUrl(url);
							}

							break;
						}
					}
				}
			}
		});

		thread.detach();
	}

	void client_finished_table::resume()
	{
		QModelIndexList selected = selectionModel()->selectedRows();
		if (selected.empty())
			return;

		const auto proxy = get_proxy_model();
		const auto model = get_source_model();
		const auto owner = model->get_entity();

		const auto finished_vector = owner->get<finished_entity_vector_component>();
		if (!finished_vector)
			return;

		const auto list = entity_list::create();

		for (const auto& index : selected)
		{
			if (!index.isValid())
				continue;

			const auto source_index = proxy->mapToSource(index);
			if (!source_index.isValid())
				continue;

			const auto row = static_cast<size_t>(source_index.row());
			const auto entity = finished_vector->get(row);

			if (entity)
			{
				const auto transfer = entity->get<transfer_component>();
				if (transfer && transfer->is_download() && !transfer->done())
					list->add(entity);
			}
		}

		auto_lock_ptr(list);

		for (const auto& entity : *list)
		{
			owner->call(function_type::finished, function_action::remove, entity);
			owner->call(function_type::transfer, function_action::add, entity);
		}

		const auto enabled = list->size() == selected.count();
		m_remove_button->setEnabled(enabled);
	}

	void client_finished_table::show_menu(const QPoint& pos)
	{
		const auto index = indexAt(pos);
		if (!index.isValid())
			return;

		QMenu menu(this);
		menu.addAction(m_file_action);
		menu.addAction(m_folder_action);
		menu.addSeparator();

		if (resumable())
		{
			menu.addAction(m_resume_action);
			menu.addSeparator();
		}

		menu.addAction(m_remove_action);		
		menu.addAction(m_clear_action);

		menu.exec(viewport()->mapToGlobal(pos));
	}
}