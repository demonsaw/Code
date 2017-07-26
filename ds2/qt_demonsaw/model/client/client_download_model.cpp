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

#include <boost/format.hpp>

#include <QDesktopServices>
#include <QTimer>
#include <QUrl>

#include "client_download_model.h"
#include "component/time_component.h"
#include "component/timer_component.h"
#include "component/timeout_component.h"
#include "component/client/client_machine_component.h"
#include "component/client/client_option_component.h"
#include "component/io/file_component.h"
#include "component/router/router_component.h"
#include "component/transfer/download_component.h"
#include "component/transfer/download_thread_component.h"
#include "component/transfer/chunk_component.h"
#include "component/transfer/download_thread_component.h"
#include "component/transfer/transfer_idle_component.h"
#include "resource/resource.h"
#include "utility/io/file_util.h"
#include "window/client_window.h"
#include "window/window.h"

namespace eja
{
	// Interface
	void client_download_model::init()
	{
		entity_table_model::init();

		// Callback
		add_callback(function_type::download);

		// Timer
		m_timer = new QTimer(this);
		m_timer->setInterval(1000);

		connect(m_timer, &QTimer::timeout, [this]()
		{
			const auto download_vector = m_entity->get<download_entity_vector_component>();
			if (!download_vector || download_vector->empty())
				return;

			QModelIndex start = createIndex(0, column::time);
			QModelIndex end = createIndex(download_vector->size() - 1, column::max - 1);
			emit dataChanged(start, end);

		});

		m_timer->start();
	}

	void client_download_model::add(const entity::ptr entity)
	{
		// Verify
		const auto download_vector = m_entity->get<download_entity_vector_component>();
		if (!download_vector || download_vector->has(entity))
			return;

		const auto option = m_entity->get<client_option_component>();
		if (!option)
			return;

		// Abort if download path is invalid
		const auto& path = option->get_download_path();
		if (!folder_util::exists(path))
		{
			const auto str = boost::str(boost::format("Invalid download path: %s") % path);
			entity->log(str);
			return;
		}

		// Time		
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
		
		const auto threads = option->get_download_threads();
		const auto thread = download_thread_component::create(threads);
		thread->add(function);

		entity->add(thread);
		thread->start();

		emit added(entity);
	}

	void client_download_model::remove(const entity::ptr entity)
	{
		entity->shutdown();

		emit removed(entity);
	}

	void client_download_model::clear()
	{
		const auto download_vector = m_entity->get<download_entity_vector_component>();
		if (!download_vector)
			return;

		const auto option = m_entity->get<client_option_component>();

		for (const auto& entity : download_vector->copy())
		{
			const auto thread = entity->get<download_thread_component>();
			if (thread && thread->removeable())
			{
				entity->shutdown();

				if (option && option->has_partial())
				{
					const auto transfer = entity->get<transfer_component>();
					if (transfer && transfer->is_download() && !transfer->done())
					{
						const auto file = entity->get<file_component>();
						if (file)
							file_util::remove(file);
					}
				}

				emit removed(entity);
			}
		}
	}

	void client_download_model::on_clear(const entity::ptr entity)
	{
		const auto download_vector = m_entity->get<download_entity_vector_component>();
		if (!download_vector)
			return;

		const auto option = m_entity->get<client_option_component>();

		for (const auto& entity : download_vector->copy())
		{
			const auto thread = entity->get<download_thread_component>();
			if (thread && thread->running())
				thread->stop();

			entity->shutdown();

			if (option && option->has_partial())
			{
				const auto transfer = entity->get<transfer_component>();
				if (transfer && transfer->is_download() && !transfer->done())
				{
					const auto file = entity->get<file_component>();
					if (file)
						file_util::remove(file);
				}
			}
		}

		beginResetModel();
		download_vector->clear();
		endResetModel();

		set_status(client_statusbar::download, download_vector->size());
	}

