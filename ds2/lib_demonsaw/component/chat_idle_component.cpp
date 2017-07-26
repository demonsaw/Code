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
\
#include "chat_idle_component.h"
#include "client/client_option_component.h"
#include "command/client/request/client_chat_request_command.h"
#include "http/http_code.h"
#include "router/router_component.h"

namespace eja
{
	// Interface
	void chat_idle_component::init()
	{
		idle_component::init();

		start();
	}

	void chat_idle_component::shutdown()
	{
		idle_component::shutdown();

		close();
	}

	// Utility
	void chat_idle_component::open()
	{
		try
		{
			close();

			// Open
			const auto owner = get_entity();	
			if (!owner)
			{
				log("Invalid entity (chat)");
				return;
			}				

			const auto router = owner->get<router_component>();
			if (!router)
			{
				log("Invalid router (chat)");
				return;
			}

			const auto option = owner->get<client_option_component>();
			if (!option)
			{
				log("Invalid option (chat)");
				return;
			}

			// Socket
			//m_socket->set_keep_alive(true);
			m_socket->set_timeout(option->get_socket_timeout());
			m_socket->open(router->get_address(), router->get_port());
		}
		catch (std::exception& /*ex*/)
		{
			//log(ex);
		}
		catch (...)
		{
			//log("Unknown Error (chat)");
		}
	}

	void chat_idle_component::close()
	{
		try
		{
			// Close
			m_socket->close();
		}
		catch (std::exception& /*ex*/)
		{
			//log(ex);
		}
		catch (...)
		{
			//log("Unknown Error (chat)");
		}
	}

	void chat_idle_component::add(const std::string& message, const entity::ptr entity /*= nullptr*/)
	{
		const auto data = std::shared_ptr<chat_data>(new chat_data { entity, message, (entity ? chat_type::client : chat_type::group) });
		m_queue.add(data);
	}

	void chat_idle_component::add(const entity::ptr entity, const std::string& message, const chat_type type)
	{
		const auto data = std::shared_ptr<chat_data>(new chat_data { entity, message, type } );
		m_queue.add(data);
	}

	bool chat_idle_component::on_run()
	{
		const auto owner = get_entity();
		if (!owner)
			return idle_component::on_run();

		const auto data = m_queue.pop();
		if (!data)
			return idle_component::on_run();

		try
		{
			// Socket
			if (m_socket->invalid())
				open();

			// Command
			const auto request_command = client_chat_request_command::create(owner, m_socket);
			const auto request_status = request_command->execute(data->entity, data->message, data->type);

			if (request_status.is_error())
			{
				// Try again?
				if (request_status.is_none())
					m_queue.push_front(data);

				//log(request_status);
				open();
			}
		}
		catch (std::exception& /*ex*/)
		{
			//log(e);
			m_queue.push_front(data);
			open();
		}
		catch (...)
		{
			//log("Unknown Error (chat)");
			m_queue.push_front(data);
			open();
		}

		// Delay
		const auto delay = m_queue.empty() ? default_timeout::client::chat : default_timeout::error;
		set_delay(delay);

		return idle_component::on_run();
	}

	bool chat_idle_component::on_stop()
	{
		close();

		return idle_component::on_stop();
	}
}
