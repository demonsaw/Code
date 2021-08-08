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
#include <QSortFilterProxyModel>

#include "entity/entity.h"
#include "entity/entity_model.h"
#include "entity/entity_table_view_ex.h"
#include "resource/resource.h"

namespace eja
{
	// Constructor
	entity_table_view_ex::entity_table_view_ex(const entity::ptr& entity, QWidget* parent /*= 0*/) : entity_table_view(entity, parent)
	{
		// Table
		setSortingEnabled(true);

		// Header
		const auto hheader = horizontalHeader();
		hheader->setSortIndicator(0, Qt::SortOrder::AscendingOrder);
		hheader->setSortIndicatorShown(true);
		sortByColumn(0);
	}

	// Interface
	void entity_table_view_ex::remove()
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

			const auto proxy = get_proxy();
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

	void entity_table_view_ex::update()
	{
		const auto model = get_model();
		const auto selected = selectionModel()->selectedRows();

		if (selected.empty())// || (selected.count() == model->size()))
			model->update();
		else
			execute([&](const QModelIndex index) { model->update(index); });
	}

	// Click
	void entity_table_view_ex::click()
	{
		const auto model = get_model();
		execute([&](const QModelIndex index) { model->click(index); });
	}

	void entity_table_view_ex::double_click()
	{
		const auto model = get_model();
		execute([&](const QModelIndex index) { model->double_click(index); });
	}

	// Move
	void entity_table_view_ex::move_top()
	{
		auto selected = selectionModel()->selectedRows();
		if (!selected.empty())
		{
			// Sort so that we don't crash
			if (selected.count() > 1)
				std::sort(selected.begin(), selected.end());

			size_t i = 0;
			const auto model = get_model();
			const auto proxy = get_proxy();

			for (const auto& index : selected)
			{
				if (index.isValid())
				{
					const auto sindex = proxy->mapToSource(index);
					if (sindex.isValid())
					{
						const auto entity = model->get_entity(sindex);
						model->removeRow(sindex.row());
						model->add(entity, i++);
					}
				}
			}
		}
	}

	void entity_table_view_ex::move_bottom()
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
			const auto proxy = get_proxy();

			while (it != selected.begin())
			{
				--it;
				const auto& index = *it;

				if (index.isValid())
				{
					const auto sindex = proxy->mapToSource(index);
					if (sindex.isValid())
					{
						const auto entity = model->get_entity(sindex);
						model->removeRow(sindex.row());
						model->add(entity, model->rowCount() - (i++));
					}
				}
			}
		}
	}

	void entity_table_view_ex::move_up()
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
			const auto proxy = get_proxy();

			for (const auto& index : selected)
			{
				if (index.isValid())
				{
					const auto sindex = proxy->mapToSource(index);
					if (sindex.isValid())
					{
						const auto entity = model->get_entity(sindex);
						model->removeRow(sindex.row());
						model->add(entity, sindex.row() - 1);
					}
				}
			}
		}
	}

	void entity_table_view_ex::move_down()
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
			const auto proxy = get_proxy();

			while (it != selected.begin())
			{
				--it;
				const auto& index = *it;

				if (index.isValid())
				{
					const auto sindex = proxy->mapToSource(index);
					if (sindex.isValid())
					{
						const auto entity = model->get_entity(sindex);
						model->removeRow(sindex.row());
						model->add(entity, sindex.row() + 1);
					}
				}
			}
		}
	}

	// Get
	QSortFilterProxyModel* entity_table_view_ex::get_proxy() const
	{
		return qobject_cast<QSortFilterProxyModel*>(model());
	}

	entity_model* entity_table_view_ex::get_model() const
	{
		const auto proxy = get_proxy();

		return qobject_cast<entity_model*>(proxy->sourceModel());
	}
}
