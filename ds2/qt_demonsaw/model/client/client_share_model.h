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

#ifndef _EJA_CLIENT_SHARE_MODEL_
#define _EJA_CLIENT_SHARE_MODEL_

#include "entity/entity.h"
#include "component/share_idle_component.h"
#include "component/client/client_component.h"
#include "component/io/folder_component.h"
#include "entity/entity_tree_model.h"

namespace eja
{
	/*
	* Entity Node Definition
	* ----------------------------------------
	* entity
	*  -> share_entity_vector_component (children)
	*  -> file_component || folder_component (data)
	*
	**/
	class client_share_model final : public entity_tree_model
	{
		Q_OBJECT

	public:
		enum column : size_t { file, size, type };

	private slots:		
		virtual void on_add(const entity::ptr entity) override;
		virtual void on_remove(const QModelIndex& index) override;
		
		virtual void on_clear() override; 
		virtual void on_clear(const entity::ptr entity) override { on_clear(); }

		virtual void on_refresh() override;
		virtual void on_refresh(const QModelIndex& index) override { refresh(); }

		virtual void on_double_click(const QModelIndex& index) override;

	public:
		client_share_model(QObject* parent = 0) : entity_tree_model(parent) { }
		client_share_model(const entity::ptr entity, QObject* parent = 0) : entity_tree_model(entity, parent) { }

		client_share_model(const entity::ptr entity, const column_names& names, QObject* parent = 0) : entity_tree_model(entity, names, parent) { }
		client_share_model(const entity::ptr entity, const column_names& names, const column_sizes& sizes, QObject* parent = 0) : entity_tree_model(entity, names, sizes, parent) { }

		client_share_model(const entity::ptr entity, column_names&& names, QObject* parent = 0) : entity_tree_model(entity, names, parent) { }
		client_share_model(const entity::ptr entity, column_names&& names, column_sizes&& sizes, QObject* parent = 0) : entity_tree_model(entity, names, sizes, parent) { }
		virtual ~client_share_model() override { }

		// Interface
		virtual void init() override;

		virtual void add() override;

		// Utility
		virtual bool empty() const;		
		void post_create();

		// Model
		virtual QVariant data(const QModelIndex& index, int role) const override;		
		virtual int rowCount(const QModelIndex& index = QModelIndex()) const override;
		
		virtual bool hasChildren(const QModelIndex& index = QModelIndex()) const override;
		virtual bool canFetchMore(const QModelIndex& index) const override;
		virtual void fetchMore(const QModelIndex& parent) override;

		QModelIndex index(int row, int column, const QModelIndex& parent) const;
		QModelIndex index(const entity::ptr entity) const;
		QModelIndex parent(const QModelIndex& index) const;

		// Accessor
		size_t get_row(const entity::ptr entity) const;
	};
}

#endif
