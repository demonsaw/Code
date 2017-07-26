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

#include <QApplication>
#include <QHeaderView>
#include <QScrollBar>
#include <QSortFilterProxyModel>
#include <QThread>
#include <QTimer>
#include <QToolButton>

#include "table_window.h"
#include "resource/resource.h"
#include "window/client_window.h"
#include "window/main_window.h"

namespace eja
{
	// Constructor
	table_window::table_window(QWidget* parent /*= 0*/) : QTableView(parent)
	{
		// Table
		resizeRowsToContents();
		resizeColumnsToContents();
		setSelectionBehavior(QAbstractItemView::SelectRows);
		setAlternatingRowColors(true);
		setSortingEnabled(true);
		setShowGrid(false);
        setAttribute(Qt::WA_MacShowFocusRect, 0);

		setSelectionMode(QAbstractItemView::ExtendedSelection);
		setDragEnabled(true);
		setAcceptDrops(true);
		setDropIndicatorShown(true);
		setDragDropMode(QAbstractItemView::InternalMove);
		setDefaultDropAction(Qt::MoveAction);
		setDragDropOverwriteMode(false);

		// Header
		const auto vheader = verticalHeader();
		vheader->setSectionsMovable(true);
		vheader->setHighlightSections(false);
		vheader->setDefaultSectionSize(23);
		vheader->hide();

		const auto hheader = horizontalHeader();		
		hheader->setSortIndicator(get_sort_column(), Qt::SortOrder::AscendingOrder); 
		hheader->setSortIndicatorShown(true);

		hheader->setHighlightSections(false);
		hheader->sectionResizeMode(QHeaderView::Fixed);
		hheader->setStretchLastSection(true);
		hheader->setSectionsMovable(true);

		// Signal
		/*connect(hheader, &QHeaderView::sectionClicked, [=](int index)
		{
			hheader->setSortIndicatorShown(true);
		});*/
	}

	// Interface
	void table_window::init()
	{
		const auto column = get_sort_column();
		sortByColumn(column);
	}

	void table_window::add()
	{
		const auto model = get_source_model();
		model->add();
	}

	void table_window::remove()
	{
		QModelIndexList selected = selectionModel()->selectedRows();
		if (selected.empty())
			return;

		// Sort so that we don't crash
		if (selected.count() > 1)
			std::sort(selected.begin(), selected.end());

		const auto proxy = get_proxy_model();
		const auto model = get_source_model();
		auto it = selected.end();

		while (it != selected.begin())
		{
			--it;
			const auto& index = *it;
			model->remove(proxy->mapToSource(index));
		}
	}

	void table_window::clear()
	{
		const auto model = get_source_model();
		model->clear();
	}

	void table_window::refresh()
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

		const auto model = get_source_model();
		QModelIndexList selected = selectionModel()->selectedRows();

		if (selected.empty())
		{
			model->refresh();
		}
		else
		{
			const auto proxy = get_proxy_model();
			for (const auto& index : selected)
				model->refresh(proxy->mapToSource(index));
		}
	}

	void table_window::double_click()
	{
		QModelIndexList selected = selectionModel()->selectedRows();
		if (selected.empty())
			return;

		const auto proxy = get_proxy_model();
		const auto model = get_source_model();

		for (const auto& index : selected)
		{
			if (index.isValid())
				model->double_click(proxy->mapToSource(index));
		}
	}

	void table_window::execute()
	{
		QModelIndexList selected = selectionModel()->selectedRows();
		if (selected.empty())
			return;

		const auto proxy = get_proxy_model();
		const auto model = get_source_model();

		for (const auto& index : selected)
		{
			if (index.isValid())
				model->execute(proxy->mapToSource(index));
		}
	}

	// Utility
	void table_window::create(QWidget* parent /*= 0*/)
	{
		// Toolbar
		m_toolbar = new QToolBar(parent);
		m_toolbar->setObjectName("tab");
		m_toolbar->setCursor(Qt::ArrowCursor);
		m_toolbar->setIconSize(QSize(16, 16));

		// Spacer
		QWidget* spacer = new QWidget(m_toolbar);
		spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		m_toolbar->addWidget(spacer);

		// Signal
		connect(this, &table_window::doubleClicked, this, &table_window::double_click);
	}

	void table_window::add_button(table_button button, const bool enabled /*= true*/)
	{
		switch (button)
		{			
			case table_button::add:
			{
				// Button
				m_add_button = new QToolButton(m_toolbar);
				m_add_button->setObjectName("tab");
				m_add_button->setIcon(QIcon(resource::menu::add));
				m_add_button->setCursor(Qt::ArrowCursor);
				m_add_button->setToolTip("Add");
				m_add_button->setEnabled(enabled);
				m_toolbar->addWidget(m_add_button);

				// Signal
				connect(m_add_button, &QToolButton::clicked, this, &table_window::add);

				break;
			}			
			case table_button::remove:
			{
				// Button
				m_remove_button = new QToolButton(m_toolbar);
				m_remove_button->setObjectName("tab");
				m_remove_button->setIcon(QIcon(resource::menu::remove));
				m_remove_button->setCursor(Qt::ArrowCursor);
				m_remove_button->setToolTip("Remove");
				m_remove_button->setEnabled(enabled);
				m_toolbar->addWidget(m_remove_button);

				// Signal
				connect(m_remove_button, &QToolButton::clicked, this, &table_window::remove);

				break;
			}
			case table_button::clear:
			{
				// Button
				m_clear_button = new QToolButton(m_toolbar);
				m_clear_button->setObjectName("tab");
				m_clear_button->setIcon(QIcon(resource::menu::clear));
				m_clear_button->setCursor(Qt::ArrowCursor);
				m_clear_button->setToolTip("Clear");
				m_clear_button->setEnabled(enabled);
				m_toolbar->addWidget(m_clear_button);

				// Signal
				connect(m_clear_button, &QToolButton::clicked, this, &table_window::clear);

				break;
			}
			case table_button::refresh:
			{
				// Button
				m_refresh_button = new QToolButton(m_toolbar);
				m_refresh_button->setObjectName("tab");
				m_refresh_button->setIcon(QIcon(resource::menu::refresh));
				m_refresh_button->setCursor(Qt::ArrowCursor);
				m_refresh_button->setToolTip("Refresh");
				m_refresh_button->setEnabled(enabled);
				m_toolbar->addWidget(m_refresh_button);

				// Signal
				connect(m_refresh_button, &QToolButton::clicked, this, &table_window::refresh);

				break;
			}
			default:
			{
				assert(false);
				break;
			}
		}
	}

	void table_window::scroll()
	{
		// Auto scroll
		const auto scrollbar = verticalScrollBar();
		const auto scroll = !(scrollbar->isSliderDown() || (scrollbar->sliderPosition() != scrollbar->maximum()));
		if (scroll)
			scrollToBottom();
	}

	// Mutator
	void table_window::setModel(QAbstractItemModel* model)
	{
		QTableView::setModel(model);

		// Width
		const auto& table_model = get_source_model();
		const auto& header_sizes = table_model->get_column_sizes();

		for (size_t i = 0; i < header_sizes.size(); ++i)
			setColumnWidth(i, header_sizes[i]);
	}

	// Accessor
	entity_table_model* table_window::get_source_model() const
	{
		const QSortFilterProxyModel* proxy = get_proxy_model();
		const auto model = qobject_cast<entity_table_model*>(proxy->sourceModel());
		assert(model);

		return model;
	}
}
