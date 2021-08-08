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
#include <QSortFilterProxyModel>
#include <QUrl>

#include "component/callback/callback.h"
#include "component/callback/callback_service_component.h"
#include "component/client/client_component.h"
#include "component/client/client_service_component.h"
#include "component/io/file_component.h"
#include "component/transfer/chunk_component.h"
#include "component/transfer/finished_component.h"
#include "component/transfer/transfer_component.h"

#include "delegate/io/file_name_delegate.h"
#include "delegate/io/file_progress_delegate.h"
#include "delegate/io/file_row_delegate.h"
#include "entity/entity.h"
#include "entity/entity_table_view_ex.h"
#include "font/font_awesome.h"
#include "model/finished_model.h"
#include "resource/resource.h"
#include "thread/thread_info.h"
#include "utility/io/file_util.h"
#include "widget/finished_widget.h"

namespace eja
{
	// Constructor
	finished_widget::finished_widget(const entity::ptr& entity, QWidget* parent /*= nullptr*/) : entity_dock_widget(entity, "Finished", fa::check, parent)
	{
		assert(thread_info::main());

		// Action
		m_download_action = make_action(" Re-download", fa::repeat);
		m_file_action = make_action(" Open File", fa::file);
		m_folder_action = make_action(" Open Folder", fa::folder_open);
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

		// Table
		m_table = new entity_table_view_ex(m_entity, this);
		m_model = new finished_model(m_entity, this);
		init_ex(m_table, m_model);

		const auto header = m_table->horizontalHeader();
		header->setSortIndicator(finished_model::column::row, Qt::SortOrder::AscendingOrder);

		m_table->setItemDelegateForColumn(finished_model::column::row, new file_row_delegate(m_table));
		m_table->setItemDelegateForColumn(finished_model::column::name, new file_name_delegate(m_table));
		m_table->setItemDelegateForColumn(finished_model::column::progress, new file_progress_delegate(m_table));
		m_table->sortByColumn(finished_model::column::row);
		m_table->set_column_sizes();

		setWidget(m_table);

		// Event
		m_table->installEventFilter(this);

		// Callback
		add_callback(callback::finished | callback::add, [&](const entity::ptr entity) { add(entity); });
		add_callback(callback::finished | callback::remove, [&](const entity::ptr entity) { m_model->remove(entity); });
		add_callback(callback::finished | callback::clear, [&]() { m_model->clear(); });

		// Signal
		//connect(this, &finished_widget::customContextMenuRequested, this, &finished_widget::show_menu);
		connect(m_table, &entity_table_view_ex::customContextMenuRequested, this, &finished_widget::show_menu);
		connect(m_table, &entity_table_view_ex::doubleClicked, this, &finished_widget::on_open_file);
		connect(header, &QHeaderView::customContextMenuRequested, [&](const QPoint& pos) { show_header_menu({ "Row", "Type", "Size", "Time", "Speed", "Progress" }, { finished_model::column::row, finished_model::column::type, finished_model::column::size, finished_model::column::time, finished_model::column::speed, finished_model::column::progress }, m_table, m_model, pos); });

		connect(m_download_action, &QAction::triggered, this, &finished_widget::on_download);
		connect(m_file_action, &QAction::triggered, this, &finished_widget::on_open_file);
		connect(m_folder_action, &QAction::triggered, this, &finished_widget::on_open_folder);
		connect(m_remove_action, &QAction::triggered, [&]() { m_table->remove(); });
		connect(m_clear_action, &QAction::triggered, [&]() { m_table->clear(); });
		connect(m_close_action, &QAction::triggered, [&]() { close(); });
	}

	// Interface
	void finished_widget::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		// Upload
		const auto transfer = entity->get<transfer_component>();
		if (transfer->is_upload())
		{
			const auto chunk = entity->get<chunk_component>();
			chunk->clear();
		}

		// Status
		const auto status = entity->get<status_component>();
		if (transfer->is_cancelled())
			status->set_status(eja::status::cancelled);
		else if (transfer->is_errored())
			status->set_status(eja::status::error);
		else
			status->set_status(eja::status::success);

		m_model->add(entity);

		m_table->scroll();
	}

	// Utility
	void finished_widget::on_download()
	{
		assert(thread_info::main());

		m_table->execute([&](const QModelIndex index)
		{
			const auto entity = m_model->get_entity(index);
			if (entity)
			{
				const auto transfer = entity->get<transfer_component>();
				transfer->reset();

				m_model->remove(entity);

				// Callback
				m_entity->async_call(callback::download | callback::add, entity);
			}
		});
	}

	void finished_widget::on_open_file()
	{
		assert(thread_info::main());

		m_table->execute([&](const QModelIndex index)
		{
			const auto entity = m_model->get_entity(index);
			if (entity)
			{
				const auto file = entity->get<file_component>();
#if _WSTRING
				const auto url = QUrl::fromLocalFile(QString::fromStdWString(file->get_wpath()));
#else
				const auto url = QUrl::fromLocalFile(QString::fromStdString(file->get_path()));
#endif
				QDesktopServices::openUrl(url);
			}
		});
	}

	void finished_widget::on_open_folder()
	{
		assert(thread_info::main());

		const auto selected = m_table->selectionModel()->hasSelection();

		if (selected)
		{
			m_table->execute([&](const QModelIndex index)
			{
				const auto entity = m_model->get_entity(index);
				if (entity)
				{
					const auto file = entity->get<file_component>();
#if _WSTRING
					const auto url = QUrl::fromLocalFile(QString::fromStdWString(file->get_wfolder()));
#else
					const auto url = QUrl::fromLocalFile(QString::fromStdString(file->get_folder()));
#endif
					QDesktopServices::openUrl(url);
				}
			});
		}
		else
		{
			const auto client = m_entity->get<client_component>();
#if _WSTRING
			const auto url = QUrl::fromLocalFile(QString::fromStdWString(client->get_path().wstring()));
#else
			const auto url = QUrl::fromLocalFile(QString::fromStdString(client->get_path().string()));
#endif
			QDesktopServices::openUrl(url);
		}
	}

	// Is
	bool finished_widget::is_done() const
	{
		assert(thread_info::main());

		const auto selected = m_table->selectionModel()->selectedRows();
		if (!selected.empty())
		{
			const auto proxy = m_table->get_proxy();

			for (const auto& index : selected)
			{
				const auto sindex = proxy->mapToSource(index);
				const auto entity = m_model->get_entity(sindex);

				const auto transfer = entity->get<transfer_component>();
				if (transfer->is_download() && transfer->get_remaining())
					return false;
			}
		}

		return true;
	}

	// Menu
	void finished_widget::show_menu(const QPoint& pos)
	{
		assert(thread_info::main());

		QMenu menu;
		menu.setStyleSheet(QString("QMenu::item { height:%1px; width:%2px; }").arg(resource::get_row_height()).arg(resource::get_menu_width()));

		if (!m_model->empty())
		{
			const auto index = m_table->indexAt(pos);
			if (index.isValid())
			{
				if (is_done())
				{
					menu.addAction(m_file_action);
				}
				else
				{
					menu.addAction(m_download_action);
				}

				menu.addAction(m_folder_action);
				menu.addSeparator();

				menu.addAction(m_remove_action);
			}
			else
			{
				menu.addAction(m_folder_action);
				menu.addSeparator();
			}

			menu.addAction(m_clear_action);
			menu.addSeparator();
		}
		else
		{
			menu.addAction(m_folder_action);
			menu.addSeparator();
		}

		menu.addAction(m_close_action);

		menu.exec(m_table->viewport()->mapToGlobal(pos));
	}
}
