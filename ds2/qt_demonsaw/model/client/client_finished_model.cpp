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

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <QDesktopServices>
#include <QTimer>
#include <QUrl>

#include "client_finished_model.h"
#include "component/timer_component.h"
#include "component/io/file_component.h"
#include "component/router/router_component.h"
#include "component/transfer/download_thread_component.h"
#include "component/transfer/finished_component.h"
#include "component/transfer/transfer_component.h"
#include "component/transfer/upload_thread_component.h"
#include "resource/resource.h"
#include "utility/io/file_util.h"
#include "window/client_window.h"
#include "window/window.h"

namespace eja
{
	// Interface
	void client_finished_model::init()
	{
		entity_table_model::init();

		// Callback
		add_callback(function_type::finished);
	}

	void client_finished_model::clear()
	{
		const auto finished_vector = m_entity->get<finished_entity_vector_component>();
		if (!finished_vector)
			return;

		const auto option = m_entity->get<client_option_component>();
		if (option && option->has_partial())
		{
			for (const auto& entity : finished_vector->copy())
			{
				const auto transfer = entity->get<transfer_component>();
				if (transfer && transfer->is_download() && !transfer->done())
				{
					const auto thread = entity->get<download_thread_component>();
					if (thread && thread->invalid())
					{
						const auto file = entity->get<file_component>();
						if (file)
							file_util::remove(file);
					}
				}
			}
		}

		entity_table_model::clear();
	}

	void client_finished_model::double_click(const QModelIndex& index)
	{
		if (!index.isValid())
			return;

		const auto finished_vector = m_entity->get<finished_entity_vector_component>();
		if (!finished_vector)
			return;

		const size_t row = static_cast<size_t>(index.row());
		const auto entity = finished_vector->get(row);
		if (entity)
		{
			const auto transfer = entity->get<transfer_component>();
			if (transfer && (transfer->is_upload() || !transfer->empty()))
			{
				const auto file = entity->get<file_component>();
				if (file)
				{
					const auto url = QUrl::fromLocalFile(QString::fromStdString(file->get_path()));
					QDesktopServices::openUrl(url);
				}
			}
		}
	}

	// Slot
	void client_finished_model::on_add(const entity::ptr entity)
	{
		assert(thread::main());

		const auto finished_vector = m_entity->get<finished_entity_vector_component>();
		if (!finished_vector)
			return;

		const auto found = finished_vector->has_if([entity](const entity::ptr e)
		{
			const auto t1 = entity->get<transfer_component>();
			const auto t2 = e->get<transfer_component>();
			return t1 && t2 && (t1->get_id() == t2->get_id());
		});

		if (found)
			return;

		{
			auto_lock_ptr(finished_vector);

			const auto row = finished_vector->size();

			beginInsertRows(QModelIndex(), row, row);
			finished_vector->add(entity);
			endInsertRows();
		}

		set_status(client_statusbar::finished, finished_vector->size());
	}

	void client_finished_model::on_remove(const entity::ptr entity)
	{
		assert(thread::main());

		const auto finished_vector = m_entity->get<finished_entity_vector_component>();
		if (!finished_vector)
			return;

		{
			auto_lock_ptr(finished_vector);

			const auto row = finished_vector->get(entity);
			if (row == type::npos)
				return;

			beginRemoveRows(QModelIndex(), row, row);
			finished_vector->remove(row);
			endRemoveRows();
		}

		set_status(client_statusbar::finished, finished_vector->size());
	}

	void client_finished_model::on_clear()
	{
		assert(thread::main());

		const auto finished_vector = m_entity->get<finished_entity_vector_component>();
		if (!finished_vector)
			return;

		const auto option = m_entity->get<client_option_component>();
		if (option && option->has_partial())
		{
			for (const auto& entity : finished_vector->copy())
			{
				const auto transfer = entity->get<transfer_component>();
				if (transfer && transfer->is_download() && !transfer->done())
				{
					const auto thread = entity->get<download_thread_component>();
					if (thread && thread->invalid())
					{
						const auto file = entity->get<file_component>();
						if (file)
							file_util::remove(file);
					}
				}
			}
		}

		beginResetModel();
		finished_vector->clear();
		endResetModel();

		set_status(client_statusbar::finished, finished_vector->size());
	}

	// Utility
	bool client_finished_model::empty() const
	{
		const auto finished_vector = m_entity->get<finished_entity_vector_component>();
		return !finished_vector || finished_vector->empty();
	}

	// Model
	bool client_finished_model::removeRows(int row, int count, const QModelIndex& parent /*= QModelIndex()*/)
	{
		assert(thread::main());

		const auto finished_vector = m_entity->get<finished_entity_vector_component>();
		if (!finished_vector)
			return false;
		
		size_t status = 0;
		const auto option = m_entity->get<client_option_component>();

		beginRemoveRows(QModelIndex(), row, (row + count - 1));

		for (auto i = (row + count - 1); i >= row; --i)
		{
			const auto entity = finished_vector->get(row);
			if (entity)
			{
				if (option && option->has_partial())
				{
					const auto transfer = entity->get<transfer_component>();
					if (transfer && transfer->is_download() && !transfer->done())
					{
						const auto thread = entity->get<download_thread_component>();
						if (thread && thread->invalid())
						{
							const auto file = entity->get<file_component>();
							if (file)
								file_util::remove(file);
						}
					}
				}				

				status += finished_vector->remove(row);
			}				
		}

		endRemoveRows();

		set_status(client_statusbar::finished, finished_vector->size());

		return status > 0;
	}

