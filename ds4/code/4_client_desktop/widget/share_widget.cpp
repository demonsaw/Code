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
#include <QApplication>
#include <QDesktopServices>
#include <QDropEvent>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QKeyEvent>
#include <QLabel>
#include <QList>
#include <QMenu>
#include <QMimeData>
#include <QPoint>
#include <QSortFilterProxyModel>
#include <QStringList>
#include <QUrl>

#include "component/callback/callback.h"
#include "component/client/client_service_component.h"
#include "component/io/file_component.h"
#include "component/io/folder_component.h"
#include "component/io/share_component.h"

#include "delegate/share/share_delegate.h"
#include "dialog/archive_dialog.h"
#include "entity/entity.h"
#include "entity/entity_tree_view_ex.h"
#include "font/font_awesome.h"
#include "model/share_model.h"
#include "model/proxy/share_proxy_model.h"
#include "resource/resource.h"
#include "thread/thread_info.h"
#include "utility/io/file_util.h"
#include "utility/io/folder_util.h"
#include "widget/image_widget.h"
#include "widget/share_widget.h"

namespace eja
{
	// Constructor
	share_widget::share_widget(const entity::ptr& entity, QWidget* parent /*= 0*/) : entity_dock_widget(entity, "Share", fa::share_alt, parent)
	{
		assert(thread_info::main());

		// Action
		m_add_action = make_action(" Add", fa::plus);
		m_remove_action = make_action(" Remove", fa::minus);
		m_file_action = make_action(" Open File", fa::file);
		m_folder_action = make_action(" Open Folder", fa::folder_open);
		m_refresh_action = make_action(" Refresh", fa::refresh);
		m_close_action = make_action(" Close", fa::close);

		// Menu
		add_button("Add", fa::plus, [&]() { add(); });
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
		m_model = new share_model(m_entity, this);
		init_ex<share_proxy_model>(m_tree, m_model);

		const auto header = m_tree->header();
		header->setSortIndicator(share_model::column::name, Qt::SortOrder::AscendingOrder);

		m_tree->setItemDelegateForColumn(share_model::column::name, new share_delegate(m_tree));
		m_tree->sortByColumn(share_model::column::name);
		m_tree->set_column_sizes();

		setWidget(m_tree);

		// Drag/Drop Image
		m_image = new image_widget(resource::drag_drop, 400 * resource::get_ratio(), this);
		m_image->raise();

		const auto share_list = m_entity->get<share_list_component>();
		m_image->setVisible(share_list->empty());

		// Event
		m_tree->installEventFilter(this);

		// Callback
		add_callback(callback::share | callback::add, [&](const entity::ptr entity) { m_model->add(entity); });
		add_callback(callback::share | callback::update, [&]() { m_model->update(); });
		add_callback(callback::share | callback::update, [&](const entity::ptr entity) { m_model->update(entity); });
		add_callback(callback::share | callback::clear, [&]() { m_model->clear(); });

		// Signal
		connect(this, &share_widget::resized, m_image, &image_widget::parent_resized);
		connect(this, &share_widget::customContextMenuRequested, this, &share_widget::show_menu);
		connect(m_tree, &entity_tree_view_ex::customContextMenuRequested, this, &share_widget::show_menu);
		connect(m_tree, &entity_tree_view_ex::doubleClicked, this, &share_widget::on_open_file);
		connect(header, &QHeaderView::customContextMenuRequested, [&](const QPoint& pos) { show_header_menu({ "Type", "Size", "Modified" }, { share_model::column::type, share_model::column::size, share_model::column::time }, m_tree, m_model, pos); });

		connect(m_file_action, &QAction::triggered, this, &share_widget::on_open_file);
		connect(m_folder_action, &QAction::triggered, this, &share_widget::on_open_folder);

		connect(m_add_action, &QAction::triggered, [&]() { add(); });
		connect(m_remove_action, &QAction::triggered, [&]()
		{
			m_tree->remove();
			m_image->setVisible(m_model->empty());
		});

		connect(m_refresh_action, &QAction::triggered, [&]() { on_update(); });
		connect(m_close_action, &QAction::triggered, [&]() { close(); });

		setAcceptDrops(true);
	}

