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

#include <boost/algorithm/string.hpp>

#include <QAction>
#include <QFileDialog>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPoint>
#include <QSortFilterProxyModel>

#include "component/pixmap_component.h"
#include "component/callback/callback.h"
#include "component/client/client_service_component.h"
#include "component/group/group_component.h"

#include "delegate/group/group_cipher_delegate.h"
#include "delegate/group/group_hash_delegate.h"
#include "delegate/group/group_entropy_delegate.h"
#include "delegate/group/group_key_size_delegate.h"
#include "delegate/group/group_percent_delegate.h"
#include "delegate/group/group_row_delegate.h"

#include "entity/entity.h"
#include "entity/entity_table_view_ex.h"
#include "factory/client_factory.h"
#include "font/font_awesome.h"
#include "model/group_model.h"
#include "resource/resource.h"
#include "security/filter/hex.h"
#include "thread/thread_info.h"
#include "widget/group_widget.h"

namespace eja
{
	// Constructor
	group_widget::group_widget(const entity::ptr& entity, QWidget* parent /*= nullptr*/) : entity_dock_widget(entity, "Group", fa::shield, parent)
	{
		assert(thread_info::main());

		// Action
		m_file_action = make_action(" Open File", fa::file);

		m_add_action = make_action(" Add", fa::plus);
		m_remove_action = make_action(" Remove", fa::minus);
		m_close_action = make_action(" Close", fa::close);

		m_move_up_action = make_action(" Up", fa::angle_up);
		m_move_down_action = make_action(" Down", fa::angle_down);
		m_move_top_action = make_action(" Top", fa::angle_double_up);
		m_move_bottom_action = make_action(" Bottom", fa::angle_double_down);

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

		// Table
		m_table = new entity_table_view_ex(m_entity, this);
		m_model = new group_model(m_entity, this);
		init_ex(m_table, m_model);

		const auto header = m_table->horizontalHeader();
		header->setSortIndicator(group_model::column::row, Qt::SortOrder::AscendingOrder);
		header->setSectionResizeMode(group_model::column::row, QHeaderView::Fixed);

		m_table->setDragEnabled(true);
		m_table->setAcceptDrops(true);
		m_table->setDropIndicatorShown(true);
		m_table->setDragDropMode(QAbstractItemView::InternalMove);
		m_table->setDefaultDropAction(Qt::MoveAction);
		m_table->setDragDropOverwriteMode(false);

		m_table->setItemDelegateForColumn(group_model::column::row, new group_row_delegate(m_table));
		m_table->setItemDelegateForColumn(group_model::column::entropy, new group_entropy_delegate(m_table));
		m_table->setItemDelegateForColumn(group_model::column::cipher, new group_cipher_delegate(m_table));
		m_table->setItemDelegateForColumn(group_model::column::bits, new group_key_size_delegate(m_table));
		m_table->setItemDelegateForColumn(group_model::column::hash, new group_hash_delegate(m_table));
		m_table->setItemDelegateForColumn(group_model::column::percent, new group_percent_delegate(m_table));
		m_table->sortByColumn(group_model::column::row);
		m_table->set_column_sizes();

		setWidget(m_table);

		// Event
		m_table->installEventFilter(this);

		// Callback
		add_callback(callback::group | callback::update, [&]() { emit set_id(); });
		add_callback(callback::group | callback::clear, [&]() { m_model->clear(); });

		// Signal
		connect(this, &group_widget::set_id, this, &group_widget::on_set_id);
		connect(this, &group_widget::customContextMenuRequested, this, &group_widget::show_menu);
		connect(m_table, &group_widget::customContextMenuRequested, this, &group_widget::show_menu);
		connect(header, &QHeaderView::customContextMenuRequested, [&](const QPoint& pos) { show_header_menu({ "Size", "Percent", "Cipher", "Key Size", "Hash", "Iterations", "Salt" }, { group_model::column::size, group_model::column::percent, group_model::column::cipher, group_model::column::bits, group_model::column::hash, group_model::column::iterations, group_model::column::salt }, m_table, m_model, pos); });

		connect(m_file_action, &QAction::triggered, this, &group_widget::on_open_file);
		connect(m_add_action, &QAction::triggered, [&]() { add(); });
		connect(m_remove_action, &QAction::triggered, [&]() { m_table->remove(); });
		connect(m_close_action, &QAction::triggered, [&]() { close(); });

		connect(m_move_up_action, &QAction::triggered, [&]() { m_table->move_up(); });
		connect(m_move_down_action, &QAction::triggered, [&]() { m_table->move_down(); });
		connect(m_move_top_action, &QAction::triggered, [&]() { m_table->move_top(); });
		connect(m_move_bottom_action, &QAction::triggered, [&]() { m_table->move_bottom(); });
	}

