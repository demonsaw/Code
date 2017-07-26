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
#include <QMouseEvent>
#include <QScrollBar>
#include <QSortFilterProxyModel>
#include <QTimer>
#include <QToolButton>

#include "tree_window.h"
#include "resource/resource.h"
#include "window/client_window.h"
#include "window/main_window.h"

namespace eja
{
	// Constructor
	tree_window::tree_window(QWidget* parent /*= 0*/) : QTreeView(parent)
	{
		// Table
		setIconSize(QSize(14, 14));
		setAllColumnsShowFocus(true);
		setUniformRowHeights(true);
		setSelectionBehavior(QAbstractItemView::SelectRows);
		setSelectionMode(ExtendedSelection);
		setAlternatingRowColors(true);
		setSortingEnabled(true);		
		setAttribute(Qt::WA_MacShowFocusRect, 0);

		// Header
		const auto hheader = header();
		hheader->setSortIndicator(get_sort_column(), Qt::SortOrder::AscendingOrder);
		hheader->setSortIndicatorShown(true);

		hheader->setHighlightSections(false);
		hheader->sectionResizeMode(QHeaderView::Stretch);
		hheader->setStretchLastSection(true);
		hheader->setDefaultSectionSize(23);
		hheader->setSectionsMovable(true);

		// Signal
		/*connect(hheader, &QHeaderView::sectionClicked, [=](int index)
		{			
			hheader->setSortIndicatorShown(true);
		});*/
	}

	// Interface
	void tree_window::init()
	{
		const auto column = get_sort_column();
		sortByColumn(column);
	}

	void tree_window::add()
	{
		const auto model = get_source_model();
		model->add();
	}

	void tree_window::remove()
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

	void tree_window::clear()
	{
		const auto model = get_source_model();
		model->clear();
	}

	void tree_window::refresh()
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

	void tree_window::double_click()
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

	void tree_window::execute()
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

	// Event
	void tree_window::mousePressEvent(QMouseEvent* event)
	{
		QModelIndex item = indexAt(event->pos());
		QTreeView::mousePressEvent(event);

		if ((item.row() == -1) && (item.column() == -1))
		{
			clearSelection();
			const QModelIndex index;
			selectionModel()->setCurrentIndex(index, QItemSelectionModel::Select);
		}
	}

	// Utility
	void tree_window::create(QWidget* parent /*= 0*/)
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
		connect(this, &tree_window::doubleClicked, this, &tree_window::double_click);
	}

	void tree_window::scroll()
	{
		// Auto scroll
		const auto scrollbar = verticalScrollBar();
		const auto scroll = !(scrollbar->isSliderDown() || (scrollbar->sliderPosition() != scrollbar->maximum()));
		if (scroll)
			scrollToBottom();
	}

	void tree_window::add_button(tree_button button, const bool enabled /*= true*/)
	{
		switch (button)
		{			
			case tree_button::add:
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
				connect(m_add_button, &QToolButton::clicked, this, &tree_window::add);

				break;
			}
			case tree_button::remove:
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
				connect(m_remove_button, &QToolButton::clicked, this, &tree_window::remove);

				break;
			}
			case tree_button::clear:
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
				connect(m_clear_button, &QToolButton::clicked, this, &tree_window::clear);

				break;
			}
			case tree_button::refresh:
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
				connect(m_refresh_button, &QToolButton::clicked, this, &tree_window::refresh);

				break;
			}
			default:
			{
				assert(false);
				break;
			}
		}
	}

	// Mutator
	void tree_window::setModel(QAbstractItemModel* model)
	{
		QTreeView::setModel(model);

		// Width
		const auto& table_model = get_source_model();
		const auto& header_sizes = table_model->get_column_sizes();

		for (size_t i = 0; i < header_sizes.size(); ++i)
			setColumnWidth(i, header_sizes[i]);
	}

	// Accessor
	entity_tree_model* tree_window::get_source_model() const
	{
		const QSortFilterProxyModel* proxy = get_proxy_model();
		const auto model = qobject_cast<entity_tree_model*>(proxy->sourceModel());
		assert(model);

		return model;
	}
}
