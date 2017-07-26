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

#include <QDesktopServices>
#include <QTimer>
#include <QUrl>

#include "client_transfer_model.h"
#include "component/time_component.h"
#include "component/timeout_component.h"
#include "component/timer_component.h"
#include "component/io/file_component.h"
#include "component/transfer/chunk_component.h"
#include "component/transfer/download_thread_component.h"
#include "component/transfer/transfer_component.h"
#include "component/transfer/upload_thread_component.h"
#include "resource/icon_provider.h"
#include "resource/resource.h"
#include "utility/io/file_util.h"
#include "window/client_window.h"
#include "window/window.h"

namespace eja
{
	// Interface
	void client_transfer_model::init()
	{
		entity_table_model::init();

		// Callback
		add_callback(function_type::transfer);
	}

	void client_transfer_model::add(const entity::ptr entity)
	{
		// Verify
		const auto transfer_vector = m_entity->get<transfer_entity_vector_component>();
		if (!transfer_vector || transfer_vector->has(entity))
			return;

		// Time
		//if (!entity->has<time_component>())
			//entity->add<time_component>();

		if (!entity->has<timer_component>())
			entity->add<timer_component>();

		if (!entity->has<timeout_component>())
			entity->add<timeout_component>();

		// Chunk
		if (!entity->has<chunk_component>())
			entity->add<chunk_component>();

		emit added(entity);
	}

	void client_transfer_model::double_click(const QModelIndex& index)
	{
		if (!index.isValid())
			return;

		const auto transfer_vector = m_entity->get<transfer_entity_vector_component>();
		if (!transfer_vector)
			return;

		const size_t row = static_cast<size_t>(index.row());
		const auto entity = transfer_vector->get(row);
		if (entity)
		{
			const auto transfer = entity->get<transfer_component>();
			if (transfer && transfer->is_upload())
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
	void client_transfer_model::on_add(const entity::ptr entity)
	{
		assert(thread::main());

		const auto transfer_vector = m_entity->get<transfer_entity_vector_component>();
		if (!transfer_vector || transfer_vector->has(entity))
			return;

		{
			auto_lock_ptr(transfer_vector);

			const auto row = transfer_vector->size();

			beginInsertRows(QModelIndex(), row, row);
			transfer_vector->add(entity);
			endInsertRows();
		}

		set_status(client_statusbar::queue, transfer_vector->size());
	}

	void client_transfer_model::on_remove(const entity::ptr entity)
	{
		assert(thread::main());

		const auto transfer_vector = m_entity->get<transfer_entity_vector_component>();
		if (!transfer_vector)
			return;

		{
			auto_lock_ptr(transfer_vector);

			const auto row = transfer_vector->get(entity);
			if (row == type::npos)
				return;

			beginRemoveRows(QModelIndex(), row, row);
			transfer_vector->remove(row);
			endRemoveRows();
		}

		set_status(client_statusbar::queue, transfer_vector->size());
	}

	void client_transfer_model::on_clear()
	{
		assert(thread::main());

		const auto transfer_vector = m_entity->get<transfer_entity_vector_component>();
		if (!transfer_vector)
			return;

		{
			beginResetModel();
			transfer_vector->clear();
			endResetModel();
		}		

		set_status(client_statusbar::queue, transfer_vector->size());
	}

	// Utility
	bool client_transfer_model::empty() const
	{
		const auto transfer_vector = m_entity->get<transfer_entity_vector_component>();
		return !transfer_vector || transfer_vector->empty();
	}

	// Model
	bool client_transfer_model::removeRows(int row, int count, const QModelIndex& parent /*= QModelIndex()*/)
	{
		assert(thread::main());

		const auto transfer_vector = m_entity->get<transfer_entity_vector_component>();
		if (!transfer_vector)
			return false;

		size_t status = 0;		

		beginRemoveRows(QModelIndex(), row, (row + count - 1));

		for (auto i = (row + count - 1); i >= row; --i)
		{
			const auto entity = transfer_vector->get(row);
			if (entity)
				status += transfer_vector->remove(row);
		}

		endRemoveRows();

		set_status(client_statusbar::queue, transfer_vector->size());

		return status > 0;
	}

	QVariant client_transfer_model::data(const QModelIndex& index, int role) const
	{
		if (!index.isValid())
			return QVariant();

		const size_t row = static_cast<size_t>(index.row());
		const size_t col = static_cast<size_t>(index.column());

		const auto transfer_vector = m_entity->get<transfer_entity_vector_component>();
		if (!transfer_vector)
			return QVariant();

		const auto entity = transfer_vector->get(row);
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
					/*case column::time:
					{
						const auto time = entity->get<time_component>();
						if (time)
							return QString::fromStdString(time->str());

						break;
					}*/
					case column::number:
					{
						const auto transfer = entity->get<transfer_component>();
						if (transfer)
						{
							if (transfer->is_download())
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
								else
								{
									const auto option = entity->get<client_option_component>();
									if (option)
										return QVariant::fromValue<uint>(option->get_download_threads());
								}
							}
							else
							{
								if (entity->has<upload_thread_component>())
								{
									const auto thread = entity->get<upload_thread_component>();
									if (thread)
									{
										const auto size = static_cast<size_t>(thread->get_size());
										return QVariant::fromValue<uint>(size);
									}
								}
								else
								{
									const auto option = entity->get<client_option_component>();
									if (option)
										return QVariant::fromValue<uint>(option->get_upload_threads());
								}
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
					/*case column::time:
					{
						const auto time = entity->get<time_component>();
						if (time)
						{
							const auto file = entity->get<file_component>();
							if (file)
								return QString("%1%2").arg(time->elapsed()).arg(file->get_size());

							return QVariant::fromValue<uint>(time->elapsed());
						}

						break;
					}*/
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
						const auto transfer = entity->get<transfer_component>();
						if (transfer)
							return QVariant::fromValue<uint>(transfer->get_speed());

						break;
					}
					case column::progress:
					{
						const auto transfer = entity->get<transfer_component>();
						if (transfer)
							return QVariant::fromValue<double>(transfer->get_ratio());

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

	int client_transfer_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		const auto transfer_vector = m_entity->get<transfer_entity_vector_component>();
		return transfer_vector ? transfer_vector->size() : 0;
	}

	// Accessor
	size_t client_transfer_model::get_row(const entity::ptr entity) const
	{
		const auto transfer_vector = m_entity->get<transfer_entity_vector_component>();
		return transfer_vector ? transfer_vector->get(entity) : type::npos;
	}
}
