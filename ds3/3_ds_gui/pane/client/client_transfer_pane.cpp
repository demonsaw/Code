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
#include <QSplitter>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>

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
#include "pane/client/client_transfer_pane.h"
#include "font/font_awesome.h"
#include "model/client/client_transfer_group_model.h"
#include "security/checksum/crc.h"
#include "security/filter/hex.h"
#include "utility/value.h"

namespace eja
{
	// Constructor
	client_transfer_pane::client_transfer_pane(QWidget* parent /*= 0*/) : entity_pane("Transfer Router Group", fa::users, callback_type::group, parent)
	{
		assert(thread_info::main());

		// Action
		m_add_action = make_action("Add", fa::plus);
		m_remove_action = make_action("Remove", fa::minus);
		m_update_action = make_action("Update", fa::bolt);
		m_file_action = make_action("Open File", fa::file);

		// Menu
		add_button("Add", fa::plus, [&]() { m_table->add(); });
		add_button("Remove", fa::minus, [&]() { remove(); });
		add_button("Update", fa::bolt, [&]() { update(); });

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
		m_titlebar->setObjectName("pane");
		m_titlebar->setLayout(hlayout);

		// Table
		m_table = new entity_table_view(m_entity, this);
		m_model = new client_transfer_group_model({ " #", "Entropy", "Size", "Percent", "Cipher", "Key Size", "Hash", "Iterations", "Salt" }, { 44, 256, 64, 64, 64, 64, 64, 64, 64 }, this);
		init(m_table, m_model);

		// Header
		const auto hheader = m_table->horizontalHeader();
		hheader->setSectionResizeMode(client_transfer_group_model::column::row, QHeaderView::Fixed);

		// Callback
		add_callback(callback_action::clear, [&](const entity::ptr entity){ emit clear(); });

		// Delegate
		m_table->setColumnWidth(client_transfer_group_model::column::row, 44);
		m_table->setItemDelegateForColumn(client_transfer_group_model::column::row, new row_checkbox_delegate(m_table));
		m_table->setItemDelegateForColumn(client_transfer_group_model::column::entropy, new group_entropy_delegate(m_table));
		m_table->setItemDelegateForColumn(client_transfer_group_model::column::cipher, new group_cipher_delegate(m_table));
		m_table->setItemDelegateForColumn(client_transfer_group_model::column::bits, new group_key_size_delegate(m_table));
		m_table->setItemDelegateForColumn(client_transfer_group_model::column::hash, new group_hash_delegate(m_table));
		m_table->setItemDelegateForColumn(client_transfer_group_model::column::percent, new group_percent_delegate(m_table));

		// Signal
		connect(m_table, &entity_table_view::customContextMenuRequested, this, &client_transfer_pane::show_menu);
		connect(m_table, &entity_table_view::clicked, this, &client_transfer_pane::on_click);

		connect(m_add_action, &QAction::triggered, [&]() { m_table->add(); });
		connect(m_remove_action, &QAction::triggered, [&]() { remove(); });
		connect(m_update_action, &QAction::triggered, [&]() { update(); });
		connect(m_file_action, &QAction::triggered, this, &client_transfer_pane::open_file);

		// Layout
		auto vlayout = new QVBoxLayout;
		vlayout->setSpacing(0);
		vlayout->setMargin(0);

		vlayout->addWidget(m_titlebar);
		vlayout->addWidget(m_table);

		setObjectName("pane");
		setLayout(vlayout);
	}

	// Interface
	void client_transfer_pane::on_update()
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

	void client_transfer_pane::on_remove()
	{
		m_table->remove();
	}

	void client_transfer_pane::open_file()
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

	// Slot
	void client_transfer_pane::on_click(const QModelIndex& index)
	{
		assert(thread_info::main());

		if (!index.isValid())
			return;

		const auto proxy = m_table->get_proxy_model();
		const auto sindex = proxy->mapToSource(index);
		if (!sindex.isValid())
			return;

		const auto entity = m_model->get_entity(sindex);
		assert(entity);

		if (entity)
		{
			const auto parent = entity->get_parent();
			if (parent->enabled())
				set_group_id(parent);
			else
				set_group_id();
		}
	}

	// Menu
	void client_transfer_pane::show_menu(const QPoint& pos)
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

	void client_transfer_pane::set_group_id()
	{
		m_title->setText("Transfer Router Group");
	}

	void client_transfer_pane::set_group_id(const entity::ptr entity)
	{
		const auto group = entity->get<group_component>();

		if (group->valid())
		{
			const auto group_id = hex::encode(static_cast<u32>(crc32()(group->get_id())));
			const auto title_text = QString("Transfer Router Group: %1").arg(group_id.c_str());
			m_title->setText(title_text);
		}
		else
		{
			const auto title_text = "Transfer Router Group";
			m_title->setText(title_text);
		}
	}
}
