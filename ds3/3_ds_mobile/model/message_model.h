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

#ifndef _EJA_MESSAGE_MODEL_H_
#define _EJA_MESSAGE_MODEL_H_

#include <memory>

#include <QModelIndex>
#include <QVariant>

#include "component/component.h"
#include "entity/entity_table_model.h"
#include "system/type.h"

namespace eja
{
	class entity;

	class message_model final : public entity_table_model
	{
		Q_OBJECT;

	//private:
	//	size_t m_rows = 0;

	public:
		enum column { name, color, message, time, self, role_name = Qt::UserRole + 1, role_color, role_message, role_time, role_self };

	private slots:
		virtual void on_add(const std::shared_ptr<entity> entity) override;
		virtual void on_clear() override;

	public:
		message_model(QObject* parent = 0) : entity_table_model(parent) { }

		// Utility
		virtual bool empty() const override;

		// Model
		virtual QVariant data(const QModelIndex& index, int role) const override;
		virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
		virtual QHash<int, QByteArray> roleNames() const;

		// Get
		using entity_table_model::get_entity;
		virtual std::shared_ptr<entity> get_entity(const size_t row) const;
		virtual size_t get_row(const std::shared_ptr<entity> entity) const;
		//size_t get_rows() const { return m_rows; }
	};
}

#endif
