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

#ifndef _EJA_ENTITY_MODEL_H_
#define _EJA_ENTITY_MODEL_H_

#include <memory>
#include <string>

#include <QAbstractItemModel>
#include <QStringList>
#include <QVariant>

#include "entity/entity_type.h"
#include "system/type.h"

class QMimeData;
class QObject;
class QTimer;

namespace eja
{
	class entity_model : public QAbstractItemModel, public entity_type
	{
		Q_OBJECT;

	protected:
		using column_names = std::vector<QString>;
		using column_sizes = std::vector<size_t>;

		column_names m_column_names;
		column_sizes m_column_sizes;
		QTimer* m_timer = nullptr;

	protected:
		entity_model(QObject* parent = nullptr) : entity_model(nullptr, parent) { }
		entity_model(column_names&& names, QObject* parent = nullptr);
		entity_model(column_names&& names, column_sizes&& sizes, QObject* parent = nullptr);
		entity_model(const column_names& names, QObject* parent = nullptr);
		entity_model(const column_names& names, const column_sizes& sizes, QObject* parent = nullptr);
		
		entity_model(const std::shared_ptr<entity>& entity, QObject* parent = nullptr);
		entity_model(const std::shared_ptr<entity>& entity, column_names&& names, QObject* parent = nullptr);
		entity_model(const std::shared_ptr<entity>& entity, column_names&& names, column_sizes&& sizes, QObject* parent = nullptr);
		entity_model(const std::shared_ptr<entity>& entity, const column_names& names, QObject* parent = nullptr);
		entity_model(const std::shared_ptr<entity>& entity, const column_names& names, const column_sizes& sizes, QObject* parent = nullptr);

	protected slots:
		virtual void on_create() { on_create(nullptr); }
		virtual void on_create(const size_t row) { assert(false); }
		virtual void on_create(const QModelIndex& index) { assert(false); }
		virtual void on_create(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_destroy() { on_destroy(nullptr); }
		virtual void on_destroy(const size_t row) { assert(false); }
		virtual void on_destroy(const QModelIndex& index) { assert(false); }
		virtual void on_destroy(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_init() { assert(false); }
		virtual void on_init(const size_t row) { assert(false); }
		virtual void on_init(const QModelIndex& index) { assert(false); }
		virtual void on_init(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_shutdown() { assert(false); }
		virtual void on_shutdown(const size_t row) { assert(false); }
		virtual void on_shutdown(const QModelIndex& index) { assert(false); }
		virtual void on_shutdown(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_update();
		virtual void on_update(const size_t row);
		virtual void on_update(const QModelIndex& index);
		virtual void on_update(const std::shared_ptr<entity> entity);

		virtual void on_add() { insertRows(rowCount(), 1); }
		virtual void on_add(const size_t row) { insertRow(row); }
		virtual void on_add(const QModelIndex& index) { insertRows(index.row(), 1); }
		virtual void on_add(const std::shared_ptr<entity> entity);
		virtual void on_add(const std::shared_ptr<entity> entity, const size_t row) { assert(false); }

		virtual void on_remove() { /*on_clear();*/ }
		virtual void on_remove(const size_t row) { removeRow(row); }
		virtual void on_remove(const QModelIndex& index) { removeRows(index.row(), 1); }
		virtual void on_remove(const std::shared_ptr<entity> entity);

		virtual void on_clear() { assert(false); }
		virtual void on_clear(const size_t row) { assert(false); }
		virtual void on_clear(const QModelIndex& index) { assert(false); }
		virtual void on_clear(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_click(const size_t row) { }
		virtual void on_click(const QModelIndex& index) { }
		virtual void on_click(const std::shared_ptr<entity> entity) { }

		virtual void on_double_click(const size_t row) { }
		virtual void on_double_click(const QModelIndex& index) { }
		virtual void on_double_click(const std::shared_ptr<entity> entity) { }

		virtual void on_execute(const size_t row) { assert(false); }
		virtual void on_execute(const QModelIndex& index) { assert(false); }
		virtual void on_execute(const std::shared_ptr<entity> entity) { assert(false); }

	signals:
		void create();
		void create(const size_t row);
		void create(const QModelIndex& index);
		void create(const std::shared_ptr<entity> entity);

		void destroy();
		void destroy(const size_t row);
		void destroy(const QModelIndex& index);
		void destroy(const std::shared_ptr<entity> entity);

		void init();
		void init(const size_t row);
		void init(const QModelIndex& index);
		void init(const std::shared_ptr<entity> entity);

		void shutdown();
		void shutdown(const size_t row);
		void shutdown(const QModelIndex& index);
		void shutdown(const std::shared_ptr<entity> entity);

		void update();
		void update(const size_t row);
		void update(const QModelIndex& index);
		void update(const std::shared_ptr<entity> entity);

		void add();
		void add(const size_t row);
		void add(const QModelIndex& index);
		void add(const std::shared_ptr<entity> entity);
		void add(const std::shared_ptr<entity> entity, const size_t row);

		void remove();
		void remove(const size_t row);
		void remove(const QModelIndex& index);
		void remove(const std::shared_ptr<entity> entity);

		void clear();
		void clear(const size_t row);
		void clear(const QModelIndex& index);
		void clear(const std::shared_ptr<entity> entity);

		void click(const size_t row);
		void click(const QModelIndex& index);
		void click(const std::shared_ptr<entity> entity);

		void double_click(const size_t row);
		void double_click(const QModelIndex& index);
		void double_click(const std::shared_ptr<entity> entity);

		void execute(const size_t row);
		void execute(const QModelIndex& index);
		void execute(const std::shared_ptr<entity> entity);

	public:
		// Utility
		virtual bool empty() const { return !rowCount(); }
		size_t size() const { return static_cast<size_t>(rowCount()); }

		// Model
		virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
		virtual QVariant data(const QModelIndex& index, int role) const = 0;		
		virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

		virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override { return createIndex(row, column); }
		virtual QModelIndex parent(const QModelIndex& index) const override { return QModelIndex(); }

		virtual bool setData(const QModelIndex& index, const QVariant& value, int role) override { return false; }
		virtual bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override { return false; }
		virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override { return false; }
		
		virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override { return m_column_names.size(); }
		virtual int rowCount(const QModelIndex& parent = QModelIndex()) const = 0;

		// Mime
		virtual QStringList mimeTypes() const override;
		virtual QMimeData* mimeData(const QModelIndexList &indexes) const override;
		virtual Qt::DropActions supportedDropActions() const;
		virtual bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;

		// Set		
		virtual void set_entity(const std::shared_ptr<entity>& entity) override;
		virtual void set_entity();

		void set_column_names(column_names&& names) { m_column_names = std::forward<column_names>(names); }
		void set_column_names(const column_names& names) { m_column_names = names; }

		void set_column_sizes(column_sizes&& sizes);
		void set_column_sizes(const column_sizes& sizes);

		// TODO: size_t functions should be moved to table_model (doesn't apply to tree_model)
		//
		//		
		virtual std::shared_ptr<entity> get_entity(const size_t row) const { return nullptr; }		

		// Get
		virtual std::shared_ptr<entity> get_entity(const QModelIndex& index) const = 0;

		virtual QModelIndex get_index(const size_t row) const { return createIndex(row, 0); }
		virtual QModelIndex get_index(const std::shared_ptr<entity>& entity) const { return createIndex(get_row(entity), 0); }

		virtual size_t get_row(const std::shared_ptr<entity>& entity) const = 0;

		const column_names& get_column_names() const { return m_column_names; }
		const column_sizes& get_column_sizes() const { return m_column_sizes; }
	};
}

#endif
