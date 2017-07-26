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

#include <QHeaderView>
#include <QMouseEvent>
#include <QScrollBar>
#include <QSortFilterProxyModel>

#include "entity/entity.h"
#include "entity/entity_tree_view.h"
#include "entity/entity_tree_model.h"

namespace eja
{
	// Constructor
	entity_tree_view::entity_tree_view(const entity::ptr entity, QWidget* parent /*= 0*/) : QTreeView(parent)
	{
		// Tree
		setContextMenuPolicy(Qt::CustomContextMenu);
		setSelectionBehavior(QAbstractItemView::SelectRows);
		setSelectionMode(ExtendedSelection);
		setAlternatingRowColors(true);

		setSortingEnabled(true);
		setAttribute(Qt::WA_MacShowFocusRect, 0);
		setAllColumnsShowFocus(true);
		setUniformRowHeights(true);
		setIconSize(QSize(16, 16));

		// Header
		const auto hheader = header();		
		hheader->setHighlightSections(false);
		hheader->sectionResizeMode(QHeaderView::ResizeToContents);
		hheader->setStretchLastSection(false);
		hheader->setDefaultSectionSize(22);
		hheader->setSectionsMovable(true);

		// Sort
		hheader->setSortIndicator(0, Qt::SortOrder::AscendingOrder);
		hheader->setSortIndicatorShown(true);
		sortByColumn(0);

		// Signal
		//connect(this, &entity_tree_view::clicked, this, &entity_tree_view::click);
		//connect(this, &entity_tree_view::doubleClicked, this, &entity_tree_view::double_click);
	}

	// Utility
	void entity_tree_view::add()
	{
		const auto model = get_source_model();
		model->add();
	}

	void entity_tree_view::remove()
	{
		const auto model = get_source_model();
		auto selected = selectionModel()->selectedRows();

		if (selected.empty())// || (selected.count() == model->rowCount()))
		{
			model->remove();
		}
		else
		{
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

				if (index.isValid())
				{
					const auto sindex = proxy->mapToSource(index);
					if (sindex.isValid())
						model->remove(sindex);
				}
			}

			clearSelection();
			selectionModel()->setCurrentIndex(QModelIndex(), QItemSelectionModel::Select);
			emit unclick();
		}
	}

	void entity_tree_view::clear()
	{
		const auto model = get_source_model();
		model->clear();
	}

	void entity_tree_view::update()
	{
		const auto model = get_source_model();
		const auto selected = selectionModel()->selectedRows();

		if (selected.empty())
		{
			model->update();
		}
		else
		{
			const auto proxy = get_proxy_model();
			const auto model = get_source_model();

			for (const auto& index : selected)
			{
				if (index.isValid())
				{
					const auto sindex = proxy->mapToSource(index);
					if (sindex.isValid())
						model->update(sindex);
				}
			}
		}
	}

	void entity_tree_view::click()
	{
		const auto selected = selectionModel()->selectedRows();
		if (selected.empty())
			return;

		const auto proxy = get_proxy_model();
		const auto model = get_source_model();

		for (const auto& index : selected)
		{
			if (index.isValid())
			{
				const auto sindex = proxy->mapToSource(index);
				if (sindex.isValid())
					model->click(sindex);
			}
		}
	}

	void entity_tree_view::double_click()
	{
		const auto selected = selectionModel()->selectedRows();
		if (selected.empty())
			return;

		const auto proxy = get_proxy_model();
		const auto model = get_source_model();

		for (const auto& index : selected)
		{
			if (index.isValid())
			{
				const auto sindex = proxy->mapToSource(index);
				if (sindex.isValid())
					model->double_click(sindex);
			}
		}
	}

	void entity_tree_view::scroll()
	{
		// Auto scroll
		const auto scrollbar = verticalScrollBar();
		const auto scroll = !(scrollbar->isSliderDown() || (scrollbar->sliderPosition() != scrollbar->maximum()));
		if (scroll)
			scrollToBottom();
	}

	// Event
	void entity_tree_view::resizeEvent(QResizeEvent* event)
	{
		QTreeView::resizeEvent(event);

		emit resize();
	}

	void entity_tree_view::mousePressEvent(QMouseEvent* event)
	{
		const auto index = indexAt(event->pos());
		if (!index.isValid())
		{
			clearSelection();
			selectionModel()->setCurrentIndex(QModelIndex(), QItemSelectionModel::Select);
			emit unclick();
		}

		QTreeView::mousePressEvent(event);
	}

	// Mutator
	void entity_tree_view::setModel(QAbstractItemModel* model)
	{
		QTreeView::setModel(model);

		// Width
		const auto& tree_model = get_source_model();
		const auto& header_sizes = tree_model->get_column_sizes();

		for (size_t i = 0; i < header_sizes.size(); ++i)
			setColumnWidth(i, header_sizes[i]);
	}

	// Accessor
	entity_tree_model* entity_tree_view::get_source_model() const
	{
		const QSortFilterProxyModel* proxy = get_proxy_model();
		return qobject_cast<entity_tree_model*>(proxy->sourceModel());
	}
}
