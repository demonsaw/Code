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

#include <QTimer>

#include "component/endpoint_component.h"
#include "component/version_component.h"
#include "component/time/timer_component.h"
#include "entity/entity.h"
#include "model/router/router_client_model.h"
#include "security/filter/hex.h"
#include "thread/thread_info.h"

Q_DECLARE_METATYPE(eja::entity::ptr);

namespace eja
{
	// Slot
	void router_client_model::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto client_list = m_entity->get<client_list_component>();
		const auto row = client_list->size();

		beginInsertRows(QModelIndex(), row, row);
		client_list->push_back(entity);
		endInsertRows();
	}

	void router_client_model::on_clear()
	{
		assert(thread_info::main());

		const auto client_list = m_entity->get<client_list_component>();

		beginResetModel();
		client_list->clear();
		endResetModel();
	}

	// Utility
	bool router_client_model::empty() const
	{
		assert(thread_info::main());

		const auto client_list = m_entity->get<client_list_component>();
		return client_list->empty();
	}

	// Model
	bool router_client_model::removeRows(int row, int count, const QModelIndex& parent /*= QModelIndex()*/)
	{
		assert(thread_info::main());

		// TODO: Remove data structures as well!
		//
		//

		const auto client_list = m_entity->get<client_list_component>();
		const auto it = client_list->cbegin() + row;
		const auto size = client_list->size();

		beginRemoveRows(QModelIndex(), row, (row + count - 1));
		client_list->erase(it, it + count);
		endRemoveRows();

		return client_list->size() < size;
	}

	QVariant router_client_model::data(const QModelIndex& index, int role) const
	{
		assert(thread_info::main());

		if (!index.isValid())
			return QVariant();

		const size_t row = static_cast<size_t>(index.row());
		const auto entity = get_entity(row);
		if (!entity)
			return QVariant();

		const size_t col = static_cast<size_t>(index.column());

		switch (role)
		{
			case Qt::DisplayRole:
			{
				switch (col)
				{				
					case column::name: 
					{
						return QVariant::fromValue(entity);
					}					
				}

				break;
			}
			case Qt::EditRole:
				{
					switch (col)
					{
						case column::name:
						{
							const auto endpoint = entity->get<endpoint_component>();
							return QString::fromStdString(endpoint->get_name());
						}
					}

					break;
				}
			case Qt::UserRole:
			{
				switch (col)
				{
					case column::name:
					{
						const auto endpoint = entity->get<endpoint_component>();
						return QString::fromStdString(endpoint->get_name()).toLower();
					}
				}

				break;
			}
		}

		return QVariant();
	}

	int router_client_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		assert(thread_info::main());

		const auto client_list = m_entity->get<client_list_component>();
		return client_list->size();
	}

	// Get
	entity::ptr router_client_model::get_entity(const size_t row) const
	{
		assert(thread_info::main());

		const auto client_list = m_entity->get<client_list_component>();
		return (row < client_list->size()) ? client_list->at(row) : nullptr;
	}

	size_t router_client_model::get_row(const entity::ptr entity) const
	{
		assert(thread_info::main());

		const auto client_list = m_entity->get<client_list_component>();
		const auto it = std::find(client_list->begin(), client_list->end(), entity);
		return (it != client_list->end()) ? (it - client_list->begin()) : npos;
	}
}
