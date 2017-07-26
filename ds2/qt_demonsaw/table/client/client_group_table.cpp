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

#include <thread>

#include <QAction>
#include <QDesktopServices>
#include <QFileDialog>
#include <QHeaderView>
#include <QMenu>
#include <QPoint>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <QUrl>

#include "client_group_table.h"
#include "component/client/client_option_component.h"
#include "component/io/file_component.h"
#include "delegate/group/group_cipher_delegate.h"
#include "delegate/group/group_key_size_delegate.h"
#include "delegate/group/group_hash_delegate.h"
#include "delegate/group/group_row_delegate.h"
#include "delegate/group/group_file_delegate.h"
#include "delegate/group/group_percent_delegate.h"
#include "model/client/client_group_model.h"
#include "resource/resource.h"
#include "window/client_window.h"
#include "window/main_window.h"

namespace eja
{
	// Constructor
	client_group_table::client_group_table(QWidget* parent /*= 0*/) : table_window(parent)
	{
		create(parent);
		layout(parent);
		signal(parent);
	}

	// Interface
	void client_group_table::create(QWidget* parent /*= 0*/)
	{
		table_window::create(parent);

		// HACK: Should be done via create function (virtual)
		//
		// Sort
		const auto hheader = horizontalHeader();
		hheader->setSortIndicator(get_sort_column(), Qt::SortOrder::AscendingOrder);

		// Action
		m_open_action = new QAction(tr("Open File"), this);
		m_open_action->setIcon(QIcon(resource::tab::file));

		m_remove_action = new QAction(tr("Remove"), this);
		m_remove_action->setIcon(QIcon(resource::menu::remove));		

		m_refresh_action = new QAction(tr("Refresh"), this);
		m_refresh_action->setIcon(QIcon(resource::menu::refresh));
	}

	void client_group_table::layout(QWidget* parent /*= 0*/)
	{
		// Delegate
		setItemDelegateForColumn(client_group_model::column::row, new group_row_delegate(this));
		setItemDelegateForColumn(client_group_model::column::file, new group_file_delegate(this));
		setItemDelegateForColumn(client_group_model::column::cipher, new group_cipher_delegate(this));
		setItemDelegateForColumn(client_group_model::column::bits, new group_key_size_delegate(this));
		setItemDelegateForColumn(client_group_model::column::hash, new group_hash_delegate(this));
		setItemDelegateForColumn(client_group_model::column::entropy, new group_percent_delegate(this));

		// Menu
		setContextMenuPolicy(Qt::CustomContextMenu);

		// Button
		add_button(table_button::add);
		add_button(table_button::remove, false);
		add_button(table_button::refresh);

		// Custom
		m_add_button->setToolTip("Add Crypto Layer");
		m_remove_button->setToolTip("Remove Crypto Layer");
	}

	void client_group_table::signal(QWidget* parent /*= 0*/)
	{
		connect(m_open_action, &QAction::triggered, this, &client_group_table::open);
		connect(m_remove_action, &QAction::triggered, this, &client_group_table::remove);
		connect(m_refresh_action, &QAction::triggered, this, &client_group_table::refresh);
		connect(this, &client_group_table::customContextMenuRequested, this, &client_group_table::show_menu);
	}

	// Slot
	void client_group_table::open()
	{
		QModelIndexList selected = selectionModel()->selectedRows();
		if (selected.empty())
			return;

		QFileDialog dialog;
		dialog.setWindowTitle("Group File");
		dialog.setFileMode(QFileDialog::ExistingFile);
		dialog.setOptions(QFileDialog::ReadOnly);
		dialog.setViewMode(QFileDialog::ViewMode::Detail);

		QStringList filters;
		filters << "BMP (*.bmp)" << "GIF (*.gif)" << "ICO (*.ico)" << "JPEG (*.jpeg *.jpg)" << "PBM (*.pbm)" << "PGM (*.pgm)" << "PNG (*.png)" << "PPM (*.ppm)" << "XBM (*.xbm)" << "XPM (*.xpm)" << "All Files (*)";
		dialog.setNameFilters(filters);
		dialog.selectNameFilter("All Files (*)");

		if (dialog.exec())
		{
			const auto qpaths = dialog.selectedFiles();
			const auto qpath = qpaths.at(0);

			const auto proxy = get_proxy_model();
			const auto model = get_source_model();

			for (const auto& index : selected)
				model->setData(proxy->mapToSource(index), qpath, Qt::EditRole);
		}
	}

	void client_group_table::refresh()
	{
		if (m_refresh_button)
		{
			m_refresh_button->setEnabled(false);
			QApplication::setOverrideCursor(Qt::WaitCursor);

			QTimer::singleShot(default_timeout::client::refresh, [this]()
			{
				m_refresh_button->setEnabled(true);
				QApplication::restoreOverrideCursor();
			});
		}

		std::thread thread([this]()
		{
			const auto model = get_source_model();
			model->refresh();
		});

		thread.detach();
	}

	void client_group_table::execute()
	{
		QModelIndexList selected = selectionModel()->selectedRows();
		if (selected.empty())
			return;

		std::thread thread([this, selected]()
		{
			const auto proxy = get_proxy_model();
			const auto model = get_source_model();
			
			const auto owner = model->get_entity();
			const auto group_vector = owner->get<group_entity_vector_component>();
			if (!group_vector)
				return;

			for (const auto& index : selected)
			{
				if (index.isValid())
				{
					const size_t row = static_cast<size_t>(index.row());
					const auto entity = group_vector->get(row);
					if (!entity)
						continue;

					const auto file = entity->get<file_component>();
					if (file)
					{
						const auto url = QUrl::fromLocalFile(QString::fromStdString(file->get_path()));
						QDesktopServices::openUrl(url);
					}
				}
			}
		});

		thread.detach();
	}

	void client_group_table::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
	{
		const auto enabled = selected.count() > 0;
		m_remove_button->setEnabled(enabled);

		// Event
		table_window::selectionChanged(selected, deselected);
	}

	void client_group_table::show_menu(const QPoint& pos)
	{
		const auto index = indexAt(pos);
		if (!index.isValid())
			return;

		QMenu menu(this);
		menu.addAction(m_open_action);				
		menu.addSeparator();

		menu.addAction(m_remove_action);
		menu.addAction(m_refresh_action);
		
		menu.exec(viewport()->mapToGlobal(pos));
	}
}