	// Interface
	void share_widget::on_add()
	{
		assert(thread_info::main());

		archive_dialog dialog;
		dialog.setWindowTitle("Add Share");

		const auto selected = m_tree->selectionModel()->selectedIndexes();
		if (!selected.empty())
		{
			const auto proxy = m_tree->get_proxy();
			const auto sindex = proxy->mapToSource(selected.first());
			const auto entity = m_model->get_entity(sindex);

			if (entity)
			{
				if (entity->has<file_component>())
				{
					const auto file = entity->get<file_component>();
#if _WSTRING
					dialog.setDirectory(QString::fromStdWString(file->get_wfolder()));
#else
					dialog.setDirectory(QString::fromStdString(file->get_folder()));
#endif
				}
				else if (entity->has<folder_component>())
				{
					const auto folder = entity->get<folder_component>();
#if _WSTRING
					dialog.setDirectory(QString::fromStdWString(folder->get_wfolder()));
#else
					dialog.setDirectory(QString::fromStdString(folder->get_folder()));
#endif
				}
			}
		}
		else
		{
			const auto qpath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
			dialog.setDirectory(qpath);
		}

		if (dialog.exec())
		{
			for (const auto& qpath : dialog.selectedFiles())
			{
#if _WSTRING
				const auto root = qpath.toStdWString();
#else
				const auto root = qpath.toStdString();
#endif
				entity::ptr entity;

				try
				{
					if (folder_util::exists(root))
					{
						const auto folder = folder_component::create(root);
						entity = entity::create(m_entity, folder);
						m_model->add(entity);
					}
					else if (file_util::exists(root))
					{
						const auto file = file_component::create(root);
						entity = entity::create(m_entity, file);
						m_model->add(entity);
					}
				}
				catch (const std::exception& ex)
				{
					m_entity->log(ex);
				}
			}

			m_image->setVisible(m_model->empty());
		}
	}

	void share_widget::on_update()
	{
		assert(thread_info::main());

		m_model->clear();

		// Service
		const auto client_service = m_entity->get<client_service_component>();
		if (client_service->valid())
			client_service->async_share();
	}

	// Utility
	void share_widget::on_open_file()
	{
		assert(thread_info::main());

		m_tree->execute([&](const QModelIndex index)
		{
			const auto entity = m_model->get_entity(index);
			if (entity)
			{
				if (entity->has<file_component>())
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

	void share_widget::on_open_folder()
	{
		assert(thread_info::main());

		m_tree->execute([&](const QModelIndex index)
		{
			const auto entity = m_model->get_entity(index);
			if (entity)
			{
				if (entity->has<file_component>())
				{
					const auto file = entity->get<file_component>();
#if _WSTRING
					const auto url = QUrl::fromLocalFile(QString::fromStdWString(file->get_wfolder()));
#else
					const auto url = QUrl::fromLocalFile(QString::fromStdString(file->get_folder()));
#endif
					QDesktopServices::openUrl(url);
				}
				else if (entity->has<folder_component>())
				{
					const auto folder = entity->get<folder_component>();
#if _WSTRING
					const auto url = QUrl::fromLocalFile(QString::fromStdWString(folder->get_wpath()));
#else
					const auto url = QUrl::fromLocalFile(QString::fromStdString(folder->get_path()));
#endif
					QDesktopServices::openUrl(url);
				}
			}
		});
	}

	// Is
	bool share_widget::is_file() const
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
				if (entity && entity->has<file_component>())
					return true;
			}
		}

		return false;
	}

	bool share_widget::is_folder() const
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
				if (entity && entity->has<folder_component>())
					return true;
			}
		}

		return false;
	}

	bool share_widget::is_removable() const
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
				if (entity && entity->has_parent())
					return false;
			}
		}

		return true;
	}

	// Event
	void share_widget::dropEvent(QDropEvent* event)
	{
		assert(thread_info::main());
		const QMimeData* mimeData = event->mimeData();

		// Get URL list
		if (mimeData->hasUrls())
		{
			QStringList pathList;
			QList<QUrl> urlList = mimeData->urls();

			// Get paths
			for (int i = 0; i < urlList.size(); ++i)
			{
#if _WSTRING
				const auto path = urlList.at(i).toLocalFile().toStdWString();
#else
				const auto path = urlList.at(i).toLocalFile().toStdString();
#endif
				try
				{
					entity::ptr entity;
					if (folder_util::exists(path))
					{
						const auto folder = folder_component::create(path);
						entity = entity::create(m_entity, folder);
						m_model->add(entity);
					}
					else if (file_util::exists(path))
					{
						const auto file = file_component::create(path);
						entity = entity::create(m_entity, file);
						m_model->add(entity);
					}
				}
				catch (const std::exception& ex)
				{
					m_entity->log(ex);
				}
			}

			m_image->setVisible(m_model->empty());
		}
	}

	bool share_widget::eventFilter(QObject* object, QEvent* event)
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
	void share_widget::show_menu(const QPoint& pos)
	{
		assert(thread_info::main());

		QMenu menu;
		menu.setStyleSheet(QString("QMenu::item { height:%1px; width:%2px; }").arg(resource::get_row_height()).arg(resource::get_menu_width()));

		if (!m_model->empty())
		{
			const auto index = m_tree->indexAt(pos);
			if (index.isValid())
			{
				if (is_file())
					menu.addAction(m_file_action);

				menu.addAction(m_folder_action);
				menu.addSeparator();
				menu.addAction(m_add_action);

				if (is_removable())
					menu.addAction(m_remove_action);

				menu.addSeparator();
			}
			else
			{
				menu.addAction(m_add_action);
				menu.addSeparator();
			}

			menu.addAction(m_refresh_action);
			menu.addSeparator();
		}
		else
		{
			menu.addAction(m_add_action);
			menu.addSeparator();
		}

		menu.addAction(m_close_action);

		menu.exec(m_tree->viewport()->mapToGlobal(pos));
	}
}
