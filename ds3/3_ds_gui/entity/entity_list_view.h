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

#ifndef _EJA_ENTITY_LIST_VIEW_H_
#define _EJA_ENTITY_LIST_VIEW_H_

#include <memory>

#include <QListView>
#include <QSortFilterProxyModel>

namespace eja
{
	class entity;
	class entity_list_model;

	class entity_list_view : public QListView
	{
		Q_OBJECT

	private:
		virtual void resizeEvent(QResizeEvent* event) override;
		virtual void mousePressEvent(QMouseEvent* event) override;

	public:
		entity_list_view(const std::shared_ptr<entity> entity, QWidget* parent = 0);

		// Interface
		virtual void init() { }
		virtual void shutdown() { }

		// Utility
		void scroll();

		// Get
		QSortFilterProxyModel* get_proxy_model() const { return qobject_cast<QSortFilterProxyModel*>(model()); }
		entity_list_model* get_source_model() const;

	public:
		virtual void add();
		virtual void remove();
		virtual void clear();
		virtual void update();

		virtual void click();
		virtual void double_click();
		
		template <typename T>
		void execute(const T t);

	signals:
		void resize();
		void unclick();
	};

	template <typename T>
	void entity_list_view::execute(const T t)
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
					t(sindex);
			}
		}
	}
}

#endif
