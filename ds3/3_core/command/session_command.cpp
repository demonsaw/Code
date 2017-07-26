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

#include "command/session_command.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "component/session/session_component.h"
#include "entity/entity.h"
#include "message/request/request_message.h"
#include "message/response/response_message.h"
#include "security/security.h"

namespace eja
{
	// Client
	session_request_message::ptr client_session_command::execute(const entity::ptr router)
	{
		return session_request_message::create();
	}

	void client_session_command::execute(const entity::ptr router, const std::shared_ptr<session_response_message> response)
	{
		// Session
		const auto session = m_entity->get<session_component>();
		session->set_id(response->get_id());
	}

	// Router
	session_response_message::ptr router_session_command::execute(const entity::ptr client, const session_request_message::ptr request /*= nullptr*/)
	{
		// Session
		const auto session = client->get<session_component>();

		while (true)
		{
			const auto session_id = security::get_id();			
			const auto session_map = m_entity->get<session_map_component>();
			const auto pair = std::make_pair(session_id, client);

			thread_lock(session_map);
			const auto it = session_map->find(session_id);

			if (it == session_map->end())
			{
				if (session->has_id())
				{
					session_map->erase(session->get_id());
					m_entity->call(callback_type::session, callback_action::remove, client);
				}

				session->set_id(session_id);								
				session_map->insert(pair);

				m_entity->call(callback_type::session, callback_action::add, client);

				break;
			}
		}

		// Response
		const auto response = session_response_message::create();
		response->set_id(session->get_id());

		return response;
	}
}
