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

#ifndef _EJA_CLIENT_TRANSFER_MODEL_H_
#define _EJA_CLIENT_TRANSFER_MODEL_H_

#include <memory>

#include <QModelIndex>
#include <QVariant>

#include "entity/entity_table_model.h"
#include "system/type.h"

namespace eja
{
	class entity;
	class function;

	class client_transfer_model final : public entity_table_model
	{
		Q_OBJECT

	private:
		std::shared_ptr<function> m_status_function;

	public:
		enum column { row, type, name, address, port, group, version };

	private:
		void set_modified(const std::shared_ptr<entity> entity = nullptr);

	private slots:
		virtual void on_add(const std::shared_ptr<entity> entity) override;
		virtual void on_clear(const std::shared_ptr<entity> entity) override;

		virtual void on_update() override;
		virtual void on_update(const QModelIndex& index) override;

	signals:
		void set_status(const std::shared_ptr<entity> entity);

	public:
		client_transfer_model(const std::shared_ptr<entity> entity, QObject* parent = 0) : entity_table_model(entity, parent) { }
		client_transfer_model(const std::shared_ptr<entity> entity, column_names&& names, QObject* parent = 0) : entity_table_model(entity, names, parent) { }
		client_transfer_model(const std::shared_ptr<entity> entity, column_names&& names, column_sizes&& sizes, QObject* parent = 0) : entity_table_model(entity, names, sizes, parent) { }
		client_transfer_model(const std::shared_ptr<entity> entity, const column_names& names, QObject* parent = 0) : entity_table_model(entity, names, parent) { }
		client_transfer_model(const std::shared_ptr<entity> entity, const column_names& names, const column_sizes& sizes, QObject* parent = 0) : entity_table_model(entity, names, sizes, parent) { }
		virtual ~client_transfer_model() override { on_shutdown(); }

		// Interface
		virtual void on_init() override;
		virtual void on_shutdown() override;

		// Utility
		virtual bool empty() const override;

		// Model
		virtual bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
		virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

		virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
		virtual QVariant data(const QModelIndex& index, int role) const override;
		virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
		virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;

		// Get
		using entity_table_model::get_entity;
		virtual std::shared_ptr<entity> get_entity(const size_t row) const;
		virtual size_t get_row(const std::shared_ptr<entity> entity) const;
	};
}

#endif
