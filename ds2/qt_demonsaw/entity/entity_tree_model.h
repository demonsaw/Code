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

#ifndef _EJA_TREE_ENTITY_MODEL_
#define _EJA_TREE_ENTITY_MODEL_

#include <cassert>
#include <string>
#include <vector>

#include <QObject>

#include "entity_model.h"
#include "entity/entity.h"
#include <system/thread/thread.h>

namespace eja
{
	class entity_tree_model : public entity_model
	{
		Q_OBJECT

	protected:
		using column_names = std::vector<std::string>;
		using column_sizes = std::vector<size_t>;

	protected:
		column_names m_column_names;
		column_sizes m_column_sizes;

	protected:		
		entity_tree_model(QObject* parent = 0) : entity_model(parent) { }
		entity_tree_model(const entity::ptr entity, QObject* parent = 0) : entity_model(entity, parent) { }

		entity_tree_model(const entity::ptr entity, const column_names& names, QObject* parent = 0) : entity_model(entity, parent), m_column_names(names) { }
		entity_tree_model(const entity::ptr entity, const column_names& names, const column_sizes& sizes, QObject* parent = 0) : entity_model(entity, parent), m_column_names(names), m_column_sizes(sizes) { }

		entity_tree_model(const entity::ptr entity, column_names&& names, QObject* parent = 0) : entity_model(entity, parent), m_column_names(std::forward<column_names>(names)) { }
		entity_tree_model(const entity::ptr entity, column_names&& names, column_sizes&& sizes, QObject* parent = 0) : entity_model(entity, parent), m_column_names(names), m_column_sizes(std::forward<column_sizes>(sizes)) { }

		virtual ~entity_tree_model() override { }

		// Using
		using entity_model::on_add;
		using entity_model::on_remove;
		using entity_model::on_refresh;

	protected slots:
		virtual void on_add() override;
		virtual void on_add(const size_t row) override;
		virtual void on_add(const QModelIndex& index) override;

		virtual void on_remove() override;
		virtual void on_remove(const size_t row) override;
		virtual void on_remove(const QModelIndex& index) override;

		virtual void on_refresh(const entity::ptr entity) override;

	public:
		// Model
		virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
		virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override { return m_column_names.size(); }

		// Mutator
		virtual void set_entity(const entity::ptr entity) override;

		void set_column_names(const column_names& names) { m_column_names = names; }
		void set_column_names(column_names&& names) { m_column_names = std::forward<column_names>(names); }
		void set_column_sizes(const column_sizes& sizes) { m_column_sizes = sizes; }
		void set_column_sizes(column_sizes&& sizes) { m_column_sizes = std::forward<column_sizes>(sizes); }

		// Accessor
		const column_names& get_column_names() const { return m_column_names; }
		const column_sizes& get_column_sizes() const { return m_column_sizes; }

		entity::ptr get_node(const QModelIndex& index) const;
		entity::ptr get_parent(const entity::ptr entity) const;
		entity::ptr get_parent(const QModelIndex& index) const;
	};
}

#endif
