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
#include "component/client/client_service_component.h"
#include "component/io/file_component.h"
#include "component/router/router_component.h"
#include "component/status/status_component.h"
#include "component/time/timeout_component.h"
#include "component/transfer/download_component.h"
#include "component/transfer/download_service_component.h"
#include "component/transfer/transfer_component.h"

#include "delegate/io/file_name_delegate.h"
#include "delegate/io/file_progress_delegate.h"
#include "delegate/io/file_row_delegate.h"
#include "entity/entity.h"
#include "entity/entity_table_view_ex.h"
#include "font/font_awesome.h"
#include "model/download_model.h"
#include "resource/icon_provider.h"
#include "resource/resource.h"
#include "thread/thread_info.h"
#include "utility/value.h"
#include "utility/io/file_util.h"
#include "widget/download_widget.h"

namespace eja
{
	// Constructor
	download_widget::download_widget(const entity::ptr& entity, QWidget* parent /*= nullptr*/) : entity_dock_widget(entity, "Downloads", fa::arrow_down, parent)
	{
		assert(thread_info::main());

		// Action
		m_file_action = make_action(" Open File", fa::file);
		m_folder_action = make_action(" Open Folder", fa::folder_open);

		m_remove_action = make_action(" Remove", fa::minus);
		m_refresh_action = make_action(" Refresh", fa::refresh);
		m_close_action = make_action(" Close", fa::close);

		m_pause_action = make_action(" Pause", fa::pause);
		m_resume_action = make_action(" Resume", fa::play);
		m_finish_action = make_action(" Finish", fa::check);

		m_move_up_action = make_action(" Up", fa::angle_up);
		m_move_down_action = make_action(" Down", fa::angle_down);
		m_move_top_action = make_action(" Top", fa::angle_double_up);
		m_move_bottom_action = make_action(" Bottom", fa::angle_double_down);

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
		m_model = new download_model(m_entity, this);
		init_ex(m_table, m_model);

		const auto header = m_table->horizontalHeader();
		header->setSortIndicator(download_model::column::row, Qt::SortOrder::AscendingOrder);

		m_table->setDragEnabled(true);
		m_table->setAcceptDrops(true);
		m_table->setDropIndicatorShown(true);
		m_table->setDragDropMode(QAbstractItemView::InternalMove);
		m_table->setDefaultDropAction(Qt::MoveAction);
		m_table->setDragDropOverwriteMode(false);

		m_table->setItemDelegateForColumn(download_model::column::row, new file_row_delegate(m_table));
		m_table->setItemDelegateForColumn(download_model::column::name, new file_name_delegate(m_table));
		m_table->setItemDelegateForColumn(download_model::column::progress, new file_progress_delegate(m_table));
		m_table->sortByColumn(download_model::column::row);

		m_table->setItemDelegateForColumn(download_model::column::row, new file_row_delegate(m_table));
		m_table->setItemDelegateForColumn(download_model::column::name, new file_name_delegate(m_table));
		m_table->setItemDelegateForColumn(download_model::column::progress, new file_progress_delegate(m_table));
		m_table->sortByColumn(download_model::column::row);
		m_table->set_column_sizes();

		setWidget(m_table);

		// Event
		m_table->installEventFilter(this);

		// Update
		const auto client = m_entity->get<client_component>();
		m_add_list.reserve(client->get_downloads());
		m_remove_list.reserve(client->get_downloads());

		// Callback
		add_callback(callback::download | callback::add, [&](const entity::ptr entity) { add(entity); });
		add_callback(callback::download | callback::remove, [&](const entity::ptr entity) { m_model->remove(entity); });
		add_callback(callback::download | callback::update, [&](const entity::ptr entity) { m_model->update(entity); });
		add_callback(callback::download | callback::update, [&]() { update(); });
		add_callback(callback::download | callback::clear, [&]() { clear(); });

		// Signal
		connect(this, &download_widget::customContextMenuRequested, this, &download_widget::show_menu);
		connect(m_table, &entity_table_view_ex::customContextMenuRequested, this, &download_widget::show_menu);
		connect(header, &QHeaderView::customContextMenuRequested, [&](const QPoint& pos) { show_header_menu({ "Row", "Type", "Size", "Time", "Speed", "Progress" }, { download_model::column::row, download_model::column::type, download_model::column::size, download_model::column::time, download_model::column::speed, download_model::column::progress }, m_table, m_model, pos); });

		connect(m_table, &entity_table_view_ex::doubleClicked, this, &download_widget::on_open_file);
		connect(m_file_action, &QAction::triggered, this, &download_widget::on_open_file);
		connect(m_folder_action, &QAction::triggered, this, &download_widget::on_open_folder);

		connect(m_remove_action, &QAction::triggered, [&]() { on_remove(); });
		connect(m_refresh_action, &QAction::triggered, [&]() { on_transfer(); });
		connect(m_close_action, &QAction::triggered, [&]() { close(); });

		connect(m_pause_action, &QAction::triggered, [&]() { pause(); });
		connect(m_resume_action, &QAction::triggered, [&]() { resume(); });
		connect(m_finish_action, &QAction::triggered, [&]() { finish(); });

		connect(m_move_up_action, &QAction::triggered, [&]() { move_up(); });
		connect(m_move_down_action, &QAction::triggered, [&]() { move_down(); });
		connect(m_move_top_action, &QAction::triggered, [&]() { move_top(); });
		connect(m_move_bottom_action, &QAction::triggered, [&]() { move_bottom(); });
	}

