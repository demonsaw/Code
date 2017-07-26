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

#ifndef _EJA_ROUTER_SERVER_MODEL_
#define _EJA_ROUTER_SERVER_MODEL_

#include "entity/entity.h"
#include "component/server/server_component.h"
#include "entity/entity_table_model.h"
#include "system/function/function.h"

class QItemSelection;

namespace eja
{
	class router_server_model final : public entity_table_model
	{
		Q_OBJECT

	public:
		enum column : size_t { name, version, address, port, passphrase };
		enum offset : size_t { icon = 64 };

	private:
		function::ptr m_function;

	private slots:
		virtual void on_add(const entity::ptr entity) override;
		virtual void on_remove(const entity::ptr entity) override;
		virtual void on_clear(const entity::ptr entity) override { clear(); }
		virtual void on_clear() override;

	public:
		router_server_model(QObject* parent = 0) : entity_table_model(parent) { }
		router_server_model(const entity::ptr entity, QObject* parent = 0) : entity_table_model(entity, parent) { }

		router_server_model(const entity::ptr entity, const column_names& names, QObject* parent = 0) : entity_table_model(entity, names, parent) { }
		router_server_model(const entity::ptr entity, const column_names& names, const column_sizes& sizes, QObject* parent = 0) : entity_table_model(entity, names, sizes, parent) { }

		router_server_model(const entity::ptr entity, column_names&& names, QObject* parent = 0) : entity_table_model(entity, names, parent) { }
		router_server_model(const entity::ptr entity, column_names&& names, column_sizes&& sizes, QObject* parent = 0) : entity_table_model(entity, names, sizes, parent) { }
		virtual ~router_server_model() override { shutdown(); }

		// Interface
		virtual void init() override;
		virtual void shutdown() override;

		virtual void remove(const entity::ptr entity) override;		
		virtual void refresh(const QModelIndex& index) override;
		virtual void refresh() override;
		virtual void clear() override;

		// Utility
		virtual bool empty() const override; 
		void post_create();

		// Model
		virtual bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
		virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
		
		virtual Qt::ItemFlags flags(const QModelIndex& index) const override; 		
		virtual QVariant data(const QModelIndex& index, int role) const override;
		virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
		virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;

		//virtual bool moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild) override;
		//virtual Qt::DropActions supportedDragActions() const override { return Qt::CopyAction | Qt::MoveAction; }
		//virtual Qt::DropActions supportedDropActions() const override { return Qt::CopyAction | Qt::MoveAction; }

		// Accessor
		virtual size_t get_row(const entity::ptr entity) const;
	};
}

#endif
