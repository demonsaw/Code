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

#include "error_model.h"
#include "component/function_component.h"
#include "component/client/client_option_component.h"
#include "component/router/router_option_component.h"
#include "window/client_window.h"

namespace eja
{
	// Interface
	void error_model::init()
	{
		entity_table_model::init();

		// Callback
		add_callback(function_type::error);
	}

	// Slot
	void error_model::on_add(const entity::ptr entity)
	{
		assert(thread::main());

		// History
		size_t max_errors = default_error::num_errors;

		if (m_entity->has<client_option_component>())
		{
			const auto option = m_entity->get<client_option_component>();
			if (option)
				max_errors = option->get_max_errors();
		}
		else if (m_entity->has<router_option_component>())
		{
			const auto option = m_entity->get<router_option_component>();
			if (option)
				max_errors = option->get_max_errors();

		}

		const auto error_vector = m_entity->get<error_entity_vector_component>();
		if (!error_vector)
			return;

		{			
			auto_lock_ptr(error_vector);

			if (error_vector->size() >= max_errors)
			{
				const auto row = 0;
				const auto last = error_vector->size() - max_errors;

				beginRemoveRows(QModelIndex(), row, last);

				for (size_t i = row; i <= last; ++i)
					error_vector->remove(row);

				endRemoveRows();
			}

			const auto row = error_vector->size();

			beginInsertRows(QModelIndex(), row, row);
			error_vector->add(entity);
			endInsertRows();
		}

		set_status(client_statusbar::error, error_vector->size());
	}

	void error_model::on_remove(const entity::ptr entity)
	{
		assert(thread::main());

		const auto error_vector = m_entity->get<error_entity_vector_component>();
		if (!error_vector)
			return;

		{
			auto_lock_ptr(error_vector);

			// O(N)
			const auto row = error_vector->get(entity);
			if (row == type::npos)
				return;

			beginRemoveRows(QModelIndex(), row, row);
			error_vector->remove(entity);
			endRemoveRows();
		}

		set_status(client_statusbar::error, error_vector->size());
	}

	void error_model::on_clear()
	{
		assert(thread::main());
		
		const auto error_vector = m_entity->get<error_entity_vector_component>();
		if (!error_vector)
			return;

		beginResetModel();
		error_vector->clear();
		endResetModel();

		set_status(client_statusbar::error, error_vector->size());
	}

	// Utility
	bool error_model::empty() const
	{
		const auto error_vector = m_entity->get<error_entity_vector_component>();
		return !error_vector || error_vector->empty();
	}

	// Model
	bool error_model::removeRows(int row, int count, const QModelIndex& parent /*= QModelIndex()*/)
	{
		assert(thread::main());

		const auto error_vector = m_entity->get<error_entity_vector_component>();
		if (!error_vector)
			return false;

		size_t status = 0;

		beginRemoveRows(QModelIndex(), row, (row + count - 1));

		for (auto i = (row + count - 1); i >= row; --i)
			status += error_vector->remove(i);
		
		endRemoveRows();

		set_status(client_statusbar::error, error_vector->size());

		return status > 0;
	}
	
	QVariant error_model::data(const QModelIndex& index, int role) const
	{
		if (!index.isValid())
			return QVariant();

		const auto error_vector = m_entity->get<error_entity_vector_component>();
		if (!error_vector)
			return QVariant();

		const size_t row = static_cast<size_t>(index.row());
		const auto entity = error_vector->get(row);
		if (!entity)
			return QVariant();

		const auto error = entity->get<error_component>();
		if (!error)
			return QVariant();

		const size_t col = static_cast<size_t>(index.column());

		switch (role)
		{
			case Qt::DisplayRole:
			{				
				switch (col)
				{
					case column::time:
					{
						
						char buff[16];
						const auto time = error->get_time();
						const auto tm = localtime(&time);
						strftime(buff, 16, "%I:%M:%S %p", tm);
						return QString(buff);
					}
					case column::name:
					{
						return QString::fromStdString(error->get_name());
					}
					case column::message:
					{
						return QString::fromStdString(error->get_text());
					}
				}
				break;
			}
			case Qt::UserRole:
			{
				switch (col)
				{
					case column::name:
					case column::message:
					{
						const auto variant = data(index, Qt::DisplayRole);
						return variant.toString().toLower();
					}
					default:
					{
						return data(index, Qt::DisplayRole);
					}
				}
			}
		}

		return QVariant();
	}

	int error_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		const auto error_vector = m_entity->get<error_entity_vector_component>();
		return error_vector ? error_vector->size() : 0;
	}

	// Accessor
	size_t error_model::get_row(const entity::ptr entity) const
	{
		const auto error_vector = m_entity->get<error_entity_vector_component>();
		return error_vector ? error_vector->get(entity) : type::npos;
	}
}
