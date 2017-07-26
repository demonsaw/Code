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

#include <QAction>
#include <QApplication>
#include <QHeaderView>
#include <QMenu>
#include <QPoint>
#include <QShowEvent>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>

#include "client_search_table.h"
#include "command/client/request/client_search_request_command.h"
#include "component/client/client_option_component.h"
#include "component/error/error_component.h"
#include "component/router/router_component.h"
#include "delegate/search/search_name_delegate.h"
#include "data/request/search_request_data.h"
#include "entity/entity.h"
#include "http/http_socket.h"
#include "resource/resource.h"
#include "window/window.h"

namespace eja
{
	// Constructor
	client_search_table::client_search_table(QWidget* parent /*= 0*/) : table_window(parent)
	{
		create(parent);
		layout(parent);
		signal(parent);
	}

	// Interface
	void client_search_table::create(QWidget* parent /*= 0*/)
	{
		table_window::create(parent);

		// HACK: Should be done via create function (virtual)
		//
		// Sort
		const auto hheader = horizontalHeader();
		hheader->setSortIndicator(get_sort_column(), Qt::SortOrder::AscendingOrder);

		// Action
		m_download_action = new QAction(tr("Download"), this);
		m_download_action->setIcon(QIcon(resource::menu::download));		

		m_remove_action = new QAction(tr("Remove"), this);
		m_remove_action->setIcon(QIcon(resource::menu::remove));		

		m_clear_action = new QAction(tr("Clear"), this);
		m_clear_action->setIcon(QIcon(resource::menu::clear));
	}

	void client_search_table::layout(QWidget* parent /*= 0*/)
	{
		// Delegate
		setItemDelegateForColumn(client_search_model::column::name, new search_name_delegate(this));

		// Menu
		setContextMenuPolicy(Qt::CustomContextMenu);
	}

	void client_search_table::signal(QWidget* parent /*= 0*/)
	{
		// Menu
		connect(m_download_action, &QAction::triggered, this, &client_search_table::double_click);

		connect(m_remove_action, &QAction::triggered, [this]
		{
			const auto function = m_functions.get(table_callback::remove);
			if (function)
				function->call();
		});

		connect(m_clear_action, &QAction::triggered, [this]
		{
			const auto function = m_functions.get(table_callback::clear);
			if (function)
				function->call();
		});

		connect(this, &client_search_table::customContextMenuRequested, this, &client_search_table::show_menu);
	}

	// Slot
	void client_search_table::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
	{
		const auto function = m_functions.get(table_callback::click);
		if (function)
			function->call();

		// Event
		table_window::selectionChanged(selected, deselected);
	}

	void client_search_table::add(const std::string& keyword, const file_filter& filter)
	{
		const auto model = get_source_model();
		model->clear();

		const auto entity = model->get_entity();
		const auto socket = http_socket::create();

		try
		{
			const auto option = entity->get<client_option_component>();
			if (!option)
				return;

			const auto router = entity->get<router_component>();
			if (!router)
				return;

			// Socket
			socket->set_timeout(option->get_socket_timeout()); 
			socket->open(router->get_address(), router->get_port());

			// Command
			const auto request_command = client_search_request_command::create(entity, socket);
			const auto request_status = request_command->execute(keyword, filter);

			switch (request_status.get_code())
			{
				case http_code::ok:
				{
					break;
				}
				case http_code::none:
				{
					const auto str = boost::str(boost::format("Search timeout: %s") % router->get_address());
					entity->log(str);
					break;
				}
				case http_code::not_implemented:
				{
					const auto str = boost::str(boost::format("Search not implemented: %s") % router->get_address());
					entity->log(str);
					break;
				}
				case http_code::bad_request:
				{
					// If the router sends a bad_request this means that it's overloaded, try again
					const auto str = boost::str(boost::format("Search bad request: %s") % router->get_address());
					entity->log(str);
					break;
				}
				default:
				{
					const auto str = boost::str(boost::format("Search error: %s") % router->get_address());
					entity->log(str);
					break;
				}
			}
		}
		catch (std::exception& e)
		{
			const auto error = entity->get<error_component>();
			if (error)
				error->log(e);
		}
		catch (...)
		{
			const auto error = entity->get<error_component>();
			if (error)
				error->log();
		}

		socket->close();
	}

	void client_search_table::show_menu(const QPoint& pos)
	{
		const auto index = indexAt(pos);
		if (!index.isValid())
			return;

		QMenu menu(this);
		menu.addAction(m_download_action);
		menu.addSeparator();

		menu.addAction(m_remove_action);
		menu.addAction(m_clear_action);

		menu.exec(viewport()->mapToGlobal(pos));
	}
}