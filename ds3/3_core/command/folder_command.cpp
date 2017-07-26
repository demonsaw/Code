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

#include "command/folder_command.h"

#include "component/browse_component.h"
#include "component/endpoint_component.h"
#include "component/client/client_io_component.h"
#include "component/io/folder_component.h"
#include "component/io/save_component.h"

#include "data/request/browse_request_data.h"
#include "entity/entity.h"
#include "message/request/request_message.h"
#include "security/security.h"

namespace eja
{
	// Client
	browse_request_message::ptr client_folder_command::execute(const entity::ptr router, const entity::ptr data)
	{
		const auto io = m_entity->get<client_io_component>();
		return execute(router, data, io->get_path());
	}

	browse_request_message::ptr client_folder_command::execute(const entity::ptr router, const entity::ptr data, const std::string& path)
	{
		assert(!path.empty());

		const auto id = security::random(security::s_max);
		data->set_id(id);

		const auto browse_map = m_entity->get<browse_map_component>();
		{
			const auto pair = std::make_pair(id, data);

			thread_lock(browse_map);
			browse_map->insert(pair);
		}

		const auto save_map = m_entity->get<save_map_component>();
		{
			const auto pair = std::make_pair(id, path);

			thread_lock(save_map);
			save_map->insert(pair);
		}

		// Data
		const auto request_data = browse_request_data::create();

		if (data->has<folder_component>())
		{
			const auto folder = data->get<folder_component>();
			request_data->set_folder(folder->get_id());
		}

		// Packer
		data_packer packer;
		packer.push_back(request_data);
		auto packed = packer.pack();

		// Group
		const auto group = m_entity->get<group_component>();
		if (group->valid())
			packed = group->encrypt(packed);

		// Request
		const auto request = browse_request_message::create();
		request->set_id(id);

		const auto endpoint = data->find<endpoint_component>();
		request->set_client(endpoint->get_id());
		request->set_data(packed);

		return request;
	}
}
