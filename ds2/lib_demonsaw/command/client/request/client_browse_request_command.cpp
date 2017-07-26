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

#include "client_browse_request_command.h"
#include "component/session_component.h"
#include "component/client/client_component.h"
#include "component/io/folder_component.h"
#include "data/request/browse_request_data.h"
#include "data/response/browse_response_data.h"
#include "json/value/json_file.h"
#include "json/value/json_folder.h"
#include "message/request/browse_request_message.h"
#include "security/base.h"
#include "system/utf/utf.h"

namespace eja
{
	// Interface
	void client_browse_request_command::clear()
	{
		client_command::clear();

		m_folders.clear();
		m_files.clear();
	}

	// Utility
	http_status client_browse_request_command::execute(const entity::ptr entity)
	{
		clear();

		// Request
		const auto request_packet = json_packet::create();
		const auto request_message = browse_request_message::create(request_packet);
		set_data(request_packet, entity);

		// Client
		const auto client = entity->get<client_component>();
		if (!client)
			return http_code::bad_request;

		const auto request_client = request_message->get_client();
		request_client->set_id(client->get_id());

		// Session
		const auto session = get_session();
		if (!session)
			return http_code::bad_request;

		// Json
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

		const auto& status = response.get_status();
		if (status.is_ok())
		{
			// Body
			const auto& body = response.get_body();
			if (body.empty())
				return http_code::not_found;

			const auto body_decoded = base64::decode(body);
			const auto body_decypted = session->decrypt(body_decoded);

			// Response
			const auto response_packet = json_packet::create(body_decypted);
			const auto response_data = get_data(response_packet);
			const auto response_browse = browse_response_data::create(response_data);

			// Folders
			const auto response_folders = response_browse->get_folders();

			for (const auto& value : *response_folders)
			{
				const auto response_value = json_value::create(value);
				const auto response_folder = json_folder::create(response_value);

				const auto folder = folder::create();
				folder->set_id(response_folder->get_id());
				folder->set_path(response_folder->get_name());
				folder->set_size(response_folder->get_size());
				folder->clean();

				m_folders.add(folder);
			}

			// Files
			const auto response_files = response_browse->get_files();

			for (const auto& value : *response_files)
			{
				const auto response_value = json_value::create(value);
				const auto response_file = json_file::create(response_value);

				const auto file = file::create();
				file->set_id(response_file->get_id());
				file->set_path(response_file->get_name());
				file->set_size(response_file->get_size());
				file->clean();

				m_files.add(file);
			}
		}

		// Response
		return response.get_status();
	}

	// Mutator
	void client_browse_request_command::set_data(const json_packet::ptr packet, const entity::ptr entity) const
	{
		if (entity->has<folder_component>())
		{
			const auto folder = entity->get<folder_component>();
			if (!folder)
				return;

			const auto request_browse = browse_request_data::create(packet);			
			const auto request_folder = request_browse->get_folder();
			request_folder->set_id(folder->get_id());
			set_data(packet, request_browse);
		}
	}
}
