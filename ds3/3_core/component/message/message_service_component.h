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

#ifndef _EJA_MESSAGE_SERVICE_COMPONENT_H_
#define _EJA_MESSAGE_SERVICE_COMPONENT_H_

#include <memory>
#include <string>

#include "component/service/service_component.h"
#include "http/http_code.h"
#include "message/message_packer.h"
#include "system/type.h"

namespace eja
{
	class entity;
	class group_security_list_component;
	class http_request;
	class http_response;
	class http_socket;
	class service_data;

	class message_service_component : public service_component
	{
		make_factory(message_service_component);

	private:
		enum class async_state { none, close, read };
		enum class tunnel_state { none, idle, running };
		
		std::shared_ptr<group_security_list_component> m_group_list;
		boost::atomic<tunnel_state> m_tunnel_state;
		bool m_motd = false;

	private:
		// Interface
		ptr shared_from_this() { return shared_from_base<message_service_component>(); }

		// Utility
		virtual void close() override;

		// Commands
		void browse(const std::shared_ptr<entity> client);

		void chat(const std::shared_ptr<entity> client, const std::shared_ptr<entity> data);
		
		void client();
		
		void file(const std::shared_ptr<entity> data);

		void folder(const std::shared_ptr<entity> client, const std::string& path);
		void folder_client(const std::shared_ptr<entity> client, const std::string& path);

		void async_group();
		void group();

		void async_handshake();
		void handshake();

		void async_join();
		void join();
		
		void quit();

		void search(const std::shared_ptr<entity> data);

		void async_socket();
		void socket();

		void async_tunnel();
		void tunnel();
		
		// Request
		void read_request(const std::shared_ptr<http_socket> socket);
		void read_request(const std::shared_ptr<http_socket> socket, const std::shared_ptr<http_request> request);

		template <typename T>
		void write_request(const T t, const std::shared_ptr<service_data> data = nullptr);
		void write_request(const std::shared_ptr<message_packer> packer, const std::shared_ptr<service_data> data = nullptr);

		template <typename T>
		void write_request(const std::shared_ptr<http_socket> socket, const T t, const std::shared_ptr<service_data> data = nullptr);
		void write_request(const std::shared_ptr<http_socket> socket, const std::shared_ptr<message_packer> packer, const std::shared_ptr<service_data> data = nullptr);

		// Response
		void read_response(const std::shared_ptr<http_socket> socket, const std::shared_ptr<message_packer> packer, const std::shared_ptr<service_data> data = nullptr);
		void read_response(const std::shared_ptr<http_response> response, const std::shared_ptr<service_data> data = nullptr);

		void write_response(const std::shared_ptr<http_socket> socket);
		void write_response(const std::shared_ptr<http_socket> socket, const http_code code, const async_state state);
		void write_response(const std::shared_ptr<http_socket> socket, const std::shared_ptr<http_response> response, const async_state state);
		
		void write_ok(const std::shared_ptr<http_socket> socket) { write_response(socket, http_code::ok, async_state::close); }
		void write_not_found(const std::shared_ptr<http_socket> socket) { write_response(socket, http_code::not_found, async_state::close); }
		void write_internal_service_error(const std::shared_ptr<http_socket> socket) { write_response(socket, http_code::internal_service_error, async_state::close); }
		void write_not_implemented(const std::shared_ptr<http_socket> socket) { write_response(socket, http_code::not_implemented, async_state::close); }
		void write_unauthorized(const std::shared_ptr<http_socket> socket) { write_response(socket, http_code::unauthorized, async_state::close); }

		// Set
		using service_component::set_status;
		virtual void set_status() override;

	public:
		message_service_component();
		message_service_component(const message_service_component& comp) : service_component(comp), m_tunnel_state(tunnel_state::none) { }
		message_service_component(const size_t threads) : service_component(threads), m_tunnel_state(tunnel_state::none) { }

		// Interface
		virtual void update() override;
		virtual void clear() override;

		// Utility
		void async_restart();

		virtual void start() override;
		virtual void restart() override;
		virtual void stop() override;

		// Operator
		message_service_component& operator=(const message_service_component& comp);

		// Commands
		void async_browse(const std::shared_ptr<entity> data);

		void async_chat(const std::shared_ptr<entity> data, const std::shared_ptr<entity> client = nullptr);
		
		void async_client();
		
		void async_file(const std::shared_ptr<entity> data);

		void async_folder(const std::shared_ptr<entity> data, const std::string& path);
		void async_folder_client(const std::shared_ptr<entity> data, const std::string& path);

		void async_quit();
		
		void async_search(const std::shared_ptr<entity> data);

		// Has
		bool has_motd() const { return m_motd; }

		// Set
		void set_motd(const bool motd) { m_motd = motd; }

		// Get
		std::shared_ptr<group_security_list_component> get_group_security_list() const { return m_group_list; }
	};

	// Request
	template <typename T>
	void message_service_component::write_request(const T t, const std::shared_ptr<service_data> data /*= nullptr*/)
	{
		const auto packer = message_packer::create();
		packer->push_back(t);

		write_request(packer, data);
	}

	template <typename T>
	void message_service_component::write_request(const std::shared_ptr<http_socket> socket, const T t, const std::shared_ptr<service_data> data /*= nullptr*/)
	{
		const auto packer = message_packer::create();
		packer->push_back(t);

		write_request(socket, packer, data);
	}
}

#endif
