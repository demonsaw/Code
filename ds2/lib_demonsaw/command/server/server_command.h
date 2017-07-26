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

#ifndef _EJA_SERVER_COMMAND_
#define _EJA_SERVER_COMMAND_

#include <memory>

#include "component/session_component.h"
#include "component/server/server_component.h"
#include "component/server/server_machine_component.h"
#include "component/server/server_option_component.h"
#include "component/server/server_security_component.h"
#include "entity/entity.h"
#include "json/json_command.h"
#include "utility/router/router_util.h"

namespace eja
{
	class server_command : public json_command
	{
	public:
		using ptr = std::shared_ptr<server_command>;

	protected:
		server_command(const server_command&) = delete; 
		server_command(const entity::ptr entity) : json_command(entity) { }
		server_command(const entity::ptr entity, const http_socket::ptr socket) : json_command(entity, socket) { }
		virtual ~server_command() override { }

		// Operator
		server_command& operator=(const server_command&) = delete;

		// Router
		server_component::ptr get_server() const { return m_entity->get<server_component>(); }
		server_machine_component::ptr get_server_machine() const { return m_entity->get<server_machine_component>(); }
		server_option_component::ptr get_server_option() const { return m_entity->get<server_option_component>(); }
		server_security_component::ptr get_server_security() const { return m_entity->get<server_security_component>(); }
	};
}

#endif
