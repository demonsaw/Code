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

#include <QApplication>
#include <QFont>
#include <QHeaderView>
#include <QMouseEvent>
#include <QScrollBar>

#include "entity/entity.h"
#include "entity/entity_model.h"
#include "entity/entity_table_view.h"
#include "resource/resource.h"

namespace eja
{
	// Constructor
	entity_table_view::entity_table_view(const entity::ptr& entity, QWidget* parent /*= 0*/) : QTableView(parent)
	{
		// Table
		resizeRowsToContents();
		resizeColumnsToContents();

		setAlternatingRowColors(true);
		setContextMenuPolicy(Qt::CustomContextMenu);
		setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		setSelectionBehavior(QAbstractItemView::SelectRows);
		setSelectionMode(QAbstractItemView::ExtendedSelection);

		setAttribute(Qt::WA_MacShowFocusRect, 0);
		setIconSize(QSize(resource::get_icon_size(), resource::get_icon_size()));
		setShowGrid(false);

		// Font
		auto font = QApplication::font();
		font.setFamily(font_family::main);
		setFont(font);

		// Header
		const auto vheader = verticalHeader();
		vheader->setDefaultSectionSize(resource::get_row_height());
		vheader->setSectionsMovable(true);
		vheader->setHighlightSections(false);
		vheader->hide();

		const auto hheader = horizontalHeader();
		hheader->setContextMenuPolicy(Qt::CustomContextMenu);
		hheader->setFixedHeight(resource::get_line_edit_height());
		hheader->setResizeContentsPrecision(0);
		hheader->setHighlightSections(false);
		hheader->setStretchLastSection(false);
		hheader->setSectionsMovable(true);

		// Signal
		//connect(this, &entity_table_view::clicked, this, &entity_table_view::click);
		//connect(this, &entity_table_view::doubleClicked, this, &entity_table_view::double_click);
	}

	// Interface
	void entity_table_view::add()
	{
		const auto model = get_model();
		model->add();
	}

	void entity_table_view::remove()
	{
		const auto model = get_model();
		auto selected = selectionModel()->selectedRows();

		if (selected.empty())// || (selected.count() == model->size()))
		{
			model->remove();
		}
		else
		{
			// Sort so that we don't crash
			if (selected.count() > 1)
				std::sort(selected.begin(), selected.end());

			const auto model = get_model();
			auto it = selected.end();

			while (it != selected.begin())
			{
				--it;
				const auto& index = *it;

				if (index.isValid())
					model->remove(index);
			}

			clearSelection();
			selectionModel()->setCurrentIndex(QModelIndex(), QItemSelectionModel::Select);
			emit unclick();
		}
	}

	void entity_table_view::update()
	{
		const auto model = get_model();
		const auto selected = selectionModel()->selectedRows();

		if (selected.empty())// || (selected.count() == model->size()))
			model->update();
		else
			execute([&](const QModelIndex index) { model->update(index); });
	}

	void entity_table_view::clear()
	{
		const auto model = get_model();
		model->clear();
	}

	// Click
	void entity_table_view::click()
	{
		const auto model = get_model();
		execute([&](const QModelIndex index) { model->click(index); });
	}

	void entity_table_view::double_click()
	{
		const auto model = get_model();
		execute([&](const QModelIndex index) { model->double_click(index); });
	}

	// Move
	void entity_table_view::move_top()
	{
		auto selected = selectionModel()->selectedRows();
		if (!selected.empty())
		{
			// Sort so that we don't crash
			if (selected.count() > 1)
				std::sort(selected.begin(), selected.end());

			size_t i = 0;
			const auto model = get_model();

			for (const auto& index : selected)
			{
				if (index.isValid())
				{
					const auto entity = model->get_entity(index);
					model->removeRow(index.row());
					model->add(entity, i++);
				}
			}
		}
	}

	void entity_table_view::move_bottom()
	{
		auto selected = selectionModel()->selectedRows();
		if (!selected.empty())
		{
			// Sort so that we don't crash
			if (selected.count() > 1)
				std::sort(selected.begin(), selected.end());

			size_t i = 0;
			auto it = selected.end();
			const auto model = get_model();

			while (it != selected.begin())
			{
				--it;
				const auto& index = *it;

				if (index.isValid())
				{
					const auto entity = model->get_entity(index);
					model->removeRow(index.row());
					model->add(entity, model->rowCount() - (i++));
				}
			}
		}
	}

	void entity_table_view::move_up()
	{
		auto selected = selectionModel()->selectedRows();
		if (!selected.empty())
		{
			// Sort so that we don't crash
			if (selected.count() > 1)
				std::sort(selected.begin(), selected.end());

			// Ignore first
			if (selected.first().row() == 0)
				return;

			const auto model = get_model();

			for (const auto& index : selected)
			{
				if (index.isValid())
				{
					const auto entity = model->get_entity(index);
					model->removeRow(index.row());
					model->add(entity, index.row() - 1);
				}
			}
		}
	}

	void entity_table_view::move_down()
	{
		auto selected = selectionModel()->selectedRows();
		if (!selected.empty())
		{
			// Sort so that we don't crash
			if (selected.count() > 1)
				std::sort(selected.begin(), selected.end());

			// Ignore last
			const auto model = get_model();
			if (selected.last().row() == (model->rowCount() - 1))
				return;

			auto it = selected.end();

			while (it != selected.begin())
			{
				--it;
				const auto& index = *it;

				if (index.isValid())
				{
					const auto entity = model->get_entity(index);
					model->removeRow(index.row());
					model->add(entity, index.row() + 1);
				}
			}
		}
	}

	// Utility
	void entity_table_view::scroll()
	{
		// Auto scroll
		const auto scrollbar = verticalScrollBar();
		const auto scroll = !(scrollbar->isSliderDown() || (scrollbar->sliderPosition() != scrollbar->maximum()));
		if (scroll)
			scrollToBottom();
	}

	// Event
	void entity_table_view::keyPressEvent(QKeyEvent* event)
	{
		switch (event->key())
		{
			case Qt::Key_Delete:
			{
				remove();
				break;
			}
			default:
			{
				QTableView::keyPressEvent(event);
				break;
			}
		}
	}

	void entity_table_view::resizeEvent(QResizeEvent* event)
	{
		QTableView::resizeEvent(event);

		emit resize();
	}

	void entity_table_view::mousePressEvent(QMouseEvent* event)
	{
		const auto index = indexAt(event->pos());
		if (!index.isValid())
		{
			clearSelection();
			selectionModel()->setCurrentIndex(QModelIndex(), QItemSelectionModel::Select);
			emit unclick();
		}

		QTableView::mousePressEvent(event);
	}

	// Set
	void entity_table_view::setModel(QAbstractItemModel* model)
	{
		QTableView::setModel(model);

		set_column_sizes();
	}

	void entity_table_view::set_column_sizes()
	{
		// Width
		const auto& header_sizes = get_model()->get_column_sizes();
		for (size_t i = 0; i < header_sizes.size(); ++i)
			setColumnWidth(i, header_sizes[i]);
	}

	// Get
	entity_model* entity_table_view::get_model() const
	{
		return qobject_cast<entity_model*>(model());
	}
}
