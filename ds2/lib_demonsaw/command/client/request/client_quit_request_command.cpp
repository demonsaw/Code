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

#include "client_quit_request_command.h"
#include "component/session_component.h"
#include "json/json_packet.h"
#include "message/request/quit_request_message.h"
#include "security/base.h"

namespace eja
{
	http_status client_quit_request_command::execute()
	{		
		// Request
		const auto request_packet = json_packet::create();
		const auto request_message = quit_request_message::create(request_packet);

		// Session
		const auto session = get_session();
		if (!session)
			return http_code::bad_request;

		const auto json = request_packet->str();		
		const auto json_encrypted = session->encrypt(json);
		const auto json_encoded = base64::encode(json_encrypted);

		// Http
		http_request request(session->get_id());
		request.set_host(get_address());
		request.set_body(json_encoded);
		m_socket->write(request);

		http_response response;
		m_socket->read(response);

		// Response
		return response.get_status();
	}
}
