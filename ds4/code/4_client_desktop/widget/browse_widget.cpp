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

#include <algorithm>
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
#include <QShortcut>

#include "component/browse_component.h"
#include "component/chat_component.h"
#include "component/callback/callback.h"
#include "component/callback/callback_service_component.h"
#include "component/client/client_component.h"
#include "component/client/client_service_component.h"
#include "component/io/file_component.h"
#include "component/io/folder_component.h"
#include "component/message/message_component.h"

#include "delegate/browse/browse_delegate.h"
#include "entity/entity.h"
#include "entity/entity_tree_view_ex.h"
#include "factory/client_factory.h"
#include "font/font_awesome.h"
#include "model/browse_model.h"
#include "model/proxy/browse_proxy_model.h"
#include "resource/icon_provider.h"
#include "resource/resource.h"
#include "thread/thread_info.h"
#include "utility/value.h"
#include "utility/io/folder_util.h"
#include "widget/browse_widget.h"
#include "window/main_window.h"

namespace eja
{
	// Constructor
	browse_widget::browse_widget(const entity::ptr& entity, QWidget* parent /*= nullptr*/) : entity_dock_widget(entity, "Browse", fa::folder_open, parent)
	{
		assert(thread_info::main());

		// Action
		m_message_action = make_action(" Message", fa::envelope);
		m_mute_action = make_action(" Mute", fa::ban);
		m_unmute_action = make_action(" Unmute", fa::circle_o);
		m_download_action = make_action(" Download", fa::arrow_down);
		m_download_to_action = make_action(" Download To...", fa::hdd_o);
		m_refresh_action = make_action(" Refresh", fa::refresh);
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

		// Tree
		m_tree = new entity_tree_view_ex(m_entity, this);
		m_model = new browse_model(m_entity, this);
		init_ex<browse_proxy_model>(m_tree, m_model);

		const auto header = m_tree->header();
		header->setSortIndicator(browse_model::column::name, Qt::SortOrder::AscendingOrder);

		m_tree->setItemDelegateForColumn(browse_model::column::name, new browse_delegate(m_tree));
		m_tree->sortByColumn(browse_model::column::name);
		m_tree->set_column_sizes();

		setWidget(m_tree);

		// Event
		m_tree->installEventFilter(this);

		// Callback
		add_callback(callback::browse | callback::add, [&](const entity::ptr entity) { add(entity); });
		add_callback(callback::browse | callback::remove, [&](const entity::ptr entity) { remove(entity); });
		add_callback(callback::browse | callback::update, [&](const entity::ptr entity) { m_model->update(entity); });
		add_callback(callback::browse | callback::update, [&]() { m_model->update(); });
		add_callback(callback::browse | callback::clear, [&]() { clear(); });

		// Signal
		connect(this, &browse_widget::customContextMenuRequested, this, &browse_widget::show_menu);
		connect(m_tree, &entity_tree_view_ex::customContextMenuRequested, this, &browse_widget::show_menu);
		connect(m_tree, &entity_tree_view_ex::doubleClicked, this, &browse_widget::on_double_click);
		connect(header, &QHeaderView::customContextMenuRequested, [&](const QPoint& pos) { show_header_menu({ "Type", "Size", "Modified" }, { browse_model::column::type, browse_model::column::size, browse_model::column::time }, m_tree, m_model, pos); });

		connect(m_message_action, &QAction::triggered, this, &browse_widget::on_message);
		connect(m_mute_action, &QAction::triggered, [&]() { on_mute(true); });
		connect(m_unmute_action, &QAction::triggered, [&]() { on_mute(false); });
		connect(m_download_action, &QAction::triggered, this, &browse_widget::on_download);
		connect(m_download_to_action, &QAction::triggered, this, &browse_widget::on_download_to);
		connect(m_refresh_action, &QAction::triggered, [&]() { update(); });
		connect(m_close_action, &QAction::triggered, [&]() { close(); });
	}

