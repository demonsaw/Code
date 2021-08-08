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
#include "component/client/client_component.h"
#include "component/io/file_component.h"
#include "component/time/timeout_component.h"
#include "component/transfer/transfer_component.h"
#include "component/transfer/upload_component.h"
#include "component/transfer/upload_service_component.h"

#include "delegate/io/file_name_delegate.h"
#include "delegate/io/file_progress_delegate.h"
#include "delegate/io/file_row_delegate.h"
#include "entity/entity.h"
#include "entity/entity_table_view_ex.h"
#include "font/font_awesome.h"
#include "model/upload_model.h"
#include "resource/resource.h"
#include "thread/thread_info.h"
#include "widget/upload_widget.h"

namespace eja
{
	// Constructor
	upload_widget::upload_widget(const entity::ptr& entity, QWidget* parent /*= nullptr*/) : entity_dock_widget(entity, "Uploads", fa::arrow_up, parent)
	{
		assert(thread_info::main());

		// Action
		m_file_action = make_action(" Open File", fa::file);
		m_folder_action = make_action(" Open Folder", fa::folder_open);
		m_remove_action = make_action(" Remove", fa::minus);
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
		m_model = new upload_model(m_entity, this);
		init_ex(m_table, m_model);

		const auto header = m_table->horizontalHeader();
		header->setSortIndicator(upload_model::column::row, Qt::SortOrder::AscendingOrder);

		m_table->setItemDelegateForColumn(upload_model::column::row, new file_row_delegate(m_table));
		m_table->setItemDelegateForColumn(upload_model::column::name, new file_name_delegate(m_table));
		m_table->setItemDelegateForColumn(upload_model::column::progress, new file_progress_delegate(m_table));
		m_table->sortByColumn(upload_model::column::row);
		m_table->set_column_sizes();

		setWidget(m_table);

		// Update
		const auto client = m_entity->get<client_component>();
		m_remove_list.reserve(client->get_downloads());

		// Event
		m_table->installEventFilter(this);

		// Callback
		add_callback(callback::upload | callback::add, [&](const entity::ptr entity) { add(entity); });
		add_callback(callback::upload | callback::remove, [&](const entity::ptr entity) { m_model->remove(entity); });
		add_callback(callback::upload | callback::update, [&](const entity::ptr entity) { m_model->update(entity); });
		add_callback(callback::upload | callback::update, [&]() { update(); });
		add_callback(callback::upload | callback::clear, [&]() { clear(); });

		// Signal
		//connect(this, &upload_widget::customContextMenuRequested, this, &upload_widget::show_menu);
		connect(m_table, &entity_table_view_ex::customContextMenuRequested, this, &upload_widget::show_menu);
		connect(m_table, &entity_table_view_ex::doubleClicked, this, &upload_widget::on_open_file);
		connect(header, &QHeaderView::customContextMenuRequested, [&](const QPoint& pos) { show_header_menu({ "Row", "Type", "Size", "Time", "Speed", "Progress" }, { upload_model::column::row, upload_model::column::type, upload_model::column::size, upload_model::column::time, upload_model::column::speed, upload_model::column::progress }, m_table, m_model, pos); });

		connect(m_file_action, &QAction::triggered, this, &upload_widget::on_open_file);
		connect(m_folder_action, &QAction::triggered, this, &upload_widget::on_open_folder);
		connect(m_remove_action, &QAction::triggered, [&]() { on_remove(); });
		connect(m_close_action, &QAction::triggered, [&]() { close(); });
	}

	// Interface
	void upload_widget::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		// Transfer
		const auto transfer = entity->get<transfer_component>();
		transfer->start();

		const auto upload_map = m_entity->get<upload_map_component>();
		{
			const auto pair = std::make_pair(transfer->get_id(), entity);

			thread_lock(upload_map);
			upload_map->insert(pair);
		}

		// Service
		const auto upload_service = m_entity->get<upload_service_component>();
		upload_service->async_start(entity);

		// Model
		m_model->add(entity);

