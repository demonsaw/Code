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

#ifndef _EJA_ENDPOINT_COMPONENT_H_
#define _EJA_ENDPOINT_COMPONENT_H_

#include <deque>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "component/name_component.h"
#include "thread/thread_safe.h"
#include "system/type.h"

namespace eja
{
	class entity;

	make_thread_safe_component(endpoint_list_component, std::vector<std::shared_ptr<entity>>);
	make_thread_safe_component(endpoint_map_component, std::map<std::string, std::shared_ptr<entity>>);
	make_thread_safe_component(endpoint_set_component, std::set<std::string>);

	//make_using(client_component, endpoint_component);	
	make_thread_safe_component(client_list_component, std::vector<std::shared_ptr<entity>>);
	make_thread_safe_component(client_map_component, std::map<std::string, std::shared_ptr<entity>>);
	make_thread_safe_component(client_set_component, std::set<std::string>);

	make_thread_safe_component(index_list_component, std::vector<std::shared_ptr<entity>>);

	//make_using(router_component, endpoint_component);
	make_thread_safe_component(router_list_component, std::vector<std::shared_ptr<entity>>);
	make_thread_safe_component(router_map_component, std::map<std::string, std::shared_ptr<entity>>);
	make_thread_safe_component(router_set_component, std::set<std::string>);

	//make_using(message_component, endpoint_component);
	make_thread_safe_component(message_list_component, std::vector<std::shared_ptr<entity>>);
	make_thread_safe_component(message_map_component, std::map<std::string, std::shared_ptr<entity>>);
	make_thread_safe_component(message_set_component, std::set<std::string>);

	//make_using(proxy_component, endpoint_component);
	make_thread_safe_component(proxy_list_component, std::vector<std::shared_ptr<entity>>);
	make_thread_safe_component(proxy_map_component, std::map<std::string, std::shared_ptr<entity>>);
	make_thread_safe_component(proxy_set_component, std::set<std::string>);

	//make_using(transfer_component, endpoint_component);
	make_thread_safe_component(transfer_list_component, std::vector<std::shared_ptr<entity>>);
	make_thread_safe_component(transfer_map_component, std::map<std::string, std::shared_ptr<entity>>);
	make_thread_safe_component(transfer_set_component, std::set<std::string>);

	// Type	
	enum class endpoint_type { none, client, message, proxy, transfer };

	class endpoint_component : public name_component
	{
		make_factory(endpoint_component);

	protected:
		std::string m_address;
		std::string m_address_ext;

		endpoint_type m_type = endpoint_type::none; 		
		u16 m_port = 0;
		u16 m_port_ext = 0;
		bool m_mute = false;

	public:
		endpoint_component() { }
		endpoint_component(const endpoint_component& comp) : name_component(comp), m_address(comp.m_address), m_port(comp.m_port), 
			m_address_ext(comp.m_address_ext), m_port_ext(comp.m_port_ext), m_type(comp.m_type), m_mute(comp.m_mute) { }
		endpoint_component(const endpoint_type type) : m_type(type) { }

		// Operator
		endpoint_component& operator=(const endpoint_component& comp);

		bool operator==(const endpoint_component& comp) { return name_component::operator==(comp) && (m_address == comp.m_address) && (m_port == comp.m_port); }
		bool operator!=(const endpoint_component& comp) { return name_component::operator!=(comp) && (m_address != comp.m_address) && (m_port != comp.m_port); }

		// Interface
		virtual void clear() override;

		// Utility		
		virtual bool valid() const override { return name_component::valid() && has_address() && m_port; }

		void mute() { set_mute(true); }
		void unmute() { set_mute(false); }

		bool muted() const { return m_mute; }
		bool unmuted() const { return !muted(); }

		// Type
		bool is_client() const { return m_type == endpoint_type::client; }
		bool is_message() const { return m_type == endpoint_type::message; }
		bool is_proxy() const { return m_type == endpoint_type::proxy; }
		bool is_transfer() const { return m_type == endpoint_type::transfer; }

		// Has
		bool has_endpoint() const { return has_address() && has_port(); }
		bool has_address() const { return !m_address.empty(); }		
		bool has_port() const { return m_port > 0; }

		bool has_address_ext() const { return !m_address_ext.empty(); }
		bool has_port_ext() const { return m_port_ext > 0; }

		bool has_type() const { return m_type != endpoint_type::none; }		
		bool has_mute() const { return m_mute; }

		// Set
		void set_address(const std::string& address) { m_address = address; }		
		void set_port(const u16 port) { m_port = port; }

		void set_address_ext(const std::string& address) { m_address_ext = address; }
		void set_port_ext(const u16 port) { m_port_ext = port; }

		void set_type(const endpoint_type type) { m_type = type; }		
		void set_mute(const bool mute) { m_mute = mute; }

		// Get
		const std::string& get_address() const { return m_address; }			
		u16 get_port() const { return m_port; }

		const std::string& get_address_ext() const { return m_address_ext; }
		u16 get_port_ext() const { return m_port_ext; }

		endpoint_type get_type() const { return m_type; }		

		// Static
		static ptr create(const endpoint_type type) { return ptr(new endpoint_component(type)); }
	};
}
#endif
