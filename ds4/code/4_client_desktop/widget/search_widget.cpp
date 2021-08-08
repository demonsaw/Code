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

#include <cassert>

#include <QAction>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QKeyEvent>
#include <QLabel>
#include <QMenu>
#include <QSortFilterProxyModel>
#include <QScrollBar>

#include "component/browse_component.h"
#include "component/callback/callback.h"
#include "component/client/client_service_component.h"
#include "component/io/file_component.h"
#include "component/io/folder_component.h"
#include "component/message/message_component.h"
#include "component/search/search_component.h"
#include "component/search/search_option_component.h"

#include "delegate/io/file_name_delegate.h"
#include "entity/entity.h"
#include "entity/entity_tree_view_ex.h"
#include "factory/client_factory.h"
#include "font/font_awesome.h"
#include "model/search_model.h"
#include "model/proxy/search_proxy_model.h"
#include "resource/resource.h"
#include "thread/thread_info.h"
#include "utility/value.h"
#include "utility/io/folder_util.h"
#include "widget/search_widget.h"
#include "window/main_window.h"

namespace eja
{
	// Constructor
	search_widget::search_widget(const entity::ptr& entity, QWidget* parent /*= nullptr*/) : entity_dock_widget(entity, "Search", fa::search,  parent)
	{
		assert(thread_info::main());

		// Action
		m_download_action = make_action(" Download", fa::arrow_down);
		m_download_to_action = make_action(" Download To...", fa::hdd_o);
		m_clear_action = make_action(" Clear", fa::trash);
		m_refresh_action = make_action(" Refresh", fa::refresh);
		m_close_action = make_action(" Close", fa::close);

		// Menu
		add_button("Close", fa::close, [&]() { close(); });

		// Title
		const auto search_option = m_entity->get<search_option_component>();
		m_title_text = QString::fromStdString(search_option->get_text());
		m_title->setText(m_title_text);

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
		m_tree = new entity_tree_view_ex(m_entity, this);
		m_model = new search_model(m_entity, this);
		init_ex<search_proxy_model>(m_tree, m_model);

		const auto header = m_tree->header();
		header->setSortIndicator(search_model::column::weight, Qt::SortOrder::DescendingOrder);

		m_tree->setItemDelegateForColumn(search_model::column::name, new file_name_delegate(m_tree));
		m_tree->sortByColumn(search_model::column::weight);
		m_tree->set_column_sizes();

		setWidget(m_tree);

		// Event
		m_tree->installEventFilter(this);

		// Signal
		connect(this, &search_widget::customContextMenuRequested, this, &search_widget::show_menu);
		connect(m_tree, &entity_tree_view_ex::customContextMenuRequested, this, &search_widget::show_menu);
		connect(m_tree, &entity_tree_view_ex::doubleClicked, this, &search_widget::on_double_click);
		connect(header, &QHeaderView::customContextMenuRequested, [&](const QPoint& pos) { show_header_menu({ "Type", "Size", "Modified", "Matches", "Seeds" }, { search_model::column::type, search_model::column::size, search_model::column::time, search_model::column::weight, search_model::column::seeds }, m_tree, m_model, pos); });

		connect(m_download_action, &QAction::triggered, this, &search_widget::on_download);
		connect(m_download_to_action, &QAction::triggered, this, &search_widget::on_download_to);
		connect(m_clear_action, &QAction::triggered, [&]() { m_entity->async_call(callback::search | callback::clear, m_entity); });
		connect(m_refresh_action, &QAction::triggered, [&]() { update(); });
		connect(m_close_action, &QAction::triggered, [&]() { close(); });
	}

	// Interface
	void search_widget::on_create()
	{
		assert(thread_info::main());

		m_model->clear();

		// Title
		const auto search_option = m_entity->get<search_option_component>();
		m_title_text = QString::fromStdString(search_option->get_text());
		m_title->setText(m_title_text);
	}