		update();
	}

	void upload_widget::on_remove()
	{
		assert(thread_info::main());

		m_table->execute([&](const QModelIndex index)
		{
			const auto entity = m_model->get_entity(index);
			if (entity)
			{
				// Transfer
				const auto transfer = entity->get<transfer_component>();
				transfer->finish();

				const auto upload_map = m_entity->get<upload_map_component>();
				{
					thread_lock(upload_map);
					upload_map->erase(transfer->get_id());
				}

				// Callback
				m_entity->async_call(callback::upload | callback::remove, entity);

				// Service
				const auto upload_service = m_entity->get<upload_service_component>();
				upload_service->async_stop(entity);
			}
		});
	}

	void upload_widget::on_update()
	{
		assert(thread_info::main());

		// Uploads
		const auto upload_list = m_entity->get<upload_list_component>();
		for (const auto& e : *upload_list)
		{
			// Verify
			const auto transfer = e->get<transfer_component>();
			if (transfer->is_running())
			{
				// Timeout
				const auto timeout = e->get<timeout_component>();
				if (!timeout->expired(upload::timeout))
					continue;

				// Cancel
				transfer->cancel();
			}

			// Clear
			const auto upload_map = m_entity->get<upload_map_component>();
			{
				thread_lock(upload_map);
				upload_map->erase(transfer->get_id());
			}

			// Remove
			m_remove_list.push_back(e);
			m_entity->async_call(callback::finished | callback::add, e);

			// Service
			const auto upload_service = m_entity->get<upload_service_component>();
			upload_service->async_stop(e);
		}

		// Remove
		if (!m_remove_list.empty())
		{
			for (const auto& e : m_remove_list)
				m_model->remove(e);

			m_remove_list.clear();
		}
	}

	void upload_widget::on_clear()
	{
		assert(thread_info::main());

		const auto upload_map = m_entity->get<upload_map_component>();
		const auto upload_list = m_entity->get<upload_list_component>();

		for (const auto& e : *upload_list)
		{
			const auto transfer = e->get<transfer_component>();
			transfer->finish();
			{
				thread_lock(upload_map);
				upload_map->erase(transfer->get_id());
			}

			// Service
			const auto upload_service = m_entity->get<upload_service_component>();
			upload_service->async_stop(e);
		}

		m_model->clear();
	}

	// Utility
	void upload_widget::on_open_file()
	{
		assert(thread_info::main());

		m_table->execute([&](const QModelIndex index)
		{
			const auto entity = m_model->get_entity(index);
			if (entity)
			{
				const auto transfer = entity->get<transfer_component>();
				if (transfer->has_size())
				{
					const auto file = entity->get<file_component>();
#if _WSTRING
					const auto url = QUrl::fromLocalFile(QString::fromStdWString(file->get_wpath()));
#else
					const auto url = QUrl::fromLocalFile(QString::fromStdString(file->get_path()));
#endif
					QDesktopServices::openUrl(url);
				}
			}
		});
	}

	void upload_widget::on_open_folder()
	{
		assert(thread_info::main());

		m_table->execute([&](const QModelIndex index)
		{
			const auto entity = m_model->get_entity(index);
			if (entity)
			{
				const auto transfer = entity->get<transfer_component>();
				if (transfer->has_size())
				{
					const auto file = entity->get<file_component>();
#if _WSTRING
					const auto url = QUrl::fromLocalFile(QString::fromStdWString(file->get_wfolder()));
#else
					const auto url = QUrl::fromLocalFile(QString::fromStdString(file->get_folder()));
#endif
					QDesktopServices::openUrl(url);
				}
			}
		});
	}

	// Menu
	void upload_widget::show_menu(const QPoint& pos)
	{
		assert(thread_info::main());

		QMenu menu;
		menu.setStyleSheet(QString("QMenu::item { height:%1px; width:%2px; }").arg(resource::get_row_height()).arg(resource::get_menu_width()));

		if (!m_model->empty())
		{
			const auto index = m_table->indexAt(pos);
			if (index.isValid())
			{
				menu.addAction(m_file_action);
				menu.addAction(m_folder_action);
				menu.addSeparator();

				menu.addAction(m_remove_action);
				menu.addSeparator();
			}
		}

		menu.addAction(m_close_action);

		menu.exec(m_table->viewport()->mapToGlobal(pos));
	}
}
