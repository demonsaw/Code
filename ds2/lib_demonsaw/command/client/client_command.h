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

#ifndef _EJA_CLIENT_COMMAND_
#define _EJA_CLIENT_COMMAND_

#include <memory>

#include "component/chat_component.h"
#include "component/search_component.h"
#include "component/session_component.h"
#include "component/share_idle_component.h"
#include "component/client/client_component.h"
#include "component/client/client_machine_component.h"
#include "component/client/client_option_component.h"
#include "component/client/client_security_component.h"
#include "component/group/group_component.h"
#include "component/io/file_component.h"
#include "component/io/folder_component.h"
#include "component/router/router_component.h"
#include "component/router/router_info_component.h"
#include "component/router/router_security_component.h"
#include "component/server/server_component.h"
#include "component/server/server_info_component.h"
#include "component/server/server_security_component.h"
#include "entity/entity.h"
#include "json/json_command.h"
#include "json/json_data.h"
#include "json/json_message.h"
#include "utility/client/client_util.h"

namespace eja
{
	class client_command : public json_command
	{
	public:
		using ptr = std::shared_ptr<client_command>;

	private:
		// NOTE: NOT ENTIRELY THREAD-SAFE
		//
		// Router
		const std::string& get_router_passphrase() const { return get_router_security()->get_passphrase(); }
		const std::string& get_router_address() const { return get_router()->get_address(); }
		const u16& get_router_port() const { return get_router()->get_port(); }

		// NOTE: NOT ENTIRELY THREAD-SAFE
		//
		// Server
		const std::string& get_server_passphrase() const { return get_server_security()->get_passphrase(); }
		const std::string& get_server_address() const { return get_server()->get_address(); }
		const u16& get_server_port() const { return get_server()->get_port(); }

	protected:
		client_command(const client_command&) = delete;
		client_command(const entity::ptr entity) : json_command(entity) { }
		client_command(const entity::ptr entity, const http_socket::ptr socket) : json_command(entity, socket) { }
		virtual ~client_command() override { }

		// Operator
		client_command& operator=(const client_command&) = delete;

		// Has
		bool has_session() const { return m_entity->has<session_component>() && m_entity->get<session_component>()->has_id(); }
		bool has_client_security() const { return m_entity->has<client_security_component>(); }

		bool has_router() const { return m_entity->has<router_component>(); }
		bool has_router_info() const { return m_entity->has<router_info_component>(); }
		bool has_router_security() const { return m_entity->has<router_security_component>(); }

		bool has_server() const { return m_entity->has<server_component>(); }
		bool has_server_info() const { return m_entity->has<server_info_component>(); }
		bool has_server_security() const { return m_entity->has<server_security_component>(); }

		// Client		
		client_component::ptr get_client() const { return m_entity->get<client_component>(); }
		client_entity_vector_component::ptr get_client_vector() const { return m_entity->get<client_entity_vector_component>(); }
		client_machine_component::ptr get_client_machine() const { return m_entity->get<client_machine_component>(); }
		client_option_component::ptr get_client_option() const { return m_entity->get<client_option_component>(); }
		client_security_component::ptr get_client_security() const { return m_entity->get<client_security_component>(); }
		
		group_component::ptr get_group() const { return m_entity->get<group_component>(); }
		session_component::ptr get_session() const { return m_entity->get<session_component>(); }
		share_idle_component::ptr get_share() const { return m_entity->get<share_idle_component>(); }
		search_component::ptr get_search() const { return m_entity->get<search_component>(); }
		
		file_map_component::ptr get_file_map() const { return m_entity->get<file_map_component>(); }
		folder_map_component::ptr get_folder_map() const { return m_entity->get<folder_map_component>(); }
		chat_entity_vector_component::ptr get_chat_vector() const { return m_entity->get<chat_entity_vector_component>(); }
		mute_entity_map_component::ptr get_mute_map() const { return m_entity->get<mute_entity_map_component>(); }

		// Router		
		router_component::ptr get_router() const { return m_entity->get<router_component>(); }
		router_info_component::ptr get_router_info() const { return m_entity->get<router_info_component>(); }
		router_security_component::ptr get_router_security() const { return m_entity->get<router_security_component>(); }

		// Server		
		server_component::ptr get_server() const { return m_entity->get<server_component>(); }
		server_info_component::ptr get_server_info() const { return m_entity->get<server_info_component>(); }
		server_security_component::ptr get_server_security() const { return m_entity->get<server_security_component>(); }

		// NOTE: NOT ENTIRELY THREAD-SAFE
		//
		// Unknown
		const std::string& get_passphrase() const { return has_server() ? get_server_passphrase() : get_router_passphrase(); }
		const std::string& get_address() const { return has_server() ? get_server_address() : get_router_address(); }
		const u16& get_port() const { return has_server() ? get_server_port() : get_router_port(); }

		// Mutator
		template <typename T = json_data>
		void set_data(const json_packet::ptr packet, const T data) const;

		// Accessor
		json_data::ptr get_data(const json_data::ptr data);
		json_data::ptr get_data(const json_packet::ptr packet);
	};

	// Mutator
	template <typename T> 
	void client_command::set_data(const json_packet::ptr packet, const T data) const
	{
		// v1.0
		//////////////////////////////////////////////////////////////
		const auto json = data->str();
		const auto security = get_client_security();

		if (!security->empty())
		{
			const auto json_encrypted = security->encrypt(json);
			const auto json_encoded = base64::encode(json_encrypted);
			const auto request_data = json_data::create(packet);
			request_data->set(json_encoded);
		}
		else
		{
			const auto request_data = json_data::create(packet);
			request_data->set(json);
		}
		//////////////////////////////////////////////////////////////

		// v2.0
		//////////////////////////////////////////////////////////////
		//if (has_client_security())
		//{
		//	// ...
		//}
		//////////////////////////////////////////////////////////////

		
	}
}

#endif
