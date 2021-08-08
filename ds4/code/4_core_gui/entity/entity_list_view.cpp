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
#include <QKeyEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QScrollBar>

#include "entity/entity.h"
#include "entity/entity_list_view.h"
#include "entity/entity_model.h"
#include "resource/resource.h"

namespace eja
{
	// Constructor
	entity_list_view::entity_list_view(const entity::ptr& entity, QWidget* parent /*= nullptr*/) : QListView(parent)
	{
		// List
		setAlternatingRowColors(true);
		setContextMenuPolicy(Qt::CustomContextMenu);
		setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		setSelectionBehavior(QAbstractItemView::SelectRows);
		setSelectionMode(ExtendedSelection);

		setAttribute(Qt::WA_MacShowFocusRect, 0);
		setIconSize(QSize(resource::get_icon_size(), resource::get_icon_size()));

		// Font
		auto font = QApplication::font();
		font.setFamily(font_family::main);
		setFont(font);

		// Signal
		//connect(this, &entity_list_view::clicked, this, &entity_list_view::click);
		//connect(this, &entity_list_view::doubleClicked, this, &entity_list_view::double_click);
	}

	// Interface
	void entity_list_view::add()
	{
		const auto model = get_model();
		model->add();
	}

	void entity_list_view::remove()
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

	void entity_list_view::update()
	{
		const auto model = get_model();
		const auto selected = selectionModel()->selectedRows();

		if (selected.empty())// || (selected.count() == model->size()))
			model->update();
		else
			execute([&](const QModelIndex index) { model->update(index); });
	}

	void entity_list_view::clear()
	{
		const auto model = get_model();
		model->clear();
	}

	void entity_list_view::click()
	{
		const auto model = get_model();
		execute([&](const QModelIndex index) { model->click(index); });
	}

	void entity_list_view::double_click()
	{
		const auto model = get_model();
		execute([&](const QModelIndex index) { model->double_click(index); });
	}

	// Utility
	void entity_list_view::scroll()
	{
		// Auto scroll
		const auto scrollbar = verticalScrollBar();
		const auto scroll = !(scrollbar->isSliderDown() || (scrollbar->sliderPosition() != scrollbar->maximum()));
		if (scroll)
			scrollToBottom();
	}

	// Event
	void entity_list_view::keyPressEvent(QKeyEvent* event)
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
				QListView::keyPressEvent(event);
				break;
			}
		}
	}

	void entity_list_view::resizeEvent(QResizeEvent* event)
	{
		QListView::resizeEvent(event);

		emit resize();
	}

	void entity_list_view::mousePressEvent(QMouseEvent* event)
	{
		const auto index = indexAt(event->pos());
		if (!index.isValid())
		{
			clearSelection();
			selectionModel()->setCurrentIndex(QModelIndex(), QItemSelectionModel::Select);
			emit unclick();
		}

		QListView::mousePressEvent(event);
	}

	// Get
	entity_model* entity_list_view::get_model() const
	{		
		return qobject_cast<entity_model*>(model());
	}
}
