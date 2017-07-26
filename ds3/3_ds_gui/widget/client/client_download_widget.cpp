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
#include <QDesktopServices>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPoint>
#include <QTimer>
#include <QToolBar>
#include <QUrl>

#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/client/client_service_component.h"
#include "component/io/file_component.h"
#include "component/io/folder_component.h"
#include "component/io/transfer_component.h"
#include "component/status/status_component.h"

#include "delegate/status_delegate.h"
#include "delegate/io/file_name_delegate.h"
#include "delegate/io/file_progress_delegate.h"
#include "entity/entity.h"
#include "entity/entity_table_view.h"
#include "font/font_awesome.h"
#include "model/client/client_download_model.h"
#include "widget/client/client_download_widget.h"

namespace eja
{
	// Constructor
	client_download_widget::client_download_widget(const entity::ptr entity, QWidget* parent /*= 0*/) : entity_widget(entity, "Downloads", fa::arrowdown, callback_type::download, parent)
	{
		assert(thread_info::main());

		// Action
		m_remove_action = make_action("Remove", fa::minus);
		m_file_action = make_action("Open File", fa::file);
		m_folder_action = make_action("Open Folder", fa::folderopen);
		m_resume_action = make_action("Resume", fa::play);
		m_stop_action = make_action("Stop", fa::stop);

		// Menu
		add_button("Remove", fa::minus, [&]() { m_model->remove(); });
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

		// Table
		m_table = new entity_table_view(m_entity, this);
		m_model = new client_download_model(m_entity, { "", "", "Name", "Type", "Checksum", "Size", "Threads", "Time", "Speed", "Progress" }, { 24, 24, 320, 72, 72, 72, 72, 72, 72, 104 }, this);
		init(m_table, m_model);

		// Header
		const auto hheader = m_table->horizontalHeader();
		hheader->setSectionResizeMode(client_download_model::column::status, QHeaderView::Fixed);
		hheader->setSortIndicator(client_download_model::column::index, Qt::SortOrder::AscendingOrder);

		// Callback
		add_callback(callback_action::add, [&](const entity::ptr entity) { m_model->add(entity); });
		add_callback(callback_action::remove, [&](const entity::ptr entity) { m_model->remove(entity); });
		add_callback(callback_action::clear, [&](const entity::ptr entity) { m_model->clear(); });

		// Delegate
		m_table->setColumnWidth(client_download_model::column::status, 24);
		m_table->setItemDelegateForColumn(client_download_model::column::status, new status_delegate(m_table));
		m_table->setItemDelegateForColumn(client_download_model::column::name, new file_name_delegate(m_table));
		m_table->setItemDelegateForColumn(client_download_model::column::progress, new file_progress_delegate(m_table));

		// Signal
		connect(m_table, &entity_table_view::doubleClicked, m_table, &entity_table_view::double_click);
		connect(m_table, &entity_table_view::customContextMenuRequested, this, &client_download_widget::show_menu);

		connect(m_remove_action, &QAction::triggered, [&]() { m_table->remove(); });
		connect(m_file_action, &QAction::triggered, this, &client_download_widget::open_file);
		connect(m_folder_action, &QAction::triggered, this, &client_download_widget::open_folder);
		connect(m_resume_action, &QAction::triggered, this, &client_download_widget::resume);
		connect(m_stop_action, &QAction::triggered, this, &client_download_widget::stop);

		setWidget(m_table);
	}

	// Utility
	void client_download_widget::open_file()
	{
		assert(thread_info::main());

		m_table->execute([&](const QModelIndex& index)
		{
			const auto entity = m_model->get_entity(index);
			if (!entity)
				return; 
			
			const auto file = entity->get<file_component>();
			const auto transfer = entity->get<transfer_component>();			

			if (!transfer->empty() || !file->has_size())
			{				
				const auto url = QUrl::fromLocalFile(QString::fromStdString(file->get_path()));
				QDesktopServices::openUrl(url);
			}
		});
	}