	// Interface
	void group_widget::on_add()
	{
		assert(thread_info::main());

		// Entity
		const auto entity = client_factory::create_group_option(m_entity);
		entity->add<pixmap_component>();
		entity->disable();

		m_model->add(entity);
	}

	// Utility
	void group_widget::on_open_file()
	{
		assert(thread_info::main());

		QModelIndexList selected = m_table->selectionModel()->selectedRows();
		if (selected.empty())
			return;

		QFileDialog dialog;
		dialog.setWindowTitle("Open File");
		dialog.setOptions(QFileDialog::ReadOnly | QFileDialog::DontUseNativeDialog);
		dialog.setFileMode(QFileDialog::ExistingFile);
		dialog.setViewMode(QFileDialog::ViewMode::List);

		if (dialog.exec())
		{
			const auto qpaths = dialog.selectedFiles();
			const auto qpath = qpaths.at(0);

			m_table->execute(group_model::column::entropy, [&](const QModelIndex index)
			{
				m_model->setData(index, qpath, Qt::EditRole);
			});
		}
	}

	// Set
	void group_widget::on_set_id()
	{
		assert(thread_info::main());

		m_model->update();

		QString qstr;
		const auto group = m_entity->get<group_component>();

		if (group->valid())
		{
			const auto group_id = hex::encode(group->get_id());

			auto prefix = group_id.substr(0, 8);
			boost::to_upper(prefix);

			auto suffix = group_id.substr(group_id.size() - 8, group_id.size());
			boost::to_upper(suffix);

			qstr = QString("Group  -  %1  %2").arg(prefix.c_str()).arg(suffix.c_str());
		}
		else
		{
			qstr = "Group";
		}

		if (qstr != m_title_text)
		{
			m_title_text = qstr;
			m_title->setText(m_title_text);

			// Callback
			m_entity->async_call(callback::chat | callback::clear);
			m_entity->async_call(callback::download | callback::clear);
			m_entity->async_call(callback::finished | callback::clear);
			m_entity->async_call(callback::statusbar | callback::clear);
			m_entity->async_call(callback::upload | callback::clear);

			m_entity->async_call(callback::pm | callback::destroy);
			m_entity->async_call(callback::room | callback::destroy);
			m_entity->async_call(callback::search | callback::destroy);
		}
	}

	// Menu
	void group_widget::show_menu(const QPoint& pos)
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
				menu.addSeparator();

				menu.addAction(m_add_action);
				menu.addAction(m_remove_action);

				auto move_menu = make_menu("  Move...", fa::arrows);
				move_menu->setStyleSheet(QString("QMenu::item { height:%1px; width:%2px; }").arg(resource::get_row_height()).arg(resource::get_menu_width()));

				move_menu->addAction(m_move_up_action);
				move_menu->addAction(m_move_down_action);
				move_menu->addAction(m_move_top_action);
				move_menu->addAction(m_move_bottom_action);
				menu.addMenu(move_menu);

				menu.addSeparator();
			}
			else
			{
				menu.addAction(m_add_action);
				menu.addSeparator();
			}
		}
		else
		{
			menu.addAction(m_add_action);
			menu.addSeparator();
		}

		menu.addAction(m_close_action);

		menu.exec(m_table->viewport()->mapToGlobal(pos));
	}
}
