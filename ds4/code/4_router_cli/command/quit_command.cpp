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

#include "command/clear_command.h"
#include "command/quit_command.h"
#include "command/router_command.h"

#include "component/client/client_component.h"
#include "component/group/group_component.h"
#include "component/router/router_acceptor_component.h"

#include "data/response/info_response_data.h"
#include "entity/entity.h"
#include "entity/entity_util.h"

namespace eja
{	
	// Interface
	void quit_command::execute(const entity::ptr& entity, const router_request_message::ptr& request_message, const quit_request_data::ptr& request_data) const
	{
		// Validate
		if (unlikely(!validate(request_message, request_data)))
			return;

		// Command
		clear_command cmd(m_entity);
		cmd.execute(entity);
	}

	// Utility
	bool quit_command::validate(const router_request_message::ptr& request_message, const quit_request_data::ptr& request_data) const
	{
		return true;
	}
}
