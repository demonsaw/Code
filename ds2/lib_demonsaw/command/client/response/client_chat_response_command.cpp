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

#include "client_chat_response_command.h"
#include "component/chat_component.h"
#include "data/response/chat_response_data.h"
#include "message/response/chat_response_message.h"

namespace eja
{
	http_status client_chat_response_command::execute(const json_message::ptr message, const json_data::ptr data)
	{
		// Request
		const auto request_message = chat_response_message::create(message);
		if (request_message->invalid())
		{
			m_socket->write(http_code::bad_request);
			return http_code::bad_request;
		}

		// Data
		const auto request_data = get_data(data);
		const auto request_chat = chat_response_data::create(request_data);
		if (request_chat->invalid())
		{
			m_socket->write(http_code::bad_request);
			return http_code::bad_request;
		}

		// Response
		http_ok_response response;
		m_socket->write(response);

		// Muted?
		const auto mute_map = get_mute_map();
		if (mute_map && mute_map->has(request_chat->get_id()))
			return response.get_status();

		// Chat
		const auto entity = entity::create();

		const auto client = client_component::create();
		client->set_id(request_chat->get_id());
		client->set_name(request_chat->get_name());
		client->set_join_name(request_chat->get_name());
		entity->add(client);

		const auto chat = chat_component::create();
		chat->set_text(request_chat->get_text());
		chat->set_type(request_chat->get_type());
		entity->add(chat);

		// Callback
		switch (chat->get_type())
		{
			case chat_type::group:
			{
				call(function_type::chat, function_action::add, entity);
				break;
			}
			case chat_type::client:
			{
				call(function_type::message, function_action::add, entity);
				break;
			}
		}

		return response.get_status();
	}
}
