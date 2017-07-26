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
#include <QFileDialog>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPoint>
#include <QTimer>
#include <QToolBar>

#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/group/group_component.h"
#include "delegate/row_checkbox_delegate.h"
#include "delegate/group/group_cipher_delegate.h"
#include "delegate/group/group_hash_delegate.h"
#include "delegate/group/group_entropy_delegate.h"
#include "delegate/group/group_key_size_delegate.h"
#include "delegate/group/group_percent_delegate.h"
#include "entity/entity.h"
#include "entity/entity_table_view.h"
#include "font/font_awesome.h"
#include "model/router/router_group_model.h"
#include "security/checksum/crc.h"
#include "security/filter/hex.h"
#include "widget/router/router_group_widget.h"

namespace eja
{
	// Constructor
	router_group_widget::router_group_widget(const entity::ptr entity, QWidget* parent /*= 0*/) : entity_widget(entity, "Groups", fa::users, callback_type::group, parent)
	{
		assert(thread_info::main());

		// Action		
		m_add_action = make_action("Add", fa::plus);
		m_remove_action = make_action("Remove", fa::minus);
		m_update_action = make_action("Update", fa::bolt);
		m_file_action = make_action("Open File", fa::file);

		// Menu
		add_button("Add", fa::plus, [&]() { m_table->add(); });
		add_button("Remove", fa::minus, [&]() { m_table->remove(); });
		add_button("Update", fa::bolt, [&]() { update(); });
		add_button("Popout", fa::clone, [&]() { popout(); });
		add_button("Close", fa::close, [&]() { close(); });

		// Layout
		auto hlayout = new QHBoxLayout;
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
		m_model = new router_group_model(m_entity, { " #", "Entropy", "Size", "Percent", "Cipher", "Key Size", "Hash", "Iterations", "Salt" }, { 44, 256, 64, 64, 64, 64, 64, 64, 64 }, this);
		init(m_table, m_model);

		// Header
		const auto hheader = m_table->horizontalHeader();
		hheader->setSectionResizeMode(router_group_model::column::row, QHeaderView::Fixed);

		// Callback		
		add_callback(callback_action::update, [&](const entity::ptr entity) { emit set_group_id(); });
		add_callback(callback_action::clear, [&](const entity::ptr entity){ m_model->clear(); });

		// Delegate
		m_table->setColumnWidth(router_group_model::column::row, 44);
		m_table->setItemDelegateForColumn(router_group_model::column::row, new row_checkbox_delegate(m_table));
		m_table->setItemDelegateForColumn(router_group_model::column::entropy, new group_entropy_delegate(m_table));
		m_table->setItemDelegateForColumn(router_group_model::column::cipher, new group_cipher_delegate(m_table));
		m_table->setItemDelegateForColumn(router_group_model::column::key_size, new group_key_size_delegate(m_table));
		m_table->setItemDelegateForColumn(router_group_model::column::hash, new group_hash_delegate(m_table));
		m_table->setItemDelegateForColumn(router_group_model::column::percent, new group_percent_delegate(m_table));

		setWidget(m_table);

		// Signal
		connect(m_table, &entity_table_view::customContextMenuRequested, this, &router_group_widget::show_menu);
		connect(this, &router_group_widget::set_group_id, this, &router_group_widget::on_set_group_id);
		connect(m_add_action, &QAction::triggered, [&]() { m_table->add(); });
		connect(m_remove_action, &QAction::triggered, [&]() { m_table->remove(); });
		connect(m_update_action, &QAction::triggered, [&]() { update(); });
		connect(m_file_action, &QAction::triggered, this, &router_group_widget::open_file);
	}

	// Utility
	void router_group_widget::on_update()
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

	void router_group_widget::open_file()
	{
		assert(thread_info::main());

		QModelIndexList selected = m_table->selectionModel()->selectedRows();
		if (selected.empty())
			return;

		QFileDialog dialog;
		dialog.setWindowTitle("Entropy File");
		dialog.setOptions(QFileDialog::ReadOnly);
		dialog.setOption(QFileDialog::DontUseNativeDialog, true);
		dialog.setFileMode(QFileDialog::ExistingFile);
		dialog.setViewMode(QFileDialog::ViewMode::List);

		if (dialog.exec())
		{
			const auto qpaths = dialog.selectedFiles();
			const auto qpath = qpaths.at(0);

			const auto proxy = m_table->get_proxy_model();
			const auto model = m_table->get_source_model();

			for (const auto& index : selected)
				model->setData(proxy->mapToSource(index), qpath, Qt::EditRole);
		}
	}

	// Menu
	void router_group_widget::show_menu(const QPoint& pos)
	{
		assert(thread_info::main());

		const auto index = m_table->indexAt(pos);
		if (!index.isValid())
			return;

		QMenu menu(this);
		menu.addAction(m_file_action); 
		menu.addSeparator();

		menu.addAction(m_add_action);
		menu.addAction(m_remove_action);
		menu.addAction(m_update_action);		

		menu.exec(m_table->viewport()->mapToGlobal(pos));
	}

	void router_group_widget::on_set_group_id()
	{
		assert(thread_info::main());

		const auto group = m_entity->get<group_component>();

		if (group->valid())
		{
			const auto group_id = hex::encode(static_cast<u32>(crc32()(group->get_id())));
			m_title_text = QString("Group: %1").arg(group_id.c_str());
		}
		else
		{
			m_title_text = "Group";
		}

		m_title->setText(m_title_text);
	}
}