	void search_widget::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());
		assert(!entity->has<file_component>() && !entity->has<folder_component>());

		// Message
		if (entity->has<browse_component>())
		{
			// HACK: Do not add if the parent has expired
			if (!entity->has_parent())
				return;

			const auto browse = entity->get<browse_component>();
			m_messages.insert(browse->get_id());
		}

		// Parent
		const auto parent = entity->has_parent() ? entity->get_parent() : m_entity;
		const auto parent_list = parent->get<browse_list_component>();

		// List
		const auto list = browse_list_component::create();
		const auto browse_list = entity->get<browse_list_component>();

		for (const auto& e : *browse_list)
		{
			if (!e->has_parent() && e->has<file_component>())
			{
				const auto file = e->get<file_component>();
				const auto browse_map = m_entity->get<browse_map_component>();
				const auto it = browse_map->find(file->get_id());

				if (it == browse_map->end())
				{
					const auto pair = std::make_pair(file->get_id(), e);
					browse_map->insert(pair);

					list->push_back(e);
				}
				else
				{
					// Swarm
					const auto& e2 = it->second;
					const auto search = e->get<search_component>();
					const auto s = e2->get<search_component>();
					s->add_swarm();

					// Matches
					if (search->get_weight() > s->get_weight())
					{
						s->set_weight(search->get_weight());
						e2->add<file_component>(e);
					}
				}
			}
			else
			{
				if (e->has<folder_component>())
				{
					const auto folder = e->get<folder_component>();
					auto it = std::find_if(parent_list->begin(), parent_list->end(), [folder](const entity::ptr e2)
					{
						if (e2->has<folder_component>())
						{
							const auto f = e2->get<folder_component>();
							return (folder->get_size() == f->get_size()) && (folder->get_name() == f->get_name());
						}

						return false;
					});

					if (it == parent_list->end())
					{
						list->push_back(e);
					}
					else
					{
						// Swarm
						const auto& e2 = *it;
						const auto search = e->get<search_component>();
						const auto s = e2->get<search_component>();
						s->add_swarm();

						// Matches
						if (search->get_weight() > s->get_weight())
						{
							s->set_weight(search->get_weight());
							e2->add<folder_component>(e);
						}
					}
				}
				else
				{
					list->push_back(e);
				}
			}
		}

		entity->set(list);

		m_model->add(entity);
	}

	void search_widget::on_update()
	{
		assert(thread_info::main());

		reset();

		// Clear
		const auto browse_map = m_entity->get<browse_map_component>();
		browse_map->clear();
		m_model->clear();

		// Service
		const auto owner = m_entity->get_owner();
		const auto service = owner->get<client_service_component>();
		if (service->valid())
		{
			const auto search_option = m_entity->get<search_option_component>();
			if (search_option->has_text())
				service->async_search(m_entity);
		}
	}

	void search_widget::on_clear()
	{
		assert(thread_info::main());

		reset();

		// Clear
		const auto search_option = m_entity->get<search_option_component>();
		search_option->set_text();

		const auto browse_map = m_entity->get<browse_map_component>();
		browse_map->clear();

		m_title_text.clear();
		m_title->setText(m_title_text);

		m_model->clear();
	}

	// Utility
	void search_widget::reset()
	{
		assert(thread_info::main());

		// Messages
		const auto owner = m_entity->get_owner();
		const auto message_map = owner->get<message_map_component>();
		{
			if (!m_messages.empty())
			{
				std::for_each(m_messages.begin(), m_messages.end(), [&](const std::string& message_id)
				{
					thread_lock(message_map);
					message_map->erase(message_id);
				});

				m_messages.clear();
			}
		}
	}

	void search_widget::on_double_click(const QModelIndex& index)
	{
		assert(thread_info::main());

		const auto proxy = m_tree->get_proxy();
		const auto sindex = proxy->mapToSource(index);
		const auto entity = m_model->get_entity(sindex);

		if (entity)
		{
			if (entity->has<file_component>())
			{
				const auto owner = m_entity->get_owner();
				const auto client = owner->get<client_component>();

				download(entity, client->get_path());

				open_tabs();
			}
		}
	}

	void search_widget::download(const entity::ptr& entity, const boost::filesystem::path& path)
	{
		if (entity->has<file_component>())
		{
			// File
			const auto owner = m_entity->get_owner();
			const auto download_entity = client_factory::create_download(owner, entity, path);

			// Callback
			m_entity->async_call(callback::download | callback::add, download_entity);
		}
		else if (entity->has<folder_component>())
		{
			try
			{
				// Folder
				folder_util::create(path);

				const auto browse_list = entity->get<browse_list_component>();
				if (browse_list->is_expanded())
				{
					for (const auto& e : *browse_list)
					{
						if (e->has<file_component>())
						{
							download(e, path);
						}
						else if (e->has<folder_component>())
						{
							// Folder
							boost::filesystem::path subpath(path);
							const auto folder = e->get<folder_component>();
							subpath /= folder->get_data();

							const auto bl = e->get<browse_list_component>();
							bl->set_download(true);
							bl->set_path(subpath);
							bl->set_search(true);

							download(e, subpath);
						}
					}
				}
				else
				{
					browse_list->set_download(true);
					browse_list->set_expanded(true);
					browse_list->set_search(true);

					// Service
					const auto owner = m_entity->get_owner();
					const auto service = owner->get<client_service_component>();
					service->async_browse(entity);

				}
			}
			catch (const std::exception& ex)
			{
				m_entity->log(ex);
			}
		}
	}

	void search_widget::on_download()
	{
		assert(thread_info::main());

		const auto owner = m_entity->get_owner();
		const auto service = owner->get<client_service_component>();
		if (service->invalid())
			return;

		m_tree->execute([&](const QModelIndex& index)
		{
			const auto entity = m_model->get_entity(index);
			if (entity)
			{
				if (entity->has<file_component>())
				{
					const auto client = owner->get<client_component>();

					download(entity, client->get_path());
				}
				else if (entity->has<folder_component>())
				{
					// Client
					const auto client = owner->get<client_component>();
					auto path = client->get_path();

					// Folder
					const auto folder = entity->get<folder_component>();
					path /= folder->get_data();

					const auto browse_list = entity->get<browse_list_component>();
					browse_list->set_path(path);

					download(entity, path);
				}
			}
		});

		open_tabs();
	}

	void search_widget::on_download_to()
	{
		assert(thread_info::main());

		const auto owner = m_entity->get_owner();
		const auto service = owner->get<client_service_component>();
		if (service->invalid())
			return;

		QFileDialog dialog;
		dialog.setWindowTitle("Download To...");
		dialog.setOptions(QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog);
		dialog.setFileMode(QFileDialog::DirectoryOnly);
		dialog.setViewMode(QFileDialog::ViewMode::List);

		const auto client = owner->get<client_component>();
#if _WSTRING
		dialog.setDirectory(QString::fromStdWString(client->get_path().wstring()));
#else
		dialog.setDirectory(QString::fromStdString(client->get_path().string()));
#endif
		if (dialog.exec())
		{
			const auto qpaths = dialog.selectedFiles();
			const auto& qpath = qpaths.at(0);
#if _WSTRING
			const auto root = qpath.toStdWString();
#else
			const auto root = qpath.toStdString();
#endif
			m_tree->execute([&](const QModelIndex& index)
			{
				const auto entity = m_model->get_entity(index);
				if (entity)
				{
					if (entity->has<file_component>())
					{
						// Folder
						boost::filesystem::path path(root);

						// File
						download(entity, path);
					}
					else if (entity->has<folder_component>())
					{
						// Folder
						boost::filesystem::path path(root);
						const auto folder = entity->get<folder_component>();
						path /= folder->get_data();

						const auto browse_list = entity->get<browse_list_component>();
						browse_list->set_path(path);

						download(entity, path);
					}
				}
			});

			open_tabs();
		}
	}

	void search_widget::open_tabs() const
	{
		// App
		const auto& app = main_window::get_app();
		if (app.is_auto_open())
		{
			// Callback
			m_entity->async_call(callback::download | callback::create);
			m_entity->async_call(callback::finished | callback::create);
		}
	}

	// Event
	void search_widget::closeEvent(QCloseEvent* event)
	{
		assert(thread_info::main());

		if (isVisible())
		{
			hide();

			reset();

			// Callback
			const auto owner = m_entity->get_owner();
			owner->async_call(callback::search | callback::destroy, m_entity);
		}
	}

	bool search_widget::eventFilter(QObject* object, QEvent* event)
	{
		if (event->type() == QEvent::KeyPress)
		{
			const auto ke = static_cast<QKeyEvent*>(event);

			switch (ke->key())
			{
				case Qt::Key_F5:
				{
					update();
					break;
				}
			}
		}

		return entity_dock_widget::eventFilter(object, event);
	}

	// Menu
	void search_widget::show_menu(const QPoint& pos)
	{
		assert(thread_info::main());

		QMenu menu;
		menu.setStyleSheet(QString("QMenu::item { height:%1px; width:%2px; }").arg(resource::get_row_height()).arg(resource::get_row_height() * 3));

		const auto owner = m_entity->get_owner();
		const auto service = owner->find<client_service_component>();

		if (service->valid())
		{
			if (!m_model->empty())
			{
				const auto index = m_tree->indexAt(pos);
				if (index.isValid())
				{
					menu.addAction(m_download_action);
					menu.addAction(m_download_to_action);
					menu.addSeparator();
				}

				menu.addAction(m_clear_action);
				menu.addSeparator();
			}

			menu.addAction(m_refresh_action);
			menu.addSeparator();
		}

		menu.addAction(m_close_action);

		menu.exec(m_tree->viewport()->mapToGlobal(pos));
	}
}
