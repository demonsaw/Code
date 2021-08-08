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

#ifndef _EJA_ENTITY_TABLE_VIEW_EX_H_
#define _EJA_ENTITY_TABLE_VIEW_EX_H_

#include <memory>

#include <QSortFilterProxyModel>

#include "entity/entity_table_view.h"

namespace eja
{
	class entity;
	class entity_model;

	class entity_table_view_ex : public entity_table_view
	{
		Q_OBJECT

	public:
		entity_table_view_ex(const std::shared_ptr<entity>& entity, QWidget* parent = nullptr);

		// Interface
		virtual void remove();
		virtual void update();

		virtual void click();
		virtual void double_click();

		template <typename T>
		void execute(const T t);

		template <typename T>
		void execute(const int column, const T t);

		// Move
		virtual void move_up() override;
		virtual void move_down() override;
		virtual void move_top() override;
		virtual void move_bottom() override;

		// Get
		QSortFilterProxyModel* get_proxy() const;
		virtual entity_model* get_model() const override;
	};

	template <typename T>
	void entity_table_view_ex::execute(const T t)
	{
		const auto selected = selectionModel()->selectedRows();
		if (!selected.empty())
		{
			const auto proxy = get_proxy();

			for (const auto& index : selected)
			{
				if (index.isValid())
				{
					const auto sindex = proxy->mapToSource(index);
					if (sindex.isValid())
						t(sindex);
				}
			}
		}
	}

	template <typename T>
	void entity_table_view_ex::execute(const int column, const T t)
	{
		const auto selected = selectionModel()->selectedRows(column);
		if (!selected.empty())
		{
			const auto proxy = get_proxy();

			for (const auto& index : selected)
			{
				if (index.isValid())
				{
					const auto sindex = proxy->mapToSource(index);
					if (sindex.isValid())
						t(sindex);
				}
			}
		}
	}
}

#endif
