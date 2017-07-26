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

#ifndef _EJA_LIST_ENTITY_MODEL_
#define _EJA_LIST_ENTITY_MODEL_

#include <QObject>

#include "entity_model.h"
#include "entity/entity.h"

namespace eja
{
	class entity_list_model : public entity_model
	{
		Q_OBJECT

	public:
		using entity_model::on_add;
		using entity_model::on_remove;
		using entity_model::on_refresh;

	protected:
		entity_list_model(QObject* parent = 0) : entity_model(parent) { }
		entity_list_model(const entity::ptr entity, QObject* parent = 0) : entity_model(entity, parent) { }
		virtual ~entity_list_model() override { }

	public slots:
		// Interface
		virtual void add() override { insertRows(rowCount(), 1); }
		virtual void add(const QModelIndex& index) override { insertRows(index.row(), 1); }

		virtual void remove() { removeRows(0, rowCount() - 1); }
		virtual void remove(const QModelIndex& index) override { removeRows(index.row(), 1); }

		virtual void refresh() override;
		virtual void refresh(const QModelIndex& index) override;

	public:
		virtual void on_refresh(const entity::ptr entity) override;

		// Accessor
		virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override { return 1; }
	};
}

#endif
