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

#ifndef _EJA_SERVICE_DATA_H_
#define _EJA_SERVICE_DATA_H_

#include "entity/entity_type.h"
#include "system/type.h"

namespace eja
{
	class entity;

	enum class service_type { none, client, message, transfer };
	enum class service_action { none, browse, chat, client, close, download, file, handshake, join, ping, quit, read, search, socket, tunnel, upload, write };

	class service_data final : public entity_type
	{
		make_factory(service_data);

	private:
		service_type m_type = service_type::none;
		service_action m_action = service_action::none;
		size_t m_retries = 0;
		size_t m_retry = 0;

	public:
		service_data();
		service_data(const service_data& data);

		service_data(const service_type type);
		service_data(const service_type type, const service_action action);	
		service_data(const service_type type, const service_action action, const std::shared_ptr<entity> entity);
		service_data(const service_type type, const std::shared_ptr<entity> entity); 
		service_data(const service_type type, const size_t retries) : m_type(type), m_retries(retries) { }

		service_data(const service_action action);
		service_data(const service_action action, const std::shared_ptr<entity> entity);
		service_data(const service_action action, const size_t retries) : m_action(action), m_retries(retries) { }

		service_data(const std::shared_ptr<entity> entity);

		// Operator
		service_data& operator=(const service_data& data);

		// Utility
		bool valid() const { return has_type(); }

		// Has
		bool has_type() const { return m_type != service_type::none; }
		bool has_action() const { return m_action != service_action::none; }
		bool has_retry(const bool update = false);

		// Is
		bool is_client_type() const { return m_type == service_type::client; }
		bool is_message_type() const { return m_type == service_type::message; }
		bool is_transfer_type() const { return m_type == service_type::transfer; }

		bool is_browse_action() const { return m_action == service_action::browse; }
		bool is_chat_action() const { return m_action == service_action::chat; }
		bool is_client_action() const { return m_action == service_action::client; }
		bool is_close_action() const { return m_action == service_action::close; }
		bool is_download_action() const { return m_action == service_action::download; }
		bool is_file_action() const { return m_action == service_action::file; }
		bool is_handshake_action() const { return m_action == service_action::handshake; }
		bool is_join_action() const { return m_action == service_action::join; }
		bool is_ping_action() const { return m_action == service_action::ping; }
		bool is_quit_action() const { return m_action == service_action::quit; }
		bool is_read_action() const { return m_action == service_action::read; }
		bool is_search_action() const { return m_action == service_action::search; }
		bool is_socket_action() const { return m_action == service_action::socket; }
		bool is_tunnel_action() const { return m_action == service_action::tunnel; }
		bool is_upload_action() const { return m_action == service_action::upload; }
		bool is_write_action() const { return m_action == service_action::write; }

		// Set
		void set_type(const service_type type) { m_type = type; }
		void set_action(const service_action action) { m_action = action; }
		void set_retries(const size_t retries) { m_retries = retries; }

		// Get
		service_type get_type() const { return m_type; }
		service_action get_action() const { return m_action; }		
		size_t get_retries() const { return m_retries; }
		size_t get_retry() { return m_retry; }

		// Static
		static ptr create(const service_type type) { return ptr(new service_data(type)); }
		static ptr create(const service_type type, const service_action action) { return ptr(new service_data(type, action)); }
		static ptr create(const service_type type, const service_action action, const std::shared_ptr<entity> entity) { return ptr(new service_data(type, action, entity)); }
		static ptr create(const service_type type, const std::shared_ptr<entity> entity) { return ptr(new service_data(type, entity)); }
		static ptr create(const service_type type, const size_t retry) { return ptr(new service_data(type, retry)); }

		static ptr create(const service_action action) { return ptr(new service_data(action)); }
		static ptr create(const service_action action, const std::shared_ptr<entity> entity) { return ptr(new service_data(action, entity)); }
		static ptr create(const service_action action, const size_t retry) { return ptr(new service_data(action, retry)); }

		static ptr create(const std::shared_ptr<entity> entity) { return ptr(new service_data(entity)); }		
	};
}

#endif
