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

#include "command/router_command.h"
#include "command/info_command.h"

#include "component/client/client_component.h"

#include "data/client_data.h"
#include "data/response/info_response_data.h"
#include "entity/entity.h"
#include "security/filter/exclusive.h"

namespace eja
{
	// Get
	router_response_message::ptr info_command::execute() const
	{
		// Data
		const auto response_data = info_response_data::create();

		// XOR: Don't really need this
		//const auto c = entity->get<client_component>();
		//const auto client = m_entity->get<client_component>();
		///*const*/ auto idx = exclusive::compute(client->get_id(), c->get_seed());
		//cdata->set_id(std::move(idx));

		// Client
		const auto cdata = client_data::create();

		const auto client = m_entity->get<client_component>();
		cdata->set_id(client->get_id());
		cdata->set_name(client->get_name());
		cdata->set_color(client->get_color());

		cdata->set_share(client->is_share());
		cdata->set_troll(client->is_troll());
		cdata->set_verified(client->is_verified());

		response_data->set_client(cdata);

		// Command
		router_command cmd(m_entity);
		const auto response_message = cmd.get_response_message(response_data);

		return response_message;
	}
}
