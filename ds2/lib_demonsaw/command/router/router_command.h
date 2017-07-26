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

#ifndef _EJA_ROUTER_COMMAND_
#define _EJA_ROUTER_COMMAND_

#include <memory>

#include "component/mutex_component.h"
#include "component/session_component.h"
#include "component/spam_component.h"
#include "component/client/client_component.h"
#include "component/group/group_component.h"
#include "component/router/router_component.h"
#include "component/router/router_machine_component.h"
#include "component/router/router_option_component.h"
#include "component/router/router_security_component.h"
#include "component/server/server_component.h"
#include "component/transfer/chunk_component.h"
#include "entity/entity.h"
#include "json/json_command.h"
#include "utility/router/router_util.h"

namespace eja
{
	class router_command : public json_command
	{
	public:
		using ptr = std::shared_ptr<router_command>;

	protected:
		router_command(const router_command&) = delete; 
		router_command(const entity::ptr entity) : json_command(entity) { }
		router_command(const entity::ptr entity, const http_socket::ptr socket) : json_command(entity, socket) { }
		virtual ~router_command() override { }

		// Operator
		router_command& operator=(const router_command&) = delete;

		// Utility
		void remove_client(const std::string& id);
		void remove_client(const entity::ptr entity);

		void remove_session(const std::string& id);
		void remove_session(const entity::ptr entity);

		void remove_spam(const std::string& id);
		void remove_spam(const entity::ptr entity);

		void remove_transfer(const std::string& id);
		void remove_transfer(const entity::ptr entity);

		bool has_passphrase() const { return get_router_security()->has_passphrase(); }

		// Router
		router_component::ptr get_router() const { return m_entity->get<router_component>(); }
		router_machine_component::ptr get_router_machine() const { return m_entity->get<router_machine_component>(); }
		router_option_component::ptr get_router_option() const { return m_entity->get<router_option_component>(); }
		router_security_component::ptr get_router_security() const { return m_entity->get<router_security_component>(); }

		// Server
		entity_vector::ptr get_active_servers() const { return router_util::get_active_servers(m_entity); }
		entity::ptr get_active_server() const { return router_util::get_active_server(m_entity); }

		// Map
		entity_map_component::ptr get_entity_map() const { return m_entity->get<entity_map_component>(); }		
		client_entity_map_component::ptr get_client_entity_map() const { return m_entity->get<client_entity_map_component>(); }
		group_entity_map_component::ptr get_group_entity_map() const { return m_entity->get<group_entity_map_component>(); }
		session_entity_map_component::ptr get_session_entity_map() const { return m_entity->get<session_entity_map_component>(); }
		chunk_entity_map_component::ptr get_chunk_entity_map() const { return m_entity->get<chunk_entity_map_component>(); }
		spam_entity_map_component::ptr get_spam_entity_map() const { return m_entity->get<spam_entity_map_component>(); }

		// List
		server_entity_vector_component::ptr get_server_vector() const { return m_entity->get<server_entity_vector_component>(); }
		
		// Misc
		mutex_component::ptr get_mutex() const { return m_entity->get<mutex_component>(); }

		// Accessor
		json_data::ptr get_data(const json_data::ptr data);
		json_data::ptr get_data(const json_packet::ptr packet);

		const std::string& get_router_passphrase() const { return get_router_security()->get_passphrase(); }
		const std::string& get_router_address() const { return get_router()->get_address(); }
		const u16& get_router_port() const { return get_router()->get_port(); }
	};
}

#endif