	// Interface
	void browse_widget::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		if (entity->has<browse_component>())
		{
			// HACK: Do not add if the parent has expired
			if (!entity->has_parent())
				return;

			const auto browse = entity->get<browse_component>();
			m_messages.insert(browse->get_id());
		}
		else if (entity->has<client_component>())
		{
			const auto client = entity->get<client_component>();
			const auto pair = std::make_pair(client->get_id(), entity);
			const auto browse_map = m_entity->get<browse_map_component>();
			{
				thread_lock(browse_map);
				const auto result = browse_map->insert(pair);
				if (!result.second)
					return;
			}
		}

		m_model->add(entity);
	}

	void browse_widget::on_remove(const entity::ptr entity)
	{
		assert(thread_info::main());

		if (entity->has<client_component>())
		{
			const auto client = entity->get<client_component>();
			const auto browse_map = m_entity->get<browse_map_component>();
			{
				thread_lock(browse_map);
				const auto result = browse_map->erase(client->get_id());
				if (!result)
					return;
			}
		}

		m_model->remove(entity);
	}

	void browse_widget::on_update()
	{
		assert(thread_info::main());

		reset();

		// Service
		const auto client_service = m_entity->get<client_service_component>();
		if (client_service->valid())
			client_service->async_room();
	}

	void browse_widget::on_clear()
	{
		assert(thread_info::main());

		reset();

		m_model->clear();
	}

	// Utility
	void browse_widget::reset()
	{
		assert(thread_info::main());

		// Messages
		const auto message_map = m_entity->get<message_map_component>();
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

	void browse_widget::download(const entity::ptr& entity, const boost::filesystem::path& path)
	{
		assert(thread_info::main());

		if (entity->has<file_component>())
		{
			// File
			const auto download_entity = client_factory::create_download(m_entity, entity, path);

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

							download(e, subpath);
						}
					}
				}
				else
				{
					browse_list->set_download(true);
					browse_list->set_expanded(true);

					// Service
					const auto client_service = m_entity->get<client_service_component>();
					client_service->async_browse(entity);
				}
			}
			catch (const std::exception& ex)
			{
				m_entity->log(ex);
			}
		}
	}

	void browse_widget::on_download()
	{
		assert(thread_info::main());

		const auto client_service = m_entity->get<client_service_component>();
		if (client_service->invalid())
			return;

		m_tree->execute([&](const QModelIndex& index)
		{
			const auto entity = m_model->get_entity(index);
			if (entity)
			{
				if (entity->has<file_component>())
				{
					const auto client = m_entity->get<client_component>();

					download(entity, client->get_path());
				}
				else if (entity->has<folder_component>())
				{
					// Client
					const auto client = m_entity->get<client_component>();
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

	void browse_widget::on_download_to()
	{
		assert(thread_info::main());

		const auto client_service = m_entity->get<client_service_component>();
		if (client_service->invalid())
			return;

		QFileDialog dialog;
		dialog.setWindowTitle("Download To...");
		dialog.setOptions(QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog);
		dialog.setFileMode(QFileDialog::DirectoryOnly);
		dialog.setViewMode(QFileDialog::ViewMode::List);

		const auto client = m_entity->get<client_component>();
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

	void browse_widget::on_double_click(const QModelIndex& index)
	{
		assert(thread_info::main());

		const auto proxy = m_tree->get_proxy();
		const auto sindex = proxy->mapToSource(index);
		const auto entity = m_model->get_entity(sindex);

		if (entity)
		{
			if (entity->has<file_component>())
			{
				// TODO: FIX ME
				//
				//
				// NOTE: Browse entity for self doesn't match m_entity because of browse_list_component
				const auto client = m_entity->get<client_component>();
				const auto c = entity->find<client_component>();
				if (client->get_id() != c->get_id())
				{
					download(entity, client->get_path());

					open_tabs();
				}
			}
		}
	}

	void browse_widget::on_message()
	{
		assert(thread_info::main());

		m_tree->execute([&](const QModelIndex& index)
		{
			const auto entity = m_model->get_entity(index);
			if (entity)
				m_entity->async_call(callback::pm | callback::create, entity);
		});
	}

	void browse_widget::on_mute(const bool value)
	{
		assert(thread_info::main());

		m_tree->execute([&](const QModelIndex index)
		{
			const auto entity = m_model->get_entity(index);
			if (entity && entity->has<client_component>() && (entity != m_entity))
			{
				const auto client = entity->get<client_component>();
				client->set_mute(value);

				// Client
				m_entity->async_call(callback::client | callback::update, entity);

				// Chat
				m_entity->async_call(callback::chat | callback::update);

				// Room
				m_entity->async_call(callback::room | callback::update);

				// Pm
				m_entity->async_call(callback::pm | callback::update, entity);

				// Service
				const auto client_service = m_entity->get<client_service_component>();
				client_service->async_mute(entity, value);
			}
		});
	}

	void browse_widget::open_tabs() const
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
	bool browse_widget::eventFilter(QObject* object, QEvent* event)
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

	// Has
	bool browse_widget::has_client() const
	{
		assert(thread_info::main());

		const auto selected = m_tree->selectionModel()->selectedRows();
		if (!selected.empty())
		{
			const auto proxy = m_tree->get_proxy();
			const auto client = m_entity->get<client_component>();

			for (const auto& index : selected)
			{
				const auto sindex = proxy->mapToSource(index);
				const auto entity = m_model->get_entity(sindex);

				// TODO: FIX ME
				//
				//
				// NOTE: Browse entity for self doesn't match m_entity because of browse_list_component
				if (!entity || entity->has_parent() || !entity->has<client_component>() || (client->get_id() == entity->get<client_component>()->get_id()))
					return false;
			}
		}

		return true;
	}

	bool browse_widget::has_file_folder() const
	{
		assert(thread_info::main());

		const auto selected = m_tree->selectionModel()->selectedRows();
		if (!selected.empty())
		{
			const auto proxy = m_tree->get_proxy();
			const auto client = m_entity->get<client_component>();

			for (const auto& index : selected)
			{
				const auto sindex = proxy->mapToSource(index);
				const auto entity = m_model->get_entity(sindex);
				if (!entity || !(entity->has<file_component>() || entity->has<folder_component>()))
					return false;

				// TODO: FIX ME
				//
				//
				// NOTE: Browse entity for self doesn't match m_entity because of browse_list_component
				const auto c = entity->find<client_component>();
				if (client->get_id() == c->get_id())
					return false;
			}
		}

		return true;
	}

	// Is
	bool browse_widget::is_muted() const
	{
		assert(thread_info::main());

		const auto selected = m_tree->selectionModel()->selectedRows();
		if (!selected.empty())
		{
			const auto proxy = m_tree->get_proxy();

			for (const auto& index : selected)
			{
				const auto sindex = proxy->mapToSource(index);
				const auto entity = m_model->get_entity(sindex);
				if (entity && entity->has<client_component>())
				{
					const auto client = entity->get<client_component>();
					if (client->is_mute())
						return true;
				}
			}
		}

		return false;
	}

	// Menu
	void browse_widget::show_menu(const QPoint& pos)
	{
		assert(thread_info::main());

		QMenu menu;
		menu.setStyleSheet(QString("QMenu::item { height:%1px; width:%2px; }").arg(resource::get_row_height()).arg(resource::get_menu_width()));

		const auto client_service = m_entity->get<client_service_component>();

		if (client_service->valid())
		{
			if (!m_model->empty())
			{
				const auto index = m_tree->indexAt(pos);
				if (index.isValid())
				{
					if (has_client())
					{
						if (!is_muted())
						{
							menu.addAction(m_message_action);
							menu.addAction(m_mute_action);
							menu.addSeparator();
						}
						else
						{
							menu.addAction(m_unmute_action);
							menu.addSeparator();
						}
					}
					else if (has_file_folder())
					{
						menu.addAction(m_download_action);
						menu.addAction(m_download_to_action);
						menu.addSeparator();
					}
				}
			}

			menu.addAction(m_refresh_action);
			menu.addSeparator();
		}

		menu.addAction(m_close_action);

		menu.exec(m_tree->viewport()->mapToGlobal(pos));
	}
}
