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

#ifndef _EJA_ENTITY_MODEL_
#define _EJA_ENTITY_MODEL_

#include <cassert>

#include <QAbstractTableModel>
#include <QObject>

#include "entity/entity.h"
#include "entity/entity_callback.h"
#include "system/function/function.h"
#include "system/function/function_type.h"
#include "system/mutex/mutex.h"
#include "system/thread/thread.h"

class QTimer;

namespace eja
{
	enum class model_callback : size_t { add, remove, clear, refresh, click, double_click, execute, resize };

	// Container
	derived_type(model_function, mutex_map<model_callback, function>);

	class entity_model : public QAbstractItemModel, public entity_callback
	{
		Q_OBJECT

	protected:
		QTimer* m_timer = nullptr;
		model_function m_functions;

	public:
		static const char* emodel;

	protected:
		entity_model(QObject* parent = 0) : QAbstractItemModel(parent) { }
		entity_model(const entity::ptr entity, QObject* parent = 0) : QAbstractItemModel(parent), entity_callback(entity) { }
		virtual ~entity_model() { }

		// Utility
		template <typename Component, typename List> 
		entity::ptr find_entity(const entity::ptr entity) const;

	protected slots:
		virtual void on_add() { assert(thread::main()); }
		virtual void on_add(const size_t row) { assert(thread::main()); }
		virtual void on_add(const QModelIndex& index) { assert(thread::main()); }
		virtual void on_add(const entity::ptr entity) { assert(thread::main()); }

		virtual void on_remove() { assert(thread::main()); }
		virtual void on_remove(const size_t row) { assert(thread::main()); }
		virtual void on_remove(const QModelIndex& index) { assert(thread::main()); }
		virtual void on_remove(const entity::ptr entity) { assert(thread::main()); }

		virtual void on_refresh();
		virtual void on_refresh(const size_t row);
		virtual void on_refresh(const QModelIndex& index);
		virtual void on_refresh(const entity::ptr entity) { assert(thread::main()); }

		virtual void on_clear() { assert(thread::main()); }
		virtual void on_clear(const size_t row) { assert(thread::main()); }
		virtual void on_clear(const QModelIndex& index) { assert(thread::main()); }
		virtual void on_clear(const entity::ptr entity) { assert(thread::main()); }

		virtual void on_click() { assert(thread::main()); }
		virtual void on_click(const size_t row) { assert(thread::main()); }
		virtual void on_click(const QModelIndex& index) { assert(thread::main()); }
		virtual void on_click(const entity::ptr entity) { assert(thread::main()); }

		virtual void on_double_click() { assert(thread::main()); }
		virtual void on_double_click(const size_t row) { assert(thread::main()); }
		virtual void on_double_click(const QModelIndex& index) { assert(thread::main()); }
		virtual void on_double_click(const entity::ptr entity) { assert(thread::main()); }

		virtual void on_execute() { assert(thread::main()); }
		virtual void on_execute(const size_t row) { assert(thread::main()); }
		virtual void on_execute(const QModelIndex& index) { assert(thread::main()); }
		virtual void on_execute(const entity::ptr entity) { assert(thread::main()); }

	public:
		// Interface
		virtual void init() override;

		// Callback
		virtual void add() { emit added(); }
		virtual void add(const size_t row) { emit added(row); }
		virtual void add(const QModelIndex& index) { emit added(index); }
		virtual void add(const entity::ptr entity) override { emit added(entity); }

		virtual void remove() { emit removed(); }
		virtual void remove(const size_t row) { emit removed(row); }
		virtual void remove(const QModelIndex& index) { emit removed(index); }
		virtual void remove(const entity::ptr entity) override { emit removed(entity); }

		virtual void refresh() { emit refreshed(); }
		virtual void refresh(const size_t row) { emit refreshed(row); }
		virtual void refresh(const QModelIndex& index) { emit refreshed(index); }
		virtual void refresh(const entity::ptr entity) override { emit refreshed(entity); }

		virtual void clear() { emit cleared(); }
		virtual void clear(const size_t row) { emit cleared(row); }
		virtual void clear(const QModelIndex& index) { emit cleared(index); }
		virtual void clear(const entity::ptr entity) override { emit cleared(entity); }

		virtual void click() { emit clicked(); }
		virtual void click(const size_t row) { emit clicked(row); }
		virtual void click(const QModelIndex& index) { emit clicked(index); }
		virtual void click(const entity::ptr entity) { emit clicked(entity); }

		virtual void double_click() { emit double_clicked(); }
		virtual void double_click(const size_t row) { emit double_clicked(row); }
		virtual void double_click(const QModelIndex& index) { emit double_clicked(index); }
		virtual void double_click(const entity::ptr entity) { emit double_clicked(entity); }

		virtual void execute() { emit executed(); }
		virtual void execute(const size_t row) { emit executed(row); }
		virtual void execute(const QModelIndex& index) { emit executed(index); }
		virtual void execute(const entity::ptr entity) { emit executed(entity); }

		// Utility
		virtual bool empty() const { return true; }

		// Model
		virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
		virtual QVariant data(const QModelIndex& index, int role) const = 0;

		virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override { return createIndex(row, column); }
		virtual QModelIndex parent(const QModelIndex& index) const override { return QModelIndex(); }

		virtual bool setData(const QModelIndex& index, const QVariant& value, int role) override { return false; }
		virtual bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override { return false; }
		virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override { return false; }

		virtual int rowCount(const QModelIndex& parent = QModelIndex()) const = 0;
		virtual int columnCount(const QModelIndex& parent = QModelIndex()) const = 0;

		// Accessor
		model_function& get_functions() { return m_functions; }
		virtual size_t get_row(const entity::ptr entity) const = 0;
		virtual QModelIndex get_index(const entity::ptr entity) const { return createIndex(get_row(entity), 0); }
		virtual QModelIndex get_index(const size_t row) const { return createIndex(row, 0); }

	signals:
		void added();
		void added(const size_t row);
		void added(const QModelIndex& index);
		void added(const entity::ptr entity);

		void removed();
		void removed(const size_t row);
		void removed(const QModelIndex& index);
		void removed(const entity::ptr entity);

		void refreshed();
		void refreshed(const size_t row);
		void refreshed(const QModelIndex& index);
		void refreshed(const entity::ptr entity);

		void cleared();
		void cleared(const size_t row);
		void cleared(const QModelIndex& index);
		void cleared(const entity::ptr entity);

		void clicked();
		void clicked(const size_t row);
		void clicked(const QModelIndex& index);
		void clicked(const entity::ptr entity);

		void double_clicked();
		void double_clicked(const size_t row);
		void double_clicked(const QModelIndex& index);
		void double_clicked(const entity::ptr entity);

		void executed();
		void executed(const size_t row);
		void executed(const QModelIndex& index);
		void executed(const entity::ptr entity);
	};

	// Utility
	template <typename Component, typename List>
	entity::ptr entity_model::find_entity(const entity::ptr entity) const
	{
		const auto list = m_entity->get<List>();
		if (!list)
			return nullptr;

		return list->find_if([entity](const entity::ptr e)
		{
			const auto comp1 = entity->get<Component>();
			const auto comp2 = e->get<Component>();
			return comp1 && comp2 && (*comp1 == *comp2);
		});
	}
}

#endif