	// Interface
	void download_widget::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto file = entity->get<file_component>();
		const auto download_map = m_entity->get<download_map_component>();
		{
			const auto pair = std::make_pair(file->get_id(), entity);
			const auto result = download_map->insert(pair);
			if (!result.second)
				return;
		}

		m_model->add(entity);

		const auto client = m_entity->get<client_component>();
		if (m_model->rowCount() <= client->get_downloads())
			on_update();
	}

	void download_widget::on_remove()
	{
		assert(thread_info::main());

		m_table->execute([&](const QModelIndex index)
		{
			const auto entity = m_model->get_entity(index);
			if (entity)
			{
				// Service
				const auto download_service = m_entity->get<download_service_component>();
				download_service->async_stop(entity);

				// Transfer
				// NOTE: Must happen AFTER we async_stop quit
				const auto transfer = entity->get<transfer_component>();
				transfer->stop();

				const auto file = entity->get<file_component>();
				const auto download_map = m_entity->get<download_map_component>();
				download_map->erase(file->get_id());

				if (!transfer->is_done())
				{
					try
					{
						// Delete partial file
						file_util::remove(file);
					}
					catch (const std::exception& ex)
					{
						m_entity->log(ex);
					}
				}

				// Callback
				m_entity->async_call(callback::download | callback::remove, entity);
			}
		});
	}

	void download_widget::on_update()
	{
		assert(thread_info::main());

		// Downloads
		size_t i = 0;
		const auto download_list = m_entity->get<download_list_component>();
		const auto download_service = m_entity->get<download_service_component>();

		const auto client = m_entity->get<client_component>();
		const auto iterations = !m_moved ? client->get_downloads() : download_list->size();

		// Normal
		for (const auto& e : *download_list)
		{
			// Optimization
			if (i >= iterations)
				break;

			// Paused downloads don't count
			const auto transfer = e->get<transfer_component>();
			if (transfer->is_paused())
				continue;

			// Active
			if (i < client->get_downloads())
			{
				if (transfer->is_running())
				{
					// Timeout
					const auto timeout = e->get<timeout_component>();
					if (timeout->expired(download::timeout))
					{
						// Service
						download_service->async_stop(e);

						// Transfer
						bool empty;
						const auto transfer_list = e->get<transfer_list_component>();
						{
							thread_lock(transfer_list);
							empty = transfer_list->empty();
						}

						// NOTE: Must happen AFTER we async_stop quit
						transfer->stop();
						transfer->set_seeds();

						if (transfer->get_retry() < download::max_retry)
						{
							// Retry
							if (!empty && !transfer->has_progress())
								transfer->retry();

							// Queue
							if (client->get_downloads() < download_list->size())
							{
								// Add/Remove
								m_add_list.push_back(e);
								m_remove_list.push_back(e);
							}
						}
						else
						{
							// Error
							transfer->error();

							// Remove
							m_remove_list.push_back(e);

							// Callback
							m_entity->async_call(callback::finished | callback::add, e);

							const auto file = e->get<file_component>();
							const auto str = boost::str(boost::format("\"%s\" timed out after %d seconds of inactivity") % file->get_name() % ((download::timeout * download::max_retry) / 1000));
							m_entity->log(str);
						}
					}
				}
				else if (!transfer->is_done())
				{
					try
					{
						// Create
						const auto file = e->get<file_component>();
						if (!file_util::exists(file))
							file_util::create(file, file->get_size());

						transfer->start();

						// Service
						const auto client_service = m_entity->get<client_service_component>();
						client_service->async_download(e);

						download_service->async_start(e);
					}
					catch (const std::exception& ex)
					{
						transfer->error();

						const auto status = e->get<status_component>();
						status->set_error();

						// Callback
						m_entity->async_call(callback::download | callback::update, e);
						m_entity->log(ex);
					}
				}
				else
				{
					// Service
					download_service->async_stop(e);

					// Remove
					m_remove_list.push_back(e);

					// Callback
					m_entity->async_call(callback::finished | callback::add, e);
				}
			}
			else
			{
				// Inactive
				if (transfer->is_running())
				{
					// Service
					download_service->async_stop(e);

					// Transfer
					// NOTE: Must happen AFTER we async_stop quit
					transfer->stop();
					transfer->set_seeds();

					const auto status = e->get<status_component>();
					status->set_pending();

					// Callback
					m_entity->async_call(callback::download | callback::update, e);
				}
				else if (transfer->is_done())
				{
					// Service
					download_service->async_stop(e);
				}
			}

			++i;
		}

		// Move
		m_moved = false;

		// Remove
		if (!m_remove_list.empty())
		{
			for (const auto& e : m_remove_list)
			{
				// Map
				const auto file = e->get<file_component>();
				const auto download_map = m_entity->get<download_map_component>();
				download_map->erase(file->get_id());

				m_model->remove(e);
			}

			m_remove_list.clear();
		}

		// Add
		if (!m_add_list.empty())
		{
			for (const auto& e : m_add_list)
				m_model->add(e);

			m_add_list.clear();
		}
	}

	void download_widget::on_clear()
	{
		assert(thread_info::main());

		const auto download_map = m_entity->get<download_map_component>();
		const auto download_list = m_entity->get<download_list_component>();

		for (const auto& e : *download_list)
		{
			// Service
			const auto download_service = m_entity->get<download_service_component>();
			download_service->async_stop(e);

			// Transfer
			// NOTE: Must happen AFTER we async_stop quit
			const auto transfer = e->get<transfer_component>();
			transfer->stop();

			// File
			const auto file = e->get<file_component>();
			download_map->erase(file->get_id());

			if (!transfer->is_done())
			{
				try
				{
					// Delete partial file
					file_util::remove(file);
				}
				catch (const std::exception& ex)
				{
					m_entity->log(ex);
				}
			}
		}

		m_model->clear();
	}

	void download_widget::on_transfer()
	{
		assert(thread_info::main());

		m_table->execute([&](const QModelIndex index)
		{
			const auto entity = m_model->get_entity(index);
			if (entity)
			{
				const auto transfer = entity->get<transfer_component>();
				if (transfer->is_running())
				{
					transfer->set_seeds();

					// Service
					const auto client_service = m_entity->get<client_service_component>();
					client_service->async_download(entity);
				}
			}
		});
	}

	// Utility
	void download_widget::on_open_file()
	{
		assert(thread_info::main());

		m_table->execute([&](const QModelIndex index)
		{
			const auto entity = m_model->get_entity(index);
			if (entity)
			{
				const auto transfer = entity->get<transfer_component>();
				if (!transfer->empty())
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

	void download_widget::on_open_folder()
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

	void download_widget::finish()
	{
		assert(thread_info::main());

		std::list<entity::ptr> entities;

		m_table->execute([&](const QModelIndex index)
		{
			const auto entity = m_model->get_entity(index);
			if (entity)
			{
				const auto transfer = entity->get<transfer_component>();
				if (!transfer->is_running() && !transfer->is_done())
				{
					// Callback
					entities.push_back(entity);
					m_entity->async_call(callback::finished | callback::add, entity);

					// Service
					const auto download_service = m_entity->get<download_service_component>();
					download_service->async_stop(entity);
				}

				// Transfer
				// NOTE: Must happen AFTER we async_stop quit
				transfer->cancel();
			}
		});

		// HACK: Avoid re-entrancy
		for (const auto& e : entities)
			m_model->remove(e);
	}

	void download_widget::pause()
	{
		assert(thread_info::main());

		m_table->execute([&](const QModelIndex index)
		{
			const auto entity = m_model->get_entity(index);
			if (entity)
			{
				const auto transfer = entity->get<transfer_component>();
				transfer->pause();
			}
		});
	}

	void download_widget::resume()
	{
		assert(thread_info::main());

		m_table->execute([&](const QModelIndex index)
		{
			const auto entity = m_model->get_entity(index);
			if (entity)
			{
				const auto transfer = entity->get<transfer_component>();
				transfer->resume();

				transfer->set_seeds();
			}
		});
	}

	void download_widget::move_up()
	{
		assert(thread_info::main());

		m_moved = true;

		m_table->move_up();
	}

	void download_widget::move_down()
	{
		assert(thread_info::main());

		m_moved = true;

		m_table->move_down();
	}

	void download_widget::move_top()
	{
		assert(thread_info::main());

		m_moved = true;

		m_table->move_top();
	}

	void download_widget::move_bottom()
	{
		assert(thread_info::main());

		m_moved = true;

		m_table->move_bottom();
	}

	// Is
	bool download_widget::is_paused() const
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
					const auto transfer = entity->get<transfer_component>();
					if (transfer->is_paused())
						return true;
				}
			}
		}

		return false;
	}

	// Menu
	void download_widget::show_menu(const QPoint& pos)
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

				menu.addAction(is_paused() ? m_resume_action : m_pause_action);
				menu.addAction(m_finish_action);
				menu.addSeparator();

				menu.addAction(m_remove_action);
				auto move_menu = make_menu("  Move...", fa::arrows);
				move_menu->setStyleSheet(QString("QMenu::item { height:%1px; width:%2px; }").arg(resource::get_row_height()).arg(resource::get_menu_width()));

				move_menu->addAction(m_move_up_action);
				move_menu->addAction(m_move_down_action);
				move_menu->addAction(m_move_top_action);
				move_menu->addAction(m_move_bottom_action);
				menu.addMenu(move_menu);
				menu.addSeparator();

				menu.addAction(m_refresh_action);
				menu.addSeparator();
			}
			else
			{
				menu.addAction(m_folder_action);
				menu.addSeparator();
			}
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