	void client_download_model::double_click(const QModelIndex& index)
	{
		if (!index.isValid())
			return;

		const auto download_vector = m_entity->get<download_entity_vector_component>();
		if (!download_vector)
			return;

		const size_t row = static_cast<size_t>(index.row());
		const auto entity = download_vector->get(row);
		if (entity)
		{
			const auto transfer = entity->get<transfer_component>();
			if (transfer && !transfer->empty())
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
	void client_download_model::on_add(const entity::ptr entity)
	{
		assert(thread::main());

		const auto download_vector = m_entity->get<download_entity_vector_component>();
		if (!download_vector || download_vector->has(entity))
			return;

		{
			auto_lock_ptr(download_vector);

			const auto row = download_vector->size();

			beginInsertRows(QModelIndex(), row, row);
			download_vector->add(entity);
			endInsertRows();
		}		

		set_status(client_statusbar::download, download_vector->size());
	}

	void client_download_model::on_remove(const entity::ptr entity)
	{
		assert(thread::main());

		const auto download_vector = m_entity->get<download_entity_vector_component>();
		if (!download_vector)
			return;

		{
			auto_lock_ptr(download_vector);

			const auto row = download_vector->get(entity);
			if (row == type::npos)
				return;

			beginRemoveRows(QModelIndex(), row, row);
			download_vector->remove(row);
			endRemoveRows();
		}

		set_status(client_statusbar::download, download_vector->size());
	}

	void client_download_model::on_clear()
	{
		assert(thread::main());

		const auto download_vector = m_entity->get<download_entity_vector_component>();
		if (!download_vector)
			return;

		beginResetModel();
		download_vector->clear();
		endResetModel();

		set_status(client_statusbar::download, download_vector->size());
	}

	// Utility
	bool client_download_model::empty() const
	{
		const auto download_vector = m_entity->get<download_entity_vector_component>();
		return !download_vector || download_vector->empty();
	}

	// Model
	bool client_download_model::removeRows(int row, int count, const QModelIndex& parent /*= QModelIndex()*/)
	{
		assert(thread::main());

		const auto download_vector = m_entity->get<download_entity_vector_component>();
		if (!download_vector)
			return false;

		size_t status = 0;
		const auto option = m_entity->get<client_option_component>();

		beginRemoveRows(QModelIndex(), row, (row + count - 1));

		for (auto i = (row + count - 1); i >= row; --i)
		{
			const auto entity = download_vector->get(row);
			if (entity)
			{
				const auto thread = entity->get<download_thread_component>();
				if (thread && thread->removeable())
				{
					entity->shutdown();
					
					if (option && option->has_partial())
					{
						const auto transfer = entity->get<transfer_component>();
						if (transfer && transfer->is_download() && !transfer->done())
						{
							const auto file = entity->get<file_component>();
							if (file)
								file_util::remove(file);
						}
					}

					status += download_vector->remove(row);					
				}
			}
		}

		endRemoveRows();

		set_status(client_statusbar::download, download_vector->size());

		return status > 0;
	}

	QVariant client_download_model::data(const QModelIndex& index, int role) const
	{
		if (!index.isValid())
			return QVariant();

		const size_t row = static_cast<size_t>(index.row());
		const size_t col = static_cast<size_t>(index.column());

		const auto download_vector = m_entity->get<download_entity_vector_component>();
		if (!download_vector)
			return QVariant();

		const auto entity = download_vector->get(row);
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
						const auto thread = entity->get<download_thread_component>();
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
						const auto thread = entity->get<download_thread_component>();
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
						const auto thread = entity->get<download_thread_component>();
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

	int client_download_model::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
	{
		const auto download_vector = m_entity->get<download_entity_vector_component>();
		return download_vector ? download_vector->size() : 0;
	}

	// Accessor
	size_t client_download_model::get_row(const entity::ptr entity) const
	{
		const auto download_vector = m_entity->get<download_entity_vector_component>();
		return download_vector ? download_vector->get(entity) : type::npos;
	}
}
