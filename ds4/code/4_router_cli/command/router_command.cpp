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

#include "command/download_command.h"
#include "command/handshake_command.h"
#include "command/join_command.h"
#include "command/mute_command.h"
#include "command/ping_command.h"
#include "command/quit_command.h"
#include "command/room_command.h"
#include "command/router_command.h"
#include "command/tunnel_command.h"
#include "command/upload_command.h"

#include "component/router/router_component.h"
#include "component/router/router_acceptor_component.h"
#include "component/session/session_component.h"

#include "data/request/download_request_data.h"
#include "data/request/handshake_request_data.h"
#include "data/request/info_request_data.h"
#include "data/request/join_request_data.h"
#include "data/request/mute_request_data.h"
#include "data/request/ping_request_data.h"
#include "data/request/quit_request_data.h"
#include "data/request/room_request_data.h"
#include "data/request/tunnel_request_data.h"
#include "data/request/upload_request_data.h"

#include "data/response/download_response_data.h"
#include "data/response/handshake_response_data.h"
#include "data/response/info_response_data.h"
#include "data/response/join_response_data.h"
#include "data/response/ping_response_data.h"
#include "data/response/room_response_data.h"
#include "data/response/upload_response_data.h"

#include "exception/not_implemented_exception.h"
#include "http/http_socket.h"
#include "message/message_packer.h"
#include "security/security.h"
#include "utility/value.h"

namespace eja
{
	// Constructor
	router_command::router_command() : m_state(router_acceptor_state::read)
	{
	}

	router_command::router_command(const entity::ptr& entity) : entity_type(entity), m_state(router_acceptor_state::read)
	{
	}

	// Interface
	router_response_message::ptr router_command::execute(const entity::ptr& entity, const http_socket::ptr& socket, const router_request_message::ptr& request_message)
	{
		// Validate
		if (unlikely(!validate(entity, request_message)))
		{
			m_state = router_acceptor_state::none;

			return nullptr;
		}

		// Unpack
		const auto unpacker = unpack(request_message);
		if (unlikely(unpacker.empty()))
			return nullptr;

		// Loop
		//assert(unpacker->size() == 1);
		data_packer packer;

		for (const auto& data : unpacker)
		{
			// Handshake
			const auto type = data->get_type();
			if (type == data_type::handshake_request)
			{
				// Session
				m_session = entity->copy<session_component>();

				// Request
				const auto request_data = std::static_pointer_cast<handshake_request_data>(data);

				// Command
				handshake_command cmd(m_entity);
				const auto response_data = cmd.execute(entity, request_message, request_data);
				if (response_data)
					packer.push_back(response_data);
			}
			else
			{
				// Session
				const auto session = entity->get<session_component>();
				if (!session->has_id())
				{
					m_state = router_acceptor_state::none;
					return nullptr;
				}

				switch (type)
				{
					case data_type::join_request:
					{
						// Request
						const auto request_data = std::static_pointer_cast<join_request_data>(data);

						// Command
						join_command cmd(m_entity);
						const auto response_data = cmd.execute(entity, request_message, request_data);
						if (response_data)
							packer.push_back(response_data);

						break;
					}
					case data_type::tunnel_request:
					{
						tunnel_command cmd(m_entity);
						const auto request_data = std::static_pointer_cast<tunnel_request_data>(data);
						cmd.execute(entity, request_message, request_data, socket);

						// Do not reuse the socket
						m_state = router_acceptor_state::tunnel;

						break;
					}
					case data_type::quit_request:
					{
						quit_command cmd(m_entity);
						const auto request_data = std::static_pointer_cast<quit_request_data>(data);
						cmd.execute(entity, request_message, request_data);

						// Do not reuse the socket
						m_state = router_acceptor_state::none;

						break;
					}
					case data_type::ping_request:
					{
						ping_command cmd(m_entity);
						const auto request_data = std::static_pointer_cast<ping_request_data>(data);
						const auto response_data = cmd.execute(entity, request_message, request_data);
						if (response_data)
							packer.push_back(response_data);

						break;
					}
					case data_type::room_request:
					{
						room_command cmd(m_entity);
						const auto request_data = std::static_pointer_cast<room_request_data>(data);
						const auto response_data = cmd.execute(entity, request_message, request_data);
						if (response_data)
							packer.push_back(response_data);

						break;
					}
					case data_type::mute_request:
					{
						mute_command cmd(m_entity);
						const auto request_data = std::static_pointer_cast<mute_request_data>(data);
						cmd.execute(entity, request_message, request_data);

						break;
					}
					case data_type::download_request:
					{
						// Transfer?
						const auto router = m_entity->get<router_component>();
						if (!router->is_transfer())
							throw not_implemented_exception("Router doesn't support transfers");

						// Request
						const auto request_data = std::static_pointer_cast<download_request_data>(data);

						// Command
						download_command cmd(m_entity);
						const auto response_data_list = cmd.execute(entity, request_message, request_data);
						if (response_data_list)
							packer.insert(packer.end(), response_data_list->begin(), response_data_list->end());

						break;
					}
					case data_type::upload_request:
					{
						// Transfer?
						const auto router = m_entity->get<router_component>();
						if (!router->is_transfer())
							throw not_implemented_exception("Router doesn't support transfers");

						// Request
						const auto request_data = std::static_pointer_cast<upload_request_data>(data);

						// Command
						upload_command cmd(m_entity);
						const auto response_data = cmd.execute(entity, request_message, request_data);
						if (response_data)
							packer.push_back(response_data);

						break;
					}
					default:
					{
						assert(false);

						break;
					}
				}
			}
		}

		// Pack
		if (!packer.empty())
		{
			// Response
			/*const*/ auto data = packer.pack();
			const auto response_message = router_response_message::create();
			response_message->set_id(request_message->get_id());
			response_message->set_data(std::move(data));

			return response_message;
		}

		return nullptr;
	}

	// Utility
	bool router_command::validate(const entity::ptr& entity, const router_request_message::ptr& request_message) const
	{
		// Id
		const auto& id = request_message->get_id();
		if (unlikely(id.size() > io::max_id_size))
			return false;

		// Clients
		for (const auto& id : request_message->get_clients())
		{
			if (unlikely(id.size() > security::get_max_size()))
				return false;
		}

		// Origin
		const auto& origin = request_message->get_origin();
		if (unlikely(origin.size() > io::max_id_size))
			return false;

		// Data
		return likely(request_message->has_data());
	}

	// Has
	bool router_command::has_state() const
	{
		return m_state != router_acceptor_state::read;
	}
}
