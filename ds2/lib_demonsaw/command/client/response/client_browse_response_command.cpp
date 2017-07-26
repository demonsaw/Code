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

#include "client_browse_response_command.h"
#include "component/session_component.h"
#include "data/request/browse_request_data.h"
#include "data/response/browse_response_data.h"
#include "json/json_packet.h"
#include "json/value/json_file.h"
#include "json/value/json_folder.h"
#include "message/request/browse_request_message.h"
#include "message/response/browse_response_message.h"

namespace eja
{
	http_status client_browse_response_command::execute(const json_message::ptr message, const json_data::ptr data)
	{
		// Request
		const auto request_message = browse_request_message::create(message);
		if (request_message->invalid())
		{			
			m_socket->write(http_code::bad_request);
			return http_code::bad_request;
		}

		const auto request_data = get_data(data);
		const auto request_browse = browse_request_data::create(request_data);

		// Response
		const auto response_packet = json_packet::create();
		const auto response_message = browse_response_message::create(response_packet);

		// Data
		if (request_browse->has_folder())
		{
			// Parent
			const auto folder_map = get_folder_map();
			if (!folder_map)
			{
				m_socket->write(http_code::internal_server_error);
				return http_code::internal_server_error;
			}

			const auto request_folder = request_browse->get_folder();			
			const auto folder = folder_map->get(request_folder->get_id());

			if (!folder)
			{
				m_socket->write(http_code::not_found);
				return http_code::not_found;
			}

			const auto response_data = json_data::create();
			const auto response_browse = browse_response_data::create(response_data);

			// Folders
			const auto response_folders = response_browse->get_folders();
			const auto folders = folder->get_folders();
			{
				auto_lock_ref(folders);
				for (const auto& item : folders)
					response_folders->add(item);
			}

			// Files
			const auto response_files = response_browse->get_files();
			const auto files = folder->get_files();
			{
				auto_lock_ref(files);
				for (const auto& item : files)
					response_files->add(item);
			}

			set_data(response_packet, response_data);
		}
		else
		{
			const auto share = get_share();
			if (!share)
			{
				m_socket->write(http_code::internal_server_error);
				return http_code::internal_server_error;
			}

			const auto response_data = json_data::create();

			// Folders
			const auto response_folders = json_folder_array::create(response_data, json::folders);
			const auto folders = share->get_folders();
			{				
				auto_lock_ref(folders);
				for (const auto& item : folders)
					response_folders->add(item);
			}

			// Files
			const auto response_files = json_file_array::create(response_data, json::files);
			const auto files = share->get_files();
			{
				auto_lock_ref(files);
				for (const auto& item : files)
					response_files->add(item);
			}

			set_data(response_packet, response_data);
		}

		// Session
		const auto session = get_session();
		if (!session)
		{
			m_socket->write(http_code::internal_server_error);
			return http_code::internal_server_error;
		}

		const auto json = response_packet->str();		
		const auto json_encrypted = session->encrypt(json);
		const auto json_encoded = base64::encode(json_encrypted);

		// Http
		http_ok_response response;
		response.set_body(json_encoded);
		m_socket->write(response);

		return response.get_status();
	}
}
