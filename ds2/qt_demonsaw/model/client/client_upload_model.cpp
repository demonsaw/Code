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

#include "client_upload_model.h"
#include "component/timer_component.h"
#include "component/timeout_component.h"
#include "component/io/file_component.h"
#include "component/router/router_component.h"
#include "component/transfer/upload_component.h"
#include "component/transfer/upload_thread_component.h"
#include "component/transfer/transfer_idle_component.h"
#include "resource/icon_provider.h"
#include "resource/resource.h"
#include "utility/io/file_util.h"
#include "window/client_window.h"
#include "window/window.h"

namespace eja
{
	// Interface
	void client_upload_model::init()
	{
		entity_table_model::init();

		// Callback
		add_callback(function_type::upload);

		// Timer
		m_timer = new QTimer(this);
		m_timer->setInterval(1000);

		connect(m_timer, &QTimer::timeout, [this]()
		{
			const auto upload_vector = m_entity->get<upload_entity_vector_component>();
			if (!upload_vector || upload_vector->empty())
				return;

			QModelIndex start = createIndex(0, column::time);
			QModelIndex end = createIndex(upload_vector->size() - 1, column::max - 1);
			emit dataChanged(start, end);

		});

		m_timer->start();
	}

	void client_upload_model::add(const entity::ptr entity)
	{
		const auto upload_vector = m_entity->get<upload_entity_vector_component>();
		if (!upload_vector || upload_vector->has(entity))
			return;

		// Timer
		const auto timer = entity->get<timer_component>();
		if (!timer)
			return;

		timer->start();

		// Timeout
		const auto timeout = entity->get<timeout_component>();
		if (!timeout)
			return;

		timeout->start();

		// Thread
		const auto lambda = [this](const entity::ptr entity) { on_refresh(entity); };
		const auto function = function::create(lambda);

		const auto option = m_entity->get<client_option_component>();
		const auto threads = option ? option->get_upload_threads() : 1;
		const auto thread = upload_thread_component::create(threads);
		thread->add(function);

		entity->add(thread);
		thread->start();

		emit added(entity);
	}	

	void client_upload_model::remove(const entity::ptr entity)
	{
		entity->shutdown();

		emit removed(entity);
	}

	void client_upload_model::clear()
	{
		const auto upload_vector = m_entity->get<upload_entity_vector_component>();
		if (!upload_vector)
			return;

		for (const auto& entity : upload_vector->copy())
		{
			const auto thread = entity->get<upload_thread_component>();
			if (thread && thread->removeable())
			{
				entity->shutdown();

				emit removed(entity);
			}
		}
	}

	void client_upload_model::on_clear(const entity::ptr entity)
	{
		const auto upload_vector = m_entity->get<upload_entity_vector_component>();
		if (!upload_vector)
			return;

		for (const auto& entity : upload_vector->copy())
		{
			const auto thread = entity->get<upload_thread_component>();
			if (thread && thread->running())
				thread->stop();

			entity->shutdown();
		}

		beginResetModel();
		upload_vector->clear();
		endResetModel();

		set_status(client_statusbar::upload, upload_vector->size());
	}

	void client_upload_model::double_click(const QModelIndex& index)
	{
		if (!index.isValid())
			return;

		const auto upload_vector = m_entity->get<upload_entity_vector_component>();
		if (!upload_vector)
			return;

		const size_t row = static_cast<size_t>(index.row());
		const auto entity = upload_vector->get(row);
		if (entity)
		{
			const auto file = entity->get<file_component>();
			if (file)
			{
				const auto url = QUrl::fromLocalFile(QString::fromStdString(file->get_path()));
				QDesktopServices::openUrl(url);
			}
		}
	}

	// Slot
	void client_upload_model::on_add(const entity::ptr entity)
	{
		assert(thread::main());

		const auto upload_vector = m_entity->get<upload_entity_vector_component>();
		if (!upload_vector || upload_vector->has(entity))
			return;

		{
			auto_lock_ptr(upload_vector);

			const auto row = upload_vector->size();

			beginInsertRows(QModelIndex(), row, row);
			upload_vector->add(entity);
			endInsertRows();
		}		

		set_status(client_statusbar::upload, upload_vector->size());
	}

