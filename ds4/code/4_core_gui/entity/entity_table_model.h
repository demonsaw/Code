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

#ifndef _EJA_ENTITY_TABLE_MODEL_H_
#define _EJA_ENTITY_TABLE_MODEL_H_

#include <QVariant>

#include "entity/entity_model.h"

namespace eja
{
	class entity_table_model : public entity_model
	{
		Q_OBJECT

	protected:
		entity_table_model(QObject* parent = nullptr) : entity_model(parent) { }
		entity_table_model(column_names&& names, QObject* parent = nullptr) : entity_model(std::forward<column_names>(names), parent) { }
		entity_table_model(column_names&& names, column_sizes&& sizes, QObject* parent = nullptr) : entity_model(std::forward<column_names>(names), std::forward<column_sizes>(sizes), parent) { }
		entity_table_model(const column_names& names, QObject* parent = nullptr) : entity_model(names, parent) { }
		entity_table_model(const column_names& names, const column_sizes& sizes, QObject* parent = nullptr) : entity_model(names, sizes, parent) { }
		
		entity_table_model(const std::shared_ptr<entity>& entity, QObject* parent = nullptr) : entity_model(entity, parent) { }
		entity_table_model(const std::shared_ptr<entity>& entity, column_names&& names, QObject* parent = nullptr) : entity_model(entity, std::forward<column_names>(names), parent) { }
		entity_table_model(const std::shared_ptr<entity>& entity, column_names&& names, column_sizes&& sizes, QObject* parent = nullptr) : entity_model(entity, std::forward<column_names>(names), std::forward<column_sizes>(sizes), parent) { }
		entity_table_model(const std::shared_ptr<entity>& entity, const column_names& names, QObject* parent = nullptr) : entity_model(entity, names, parent) { }
		entity_table_model(const std::shared_ptr<entity>& entity, const column_names& names, const column_sizes& sizes, QObject* parent = nullptr) : entity_model(entity, names, sizes, parent) { }

	public:
		using entity_model::get_entity;
		virtual std::shared_ptr<entity> get_entity(const QModelIndex& index) const override;
	};
}

#endif