	QVariant client_finished_model::data(const QModelIndex& index, int role) const
	{
		if (!index.isValid())
			return QVariant();

		const size_t row = static_cast<size_t>(index.row());
		const size_t col = static_cast<size_t>(index.column());

		const auto finished_vector = m_entity->get<finished_entity_vector_component>();
		if (!finished_vector)
			return QVariant();

		const auto entity = finished_vector->get(row);
		if (!entity)
			return QVariant();

		const auto file = entity->get<file_component>();
		if (!file)
			return QVariant();

		switch (role)
		{
			case Qt::DisplayRole:
			{
				switch (col)
				{
					case column::number:
					{
						if (entity->has<download_thread_component>())
						{
							const auto thread = entity->get<download_thread_component>();
							if (thread)
							{
								const auto size = static_cast<size_t>(thread->get_size());
								return QVariant::fromValue<uint>(size);
							}
						}
						else if (entity->has<upload_thread_component>())
						{
							const auto thread = entity->get<upload_thread_component>();
							if (thread)
							{
								const auto size = static_cast<size_t>(thread->get_size());
								return QVariant::fromValue<uint>(size);
							}
						}

						break;
					}
					case column::status:
					case column::file:
					case column::type:
					{
						return QVariant::fromValue(entity);
					}
					case column::size:
					{
						const auto& size = file->get_size();
						return QString::fromStdString(file_util::get_size(size));
					}
					case column::time:
					{
						const auto timer = entity->get<timer_component>();
						if (timer)
							return QString::fromStdString(timer->str());

						break;
					}
					case column::speed:
					{
						const auto transfer = entity->get<transfer_component>();
						if (transfer)
							return QString::fromStdString(file_util::get_speed(transfer->get_speed()));

						break;
					}
					case column::progress:
					{
						return QVariant::fromValue(entity);
					}
				}
				break;
			}
			case Qt::UserRole:
			{
				switch (col)
				{
					case column::status:
					{
						if (entity->has<download_thread_component>())
						{
							const auto thread = entity->get<download_thread_component>();
							if (thread)
							{
								const auto status = static_cast<size_t>(thread->get_status());
								return QVariant::fromValue<uint>(status);
							}
						}
						else if (entity->has<upload_thread_component>())
						{
							const auto thread = entity->get<upload_thread_component>();
							if (thread)
							{
								const auto status = static_cast<size_t>(thread->get_status());
								return QVariant::fromValue<uint>(status);
							}
						}

						break;
					}
					case column::file:
					{
						const auto& name = file->get_name();
						return QString::fromStdString(name).toLower();
					}
					case column::type:
					{
						const auto transfer = entity->get<transfer_component>();
						if (transfer)
						{
							const auto type = static_cast<size_t>(transfer->get_type());
							return QVariant::fromValue<uint>(type);
						}

						break;
					}
					case column::size:
					{
						const auto& size = file->get_size();
						return QVariant::fromValue<qulonglong>(size);
					}
					case column::time:
					{
						const auto timer = entity->get<timer_component>();
						if (timer)
							return  QVariant::fromValue<uint>(timer->elapsed());

						break;
					}
					case column::speed:
					{
						const auto finished = entity->get<transfer_component>();
						if (finished)
							return QVariant::fromValue<uint>(finished->get_speed());

						break;
					}
					case column::progress:
					{
						const auto finished = entity->get<transfer_component>();
						if (finished)
							return QVariant::fromValue<double>(finished->get_ratio());

						break;
					}
					default:
					{
						return data(index, Qt::DisplayRole);
					}
				}
				break;
			}
			case Qt::EditRole:
			{
				switch (col)
				{
					case column::file:
					{
						const auto& name = file->get_name();
						return QString::fromStdString(name);
					}
				}
				break;
			}
			case Qt::TextAlignmentRole:
			{
				switch (col)
				{

					case column::number:
					{
						return static_cast<int>(Qt::AlignVCenter | Qt::AlignHCenter);
					}
					case column::size:
					case column::time:
					case column::speed:
					{
						return static_cast<int>(Qt::AlignVCenter | Qt::AlignRight);
					}
				}

				break;
			}
		}

		return QVariant();
	}

	int client_finished_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		const auto finished_vector = m_entity->get<finished_entity_vector_component>();
		return finished_vector ? finished_vector->size() : 0;
	}

	// Accessor
	size_t client_finished_model::get_row(const entity::ptr entity) const
	{
		const auto finished_vector = m_entity->get<finished_entity_vector_component>();
		return finished_vector ? finished_vector->get(entity) : type::npos;
	}
}
