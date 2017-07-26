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
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QTimer>
#include <QToolBar>
#include <QUrl>

#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/io/file_component.h"
#include "component/io/folder_component.h"
#include "component/io/share_component.h"

#include "delegate/io/file_name_delegate.h"
#include "entity/entity.h"
#include "entity/entity_tree_view.h"
#include "font/font_awesome.h"
#include "widget/client/client_share_widget.h"

namespace eja
{
	// Constructor
	client_share_widget::client_share_widget(const entity::ptr entity, QWidget* parent /*= 0*/) : entity_widget(entity, "Shares", fa::sharealt, callback_type::share, parent)
	{
		assert(thread_info::main());

		// Action		
		m_add_action = make_action("Add", fa::plus);
		m_remove_action = make_action("Remove", fa::minus);
		m_update_action = make_action("Update", fa::bolt);
		m_file_action = make_action("Open File", fa::file);
		m_folder_action = make_action("Open Folder", fa::folderopen);

		// Menu
		add_button("Add", fa::plus, [&]() { m_tree->add(); });
		add_button("Remove", fa::minus, [&]() { m_tree->remove(); });
		add_button("Update", fa::bolt, [&]() { update(); });
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

		// Tree
		m_tree = new entity_tree_view(m_entity, this);
		m_model = new client_share_model(m_entity, { "Name", "Type", "Checksum", "Size" }, { 320, 72, 72, 72 }, this);
		init(m_tree, m_model);

		// Callback
		add_callback(callback_action::add, [&](const entity::ptr entity) { m_model->add(entity); });
		add_callback(callback_action::update, [&](const entity::ptr entity) { m_model->update(entity); });
		add_callback(callback_action::clear, [&](const entity::ptr entity){ m_model->clear(); });

		// Delegate
		m_tree->setItemDelegateForColumn(client_share_model::column::name, new file_name_delegate(m_tree));

		// Widget
		setWidget(m_tree);

		// Signal
		connect(m_tree, &entity_tree_view::doubleClicked, m_tree, &entity_tree_view::double_click);
		connect(m_tree, &entity_tree_view::customContextMenuRequested, this, &client_share_widget::show_menu);
		
		connect(m_add_action, &QAction::triggered, [&]() { m_tree->add(); });
		connect(m_remove_action, &QAction::triggered, [&]() { m_tree->remove(); });
		connect(m_update_action, &QAction::triggered, [&]() { update(); });

		connect(m_file_action, &QAction::triggered, [&]() { m_tree->double_click(); });
		connect(m_folder_action, &QAction::triggered, this, &client_share_widget::open_folder);
	}

	// Utility
	void client_share_widget::on_update()
	{
		assert(thread_info::main());

		// Spam
		auto button = get_button(fa::bolt);
		if (button)
		{
			button->setEnabled(false);
			QTimer::singleShot(ui::refresh, [&]() { set_enabled(fa::bolt); });
		}
		
		m_model->update();
	}

	void client_share_widget::open_folder()
	{
		assert(thread_info::main());

		const auto selected = m_tree->selectionModel()->selectedIndexes();
		if (selected.empty())
			return;

		const auto proxy = m_tree->get_proxy_model();
		const auto model = m_tree->get_source_model();

		for (const auto& index : selected)
		{
			if (!index.isValid())
				continue;

			const auto sindex = proxy->mapToSource(index);
			if (!sindex.isValid())
				continue;

			const auto entity = model->get_entity(sindex);
			if (!entity)
				return;

			if (entity)
			{
				if (entity->has<file_component>())
				{
					const auto file = entity->get<file_component>();
					const auto url = QUrl::fromLocalFile(QString::fromStdString(file->get_folder()));
					QDesktopServices::openUrl(url);
				}
				else if (entity->has<folder_component>())
				{
					const auto folder = entity->get<folder_component>();
					const auto url = QUrl::fromLocalFile(QString::fromStdString(folder->get_path()));
					QDesktopServices::openUrl(url);
				}
			}
		}
	}

	bool client_share_widget::is_file() const
	{
		assert(thread_info::main());

		const auto selected = m_tree->selectionModel()->selectedIndexes();
		if (selected.empty())
			return false;

		const auto proxy = m_tree->get_proxy_model();
		const auto model = m_tree->get_source_model();

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

	bool client_share_widget::is_folder() const
	{
		assert(thread_info::main());

		const auto selected = m_tree->selectionModel()->selectedIndexes();
		if (selected.empty())
			return false;

		const auto proxy = m_tree->get_proxy_model();
		const auto model = m_tree->get_source_model();

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

	bool client_share_widget::is_removable() const
	{
		assert(thread_info::main());

		const auto selected = m_tree->selectionModel()->selectedIndexes();
		if (selected.empty())
			return false;

		const auto proxy = m_tree->get_proxy_model();
		const auto model = m_tree->get_source_model();
		const auto share_list = m_entity->get<share_list_component>();

		for (const auto& index : selected)
		{
			if (!index.isValid())
				continue;

			const auto entity = model->get_entity(proxy->mapToSource(index));
			if (entity)
			{
				const auto it = std::find(share_list->begin(), share_list->end(), entity);
				if (it != share_list->end())
					return true;
			}			
		}

		return false;
	}

	// Menu
	void client_share_widget::show_menu(const QPoint& pos)
	{
		assert(thread_info::main());

		const auto index = m_tree->indexAt(pos);
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

		menu.addAction(m_add_action);

		if (is_removable())
			menu.addAction(m_remove_action);

		menu.addAction(m_update_action);

		menu.exec(m_tree->viewport()->mapToGlobal(pos));
	}
}