	void client_upload_model::on_remove(const entity::ptr entity)
	{
		assert(thread::main());

		const auto upload_vector = m_entity->get<upload_entity_vector_component>();
		if (!upload_vector)
			return;

		{
			auto_lock_ptr(upload_vector);

			const auto row = upload_vector->get(entity);
			if (row == type::npos)
				return;

			beginRemoveRows(QModelIndex(), row, row);
			upload_vector->remove(row);
			endRemoveRows();
		}

		set_status(client_statusbar::upload, upload_vector->size());
	}

	void client_upload_model::on_clear()
	{
		assert(thread::main());

		const auto upload_vector = m_entity->get<upload_entity_vector_component>();
		if (!upload_vector)
			return;

		beginResetModel();
		upload_vector->clear();
		endResetModel();

		set_status(client_statusbar::upload, upload_vector->size());
	}

	// Utility
	bool client_upload_model::empty() const
	{
		const auto upload_vector = m_entity->get<upload_entity_vector_component>();
		return !upload_vector || upload_vector->empty();
	}

	// Model
	bool client_upload_model::removeRows(int row, int count, const QModelIndex& parent /*= QModelIndex()*/)
	{
		assert(thread::main());

		const auto upload_vector = m_entity->get<upload_entity_vector_component>();
		if (!upload_vector)
			return false;

		size_t status = 0;;

		beginRemoveRows(QModelIndex(), row, (row + count - 1));

		for (auto i = (row + count - 1); i >= row; --i)
		{
			const auto entity = upload_vector->get(i);
			if (entity)
			{
				const auto thread = entity->get<upload_thread_component>();
				if (thread && thread->removeable())
				{
					entity->shutdown();
					status += upload_vector->remove(row);
				}
			}
		}

		endRemoveRows();

		set_status(client_statusbar::upload, upload_vector->size());

		return status > 0;
	}

	QVariant client_upload_model::data(const QModelIndex& index, int role) const
	{
		if (!index.isValid())
			return QVariant();

		const size_t row = static_cast<size_t>(index.row());
		const size_t col = static_cast<size_t>(index.column());

		const auto upload_vector = m_entity->get<upload_entity_vector_component>();
		if (!upload_vector)
			return QVariant();

		const auto entity = upload_vector->get(row);
		if (!entity)
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
						const auto thread = entity->get<upload_thread_component>();
						if (thread)
							return QVariant::fromValue<uint>(thread->get_size());

						break;
					}
					case column::status:
					case column::file:
					{
						return QVariant::fromValue(entity);
					}
					case column::router:
					{
						const auto router = entity->get<router_component>();
						if (router)
							return QString::fromStdString(router->get_address());

						break;
					}
					case column::version:
					{
						const auto router = entity->get<router_component>();
						if (router)
							return QString::fromStdString(router->get_version());

						break;
					}
					case column::size:
					{
						const auto file = entity->get<file_component>();
						if (file)
							return QString::fromStdString(file_util::get_size(file->get_size()));

						break;
					}
					case column::time:
					{						
						const auto thread = entity->get<upload_thread_component>();
						if (thread)
						{
							const auto transfer = entity->get<transfer_component>();
							if (transfer && thread->running() && !transfer->done())
							{
								return QString::fromStdString(transfer->get_estimate());
							}
							else
							{
								const auto timer = entity->get<timer_component>();
								if (timer)
									return QString::fromStdString(timer->str());
							}
						}

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
						const auto thread = entity->get<upload_thread_component>();
						if (thread)
						{
							const auto status = static_cast<size_t>(thread->get_status());
							return QVariant::fromValue<uint>(status);
						}

						break;
					}
					case column::file:
					{
						const auto file = entity->get<file_component>();
						if (file)
							return QString::fromStdString(file->get_name()).toLower();

						break;
					}
					case column::router:
					{
						const auto router = entity->get<router_component>();
						if (router)
							return QString::fromStdString(router->get_address()).toLower();

						break;
					}
					case column::size:
					{
						const auto file = entity->get<file_component>();
						if (file)
							return QVariant::fromValue<qulonglong>(file->get_size());

						break;
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
						const auto file = entity->get<file_component>();
						if (file)
							return QString::fromStdString(file->get_name());

						break;
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

	int client_upload_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		const auto upload_vector = m_entity->get<upload_entity_vector_component>();
		return upload_vector ? upload_vector->size() : 0;
	}

	// Accessor
	size_t client_upload_model::get_row(const entity::ptr entity) const
	{
		const auto upload_vector = m_entity->get<upload_entity_vector_component>();
		return upload_vector ? upload_vector->get(entity) : type::npos;
	}
}