	void client_download_widget::open_folder()
	{
		assert(thread_info::main());

		m_table->execute([&](const QModelIndex& index)
		{
			const auto entity = m_model->get_entity(index);
			if (!entity)
				return;

			const auto folder = entity->get<folder_component>();
			const auto url = QUrl::fromLocalFile(QString::fromStdString(folder->get_path()));
			QDesktopServices::openUrl(url);
		});
	}

	void client_download_widget::resume()
	{
		assert(thread_info::main());

		m_table->execute([&](const QModelIndex& index)
		{
			const auto entity = m_model->get_entity(index);
			if (!entity)
				return;

			const auto transfer = entity->get<transfer_component>();
			if (transfer->invalid() && transfer->incomplete())
			{				
				// Service
				const auto parent = entity->get_parent();
				const auto service = parent->get<client_service_component>();
				if (service->valid())
					service->async_resume(entity);
			}				
		});
	}

	void client_download_widget::stop()
	{
		assert(thread_info::main());

		m_table->execute([&](const QModelIndex& index)
		{
			const auto entity = m_model->get_entity(index);
			if (!entity)
				return;

			const auto transfer = entity->get<transfer_component>();
			if (transfer->valid())
				transfer->stop();
		});
	}

	// Is
	bool client_download_widget::is_file() const
	{
		assert(thread_info::main());

		const auto selected = m_table->selectionModel()->selectedIndexes();
		if (selected.empty())
			return false;

		const auto proxy = m_table->get_proxy_model();
		const auto model = m_table->get_source_model();

		for (const auto& index : selected)
		{
			if (!index.isValid())
				continue;

			const auto entity = model->get_entity(proxy->mapToSource(index));
			if (entity && entity->has<file_component>())
				return true;
		}

		return false;
	}

	bool client_download_widget::is_folder() const
	{
		assert(thread_info::main());

		const auto selected = m_table->selectionModel()->selectedIndexes();
		if (selected.empty())
			return false;

		const auto proxy = m_table->get_proxy_model();
		const auto model = m_table->get_source_model();

		for (const auto& index : selected)
		{
			if (!index.isValid())
				continue;

			const auto entity = model->get_entity(proxy->mapToSource(index));
			if (entity && entity->has<folder_component>())
				return true;
		}

		return false;
	}

	bool client_download_widget::is_resumeable() const
	{
		assert(thread_info::main());

		const auto selected = m_table->selectionModel()->selectedIndexes();
		if (selected.empty())
			return false;

		const auto proxy = m_table->get_proxy_model();
		const auto model = m_table->get_source_model();

		for (const auto& index : selected)
		{
			if (!index.isValid())
				continue;

			const auto entity = model->get_entity(proxy->mapToSource(index));
			if (entity)
			{
				const auto transfer = entity->get<transfer_component>();
				if (transfer->invalid() && transfer->incomplete())
					return true;
			}
		}

		return false;
	}

	bool client_download_widget::is_stoppable() const
	{
		assert(thread_info::main());

		const auto selected = m_table->selectionModel()->selectedIndexes();
		if (selected.empty())
			return false;

		const auto proxy = m_table->get_proxy_model();
		const auto model = m_table->get_source_model();

		for (const auto& index : selected)
		{
			if (!index.isValid())
				continue;

			const auto entity = model->get_entity(proxy->mapToSource(index));
			if (entity)
			{
				const auto transfer = entity->get<transfer_component>();
				if (transfer->valid())
					return true;
			}				
		}

		return false;
	}

	// Menu
	void client_download_widget::show_menu(const QPoint& pos)
	{
		assert(thread_info::main());

		const auto index = m_table->indexAt(pos);
		if (!index.isValid())
			return;

		QMenu menu(this);

		if (is_file())
		{			
			menu.addAction(m_file_action);
			menu.addAction(m_folder_action);			

			if (is_stoppable())
				menu.addAction(m_stop_action);
			else if (is_resumeable())
				menu.addAction(m_resume_action);

			menu.addSeparator();
		}
		else if (is_folder())
		{
			menu.addAction(m_folder_action);
			menu.addSeparator();
		}

		menu.addAction(m_remove_action);

		menu.exec(m_table->viewport()->mapToGlobal(pos));
	}
}
